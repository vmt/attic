//-----------------------------------------------------------------------------
// sock.hh - Virtual Node Socket.
//-----------------------------------------------------------------------------
#ifndef _SOCKET_HH_
#define _SOCKET_HH_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <protocol.hh>
#include <packet.hh>
#include <queue.hh>
#include <auth.hh>
#include <node.hh>

//-----------------------------------------------------------------------------
// VirtualNodeSocket - Virtual Machine Node Socket
//-----------------------------------------------------------------------------
class VirtualNodeSocket :public Authenticator
{
protected:

	int			socknetfd;
	int			socksrvfd;
	fd_set			sockrfdset;
	fd_set			sockwfdset;
	Tnid			sockid;
	std::string		sockhostname;
	std::string		socksrvhostname;
	Tushort			sockport;
	Tushort			socksrvport;
	VirtualNode*		socknodes;
	Tnid 			socknodescount;
	PacketQueue		socksrvoutq;
	sockaddr_in		socksa;
	socklen_t		socksalen;

public:
	enum error { eSetup };

	VirtualNodeSocket(const std::string&, unsigned int, unsigned int);
	void setupNodeCache();
	void netroute();
	void initNode(Tnid);
	void activate(Tnid);
	void deactivate(Tnid);
	void connectNode(Tnid);
	void disconnectNode(Tnid);
	void acceptConnection();
	void authenticate(int, Packet*);

	void queueNodePacket(Packet*);
	void queueSrvPacket(Packet*);
	bool sendPacket(VirtualNode&, Packet*);
	Packet* recvPacket(VirtualNode&);
	virtual ~VirtualNodeSocket();
	virtual void acceptNodePacket(Packet*) = 0;
	virtual void acceptSrvPacket (Packet*) = 0;
	bool sendSrvPacket(Packet* packet);
	Packet* recvSrvPacket();
};

#endif
