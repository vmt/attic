//-----------------------------------------------------------------------------
// queue.hh - Packet queue
//-----------------------------------------------------------------------------
#ifndef _QUEUE_HH_
#define _QUEUE_HH_

#include <protocol.hh>
#include <packet.hh>

class PacketQueue
{
private:
	size_t  qsize;
	Packet* qhead;
	Packet* qtail;
public:
	inline PacketQueue();
	inline ~PacketQueue();
	inline void enq(Packet*);
	inline Packet* deq();
	inline Packet* peek();
	inline bool isEmpty();
	inline size_t size();

	Packet* deq(Packet*, Packet*);
	Packet* deq(size_t);
	Packet* deq(Tnid, Ttid, Ttag);
	void flush();
};

//-----------------------------------------------------------------------------
// PacketQueue() - PacketQueue Constructor/Destructor
//-----------------------------------------------------------------------------
PacketQueue::PacketQueue() : qhead(0), qtail(0) 
{ }
PacketQueue::~PacketQueue()
{
	flush();
}

//-----------------------------------------------------------------------------
// enq() - Add Packet to Queue.
//-----------------------------------------------------------------------------
void PacketQueue::enq(Packet* p) 
{
	if (qhead == NULL) {
		qhead = (qtail = p);
		qhead->next() = 0;
		qtail->next() = 0;
	} else {
		qtail->next() = p;
		qtail = qtail->next();
		qtail->next() = 0;
	}

	++qsize;
}

//-----------------------------------------------------------------------------
// deq() - Remove Packet from Queue.
//-----------------------------------------------------------------------------
Packet* PacketQueue::deq() 
{		
	Packet *p = qhead;
	if (p != NULL) {
		if (qhead == qtail)
			qtail = 0;
		qhead = qhead->next();
	}
	return p;
}

//-----------------------------------------------------------------------------
// isEmpty() - Queue empty ?
//-----------------------------------------------------------------------------
bool PacketQueue::isEmpty()
{ 	
	return (qhead == NULL) ? true : false;	
}

//-----------------------------------------------------------------------------
// size() - Queue size
//-----------------------------------------------------------------------------
size_t PacketQueue::size()
{ 	
	return qsize;
}

//-----------------------------------------------------------------------------
// peek into queue without removing
//-----------------------------------------------------------------------------
Packet* PacketQueue::peek()
{ 
	return qhead;			
}

#endif
