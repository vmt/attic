//-----------------------------------------------------------------------------
// wrap.cc Wrappers for libc (wrap.h)
//-----------------------------------------------------------------------------
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <error.hh>
#include <wrap.hh>

using namespace std;

static char BUFFER[128];

//-----------------------------------------------------------------------------
// GetHostByName(hostname, socket-structure) - Gets the address associated 
// with a host name.
//-----------------------------------------------------------------------------
extern void GetHostByName(const string& hostname, struct sockaddr_in& sa)
{
	struct hostent* hp;
retry:
	if ((hp = gethostbyname(hostname.c_str())) == NULL) {
		if (h_errno == TRY_AGAIN)
			goto retry;
		throw Error(h_errno, hstrerror(h_errno));
	}
	if (hp->h_addrtype != AF_INET) 
		throw Error(0, "Unknown Address Family.");
	memcpy(&(sa.sin_addr), hp->h_addr_list[0], sizeof(sa.sin_addr));
}

//-----------------------------------------------------------------------------
// GetIP(socket-structure) - Returns the ip address in string form, from the 
// socket address structure.
//-----------------------------------------------------------------------------
extern string GetIP(struct sockaddr_in& sa)
{
	if (inet_ntop(AF_INET, (const void*)&(sa.sin_addr), BUFFER, 
			sizeof(BUFFER)) == NULL)
		throw LibError();
	return string(BUFFER);
}

//-----------------------------------------------------------------------------
// GetHostName() - Returns the host name of the system.
//-----------------------------------------------------------------------------
extern string GetHostName()
{
	if (gethostname(BUFFER, sizeof(BUFFER)) < 0) 
		throw LibError();
	return string(BUFFER);
}
