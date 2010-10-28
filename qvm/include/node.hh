//-----------------------------------------------------------------------------
// node.h - Virtual Machine Node.
//-----------------------------------------------------------------------------
#ifndef _NODE_HH_
#define _NODE_HH_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <protocol.hh>
#include <queue.hh>
#include <packet.hh>

//-----------------------------------------------------------------------------
// Node - Virtual Machine Node Object Definition.
//-----------------------------------------------------------------------------
struct VirtualNode
{			
	int			nfd;
	Tnid			nid;
	bool			nactive;
	bool			nconnected;
	size_t			nrefcount;
	std::string		nHostName;
	Tushort			nPort;
	struct sockaddr_in	nsa;
	socklen_t		nsalen;
	PacketQueue		noutq;
	PacketQueue		ninq;	
};

#endif
