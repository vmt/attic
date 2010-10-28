//-----------------------------------------------------------------------------
// server.hh - Virtual Machine Server.
//-----------------------------------------------------------------------------
#ifndef _SERVER_HH_
#define _SERVER_HH_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <queue.hh>
#include <error.hh>
#include <packetio.hh>
#include <auth.hh>

//-----------------------------------------------------------------------------
// Node
//-----------------------------------------------------------------------------
struct Node
{			
	int			nfd;
	Tnid			nid;
	Tubyte			ncpuload;
	Tushort			ntaskcount;
	bool			nactive;
	struct sockaddr_in	nsa;
	socklen_t		nsalen;
	std::string		nhostname;
	Tushort			nport;
	PacketQueue		noutq;
};

//-----------------------------------------------------------------------------
// The Virtual Machine Server
//-----------------------------------------------------------------------------
class VirtualMachineServer : public Authenticator
{
private:
	int			vfd;
	Node*			vnodes;
	Tubyte			vnodescount;
	unsigned int		vport;
	std::string 		vconffilename;
	std::string		vname;
	fd_set			vrfdset;
	fd_set			vwfdset;
	bool			vloop;
	PacketQueue		vpacketq;

public:
	enum { eXmlConf };

	VirtualMachineServer(const std::string&, unsigned int);
	void setupConf();
	void initNode(Tnid, const std::string&, Tushort);
	void sendNodesInfo(Node&, Packet*);
	void work();
	void netroute();
	void acceptConnection();
	void acceptPacket(Packet*, Node&);
	void activate(Node&, int);
	void deactivate(Node&);
	void authenticate(int, Packet*);
	void spawnTask(Packet*);
	void updateStatus(Node&, Packet*);
	bool sendPacket(Node&, Packet*);
	Packet* recvPacket(Node& node);
	void sigint();
	void queue(Node&, Packet*);
	~VirtualMachineServer();
};

#endif
