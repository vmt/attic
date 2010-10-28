//-----------------------------------------------------------------------------
// task.cc - qtask implementation.
//-----------------------------------------------------------------------------
#include <cerrno>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <qvm/types.hh>
#include <qvm/error.hh>
#include <qvm/task.hh>
#include <qvm/status.hh>
#include <qvm/stream.hh>

using namespace std;

//-----------------------------------------------------------------------------
// Default event handlers.
//-----------------------------------------------------------------------------
void qeventhandler::entry(qstatus&) { }

class DefEventHndlr : public qeventhandler { 
	public: void entry(qstatus& s) { }
}  spawnEventHdnlrObj,  taskEventHndlrObj, genericEventHndlrObj;

//-----------------------------------------------------------------------------
// qtask(argc, argv) - Task Object Constructor. Accepts the program cmd line 
// arguments as parameters. The constructor pings the node server and waits for 
// an echo.
//-----------------------------------------------------------------------------
qtask::qtask(int argc, char* argv[])
{
	if (argc < 3)
		throw Error(qerror::eInvalidCmdLine, "Invalid cmdline.");

	// task-desc = arg(1), task-node-id = arg(2)
	tfd = atoi(argv[1]);
	tid.nid = atoi(argv[2]);

	// make ping packet
	qpacket* packet = mkpacket(P_PING, tid.nid, 0, tid.nid, 0);

	// send to node server
	write(packet);

	// recv an echo. discard all other packets.
	while (1) {
		read(packet);
		if (packet->h_itag() == P_PING) {
			tid.nid = packet->h_tnid();
			tid.tid = packet->h_ttid();
			tpid.nid= packet->h_fnid();
			tpid.tid= packet->h_ftid();
			break;
		} 
	}

	// set event handlers
	tspawneventhndlr  = &spawnEventHdnlrObj;
	ttaskeventhndlr   = &taskEventHndlrObj;
	tgenericeventhndlr= &genericEventHndlrObj;

	// release packet
	freepk(packet);
}

//-----------------------------------------------------------------------------
// spawn() - Spawn Task
//-----------------------------------------------------------------------------
void qtask::spawn(const string& name, qtag tag)
{
	// make cmd packet
	qpacket* packet = mkpacket(P_SPAWN, tid.nid, 0, tid.nid, 0);

	packet->h_utag() = tag;
	packet->pack(mytid().nid);
	packet->pack(mytid().tid);
	packet->pack(name);
	packet->pack(tag);	

	// send to nodeserver
	write(packet);

	// release packet
	freepk(packet);
}

//-----------------------------------------------------------------------------
// mutators for even handler pointers
//-----------------------------------------------------------------------------
void qtask::setspawneventhandler(qeventhandler *e)
{
	tspawneventhndlr = e;
}
void qtask::settaskeventhandler(qeventhandler *e)
{
	ttaskeventhndlr = e;
}
void qtask::setgenericeventhandler(qeventhandler *e)
{
	tgenericeventhndlr = e;
}

//-----------------------------------------------------------------------------
// mkstream() - Create a stream
//-----------------------------------------------------------------------------
qstream* qtask::mkstream(const qtaskid& taskid, qtag tag)
{
	return new qstream(this, taskid, tag);
}

//-----------------------------------------------------------------------------
// recvfrom() - Packet Receiver.
//-----------------------------------------------------------------------------
qpacket* qtask::recvfrom(const qtaskid& vtid, qtag sid)
{
	qpacket* packet;
	qtaskid id;

	// First check if the packet is already available in the
	// in-queue.
	//
	if ((packet = tinq.deq(vtid.nid, vtid.tid, sid)))
		return packet;
	
again:	// readin a packet in
	packet = recv(id);
	if (id == vtid && packet->h_utag() == sid)
		return packet;
	// if the packet was not excpected, queue it. and
	// try again
	else {
		tinq.enq(packet);
		goto again;
	}
	return packet;
}

//-----------------------------------------------------------------------------
// recvfrom_nb(vid) - Non-Blocking Packet Receiver.
//-----------------------------------------------------------------------------
qpacket* qtask::recvfrom_nb(const qtaskid& vtid, qtag sid)
{
	qpacket* packet;
	qtaskid id;

	// First check if the packet is already available in the
	// in-queue.
	//
	if ((packet = tinq.deq(vtid.nid, vtid.tid, sid)))
		return packet;
	
	// try reading a packet in
again:	if ((packet = recv_nb(id))) {
		if (id == vtid && packet->h_utag() == sid)
			return packet;
		// if the packet was not excpected, queue it. and
		// try again
		else {
			tinq.enq(packet);
			goto again;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// sendto()  - Wrapper for send.
//-----------------------------------------------------------------------------
void qtask::sendto(qpacket* packet, const qtaskid& vid)
{
	// set header fields
	packet->h_itag() = P_DATA;
	packet->h_tnid() = vid.nid;
	packet->h_ttid() = vid.tid;

	// write to stream
	write(packet);
}

//-----------------------------------------------------------------------------
// sendto_nb()  - Wrapper for non-blocking send
//-----------------------------------------------------------------------------
bool qtask::sendto_nb(qpacket* packet, const qtaskid& vid)
{
	// set header fields
	packet->h_itag() = P_DATA;
	packet->h_tnid() = vid.nid;
	packet->h_ttid() = vid.tid;

	// write to stream
	return (write_nb(packet)) ? true : false;
}


//-----------------------------------------------------------------------------
// callback() - Status packet callback`er.
//-----------------------------------------------------------------------------
void qtask::callback(qpacket* packet)
{	
	switch (packet->h_itag()) {
		case P_INVTASK :
		case P_TIMEOUT :
		case P_TASKQUIT: {
			qtaskstatus ts(packet);
			ttaskeventhndlr->entry(ts);
			return;
		}
		case P_SPAWNED : 
		case P_SPAWN_FAILED: {
			qspawnstatus ss(packet);
			tspawneventhndlr->entry(ss);
			return;
		}
		default: {
			qstatus s(packet);
			tgenericeventhndlr->entry(s);
		}
	}
}

//-----------------------------------------------------------------------------
// recv() - Wrapper for read.
//-----------------------------------------------------------------------------
qpacket* qtask::recv(qtaskid& vtid)
{
again:	qpacket* packet = allocpk();	

	read(packet);
	if (packet->h_itag() == P_DATA) {
		// set header fields
		vtid.nid = packet->h_fnid();
		vtid.tid = packet->h_ftid();
		return packet;
	} else {
		// callback an eventhandler.
		callback(packet);
		goto again;
	}
}

//-----------------------------------------------------------------------------
// recv_nb() - non-blocking recv
//-----------------------------------------------------------------------------
qpacket* qtask::recv_nb(qtaskid& vtid)
{
again:	qpacket* packet = allocpk();

	if (read_nb(packet)) {
		if (packet->h_itag() == P_DATA) {
			// set header fields
			vtid.nid = packet->h_fnid();
			vtid.tid = packet->h_ftid();
			return packet;
		} else {
			callback(packet);
			goto again;
		}
	}
	freepk(packet);
	return 0;
}

//-----------------------------------------------------------------------------
// probe() - probe for an event. queue all other packets.
//-----------------------------------------------------------------------------
void qtask::probe()
{	
again:	qpacket* packet = allocpk();
	
	// try reading a packet in
	read(packet);
	if (packet->h_itag() == P_DATA) {
		tinq.enq(packet);
		goto again;
	} else {
		callback(packet);
	}
}

//-----------------------------------------------------------------------------
// probe_nb() - non-blocking probe for an event. queues all other packets.
//-----------------------------------------------------------------------------
void qtask::probe_nb()
{
again:	qpacket* packet = allocpk();
	
	// try reading a packet in
	if (read_nb(packet)) {
		if (packet->h_itag() == P_DATA) {
			tinq.enq(packet);
			goto again;
		} else {
			callback(packet);
			packet->reset();
		}
	}

	// release packet
	freepk(packet);
}

//-----------------------------------------------------------------------------
// read() - read packet from router.
//-----------------------------------------------------------------------------
void qtask::read(qpacket* packet)
{
	// try reading.
again:	if (::recv(tfd, (void*) (packet->buffer()), 
		   packet->size(), 0) < 0) {
		if (errno == EINTR)
			goto again;
		throw LibError();
	}
}

//-----------------------------------------------------------------------------
// write() - write packet to router.
//-----------------------------------------------------------------------------
void qtask::write(qpacket* packet)
{
again:	size_t  offset = 0;
	ssize_t rsize  = 0;
	if ((rsize = ::write(tfd, 
			     (const void*)(packet->buffer()+offset), 
			     packet->size() - offset)) < 0) {
		if (errno == EINTR)
			goto again;
		throw LibError();
	}	
	if ((unsigned)rsize < packet->size()) {
		offset += rsize;
		goto again;
	}	
}

//-----------------------------------------------------------------------------
// read_nb() - Non-blocking, recv from router.
//-----------------------------------------------------------------------------
qpacket* qtask::read_nb(qpacket* packet)
{
	// save descriptor flags, and set to non-blocking
	int flags; 
	flags = fcntl(tfd, F_GETFL, 0);
	fcntl(tfd, F_SETFL, flags | O_NONBLOCK);


	// try reading.
again:	if (::recv(tfd, (void*) (packet->buffer()), 
		   packet->size(), MSG_WAITALL) < 0) {
		if (errno == EINTR)
			goto again;
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw LibError();
		// restore flags and return.
		fcntl(tfd, F_SETFL, flags);
		// the sys call would block, so return null
		return(NULL);
	}

	// restore flags and return.
	fcntl(tfd, F_SETFL, flags);
	return(packet);
}

//-----------------------------------------------------------------------------
// write_nb() - Non-blocking, send to router.
//-----------------------------------------------------------------------------
qpacket* qtask::write_nb(qpacket* packet)
{
	// save descriptor flags, and set to non-blocking
	int flags; 
	flags = fcntl(tfd, F_GETFL, 0);
	fcntl(tfd, F_SETFL, flags | O_NONBLOCK);	

again:	size_t  offset = 0;
	ssize_t rsize  = 0;
	if ((rsize = ::write(tfd, 
			     (const void*)(packet->buffer()+offset), 
			     packet->size() - offset)) < 0) {
		if (errno == EINTR)
			goto again;
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			throw LibError();
	}
	if ((unsigned)rsize < packet->size()) {
		offset += rsize;
		goto again;
	}
	// restore flags and return packet
	fcntl(tfd, F_SETFL, flags);
	return(packet);
}

//-----------------------------------------------------------------------------
// ~qtask() - Task Object Destructor. 
//-----------------------------------------------------------------------------
qtask::~qtask()
{
	close(tfd);
}
