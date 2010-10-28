//-----------------------------------------------------------------------------
// packetio.cc - Packet related io wrappers.
//-----------------------------------------------------------------------------
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <protocol.hh>
#include <packet.hh>
#include <error.hh>
#include <packetio.hh>

using namespace std;

//-----------------------------------------------------------------------------
// recvPacket() - recv packet from socket.
// - If the socket, by option, is blocking, it will wait for the whole packet 
//   and can be verified through a non-null return. 
// - In case that the socket is set to non blocking, if the required data is 
//   not available, a null is returned.
// - If the size of the packet is less than whats expected, or an EOF is 
//   encountered, it throws a 0.
// - If there is a socket error, an exception object is thrown.
//-----------------------------------------------------------------------------
Packet* recvPacket(int fd, Packet* packet)
{
again:	ssize_t size;

	// recv a packet of data. the MSG_WAITALL flag waits for the whole
	// packet to arrive.
	if ((size = ::recvfrom(fd, (void*)packet->buffer(), packet->size(), 
			   MSG_WAITALL, 0, 0)) < 0) {
		// syscall was interrupted, try again
		if (errno == EINTR)
			goto again;
		
		// if the socket is non-blocking, and the syscall would.
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return(0);

		// in all other cases, its an error,
		throw LibError();
	}

	// If  the size of the packet is less than what is expected its an
	// EOF or a broken connection.
	if ((unsigned)size < packet->size())
		throw int(0);
	else 
		return(packet);
}

//-----------------------------------------------------------------------------
// sendPacket() - Write Packet to Socket
// - If the socket, by option, is blocking, it will wait for the whole packet 
//   to be written and can be verified through a true return. 
// - In case that the socket is set to non blocking, if the packet as a whole
//   cannot be written, it returns a false.
//-----------------------------------------------------------------------------
bool sendPacket(int fd, Packet* p)
{
	size_t  offset = 0;
	ssize_t size   = 0;

again:	if ((size = ::write(fd, (const void*)(p->buffer()+offset), 
			    p->size() - offset)) <= 0) {
		if (size < 0) {
			// syscall interrupted, try again..
			if (errno == EINTR)
				goto again;
			// nonblocking socket would block..
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				// if no bytes have been read so far
				if (offset == 0) 
					return false;
		} else 
			throw LibError();
	}

	// accumulate
	if ((unsigned)size < p->size()) {
		offset += size;
		goto again;
	}

	return true;
}
