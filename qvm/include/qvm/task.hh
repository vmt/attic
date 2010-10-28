#ifndef _QVMLIB_TASK_HH_
#define _QVMLIB_TASK_HH_

#include <protocol.hh>
#include <queue.hh>
#include <qvm/types.hh>
#include <qvm/packet.hh>
#include <qvm/task.hh>
#include <qvm/taskid.hh>
#include <qvm/status.hh>

class qeventhandler { public: virtual void entry(qstatus&); };


//-----------------------------------------------------------------------------
// qtask  - The Task Object in the Virtual Machine
//-----------------------------------------------------------------------------
class qtask
{
	friend class qstream;

private:
	int		tfd;
	qtaskid		tid;
	qtaskid		tpid;
	qpacketq	tinq;
	qeventhandler*	tspawneventhndlr;
	qeventhandler*	ttaskeventhndlr;
	qeventhandler*	tgenericeventhndlr;

	void callback(qpacket*);
	void read(qpacket*);
	void write(qpacket*);
	qpacket* read_nb(qpacket*);
	qpacket* write_nb(qpacket*);

protected:
	qpacket* recv(qtaskid&);
	qpacket* recv_nb(qtaskid&);
	void sendto(qpacket*, const qtaskid&);
	bool sendto_nb(qpacket*, const qtaskid&);
	qpacket* recvfrom(const qtaskid&, qtag);
	qpacket* recvfrom_nb(const qtaskid&, qtag);
public:	
	qtask(int, char**); ~qtask();
	const qtaskid& mytid() { return tid;  }
	const qtaskid& myptid(){ return tpid; }
	void spawn(const string&, qtag = 0);
	void setspawneventhandler(qeventhandler*);
	void settaskeventhandler(qeventhandler*);
	void setgenericeventhandler(qeventhandler*);
	void probe();
	void probe_nb();
	qstream* mkstream(const qtaskid&, qtag tag);
};

#endif
