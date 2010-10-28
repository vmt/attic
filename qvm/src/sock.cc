//-----------------------------------------------------------------------------
// sock.cc - Virtual VirtualNode Socket Object Implementation (sock.hh)
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
#include <protocol.hh>
#include <error.hh>
#include <wrap.hh>
#include <packet.hh>
#include <queue.hh>
#include <packetio.hh>
#include <auth.hh>
#include <sock.hh>

using namespace std;

//-----------------------------------------------------------------------------
// VirtualNodeSocket() - VirtualNodeSocket Constructor.
//-----------------------------------------------------------------------------
VirtualNodeSocket::VirtualNodeSocket(const string& srvHostName, 
				     unsigned int srvPort, 
				     unsigned int myPort):
	Authenticator(sockrfdset),
	sockhostname(GetHostName()),
	socksrvhostname(srvHostName),
	sockport(myPort),
	socksrvport(srvPort),
	socknodes(0)
{
	// create tcp socket connecting to server
	if ((socksrvfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		throw LibError();

	// set socket options
	int y = 1;
	struct linger l = { 1, 0 };
	if (setsockopt(socksrvfd, SOL_SOCKET, 
		       SO_LINGER, &l, sizeof(l)) < 0)	 
		throw LibError();
	if (setsockopt(socksrvfd, SOL_SOCKET, 
		       SO_REUSEADDR, &y, sizeof(y)) < 0)
		throw LibError();
	if (setsockopt(socksrvfd, SOL_SOCKET, 
		       SO_KEEPALIVE, &y, sizeof(y)) < 0)
		throw LibError();

	// connect to server 
	struct sockaddr_in sa;
	memset((void*)&sa, 0, sizeof(sa));	
	GetHostByName(socksrvhostname, sa);
	sa.sin_family = AF_INET;
	sa.sin_port = htons(socksrvport);
	if (connect(socksrvfd, (const sockaddr*)&sa, sizeof(sa)) < 0)
		throw LibError();

	// create tcp listener socket
	if ((socknetfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		throw LibError();

	// set socket options
	if (setsockopt(socknetfd, SOL_SOCKET, 
		       SO_REUSEADDR, &y, sizeof(y)) < 0)
		throw LibError();
	if (setsockopt(socknetfd, SOL_SOCKET, 
		       SO_LINGER,    &l, sizeof(l)) < 0)	 
		throw LibError();
	if (setsockopt(socknetfd, SOL_SOCKET, 
		       SO_KEEPALIVE, &y, sizeof(y)) < 0)
		throw LibError();

	// bind to local address
	memset((void*)&socksa, 0, sizeof(socksa));	
	socksalen = sizeof(socksa);
	socksa.sin_family = AF_INET;
	socksa.sin_addr.s_addr = INADDR_ANY;
	socksa.sin_port = htons(sockport);
	if (bind(socknetfd, (const sockaddr*)&socksa, socksalen) < 0) 
		throw LibError();

	// listen, for connections
	if (listen(socknetfd, 10) < 0)
		throw LibError();

	// setup node cache
	setupNodeCache();
}

//-----------------------------------------------------------------------------
// setupNodeCache() - Setup info on nodes.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::setupNodeCache()
{
	croak("Setting up node-cache..");
	croak("Sending authentication-packet..");

	// create a authentication packet and send it to server.
	Packet* packet = mkpacket(P_AUTH, sockid, 0, 0, 0, allocpk());
	packet->pack(sockhostname);
	packet->pack(socksa.sin_port);
	if (! ::sendPacket(socksrvfd, packet)) {
		freepk(packet);
		throw Error(eSetup, "Failed Authentication");
	}

	croak("Sending request for nodes-info..");

	// create a request packet and send it to server.
	packet = mkpacket(P_NODESINFO, sockid, 0, 0, 0, allocpk());
	if (! ::sendPacket(socksrvfd, packet)) {
		freepk(packet);
		throw Error(eSetup, "Failed to setup node cache");
	}
	
	croak("Waiting for server-response..");

	// wait for the reply.
again:	if ((packet = ::recvPacket(socksrvfd, packet)) == 0) {
		throw Error(eSetup, "Failed to setup node cache");
		freepk(packet);
	}

	// verify packet header
	if (packet->h_itag() == P_NODESINFO) {
		// unpack cache details
		packet->unpack(sockid);
		packet->unpack(socknodescount);

		// allocate node cache
		socknodes = new VirtualNode[socknodescount];

		croak("Id = "<<int(sockid));
		croak("Cache-size = "<<int(socknodescount));

		// unpack info on all nodes
		for (unsigned i = 0; i < socknodescount; ++i) {
			socknodes[i].nid = i;
			socknodes[i].nsa.sin_family = AF_INET;
			packet->unpack(socknodes[i].nsa.sin_addr.s_addr);
			packet->unpack(socknodes[i].nsa.sin_port);
			initNode(i);
		}
	} else goto again;
}

//-----------------------------------------------------------------------------
// netroute() 
//-----------------------------------------------------------------------------
void VirtualNodeSocket::netroute()
{
	// if there is a connection waiting, accept it
	if (FD_ISSET(socknetfd, &sockrfdset))
		acceptConnection();
	else FD_SET(socknetfd, &sockrfdset);

	// process authentication pending connections
	if (Authenticator::pending())
		Authenticator::process();

	// go through each node checking for packets to be sent
	for (Tnid id = 0; id < socknodescount; ++id) {
		VirtualNode& node = socknodes[id];

		// if inactive, move on
		if (node.nactive == false)
			continue;

		// if there are queued packets
		if (! node.noutq.isEmpty()) {

			// if not connected
			if (! node.nconnected)
				connectNode(id);

			// if writeable, send packet
			if (FD_ISSET(node.nfd, &sockwfdset)) {
				// try sending
				Packet* packet = node.noutq.peek();
				if (sendPacket(node, packet))
					freepk(node.noutq.deq());

				// if empty, clear from desc set
				if (node.noutq.isEmpty()) 
					FD_CLR(node.nfd, &sockwfdset);

			} else  
				// set bit in descriptor set.
				FD_SET(node.nfd, &sockwfdset);
		}

		// if there are incoming packets, read
		if (node.nconnected && FD_ISSET(node.nfd, &sockrfdset)) {
			Packet* packet = recvPacket(node);
			if (packet)
				acceptNodePacket(packet);
		} else FD_SET(node.nfd, &sockrfdset);
	}

	// if there are packets queued for server
	if (! socksrvoutq.isEmpty()) {

		// if writeable, send packet
		if (FD_ISSET(socksrvfd, &sockwfdset)) {
			// try sending
			Packet* packet = socksrvoutq.peek();
			if (sendSrvPacket(packet))
				freepk(socksrvoutq.deq());

			// if empty, clear from desc set
			if (socksrvoutq.isEmpty())
				FD_CLR(socksrvfd, &sockwfdset);
		} else  
			// set bit in descriptor set.
			FD_SET(socksrvfd, &sockwfdset);
	}

	// if there are incoming packets from server, read
	if (FD_ISSET(socksrvfd, &sockrfdset)) {
		Packet* packet;
		if ((packet = recvSrvPacket()))
			acceptSrvPacket(packet);
	} else FD_SET(socksrvfd, &sockrfdset);
}

//-----------------------------------------------------------------------------
// initNode() - Initializes a local instace of a VirtualNode.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::initNode(Tnid id)
{
	VirtualNode& node = socknodes[id];

	// init attribs
	node.nfd        = 0 ;
	node.nid        = id;
	node.nrefcount  = 0 ;	
	node.nconnected = false;
	node.nactive    = true;
	node.nsalen    = sizeof(node.nsa);
}

//-----------------------------------------------------------------------------
// connectNode() - Connects to a node.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::connectNode(Tnid id)
{
	VirtualNode& node = socknodes[id];

	// create tcp socket connecting to server
	if ((node.nfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		throw LibError();

	// set socket options
	struct linger l = { 1, 0 };
	if (setsockopt(node.nfd, SOL_SOCKET, 
		       SO_LINGER,  &l, sizeof(l)) < 0)	 
		throw LibError();

	// try connect
	if (connect(node.nfd, (const sockaddr*)&node.nsa, node.nsalen) < 0) {
		if (errno == ECONNREFUSED ||  errno == ENETUNREACH  ||
		    errno == EHOSTUNREACH ||  errno == ETIMEDOUT) {
			deactivate(id);
			return;
		}
		else
			throw LibError();
	}
	node.nconnected = true;

	// send an authentication packet
	Packet* authpack = mkpacket(P_AUTH, sockid, 0, id, 0);
	authpack->pack(sockid);

	// send packet
	if (!sendPacket(node, authpack))
		disconnectNode(id);
	freepk(authpack);
}

//-----------------------------------------------------------------------------
// disconnectNode() - Disconnects from a node.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::disconnectNode(Tnid id)
{
	VirtualNode& node = socknodes[id];

	assert(node.nfd != 0);

	// clear socket from descriptor sets and close
	FD_CLR(node.nfd, &sockrfdset);
	FD_CLR(node.nfd, &sockwfdset);
	close(node.nfd);

	// reset flags
	node.nfd = 0;
	node.nconnected = false;
}

//-----------------------------------------------------------------------------
// acceptConnection() - Accepts connection from peer.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::acceptConnection()
{
	// accept connection req from peer.
	struct sockaddr_in sa;
	socklen_t salen = sizeof(sa);
	int sd;
again:	if ((sd = accept(socknetfd, (sockaddr*)(&sa), &salen)) < 0) {
		if (errno == EINTR)
			goto again;
		if (errno == ECONNABORTED)
			return;
		throw LibError();
	}

	// queue for authentication/identification
	Authenticator::que(sd);
}

//-----------------------------------------------------------------------------
// Auth::authenticate() - Authenticator
//-----------------------------------------------------------------------------
void VirtualNodeSocket::authenticate(int sd, Packet* packet)
{
	Tnid id;
	packet->unpack(id);

	if (id >= socknodescount) {
		close(sd);	
	} else {
		VirtualNode& node = socknodes[id];
		// if already connected, close
		if (node.nactive && node.nconnected)
			close(sd);
		else {
			// else activate
			activate(id);
			node.nfd        = sd  ;
			node.nconnected = true;

			croak("Node("<<int(id)<<"): Authenticated "
			      "connection.");
		}
	}
	// release packet
	freepk(packet);
}

//-----------------------------------------------------------------------------
// activate() - Activates a node.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::activate(Tnid id)
{
	VirtualNode& node = socknodes[id];
	node.nactive = true;
	croak("Node("<<int(id)<<"): Activated locally.");
}

//-----------------------------------------------------------------------------
// deactivate() - Deactivates a node.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::deactivate(Tnid id)
{
	VirtualNode& node = socknodes[id];
	node.nactive = false;
	if (node.nconnected)
		disconnectNode(id);
	croak("Node("<<int(id)<<"): Deactivated locally.");
}

//-----------------------------------------------------------------------------
// queueNodePacket() - Queues packet for sending to peer nodes.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::queueNodePacket(Packet* packet)
{
	assert(packet->h_tnid() < socknodescount);	
	assert(packet->h_tnid() != sockid);

	VirtualNode& node = socknodes[packet->h_tnid()];
	node.noutq.enq(packet);
	FD_SET(node.nfd, &sockwfdset);
}

//-----------------------------------------------------------------------------
// queueSrvPacket() - Queues packet for server.
//-----------------------------------------------------------------------------
void VirtualNodeSocket::queueSrvPacket(Packet* packet)
{	
	socksrvoutq.enq(packet);
	FD_SET(socksrvfd, &sockwfdset);
}

//-----------------------------------------------------------------------------
// sendPacket() - (Wrapper) Send packet to Node.
//-----------------------------------------------------------------------------
bool VirtualNodeSocket::sendPacket(VirtualNode& node, Packet* packet)
{
	// try sending, if an error occurs the node is deactivated.
	try { return ::sendPacket(node.nfd, packet); } catch(Exception& e) {
		deactivate(node.nid);
	}
	return(false);
}

//-----------------------------------------------------------------------------
// recvCtrlMsg() - (Wrapper) Recv packet from Node.
//-----------------------------------------------------------------------------
Packet* VirtualNodeSocket::recvPacket(VirtualNode& node)
{	
	Packet* packet = allocpk();
	try {
		// try recving, if an error occurs the node is deactivated.
		if (::recvPacket(node.nfd, packet) == 0) {
			deactivate(node.nid);
			freepk(packet);
			return(0);
		} else 
			return(packet);

	} catch (...) {
		freepk(packet);
		return(0);
	}
}

//-----------------------------------------------------------------------------
// sendSrvPacket() - (Wrapper) Send packet to Server.
//-----------------------------------------------------------------------------
bool VirtualNodeSocket::sendSrvPacket(Packet* packet)
{
	// try sending,
	try { return ::sendPacket(socksrvfd, packet); } catch(Exception& e) {
		exit(1);
	}
	return(false);
}

//-----------------------------------------------------------------------------
// recvSrvPacket() - (Wrapper) Recv packet from Node.
//-----------------------------------------------------------------------------
Packet* VirtualNodeSocket::recvSrvPacket()
{	
	Packet* packet = allocpk();
	try {
		// try recving, if an error occurs the node is deactivated.
		if (::recvPacket(socksrvfd, packet) == 0) {
			freepk(packet);
			return(0);
		} else 
			return(packet);

	} catch (...) {
		freepk(packet);
		exit(1);
		return(0);
	}
}

//-----------------------------------------------------------------------------
// ~VirtualNodeSocket() - destructor
//-----------------------------------------------------------------------------
VirtualNodeSocket::~VirtualNodeSocket()
{
	// go through each node
	for (Tnid id = 0; id < socknodescount; ++id) {
		VirtualNode& node = socknodes[id];
		// deactivate
		if (node.nactive && node.nconnected)
			deactivate(node.nid);
	}

	// delete node cache
	delete[] socknodes;

	// close listening socket
	close(socknetfd);
	// close server socket
	close(socksrvfd);
}
