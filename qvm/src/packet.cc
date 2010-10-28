#include <packet.hh>
#include <queue.hh>

using namespace std;

// global packet heap.
static PacketQueue GlobalPacketHeap;

//-----------------------------------------------------------------------------
// allocpk() - allocate packet
//-----------------------------------------------------------------------------
extern Packet* allocpk()
{
	// if a previously allocated but free'd packet is available
	// return that
	if (! GlobalPacketHeap.isEmpty())
		return GlobalPacketHeap.deq();
	// else return a new packet
	else return new Packet;
}

//-----------------------------------------------------------------------------
// freepk() - free packet. All allocated packets (must) return here.
//-----------------------------------------------------------------------------
extern void freepk(Packet* packet)
{
	// keep the packet reuseable.
	packet->reuse();
	GlobalPacketHeap.enq(packet);
}

//-----------------------------------------------------------------------------
// mkpacket() - Makes new packet, allocates packet and initializes header.
//-----------------------------------------------------------------------------
extern Packet* mkpacket(Ttag tag, Tnid fnid, Ttid ftid, Tnid tnid, 
			       Ttid ttid, Packet* packet)
{
	if (packet == 0) 
		packet = allocpk();
	packet->h_itag() = tag ;
	packet->h_fnid() = fnid;
	packet->h_ftid() = ftid;
	packet->h_tnid() = tnid;
	packet->h_ttid() = ttid;

	return packet;
}
