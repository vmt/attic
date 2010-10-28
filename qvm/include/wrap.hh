//-----------------------------------------------------------------------------
// wrap.h - Wrappers for libc (wrap.cc)
//-----------------------------------------------------------------------------
#ifndef _WRAP_H_
#define _WRAP_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

extern void GetHostByName(const std::string&, struct sockaddr_in&);
extern std::string GetIP(struct sockaddr_in&);
extern std::string GetHostName();

inline bool operator == (const struct sockaddr_in& s1, 
			 const struct sockaddr_in& s2)
{
	if (s1.sin_family	== s2.sin_family  &&
	    s1.sin_addr.s_addr	== s2.sin_addr.s_addr &&
	    s1.sin_port		== s2.sin_port) {
		return true;
	} else {
		return false;
	}
}

#endif
