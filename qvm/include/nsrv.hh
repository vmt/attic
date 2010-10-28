//-----------------------------------------------------------------------------
// nsrv.hh - VM Node Server Object Definitions (nsrv.cc)
//-----------------------------------------------------------------------------
#ifndef _NODESERVER_HH_
#define _NODESERVER_HH_

#include <protocol.hh>
#include <cpu.hh>
#include <bin.hh>
#include <task.hh>
#include <packet.hh>
#include <sock.hh>

//-----------------------------------------------------------------------------
// NODESERVER - Node server object.
//-----------------------------------------------------------------------------
class VirtualNodeServer : public VirtualNodeSocket
{
private:
	Task* 		vtasks;
	Ttid 		vtaskscount;
	Ttid		vtasksmax;
	bool		vloop;
	Bin		vbin;
protected:
	Task& allocTask();
	Task* getTask(Ttid);
	Ttid spawnTask(Tnid, Ttid, const std::string&);
	Ttid spawnTask(Packet*);
	void serveTask(Task&);
	void acquireZombieTask(Task&);
	void acceptNodePacket(Packet*);
	void acceptSrvPacket(Packet*);
	void queueTaskPacket(Packet*);
	void taskroute();

public:
	enum error { eNodeServer };
	
	VirtualNodeServer(const std::string&, const std::string&,
			  unsigned int, unsigned int, unsigned int);
	void work();
	void sigChild();
	void sigInt();
	void sigTimer() { }
	~VirtualNodeServer();
};

#ifdef WITH_X
#define EXEC(file, sock, nid) \
	execlp("xterm", "xterm", "-e", file, sock, nid, NULL);
#else
#define EXEC(file, sock, nid) \
	execlp(filename, filen, sock, nid, NULL);
#endif
#endif
