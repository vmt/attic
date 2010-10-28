//-----------------------------------------------------------------------------
// (include/protocol.hh) Type defnitions for the system protocol.
//-----------------------------------------------------------------------------
#ifndef _PROTOCOL_HH_
#define _PROTOCOL_HH_

#include <string>

//-----------------------------------------------------------------------------
// Type Definitions Based on Number of Bits.
//-----------------------------------------------------------------------------
typedef unsigned char		Tuchar;
typedef unsigned char		Tubyte;
typedef unsigned short		Tushort;
typedef unsigned int		Tuint;
typedef char			Tchar;
typedef signed char		Tbyte;
typedef signed short		Tshort;
typedef signed int		Tint;
typedef unsigned long int	Tulong;
typedef signed long int		Tlong;
typedef std::string		Tstring;
typedef bool			Tbool;
typedef float			Tfloat;
typedef double			Tdouble;

//-----------------------------------------------------------------------------
// Packet Header Types
//-----------------------------------------------------------------------------
typedef Tshort			Tpknum;
typedef Tshort 			Ttid;
typedef Tubyte 			Tnid;
typedef Tubyte			Ttag;
typedef Tubyte			Tpkptr;

//-----------------------------------------------------------------------------
// Packet Control Tags
//-----------------------------------------------------------------------------
enum P_CODES
{
	P_NONE,
	P_ACK,
	P_NODESINFO,
	P_SPAWN,
	P_SPAWNED,
	P_SPAWN_FAILED,
	P_INVTASK,
	P_TASKQUIT,
	P_TIMEOUT,
	P_DATA,
	P_PING,
	P_ACTIVE,
	P_INACTIVE,
	P_STATUSUPDATE,
	P_AUTH
};

#endif
