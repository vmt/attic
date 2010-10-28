#include <unistd.h>
#include <auth.hh>
#include <packetio.hh>

using namespace std;

//-----------------------------------------------------------------------------
// Authenticator() - constructor.
//-----------------------------------------------------------------------------
Authenticator::Authenticator(fd_set& fdset):
	afdset(fdset),
	ancount(0),
	anlisthead(0),
	anlisttail(0)
{

}

//-----------------------------------------------------------------------------
// add() - Queues connected socket for authentication.
//-----------------------------------------------------------------------------
void Authenticator::que(int sockd)
{
	if (! anlisthead) {
		anlisthead = new ANode(sockd);
		anlisttail = anlisthead;
	} else {
		anlisttail->nnext = new ANode(sockd);
		anlisttail = anlisttail->nnext;
	}

	++ancount;
}

//-----------------------------------------------------------------------------
// authenticate() - working loop
//-----------------------------------------------------------------------------
void Authenticator::process()
{
	// go through each authentication node
	for (ANode* anode = anlisthead,
		  * aprev = anlisthead; anode ; ) {

		// if the socket is readable
		if (FD_ISSET(anode->nfd, &afdset)) {
			Packet* packet = mkpacket(0, 0, 0, 0, 0);
			// recv packet
			try {
				if (::recvPacket(anode->nfd, packet) == 0)
					continue;

				// check if its an authentication packet, if
				// not close the connection.
				if (packet->h_itag() != P_AUTH)
					close(anode->nfd);
				else
					// callback authentication module
					authenticate(anode->nfd, packet);
			} catch(Exception& e) {
				close(anode->nfd);
			}

			// reduce the count
			--ancount;
			// clear bit in descriptor set
			FD_CLR(anode->nfd, &afdset);

			// delete the node
			aprev->nnext = anode->nnext;
			if (anlisthead == anlisttail) {
				anlisthead = (anlisttail = NULL);
				delete anode;
				anode = (aprev = 0);
				continue;
			} else if (anode == anlisthead) {
				anode = anode->nnext;
				anlisthead = anode;
				delete aprev;
				aprev = anode;
				continue;
			} else if (anode == anlisttail) {
				anlisttail = aprev;
				delete anode;
				continue;
			}
		} 
		else {
			// set the bit in descriptor set.
			FD_SET(anode->nfd, &afdset);
			// move to next node
			aprev = anode;
			anode = anode->nnext;
		}
	}
}

//-----------------------------------------------------------------------------
// ~Authenticator() - Destructor
//-----------------------------------------------------------------------------
Authenticator::~Authenticator()
{
	// go through each authentication node
	for (ANode* anode = anlisthead,
		  * aprev = anlisthead; anode ; ) {
		aprev = anode;
		anode = anode->nnext;
		delete aprev;
	}
}
