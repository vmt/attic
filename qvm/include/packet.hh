//----------------------------------------------------------------------------
// packet.hh - Packets are the fundamental units of message transmission in
// the system.
//----------------------------------------------------------------------------
#ifndef _PACKET_HH_
#define _PACKET_HH_

#include <string>
#include <cstring>
#include <sys/types.h>
#include <protocol.hh>
#include <error.hh>

//-----------------------------------------------------------------------------
// Packet Macros - be careful adjusting values !
//-----------------------------------------------------------------------------
#define PK_ITAG(b)		(*(Ttag*)   ((b) + 0x0))
#define PK_UTAG(b)		(*(Ttag*)   ((b) + 0x1))
#define PK_TNID(b)		(*(Tnid*)   ((b) + 0x2))
#define PK_FNID(b)		(*(Tnid*)   ((b) + 0x3))
#define PK_TTID(b)		(*(Ttid*)   ((b) + 0x4))
#define PK_FTID(b)		(*(Ttid*)   ((b) + 0x6))
#define PK_EOP(b)		(*(Tpkptr*) ((b) + 0x8))
#define PK_HSIZE()		(0x0A)
#define PK_SIZE()		(0x80)

//-----------------------------------------------------------------------------
// Base definition for packets
//-----------------------------------------------------------------------------
class Packet
{
protected:
	Tbyte		pbuffer[PK_SIZE()];
	Tpkptr 		pbuffersize;
	Tpkptr		peop;
	Tpkptr 		ppackptr;
	Tpkptr 		punpackptr;
	Packet*		pnext;

public:
	enum error { eOverFlow };

	inline Packet(); 
	inline void flush ();
	inline void reset ();
	inline void reuse ();
	inline void fixeop();
	inline void lodeop();

	inline Tbyte* buffer();
	inline size_t size();
	inline Tpkptr& EOP();
	inline Packet*& next();
	inline Ttag& h_itag();
	inline Ttag& h_utag();
	inline Tnid& h_tnid();
	inline Ttid& h_ttid();
	inline Tnid& h_fnid();
	inline Ttid& h_ftid();

	inline void packData  (const void*, size_t s);
	inline void unpackData(void*, size_t s);
	inline size_t packDataAvail(const void*, size_t, size_t);
	inline size_t unpackDataAvail(void*, size_t, size_t);

	inline void pack(Tuchar);
	inline void pack(Tushort); 
	inline void pack(Tuint);   
	inline void pack(Tulong); 
	inline void pack(Tchar);
	inline void pack(Tshort); 
	inline void pack(Tint);    
	inline void pack(Tlong);   
	inline void pack(Tfloat);   
	inline void pack(Tdouble);   
	inline void pack(const Tchar*,   size_t);
	inline void pack(const Tshort*,  size_t);
	inline void pack(const Tint*,    size_t);
	inline void pack(const Tlong*,   size_t);
	inline void pack(const Tuchar*,  size_t);
	inline void pack(const Tushort*, size_t);
	inline void pack(const Tuint*,   size_t);
	inline void pack(const Tulong*,  size_t);
	inline void pack(const Tfloat*,  size_t);
	inline void pack(const Tdouble*, size_t);

	inline void unpack(Tchar&);
	inline void unpack(Tuchar&);
	inline void unpack(Tshort&);
	inline void unpack(Tushort&);
	inline void unpack(Tint&);
	inline void unpack(Tuint&);
	inline void unpack(Tlong&);
	inline void unpack(Tulong&);
	inline void unpack(Tfloat&);
	inline void unpack(Tdouble&);
	inline void unpack(Tchar*,   size_t);
	inline void unpack(Tshort*,  size_t);
	inline void unpack(Tint*,    size_t);
	inline void unpack(Tlong*,   size_t);
	inline void unpack(Tuchar*,  size_t);
	inline void unpack(Tushort*, size_t);
	inline void unpack(Tuint*,   size_t);
	inline void unpack(Tulong*,  size_t);
	inline void unpack(Tfloat*,  size_t);
	inline void unpack(Tdouble*, size_t);

	inline void pack(const Tstring&);
	inline void unpack(Tstring&);
};

extern Packet* allocpk();
extern void freepk(Packet*);
extern Packet* mkpacket(Ttag, Tnid, Ttid, Tnid, Ttid, Packet* = 0);

//-----------------------------------------------------------------------------
// Packet() - Constructor
//-----------------------------------------------------------------------------
Packet::Packet(): 
	pbuffersize(PK_SIZE()),
	peop(PK_SIZE()),
	ppackptr(PK_HSIZE()), 
	punpackptr(PK_HSIZE()),
	pnext(0) { }

//-----------------------------------------------------------------------------
// reset() - Reset unpacker pointer.
//-----------------------------------------------------------------------------
void Packet::reset()
{	
	punpackptr = PK_HSIZE();	
}

//-----------------------------------------------------------------------------
// flush() - Reset packer pointer.
//-----------------------------------------------------------------------------
void Packet::flush()
{	
	ppackptr = PK_HSIZE();	
	PK_EOP(pbuffer) = PK_SIZE();
	peop = PK_SIZE();	
}

//-----------------------------------------------------------------------------
// reuse() - Reuse packet.
//-----------------------------------------------------------------------------
void Packet::reuse()
{	
	flush();
	reset();
}

//-----------------------------------------------------------------------------
// fixeop() - Set end of packet to packer-pointer.
//-----------------------------------------------------------------------------
void Packet::fixeop()
{	
	PK_EOP(pbuffer) = ppackptr;
	peop = ppackptr;
}

//-----------------------------------------------------------------------------
// lodeop() - Load end of packet marker from header.
//-----------------------------------------------------------------------------
void Packet::lodeop()
{	
	peop = PK_EOP(pbuffer);		
}

//-----------------------------------------------------------------------------
// buffer() - Pointer to buffer
//-----------------------------------------------------------------------------
Tbyte* Packet::buffer()
{
	return pbuffer;
}

//-----------------------------------------------------------------------------
// size() - buffer size
//-----------------------------------------------------------------------------
size_t Packet::size()
{
	return pbuffersize;
}

//-----------------------------------------------------------------------------
// Next in linked list.
//-----------------------------------------------------------------------------
Packet*& Packet::next()
{
	return pnext;
}

//-----------------------------------------------------------------------------
// End Of Packet mark
//-----------------------------------------------------------------------------
Tpkptr& Packet::EOP()
{
	return peop;
}

//-----------------------------------------------------------------------------
// Header accesor/mutators
//-----------------------------------------------------------------------------
Ttag& Packet::h_itag() 
{ 	return PK_ITAG(pbuffer); 	}
Ttag& Packet::h_utag() 
{ 	return PK_UTAG(pbuffer); 	}
Tnid& Packet::h_tnid() 
{ 	return PK_TNID(pbuffer); 	}
Ttid& Packet::h_ttid() 
{ 	return PK_TTID(pbuffer); 	}
Tnid& Packet::h_fnid() 
{ 	return PK_FNID(pbuffer); 	}
Ttid& Packet::h_ftid() 
{ 	return PK_FTID(pbuffer); 	}

//-----------------------------------------------------------------------------
// packData() - Packer.
//-----------------------------------------------------------------------------
void Packet::packData(const void* src, size_t size)
{
	// if such an operation crosses the EOP, throw an exception
	if ((ppackptr + size) >= peop) 
		throw (Packet::eOverFlow);
	memcpy((void*)(pbuffer+ppackptr), src, size);
	ppackptr += size;
}

//-----------------------------------------------------------------------------
// unpackData() - Unpacker.
//-----------------------------------------------------------------------------
void Packet::unpackData(void* dst, size_t size)
{
	// if such an operation crosses the EOP, throw an exception
	if ((punpackptr + size) >= peop)
		throw (Packet::eOverFlow);
	memcpy(dst, (const void*)(pbuffer+punpackptr), size);
	punpackptr += size;
}

//-----------------------------------------------------------------------------
// packDataAvail() - Pack in available space
//-----------------------------------------------------------------------------
size_t Packet::packDataAvail(const void* src, size_t elsize, size_t n)
{
	// check if there is enough space for all the elements. If not
	// only the possible elements are written.
	if ((ppackptr + (elsize * n)) >= peop)
		n = (peop - ppackptr) / elsize;
	memcpy((void*)(pbuffer+ppackptr), src, elsize * n);
	ppackptr += (elsize * n);
	return n;
}

//-----------------------------------------------------------------------------
// unpackDataAvail() - Unpack from available space
//-----------------------------------------------------------------------------
size_t Packet::unpackDataAvail(void* dst, size_t elsize, size_t n)
{
	// check if there is enough space for all the elements. If not
	// only the possible number of elements are read.
	if ((punpackptr + (elsize * n)) >= peop)
		n = (peop - punpackptr) / elsize;
	memcpy(dst, (const void*)(pbuffer+punpackptr), elsize*n);
	punpackptr += elsize*n;
	return n;
}

//-----------------------------------------------------------------------------
// Packer Overloaded for different types.
//-----------------------------------------------------------------------------
void Packet::pack(Tchar  d)  
{ 	packData(&d, sizeof(Tchar));   }
void Packet::pack(Tshort d)  
{ 	packData(&d, sizeof(Tshort));  }
void Packet::pack(Tint  d)   
{ 	packData(&d, sizeof(Tint));    }
void Packet::pack(Tlong d)   
{ 	packData(&d, sizeof(Tlong));   }
void Packet::pack(Tuchar d)  
{ 	packData(&d, sizeof(Tuchar));  }
void Packet::pack(Tushort d) 
{ 	packData(&d, sizeof(Tushort)); }
void Packet::pack(Tuint  d)  
{ 	packData(&d, sizeof(Tuint));   }
void Packet::pack(Tulong d)  
{ 	packData(&d, sizeof(Tulong));  }
void Packet::pack(Tfloat d)  
{ 	packData(&d, sizeof(Tfloat));  }
void Packet::pack(Tdouble d)  
{ 	packData(&d, sizeof(Tdouble)); }

//-----------------------------------------------------------------------------
// Array Packer Overloaded for different types.
//-----------------------------------------------------------------------------
void Packet::pack(const Tchar*   d, size_t s) 
{ 	packData(d, sizeof(Tchar )  * s);	}
void Packet::pack(const Tshort*  d, size_t s) 
{ 	packData(d, sizeof(Tshort)  * s); 	}
void Packet::pack(const Tint*    d, size_t s) 
{ 	packData(d, sizeof(Tint)    * s); 	}
void Packet::pack(const Tlong*   d, size_t s) 
{ 	packData(d, sizeof(Tlong)   * s); 	}
void Packet::pack(const Tuchar*  d, size_t s) 
{ 	packData(d, sizeof(Tuchar ) * s); 	}
void Packet::pack(const Tushort* d, size_t s) 
{ 	packData(d, sizeof(Tushort) * s); 	}
void Packet::pack(const Tuint*   d, size_t s) 
{ 	packData(d, sizeof(Tuint)   * s);	}
void Packet::pack(const Tulong*  d, size_t s) 
{ 	packData(d, sizeof(Tulong)  * s); 	}
void Packet::pack(const Tfloat*  d, size_t s) 
{ 	packData(d, sizeof(Tfloat)  * s); 	}
void Packet::pack(const Tdouble* d, size_t s) 
{ 	packData(d, sizeof(Tdouble) * s); 	}

//-----------------------------------------------------------------------------
// Unpacker Overloaded for different types.
//-----------------------------------------------------------------------------
void Packet::unpack(Tchar&   d) 
{	unpackData(&d, sizeof(Tchar ));		}
void Packet::unpack(Tuchar&  d) 
{ 	unpackData(&d, sizeof(Tuchar ));	}
void Packet::unpack(Tshort&  d) 
{ 	unpackData(&d, sizeof(Tshort)); 	}
void Packet::unpack(Tushort& d) 
{ 	unpackData(&d, sizeof(Tushort)); 	}
void Packet::unpack(Tint&    d) 
{	unpackData(&d, sizeof(Tint)); 		}
void Packet::unpack(Tuint&   d) 
{ 	unpackData(&d, sizeof(Tuint)); 		}
void Packet::unpack(Tlong&   d) 
{ 	unpackData(&d, sizeof(Tlong));		}
void Packet::unpack(Tulong&  d) 
{ 	unpackData(&d, sizeof(Tulong)); 	}
void Packet::unpack(Tfloat&  d) 
{ 	unpackData(&d, sizeof(Tfloat)); 	}
void Packet::unpack(Tdouble&  d) 
{ 	unpackData(&d, sizeof(Tdouble)); 	}

//-----------------------------------------------------------------------------
// Array Unpacker Overloaded.
//-----------------------------------------------------------------------------
void Packet::unpack(Tchar*  d,  size_t s) 
{ 	unpackData(d, sizeof(Tchar )*s); 	}
void Packet::unpack(Tshort* d,  size_t s) 
{ 	unpackData(d, sizeof(Tshort)*s); 	}
void Packet::unpack(Tint* d,    size_t s) 
{ 	unpackData(d, sizeof(Tint)*s); 		}
void Packet::unpack(Tlong* d,   size_t s) 
{	unpackData(d, sizeof(Tlong)*s); 	}
void Packet::unpack(Tuchar* d,  size_t s) 
{	unpackData(d, sizeof(Tuchar )*s); 	}
void Packet::unpack(Tushort* d, size_t s) 
{ 	unpackData(d, sizeof(Tushort)*s); 	}
void Packet::unpack(Tuint* d,   size_t s) 
{ 	unpackData(d, sizeof(Tuint)*s); 	}
void Packet::unpack(Tulong* d,  size_t s) 
{ 	unpackData(d, sizeof(Tulong)*s); 	}
void Packet::unpack(Tfloat* d,  size_t s) 
{ 	unpackData(d, sizeof(Tfloat)*s); 	}
void Packet::unpack(Tdouble* d,  size_t s) 
{ 	unpackData(d, sizeof(Tdouble)*s); 	}


//-----------------------------------------------------------------------------
// String packet/unpacker.
//-----------------------------------------------------------------------------
void Packet::pack(const Tstring& s)
{	
	pack(Tushort(s.length()));     
	pack((Tchar*)s.c_str(), s.length()+1);
}
void Packet::unpack(Tstring& s)
{	Tushort length;
	unpack(length);
	Tbyte* str = new Tbyte[length+1];
	unpack((Tchar*)str, length+1);
	s = (const char*)str;
	delete[] str;
}

#endif
