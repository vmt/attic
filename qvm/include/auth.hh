//-----------------------------------------------------------------------------
// auth.hh - Authentication Object.
//-----------------------------------------------------------------------------
#ifndef _AUTH_HH_
#define _AUTH_HH_

#include <unistd.h>
#include <protocol.hh>
#include <packet.hh>

//-----------------------------------------------------------------------------
// Authenticator - Authentication Object Definition.
//-----------------------------------------------------------------------------
class Authenticator
{
private:
	struct ANode	{
		int	nfd;
		Tnid	nid;
		ANode*	nnext;
		ANode(int sockd): nfd(sockd), nid(0), nnext(0) {}
	};

	fd_set& afdset;
	size_t	ancount;
	ANode*	anlisthead;
	ANode*	anlisttail;
public:
	Authenticator(fd_set&);
	void que(int);
	void process();
	bool pending() { return bool(ancount); }
	virtual void authenticate(int, Packet*) = 0;
	virtual ~Authenticator();
};

#endif
