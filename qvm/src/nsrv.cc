//-----------------------------------------------------------------------------
// nsrv.cc - Virtual Machine VirtualNode Server Object Implementation.
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sock.hh>
#include <error.hh>
#include <protocol.hh>
#include <bin.hh>
#include <cpu.hh>
#include <packetio.hh>
#include <nsrv.hh>

using namespace std;

//-----------------------------------------------------------------------------
// VirtualNodeServer() - VirtualNode Server Constructor
//-----------------------------------------------------------------------------
VirtualNodeServer::VirtualNodeServer(const string& binFile,
				     const string& serverHostName,
				     unsigned int  serverPort,
				     unsigned int  myPort, 
				     unsigned int  taskSetSize):
	VirtualNodeSocket(serverHostName, serverPort, myPort),
	vtaskscount(0),
	vtasksmax(taskSetSize),
	vloop(true),
	vbin(binFile.c_str())
{

	if (vtasksmax == 0)
		throw Error(eNodeServer, "Invalid Task Set Size.");
	else 	// allocate the task set
		vtasks = new Task[vtasksmax];

	// assign default values to all task slots
	for (Ttid i = 0 ; i < vtasksmax; ++i) {
		Task& task = vtasks[i];

		task.talloced = false;		
		task.tzombie  = false;
		task.tfd = 0;
		task.tid = 0;
	}

	// allocate task 0 to nodeserver.
	allocTask();

	// spawn console as task 1.
	croak("Starting console...");
	spawnTask(sockid, 0, "console");
}

//-----------------------------------------------------------------------------
// allocTask() - Allocates a new task. 
//-----------------------------------------------------------------------------
Task& VirtualNodeServer::allocTask()
{
	// check if the set is full.
	if (vtaskscount == vtasksmax)
		throw Error(eNodeServer, "Task Table Overflow.");

	// search for a free slot.
	Ttid id;
	for (id = 0 ; vtasks[id].talloced ; ++id);
	Task& task = vtasks[id];

	// if tid is 0, it means it hasn't been allocated previously. So
	// give it an appropriate id.
	if (task.tid == 0)
		task.tid  = id;
	else 	// else alloc a unique id
		task.tid += vtasksmax;

	// set flags/attribs
	task.talloced = true;
	task.tzombie  = false;
	task.tpnid    = 0;
	task.tptid    = 0;
	task.tfd      = 0;

	// return a reference
	return(task);
}

//-----------------------------------------------------------------------------
// getTask(Ttid tid) - returns pointer to task by tid, only if its allocated.
//-----------------------------------------------------------------------------
Task* VirtualNodeServer::getTask(Ttid id)
{
	Task& task = vtasks[id % vtasksmax];
	if (task.talloced && task.tid == id)
		return(&task);
	else return(0);
}

//-----------------------------------------------------------------------------
// spawnTask() - Spawns off a task.
//-----------------------------------------------------------------------------
Ttid VirtualNodeServer::spawnTask(Tnid pnid, Ttid ptid, const string& binName)
{
	char sockdarg[12], nidarg[12];
	const char* filename;
	int sockd[2];
	pid_t pid;

	// get actual path associated with bin name from hash table.
	if ((filename = vbin.file(binName.c_str())) == NULL)
		throw Error(eNodeServer, "Invalid Task.");
		
	// allocate a new ask.
	Task& task  = allocTask();
	task.tptid  = ptid;
	task.tpnid  = pnid;
	task.tzombie= false;

	// create socket pair, connecting task and node server
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockd) < 0) 
		throw LibError();

	// fork and exec task
	if ((pid = fork()) < 0)
		throw LibError();
	if (pid) {
		// this is the parent. close one end of the socket.
		close(sockd[1]);
		// set pid and sock desc.
		task.tpid = pid;
		task.tfd  = sockd[0];
	} else {
		// this is the child. close one end of the socket
		close(sockd[0]);
		// make arguments for exec. 
		sprintf(sockdarg, "%d", sockd[1]);
		sprintf(nidarg,   "%d", sockid);
		EXEC(filename, sockdarg, nidarg);
		// failed.
		exit(EXIT_FAILURE);
	}
	
	// increment task count
	++vtaskscount;

	// set in file descriptor set
	FD_SET(task.tfd, &sockrfdset);

	// return the id of the new task
	return(task.tid);
}

//-----------------------------------------------------------------------------
// spawnTask() - Overloaded spawnTask routine which accepts a P_SPAWN request
// packet.
//-----------------------------------------------------------------------------
Ttid VirtualNodeServer::spawnTask(Packet* packet)
{
	Ttag tag; Ttid tid, ptid; Tnid pnid;
	string name; 

	// unpack details
	packet->unpack(pnid);
	packet->unpack(ptid);
	packet->unpack(name);
	packet->unpack(tag );

	try {

		// spawn task
		tid = spawnTask(pnid, ptid, name);

		// make reply packet and send
		Packet *reply;
		reply = mkpacket(P_SPAWNED, sockid, tid, 
				 pnid, ptid, allocpk());
		reply->pack(name);
		reply->pack(tag);

		// route packet depending on destination
		if (pnid == sockid)
			queueTaskPacket(reply);
		else queueNodePacket(reply);

		// send status update
		packet = mkpacket(P_STATUSUPDATE, sockid, 0, 
				  0, 0, packet);
		packet->pack(Tubyte(50));
		packet->pack(Tushort(vtaskscount));
		queueSrvPacket(packet);

		// return the id.
		return tid;

	} catch (...) {

		// make reply and send
		Packet *reply;
		reply = mkpacket(P_SPAWN_FAILED, sockid, 0, 
				 pnid, ptid, allocpk());
		reply->pack(name);
		reply->pack(tag);

		// route packet depending on destination
		if (pnid == sockid)
			queueTaskPacket(reply);
		else queueNodePacket(reply);

		// free packet
		freepk(packet);
	}

	return 0;
}

//-----------------------------------------------------------------------------
// serveTask() - Serve Task. Routes packets from task.
//-----------------------------------------------------------------------------
void VirtualNodeServer::serveTask(Task& task)
{
	Packet* packet = allocpk();

	// return if the packet returned is null. probably is an error. so
	// the task is set to zombie
	if (::recvPacket(task.tfd, packet) == NULL) {
		task.tzombie = true;
		freepk(packet);
		return;
	}

	// set default parameters.
	packet->h_fnid() = sockid;
	packet->h_ftid() = task.tid;

	// if packet is directed to node server..
	if (packet->h_tnid() == sockid) {

		// Task ping - A task pings the node server to get its own
		// id plus that of its parent. Only the packet header is
		// modified and returned.
		if (packet->h_ttid() == 0 && packet->h_itag() == P_PING) {
			croak("Task("<<task.tid<<"): ping.");
			packet = mkpacket(P_PING, task.tpnid, task.tptid,
					  sockid, task.tid, packet);
			queueTaskPacket(packet);
			return;

		} 

		// Task spawn - A task can request for a child to be spawned. 
		// This request is actually routed to the server which decides
		// where to spawn.
		else if (packet->h_ttid() == 0 && 
			 packet->h_itag() == P_SPAWN) {
			croak("Task("<<task.tid<<"): requested spawn.");
			queueSrvPacket(packet);
			return;

		} 

		// Data - A data packet for local task is directly queued into
		// the task queue.
		else if (packet->h_ttid() && packet->h_itag() == P_DATA) {
			queueTaskPacket(packet);
			return;
		}

		// else just free the packet
		else {
			freepk(packet);
		}

	} 

	// packet is destined for another node..
	else if (packet->h_itag() == P_DATA) {

		// validate destination node. 
		if (packet->h_tnid() >= socknodescount) {
			// if invalid, an error packet is sent.
			packet = mkpacket(P_INVTASK, 
					  packet->h_tnid(), packet->h_ttid(),
	 			  	  packet->h_fnid(), packet->h_ftid(), 
					  packet );
			queueTaskPacket(packet);
			return;
		}

		VirtualNode& node = socknodes[packet->h_tnid()];
		if (node.nactive)
			// if node is set active, queue packet for sending.
			queueNodePacket(packet);
		else {
			// if node is inactive, a timeout is sent.
			packet = mkpacket(P_TIMEOUT, 
					  packet->h_tnid(), packet->h_ttid(),
	 			  	  packet->h_fnid(), packet->h_ftid(), 
					  packet );
			queueTaskPacket(packet);
		}
		return;
	} 

	else {
		freepk(packet);
	}
}

//-----------------------------------------------------------------------------
// acquireZombieTask() - clean up a zombie task. 
//-----------------------------------------------------------------------------
void VirtualNodeServer::acquireZombieTask(Task& task)
{
	assert(task.talloced == true);
	assert(task.tzombie == true);
	assert(task.tfd != 0);

	croak("Task("<<task.tid<<"): zombie, cleaning up.");

	// reduce the task count
	--vtaskscount;

	// send status update
	Packet* packet = mkpacket(P_STATUSUPDATE, sockid, 0, 
			  0, 0, allocpk());
	packet->pack(Tubyte(50));
	packet->pack(Tushort(vtaskscount));
	queueSrvPacket(packet);

	// clear the stream descriptor
	FD_CLR(task.tfd, &sockrfdset);
	FD_CLR(task.tfd, &sockwfdset);
	close(task.tfd);
	task.tfd = 0;

	// set flags and flush any packets directed towards it.
	task.talloced = false;
	task.tzombie  = false;
	task.toutq.flush();

	if (task.tpnid == sockid && task.tptid == 0)
		return;
	if (task.tpnid == sockid) {

		Task* ptask = getTask(task.tptid);

		// if parent doesn't exist, don't send a status message
		if (ptask == NULL)
			return;
		// send a status message to parent.
		Packet *packet = mkpacket(P_TASKQUIT, sockid, task.tid, 
					  task.tpnid, task.tptid, 
					  allocpk());
		queueTaskPacket(packet);
		return;
	} 
	else {
		Packet *packet = mkpacket(P_TASKQUIT, 
			sockid, task.tid, task.tpnid, task.tptid, allocpk());
		queueNodePacket(packet);
		return;		
	}
}

//-----------------------------------------------------------------------------
// acceptNodePacket() - Accept packets from peers.
//-----------------------------------------------------------------------------
void VirtualNodeServer::acceptNodePacket(Packet* packet)
{
	assert(packet != NULL);
	assert(packet->h_tnid() == sockid);
	assert(packet->h_fnid() != sockid);

	switch (packet->h_itag()) {
		// if packet is any one of these types ..
		case P_DATA:
		case P_SPAWNED:
		case P_SPAWN_FAILED:
		case P_TASKQUIT:
		case P_INVTASK:
			// queue for the destined task.
			queueTaskPacket(packet);
			return;
	default:
		// else release packet
		freepk(packet);
	}
}

//-----------------------------------------------------------------------------
// acceptSrvPacket() - Accept packets from Server.
//-----------------------------------------------------------------------------
void VirtualNodeServer::acceptSrvPacket(Packet* packet)
{
	switch (packet->h_itag()) {
		// control message: SPAWN - spawn a new task.
		case P_SPAWN: 
			spawnTask(packet);
			return;

		// control message: ACTIVE - set node to active.
		case P_ACTIVE: {
			Tnid nodeid;
			packet->unpack(nodeid);
			if (socknodes[nodeid].nactive == false)
				activate(nodeid);
			freepk(packet);
			return;
		}

		// control message: Inactive - set node to inactive
		case P_INACTIVE: {
			Tnid nodeid;
			packet->unpack(nodeid);
			deactivate(nodeid);
			freepk(packet);
			return;
		}

		default: {
			freepk(packet);
		}
	}
}

//-----------------------------------------------------------------------------
// queueTaskPacket() - Queues packet for task.
//-----------------------------------------------------------------------------
void VirtualNodeServer::queueTaskPacket(Packet* packet)
{
	assert(packet != NULL);

	Task* task = getTask(packet->h_ttid());

	// if the task doesn't exist
	if (task == NULL) {
		if  (packet->h_itag() == P_DATA) {
			// make invtask packet
			packet = mkpacket(P_INVTASK, packet->h_tnid(), 
					  packet->h_ttid(), packet->h_fnid(), 
					  packet->h_ftid(), packet );
			// route packet
			if (packet->h_tnid() == sockid)
				queueTaskPacket(packet);
			else queueNodePacket(packet);
		} else freepk(packet);

	} else {
		// queue in task packet queue
		task->toutq.enq(packet);
		FD_SET(task->tfd, &sockwfdset);
	}
}


//-----------------------------------------------------------------------------
// taskroute()
//-----------------------------------------------------------------------------
void VirtualNodeServer::taskroute()
{
	// go through each of the tasks and process
	for (Ttid id = 1; id < vtasksmax; ++id) {
		Task& task = vtasks[id];

		// acquire the task if its been set a zombie.
		if (task.tzombie) 
			acquireZombieTask(task);

		// if task is not allocated, skip
		if (task.talloced == false)
			continue;

		// if packets are queued in the task queue
		if (! task.toutq.isEmpty()) {
			// if writable, send the packet
			if (FD_ISSET(task.tfd, &sockwfdset)) {
				Packet* packet = task.toutq.deq();
				::sendPacket(task.tfd, packet);
				if (task.toutq.isEmpty())
					FD_CLR(task.tfd, &sockwfdset);
				freepk(packet);
			} else
				FD_SET(task.tfd, &sockwfdset);
		}

		if (FD_ISSET(task.tfd, &sockrfdset))
			serveTask(task);
		else FD_SET(task.tfd, &sockrfdset);	

	}
}

//-----------------------------------------------------------------------------
// work() - work loop
//-----------------------------------------------------------------------------
void VirtualNodeServer::work()
{
	int flags;
	flags = fcntl(socknetfd, F_GETFL, 0);
	fcntl(socknetfd, F_SETFL, flags | O_NONBLOCK);

	// loop ...
	while (vloop) {
		if (select(256, &sockrfdset, &sockwfdset, NULL, NULL) < 0) {
			if (errno != EINTR)
				throw LibError();
			continue;
		}
		netroute();
		taskroute();
	}
}

//-----------------------------------------------------------------------------
// sigChild() - SIGCHLD Handler. This signal is generated when one any spawned
// task quits. Correspondingly in the table, that process is set to be zombie
// so it can be detected and acquired in the normal course of action.
//-----------------------------------------------------------------------------
void VirtualNodeServer::sigChild() 
{ 
	register Task* task = vtasks;
	register Task* tend = vtasks + vtasksmax;
	int status;
	pid_t pid = wait(&status); 
 
	for (; task < tend; ++task) {
		if (task->talloced && task->tpid == pid) {
			task->tzombie = true;
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// sigIntHandler() - SIGINT terminal interrupt handler. This causes the kernel
// loop to stop and properly clean up.
//-----------------------------------------------------------------------------
void VirtualNodeServer::sigInt()
{
	vloop = false;
}
 
//-----------------------------------------------------------------------------
// ~VirtualNodeServer() - VirtualNode server destructor.
//-----------------------------------------------------------------------------
VirtualNodeServer::~VirtualNodeServer()
{
	delete[] vtasks;
}
