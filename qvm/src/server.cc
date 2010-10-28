//-----------------------------------------------------------------------------
// server.cc - Virtual Machine Server Implementation.
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <protocol.hh>
#include <error.hh>
#include <packet.hh>
#include <packetio.hh>
#include <queue.hh>
#include <wrap.hh>
#include <server.hh>

using namespace std;

//-----------------------------------------------------------------------------
// VirtualMachineServer() - Virtual Machine Server Object Constructor
//-----------------------------------------------------------------------------
VirtualMachineServer::VirtualMachineServer(const string& confFileName, 
					   unsigned int myPort):
	Authenticator(vrfdset),
	vnodes(0),
	vnodescount(0),	
	vport(myPort),
	vconffilename(confFileName),
	vloop(true)
{
	croak("Starting Virtual-Machine-Server..");

	// setup configuration
	setupConf();

	// create tcp socket
	if ((vfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		throw LibError();

	// set socket options
	int y = 1;
	struct linger l = { 1, 0 };
	if (setsockopt(vfd, SOL_SOCKET, SO_LINGER,    &l, sizeof(l)) < 0)	 
		throw LibError();
	if (setsockopt(vfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)) < 0)
		throw LibError();
	if (setsockopt(vfd, SOL_SOCKET, SO_KEEPALIVE, &y, sizeof(y)) < 0)
		throw LibError();

	// bind to local address
	struct sockaddr_in sa;
	memset((void*)&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(vport);
	if (::bind(vfd, (const sockaddr*)&sa, sizeof(sa)) < 0) 
		throw LibError();

	// listen, for connections
	if (::listen(vfd, 10) < 0)
		throw LibError();

	FD_ZERO(&vrfdset);
	FD_ZERO(&vwfdset);
	FD_SET(vfd, &vrfdset);
}

//-----------------------------------------------------------------------------
// setupConf() - Setup the vm server from the xml configuration file.
//-----------------------------------------------------------------------------
void VirtualMachineServer::setupConf()
{
	croak("Loading configuration ("<<vconffilename<<")...");

	xmlDocPtr  doc;
	xmlNodePtr cur;
	xmlChar*   key;

	if ((doc = xmlParseFile(vconffilename.c_str())) == 0) 
		throw Error(eXmlConf, "Libxml2: parser failed.");
	if ((cur = xmlDocGetRootElement(doc)) == 0) {
		xmlFreeDoc(doc);
		throw Error(eXmlConf, "Libxml2: parser failed.");
	}
	if (xmlStrcmp(cur->name, (const xmlChar *)"vm")) {
		xmlFreeDoc(doc);
		throw Error(eXmlConf, "Root node is not <vm>.");
	}
	if ((key = xmlGetProp(cur, (xmlChar*)"nodes")) == 0)
		throw Error(eXmlConf, "Count attribute not specified.");

	vnodescount = atoi((const char*)key);
	vnodes = new Node[vnodescount];

	croak(int(vnodescount)<<" nodes in virtual-machine.");

	xmlFree(key);

	cur = cur->xmlChildrenNode;

	for (size_t i = 0; cur != NULL && i < vnodescount;) {
		xmlChar *key = NULL;

		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name"))) {
			key = xmlNodeListGetString(doc, 
					cur->xmlChildrenNode, 1);
			if (key) {
				vname = (const char*)key;
				xmlFree(key);
			}
		}

		if ((!xmlStrcmp(cur->name, (const xmlChar *)"node"))) {

			if ((key = xmlGetProp(cur, 
				(const xmlChar*)"hostname")) == NULL)
				throw Error(eXmlConf, 
					"Hostname not specified.");
			string hostname = (const char*)key;
			xmlFree(key);
			if ((key = xmlGetProp(cur, 
				(const xmlChar*)"port")) == NULL)
				throw Error(eXmlConf, 
					"Port not specified.");
			Tushort port = atoi((const char*)key);
			xmlFree(key);

			// init node.			
			initNode(i, hostname, port);

			++i;
		}
		cur = cur->next;
	}	
	xmlFreeDoc(doc);
}

//-----------------------------------------------------------------------------
// initNode() - Initializes local node object.
//-----------------------------------------------------------------------------
void VirtualMachineServer::initNode(Tnid id, const string& hostname, 
				    Tushort port)
{
	croak("Initializing node("<<int(id)<<") at "<<hostname<<":"<<port);

	Node& node = vnodes[id];

	// init attributes
	node.nfd       = 0;
	node.nid       = id;
	node.ntaskcount= 0xFFFF;
	node.ncpuload  = 0xFF;
	node.nactive   = false;
	node.nhostname = hostname;
	node.nport     = port;
	
	// resolve host name
	memset((void*)&node.nsa, 0, sizeof(node.nsa));
	node.nsa.sin_family = AF_INET;
	node.nsa.sin_port   = htons (node.nport);
	node.nsalen         = sizeof(node.nsa);
	GetHostByName(node.nhostname, node.nsa);
}

//-----------------------------------------------------------------------------
// work() - Main working loop.
//-----------------------------------------------------------------------------
void VirtualMachineServer::work()
{
	croak("Entering work-loop, listening for connections..");

	// loop ...
	while (vloop) {
		// select readable/writeable sockets
		if (select(256, &vrfdset, &vwfdset, NULL, NULL) < 0) {
			if (errno != EINTR)
				throw LibError();
			continue;
		}	

		// network i/o core
		netroute();
	}
}

//-----------------------------------------------------------------------------
// netroute() - network related packet routing.
//-----------------------------------------------------------------------------
void VirtualMachineServer::netroute()
{
	// if there is a connection waiting, accept it
	if (FD_ISSET(vfd, &vrfdset))
		acceptConnection();
	else FD_SET(vfd, &vrfdset);

	// process authentication pending connections
	if (Authenticator::pending())
		Authenticator::process();

	// go through each node checking for packets to be sent
	for (Tnid id = 0; id < vnodescount; ++id) {
		Node& node = vnodes[id];

		// if node is inactive, skip
		if (node.nactive == false)
			continue;

		// if there are queued packets
		if (! node.noutq.isEmpty()) {

			// if the node socket is set writeable		
			if (FD_ISSET(node.nfd, &vwfdset)) {
				// try sending
				Packet* packet = node.noutq.peek();
				if (sendPacket(node, packet))
					freepk(node.noutq.deq());
	
				// if the queue is empty, clear from set
				if (node.noutq.isEmpty())
					FD_CLR(node.nfd, &vwfdset);
			} else  
				// set bit in descriptor set.
				FD_SET(node.nfd, &vwfdset);

		} 

		// if there are incoming packets, read
		if (FD_ISSET(node.nfd, &vrfdset)) {
			Packet* packet;
			if ((packet = recvPacket(node)))
				acceptPacket(packet, node);
		} else FD_SET(node.nfd, &vrfdset);
	}
}

//-----------------------------------------------------------------------------
// acceptConnection() - Accept a connection from a node.
//-----------------------------------------------------------------------------
void VirtualMachineServer::acceptConnection()
{	
	// accept connection.
	int sockd;
again:	if ((sockd = accept(vfd, 0, 0)) < 0) {
		if (errno == EINTR)
			goto again;
		if (errno == ECONNABORTED)
			return;
		if (errno == ECONNRESET)
			return;
		throw LibError();
	}

	// queue for authentication/identification
	Authenticator::que(sockd);
}


//-----------------------------------------------------------------------------
// authenticate() - Authenticator
//-----------------------------------------------------------------------------
void VirtualMachineServer::authenticate(int sd, Packet* packet)
{
	string hostname;
	sockaddr_in sa;
	sa.sin_family = AF_INET;	
	packet->unpack(hostname);
	packet->unpack(sa.sin_port);
	GetHostByName(hostname, sa);

	// go through each node to match with the address signature
	Tnid id = 0;
	for (;id < vnodescount; ++id) {
		Node& node = vnodes[id];
		// match 
		if (sa == node.nsa) {
			croak("Authenticated: "<<GetIP(sa)<<":"
			      <<ntohs(sa.sin_port));
			activate (node, sd);
			break;
		}
	}

	if (id == vnodescount) {
		croak("Rejected: "<<GetIP(sa)<<":"
		      <<ntohs(sa.sin_port));
		// invalid node, close connection
		close(sd);
		// release packet for reuse.
		freepk(packet);
	}
}

//-----------------------------------------------------------------------------
// activate() - Activate node in virtual machine.
//-----------------------------------------------------------------------------
void VirtualMachineServer::activate(Node& node, int sockd)
{
	// notify all peers
	for (Tnid id = 0; id < vnodescount; ++id) {
		Node& peer = vnodes[id];

		// if node is active, send notification
		if (peer.nactive) {
			Packet *packet = mkpacket(P_ACTIVE, 0, 0,
						 0, 0);
			packet->pack(node.nid);
			queue(peer, packet);
		}
	}

	// set local status variables
	node.ncpuload  = 0;
	node.ntaskcount= 0;
	node.nactive   = true;
	node.nfd       = sockd;

	croak("Node("<<int(node.nid)<<"): Activated.");
}

//-----------------------------------------------------------------------------
// deactivate() - Deactivate node in virtual machine.
//-----------------------------------------------------------------------------
void VirtualMachineServer::deactivate(Node& node)
{
	// reset local status variables
	FD_CLR(node.nfd, &vrfdset);
	node.nactive = false;
	close(node.nfd);
	node.nfd = 0;	

	// notify all peers
	for (Tnid id = 0; id < vnodescount; ++id) {
		Node& peer = vnodes[id];

		// if peer is active, send notification
		if (peer.nactive) {
			Packet *packet = mkpacket(P_INACTIVE, 0, 0,
						 0, 0);
			packet->pack(node.nid);
			queue(peer, packet);
		}
	}

	croak("Node("<<int(node.nid)<<"): Deactivated.");
}

//-----------------------------------------------------------------------------
// acceptPacket() - from netroute()
//-----------------------------------------------------------------------------
void VirtualMachineServer::acceptPacket(Packet* packet, Node& node)
{
	// switch through packets tags
	switch (packet->h_itag()) {
		case P_NODESINFO:
			sendNodesInfo(node, packet);
			break;
		case P_STATUSUPDATE:
			updateStatus(node, packet);
			break;
		case P_SPAWN:
			spawnTask(packet);
			break;
		default:
			croak(int(node.nid)<< ": Invalid request.");
			freepk(packet);
	}
}

//-----------------------------------------------------------------------------
// sendNodesInfo() - Sends Info to node on all its peers.
//-----------------------------------------------------------------------------
void VirtualMachineServer::sendNodesInfo(Node& node, Packet* packet)
{
	// reuse packet
	packet = ::mkpacket(P_NODESINFO, 0, 0, node.nid, 0, packet);

	// pack the id and the number of nodes in the virtual machine
	packet->pack(node.nid);
	packet->pack(vnodescount);
	
	// pack info on all peers
	for (Tnid id = 0; id < vnodescount; ++id) {
		Node& peer = vnodes[id];		
		packet->pack(peer.nsa.sin_addr.s_addr);
		packet->pack(peer.nsa.sin_port);
	}

	// send
	queue(node, packet);
	croak("Node("<<int(node.nid)<<"): requested nodes-info.");
}

//-----------------------------------------------------------------------------
// updateStatus() - Status update message from node.
//-----------------------------------------------------------------------------
void VirtualMachineServer::updateStatus(Node& node, Packet* packet)
{
	// unpack status variables
	packet->unpack(node.ncpuload);
	packet->unpack(node.ntaskcount);

	croak("Node("<<int(node.nid)<<"): has "<<node.ntaskcount
	      <<" tasks running.");

	// release packet
	freepk(packet);
}

//-----------------------------------------------------------------------------
// spawnTask() - Routes a spawn message to node.
//-----------------------------------------------------------------------------
void VirtualMachineServer::spawnTask(Packet* packet)
{
	// find the least loaded node
	Tnid id = 0, low = 0;

	for (id = 0; id < vnodescount; ++id) {
		Node& node = vnodes[id];

		// if node is active
		if (node.nactive) {
			low = id;
			break;
		}
	}

	for (; id < vnodescount; ++id) {
		Node& node = vnodes[id];

		// if node is active
		if (node.nactive) {
			if (vnodes[low].ntaskcount > node.ntaskcount) 
				low = node.nid;
		}
	}
	// increment the task count
	vnodes[low].ntaskcount++;
	// send
	queue(vnodes[low], packet);

	croak("Node("<<int(packet->h_fnid())<<"): spawn routed to "
	      "node("<<int(low)<<")");
}

//-----------------------------------------------------------------------------
// sigint() - Entry point for SIGINT
//-----------------------------------------------------------------------------
void VirtualMachineServer::sigint()
{
	vloop = false;
}

//-----------------------------------------------------------------------------
// queue() - Queues packet for sending.
//-----------------------------------------------------------------------------
void VirtualMachineServer::queue(Node& node, Packet* packet)
{
	node.noutq.enq(packet);
	FD_SET(node.nfd, &vwfdset);
}

//-----------------------------------------------------------------------------
// sendPacket() - (Wrapper) Send packet to Node.
//-----------------------------------------------------------------------------
bool VirtualMachineServer::sendPacket(Node& node, Packet* packet)
{
	// try sending, if an error occurs the node is deactivated.
	try { return ::sendPacket(node.nfd, packet); } catch(Exception& e) {
		deactivate(node);
	}
	return(false);
}

//-----------------------------------------------------------------------------
// recvPacket() - (Wrapper) Recv Control Message from Node.
//-----------------------------------------------------------------------------
Packet* VirtualMachineServer::recvPacket(Node& node)
{	
	Packet* packet = allocpk();
	try {
		// try recving, if an error occurs the node is deactivated.
		if (::recvPacket(node.nfd, packet) == 0) {
			freepk(packet);
			return(0);
		} else 
			return(packet);

	} catch (...) {
		freepk(packet);
		deactivate(node);
		return(0);
	}
}

//-----------------------------------------------------------------------------
// VirtualMachineServer - Destructor. Closes all sockets.
//-----------------------------------------------------------------------------
VirtualMachineServer::~VirtualMachineServer()
{
	// close listening socket
	close(vfd);

	// close connections
	for (Tnid id = 0; id < vnodescount; ++id) {
		Node& node = vnodes[id];
		// if node is active
		if (node.nactive)
			// deactivate
			close(node.nfd);
	}

	// delete node cache
	delete[] vnodes;
}
