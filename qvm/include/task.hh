//-----------------------------------------------------------------------------
// task.hh - Task object definitions. 
//-----------------------------------------------------------------------------
#ifndef _TASK_HH_
#define _TASK_HH_

#include <protocol.hh>
#include <packet.hh>
#include <queue.hh>

//-----------------------------------------------------------------------------
// task.h - Task object. 
//-----------------------------------------------------------------------------
struct Task
{
	int		tfd;		
     	pid_t		tpid;		
	Ttid		tid;		
	Ttid		tptid;		
	Tnid		tpnid;		
	bool		talloced;	
	bool		tzombie;	
	PacketQueue 	toutq;
};

#endif
