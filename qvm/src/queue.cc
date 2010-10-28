#include <queue.hh>

using namespace std;

//-----------------------------------------------------------------------------
// deq() - Remove Packet from a position Queue.
//-----------------------------------------------------------------------------
Packet* PacketQueue::deq(Packet* prev, Packet* p) 
{
	prev->next() = p->next();
	if (qhead == qtail)
		qhead = (qtail = NULL);
	else if (p == qhead) 
		qhead = p->next();
	else if (p == qtail)
		qtail = prev;
	--qsize;
	return p;
}

//-----------------------------------------------------------------------------
// deq() - Remove Packet from a position Queue.
//-----------------------------------------------------------------------------
Packet* PacketQueue::deq(size_t pos)
{
	Packet* p = peek();
	Packet* prev = p;
	while (p != NULL && pos --) {
		prev = p;
		p = p->next();
	}
	return deq(prev, p);
}

//-----------------------------------------------------------------------------
// deq() - Selectively Remove Packet from the Queue.
//-----------------------------------------------------------------------------
Packet* PacketQueue::deq(Tnid nid, Ttid tid, Ttag utag)
{
	Packet* p = peek();
	Packet* prev = p;

	while (p) {
		if (p->h_fnid() == nid && p->h_ftid() == tid &&
		    p->h_utag() == utag )
			return deq(prev, p);
		prev = p;
		p = p->next();
	}
	return(NULL);
}

//-----------------------------------------------------------------------------
// flushq() - Flush packets from Queue.
//-----------------------------------------------------------------------------
void PacketQueue::flush() 
{
	if (qhead)
	while (qhead != NULL) {
		qtail = qhead;
		qhead = qhead->next();
		delete qtail;
	}
	qhead = 0;
	qtail = 0;
}
