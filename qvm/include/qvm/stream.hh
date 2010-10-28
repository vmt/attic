//-----------------------------------------------------------------------------
// stream.hh - data stream management
//-----------------------------------------------------------------------------
#ifndef _QVMLIB_STREAM_HH_
#define _QVMLIB_STREAM_HH_

#include <protocol.hh>
#include <queue.hh>
#include <unistd.h>
#include <error.hh>
#include <qvm/types.hh>
#include <qvm/packet.hh>
#include <qvm/task.hh>
#include <qvm/status.hh>

//-----------------------------------------------------------------------------
// qstream - data stream object
//-----------------------------------------------------------------------------
class qstream
{
private:
	qtaskid		speerid;
	qtag		stag;
	qtask*		sTask;
	qpacket*	sinpacket;
	qpacket*	soutpacket;
	qpacketq	soutq;
	bool		sstrgetop;
	qushort		sstrgetlen;
	qushort		sstrgetdone;
	qchar*		sstrgetbuffer;

	inline void despatch();
public:
	inline qstream(qtask*, const qtaskid&, qtag);
	inline ~qstream();
	inline void flush();
	inline void flush_nb();
	inline void load();
	inline bool load_nb();
	inline void get(qchar*, size_t, size_t);
	inline size_t get_nb(qchar*, size_t, size_t);
	inline void get(qchar&);
	inline void get(quchar&);
	inline void get(qshort&);
	inline void get(qushort&);
	inline void get(qint&);
	inline void get(quint&);
	inline void get(qlong&);
	inline void get(qulong&);
	inline bool get_nb(qchar&);
	inline bool get_nb(quchar&);
	inline bool get_nb(qshort&);
	inline bool get_nb(qushort&);
	inline bool get_nb(qint&);
	inline bool get_nb(quint&);
	inline bool get_nb(qlong&);
	inline bool get_nb(qulong&);
	inline void get(qchar*, size_t);
	inline void get(quchar*, size_t);
	inline void get(qshort*, size_t);
	inline void get(qushort*, size_t);
	inline void get(qint*, size_t);
	inline void get(quint*, size_t);
	inline void get(qlong*, size_t);
	inline void get(qulong*, size_t);
	inline size_t get_nb(qchar*, size_t);
	inline size_t get_nb(quchar*, size_t);
	inline size_t get_nb(qshort*, size_t);
	inline size_t get_nb(qushort*, size_t);
	inline size_t get_nb(qint*, size_t);
	inline size_t get_nb(quint*, size_t);
	inline size_t get_nb(qlong*, size_t);
	inline size_t get_nb(qulong*, size_t);
	inline void get(qstring&);
	inline bool get_nb(qstring&);
	inline qstream& operator >> (qchar&);
	inline qstream& operator >> (quchar&);
	inline qstream& operator >> (qshort&);
	inline qstream& operator >> (qushort&);
	inline qstream& operator >> (qint&);
	inline qstream& operator >> (quint&);
	inline qstream& operator >> (qlong&);
	inline qstream& operator >> (qulong&);
	inline qstream& operator >> (qstring&);

	inline void put(const qchar*, size_t, size_t);
	inline size_t put_nb(const qchar*, size_t, size_t);
	inline void put(const qchar&);
	inline void put(const quchar&);
	inline void put(const qshort&);
	inline void put(const qushort&);
	inline void put(const qint&);
	inline void put(const quint&);
	inline void put(const qlong&);
	inline void put(const qulong&);
	inline void put_nb(const qchar&);
	inline void put_nb(const quchar&);
	inline void put_nb(const qshort&);
	inline void put_nb(const qushort&);
	inline void put_nb(const qint&);
	inline void put_nb(const quint&);
	inline void put_nb(const qlong&);
	inline void put_nb(const qulong&);
	inline void put(const qchar*, size_t);
	inline void put(const quchar*, size_t);
	inline void put(const qshort*, size_t);
	inline void put(const qushort*, size_t);
	inline void put(const qint*, size_t);
	inline void put(const quint*, size_t);
	inline void put(const qlong*, size_t);
	inline void put(const qulong*, size_t);
	inline void put_nb(const qchar*, size_t);
	inline void put_nb(const quchar*, size_t);
	inline void put_nb(const qshort*, size_t);
	inline void put_nb(const qushort*, size_t);
	inline void put_nb(const qint*, size_t);
	inline void put_nb(const quint*, size_t);
	inline void put_nb(const qlong*, size_t);
	inline void put_nb(const qulong*, size_t);
	inline void put(const qstring&);
	inline void put_nb(const qstring&);
	inline qstream& operator << (const qchar&);
	inline qstream& operator << (const quchar&);
	inline qstream& operator << (const qshort&);
	inline qstream& operator << (const qushort&);
	inline qstream& operator << (const qint&);
	inline qstream& operator << (const quint&);
	inline qstream& operator << (const qlong&);
	inline qstream& operator << (const qulong&);
	inline qstream& operator << (const qstring&);
};

//-----------------------------------------------------------------------------
// qstream - stream constructor
//-----------------------------------------------------------------------------
qstream::qstream(qtask* task, const qtaskid& peerid, qtag stag):
	speerid(peerid),
	stag(stag),
	sTask(task),
	sinpacket(NULL),
	soutpacket(NULL)
{
	despatch();
	load_nb();	
}

//-----------------------------------------------------------------------------
// flush(_nb)() - blocking and non-blocking stream flush routines.
//-----------------------------------------------------------------------------
void qstream::flush()
{
	despatch();
	for (qpacket* p = soutq.deq(); p ; p = soutq.deq()) {
		sTask->sendto(p, speerid);
		freepk(p);
	}
}

void qstream::flush_nb()
{
	despatch();
	while (sTask->sendto_nb(soutq.peek(), speerid))
		freepk(soutq.deq());
}

//-----------------------------------------------------------------------------
// despatch() - queues the current out-bound packet, and allocate a new one.
//-----------------------------------------------------------------------------
void qstream::despatch()
{
	if (soutpacket) {
		soutpacket->h_utag() = stag;
		soutpacket->fixeop();
		soutq.enq(soutpacket);
		soutpacket = NULL;
	}
	soutpacket = allocpk();
	soutpacket->flush();
	soutpacket->reset();
}

//-----------------------------------------------------------------------------
// load(_nb)() - blocking and non-blocking stream loading routines.
//-----------------------------------------------------------------------------
void qstream::load()
{
	if (sinpacket) {
		freepk(sinpacket);
		sinpacket = 0;
	}
	sinpacket = sTask->recvfrom(speerid, stag);
	sinpacket->lodeop();
}

bool qstream::load_nb()
{
	if (sinpacket) {
		freepk(sinpacket);
		sinpacket = NULL;
	}
	if ((sinpacket = sTask->recvfrom_nb(speerid, stag))) {
		sinpacket->lodeop();
		return true;
	} else return false;
}

//-----------------------------------------------------------------------------
// get(_nb)() - blocking and non-blocking stream input routines.
//-----------------------------------------------------------------------------
void qstream::get(qchar* dst, size_t elsize, size_t n)
{
	if (sinpacket == NULL)
		load();

	size_t avail = 0;
	// attempt to read the whole data.
	if ((avail = sinpacket->unpackDataAvail(dst, elsize, n)) < n) {
		// in case the packet is full, load the next packet
		load();
		get(dst + (elsize*avail), elsize, n - avail);
	}
}

size_t qstream::get_nb(qchar* dst, size_t elsize, size_t n)
{
	if (sinpacket == NULL) {
		if (load_nb() == false)
			return 0;
	}
	size_t avail = 0;
	// attempt to read the whole data.
	if ((avail = sinpacket->unpackDataAvail(dst, elsize, n)) < n) {

		croak("availed = "<<avail);

		// in case the packet is full, load the next packet
		if (load_nb())
			return avail + get_nb(dst + (elsize*avail), 
					      elsize, n - avail);
	}
	return avail;
}

void qstream::get(qchar& c)
{	get((qchar*)&c, sizeof(qchar), 1);	}
void qstream::get(quchar& c)
{	get((qchar*)&c, sizeof(quchar), 1);	}
void qstream::get(qshort& c)
{	get((qchar*)&c, sizeof(qshort), 1);	}
void qstream::get(qushort& c)
{	get((qchar*)&c, sizeof(qushort), 1);	}
void qstream::get(qint& c)
{	get((qchar*)&c, sizeof(qint), 1);	}
void qstream::get(quint& c)
{	get((qchar*)&c, sizeof(quint), 1);	}
void qstream::get(qlong& c)
{	get((qchar*)&c, sizeof(qlong), 1);	}
void qstream::get(qulong& c)
{	get((qchar*)&c, sizeof(qlong), 1);	}
void qstream::get(qchar* c, size_t n)
{	get((qchar*)c, sizeof(qchar), n);	}
void qstream::get(quchar* c, size_t n)
{	get((qchar*)c, sizeof(quchar), n);	}
void qstream::get(qshort* c, size_t n)
{	get((qchar*)c, sizeof(qshort), n);	}
void qstream::get(qushort* c, size_t n)
{	get((qchar*)c, sizeof(qushort), n);	}
void qstream::get(qint* c, size_t n)
{	get((qchar*)c, sizeof(qint), n);	}
void qstream::get(quint* c, size_t n)
{	get((qchar*)c, sizeof(quint), n);	}
void qstream::get(qlong* c, size_t n)
{	get((qchar*)c, sizeof(qlong), n);	}
void qstream::get(qulong* c, size_t n)
{	get((qchar*)c, sizeof(qlong), n);	}
bool qstream::get_nb(qchar& c)
{	return get_nb((qchar*)&c, sizeof(qchar), 1);	}
bool qstream::get_nb(quchar& c)
{	return get_nb((qchar*)&c, sizeof(quchar), 1);	}
bool qstream::get_nb(qshort& c)
{	return get_nb((qchar*)&c, sizeof(qshort), 1);	}
bool qstream::get_nb(qushort& c)
{	return get_nb((qchar*)&c, sizeof(qushort), 1);	}
bool qstream::get_nb(qint& c)
{	return get_nb((qchar*)&c, sizeof(qint), 1);	}
bool qstream::get_nb(quint& c)
{	return get_nb((qchar*)&c, sizeof(quint), 1);	}
bool qstream::get_nb(qlong& c)
{	return get_nb((qchar*)&c, sizeof(qlong), 1);	}
bool qstream::get_nb(qulong& c)
{	return get_nb((qchar*)&c, sizeof(qlong), 1);	}
size_t qstream::get_nb(qchar* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qchar), n);	}
size_t qstream::get_nb(quchar* c, size_t n)
{	return get_nb((qchar*)c, sizeof(quchar), n);	}
size_t qstream::get_nb(qshort* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qshort), n);	}
size_t qstream::get_nb(qushort* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qushort), n);	}
size_t qstream::get_nb(qint* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qint), n);	}
size_t qstream::get_nb(quint* c, size_t n)
{	return get_nb((qchar*)c, sizeof(quint), n);	}
size_t qstream::get_nb(qlong* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qlong), n);	}
size_t qstream::get_nb(qulong* c, size_t n)
{	return get_nb((qchar*)c, sizeof(qlong), n);	}

void qstream::get(qstring& s)
{	qushort length;
	get(length);
	qchar* str = new qchar[length+1];
	get((qchar*)str, length+1);
	s = (const qchar*)str;
	delete[] str;
}
bool qstream::get_nb(qstring& s)
{	
	if (! sstrgetop) {
		if (get_nb(sstrgetlen)) {
			sstrgetbuffer = 
				new qchar[sstrgetlen+1];
			sstrgetop = true;
			sstrgetdone = 0;
			sstrgetlen++;
		} else return false;
	}
	if (sstrgetop) {
		sstrgetdone += get_nb((qchar*) sstrgetbuffer[sstrgetdone], 
				      sstrgetlen - sstrgetdone);
		if (sstrgetdone == sstrgetlen) {
			sstrgetop = false;
			s = (const char*)sstrgetbuffer;
			delete[] sstrgetbuffer;
			return true;
		} 
		return false;
	}
	return false;
}

qstream& qstream::operator >> (qchar& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (quchar& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qshort& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qushort& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qint& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (quint& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qlong& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qulong& c)
{	get(c); return *this;		}
qstream& qstream::operator >> (qstring& c)
{	get(c); return *this;		}

//-----------------------------------------------------------------------------
// put(_nb)() - blocking and non-blocking stream output routines.
//-----------------------------------------------------------------------------
void qstream::put(const qchar* src, size_t elsize, size_t n)
{
	size_t avail = 0;
	// attempt to write the whole data.
	if ((avail = soutpacket->packDataAvail(src, elsize, n)) < n) {
		// in case the packet is full, flush the packet which
		// creates a new buffer.
		flush();
		put(src + (elsize*avail), elsize, n - avail);
	}
}

size_t qstream::put_nb(const qchar* src, size_t elsize, size_t n)
{
	size_t avail = 0;
	// attempt to write the whole data.
	if ((avail = soutpacket->packDataAvail(src, elsize, n)) < n) {
		// in case the packet is full, flush the packet which
		// creates a new buffer.
		flush_nb();
		return avail + put_nb(src + (elsize*avail), 
				      elsize, n - avail);
	}
	return avail;
}

void qstream::put(const qchar& c)
{	put((const qchar*)&c, sizeof(qchar), 1);	}
void qstream::put(const quchar& c)
{	put((const qchar*)&c, sizeof(quchar), 1);	}
void qstream::put(const qshort& c)
{	put((const qchar*)&c, sizeof(qshort), 1);	}
void qstream::put(const qushort& c)
{	put((const qchar*)&c, sizeof(qushort), 1);	}
void qstream::put(const qint& c)
{	put((const qchar*)&c, sizeof(qint), 1);		}
void qstream::put(const quint& c)
{	put((const qchar*)&c, sizeof(quint), 1);	}
void qstream::put(const qlong& c)
{	put((const qchar*)&c, sizeof(qlong), 1);	}
void qstream::put(const qulong& c)
{	put((const qchar*)&c, sizeof(qlong), 1);	}
void qstream::put(const qchar* c, size_t n)
{	put((const qchar*)c, sizeof(qchar), n);		}
void qstream::put(const quchar* c, size_t n)
{	put((const qchar*)c, sizeof(quchar), n);	}
void qstream::put(const qshort* c, size_t n)
{	put((const qchar*)c, sizeof(qshort), n);	}
void qstream::put(const qushort* c, size_t n)
{	put((const qchar*)c, sizeof(qushort), n);	}
void qstream::put(const qint* c, size_t n)
{	put((const qchar*)c, sizeof(qint), n);		}
void qstream::put(const quint* c, size_t n)
{	put((const qchar*)c, sizeof(quint), n);		}
void qstream::put(const qlong* c, size_t n)
{	put((const qchar*)c, sizeof(qlong), n);		}
void qstream::put(const qulong* c, size_t n)	
{	put((const qchar*)c, sizeof(qlong), n);		}
void qstream::put_nb(const qchar& c)
{	put_nb((const qchar*)&c, sizeof(qchar), 1);	}
void qstream::put_nb(const quchar& c)
{	put_nb((const qchar*)&c, sizeof(quchar), 1);	}
void qstream::put_nb(const qshort& c)
{	put_nb((const qchar*)&c, sizeof(qshort), 1);	}
void qstream::put_nb(const qushort& c)
{	put_nb((const qchar*)&c, sizeof(qushort), 1);	}
void qstream::put_nb(const qint& c)
{	put_nb((const qchar*)&c, sizeof(qint), 1);	}
void qstream::put_nb(const quint& c)
{	put_nb((const qchar*)&c, sizeof(quint), 1);	}
void qstream::put_nb(const qlong& c)
{	put_nb((const qchar*)&c, sizeof(qlong), 1);	}
void qstream::put_nb(const qulong& c)
{	put_nb((const qchar*)&c, sizeof(qlong), 1);	}
void qstream::put_nb(const qchar* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qchar), n);	}
void qstream::put_nb(const quchar* c, size_t n)
{	put_nb((const qchar*)c, sizeof(quchar), n);	}
void qstream::put_nb(const qshort* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qshort), n);	}
void qstream::put_nb(const qushort* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qushort), n);	}
void qstream::put_nb(const qint* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qint), n);	}
void qstream::put_nb(const quint* c, size_t n)
{	put_nb((const qchar*)c, sizeof(quint), n);	}
void qstream::put_nb(const qlong* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qlong), n);	}
void qstream::put_nb(const qulong* c, size_t n)
{	put_nb((const qchar*)c, sizeof(qlong), n);	}

void qstream::put(const qstring& s)
{	
	put(qushort(s.length()));
	put((const qchar*)s.c_str(), s.length() + 1);
}
void qstream::put_nb(const qstring& s)
{	
	put_nb(qushort(s.length()));
	put_nb((const qchar*)s.c_str(), s.length() + 1);
}

qstream& qstream::operator << (const qchar& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const quchar& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qshort& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qushort& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qint& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const quint& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qlong& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qulong& c)
{	put(c); return *this;		}
qstream& qstream::operator << (const qstring& c)
{	put(c); return *this;		}

//-----------------------------------------------------------------------------
// flush stream before ending.
//-----------------------------------------------------------------------------
qstream::~qstream()
{
	flush();
}

#endif
