#ifndef _PACKETIO_HH_
#define _PACKETIO_HH_

#include <protocol.hh>
#include <packet.hh>

extern Packet* recvPacket(int, Packet*);
extern bool    sendPacket(int, Packet*);

#endif
