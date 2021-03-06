//-----------------------------------------------------------------------------
// status.hh - qstatus object definitions
//-----------------------------------------------------------------------------
#ifndef _QVMLIB_STATUS_HH_
#define _QVMLIB_STATUS_HH_

#include <qvm/types.hh>
#include <qvm/taskid.hh>
#include <qvm/packet.hh>

//-----------------------------------------------------------------------------
// qstatus - qvm status object. This is the base class of various events
// that occur in the qvm.
//-----------------------------------------------------------------------------
class qstatus
{
public:
	enum code { NONE, OK, FAILED, TIMEOUT, INVALID, QUIT  };
protected:
	qpacket*	stat_packet;
	qtaskid	 	stat_tid;
	qtag	 	stat_tag;
	qstatus::code	stat_code;
	std::string	stat_name;
public:
	qstatus(qpacket*);
	qtag tag()		const { return stat_tag;    }
	qpacket* packet()	const { return stat_packet; }
	qstatus::code status()	const { return stat_code;   }
	const qtaskid& id()	const { return stat_tid;    }
	const string& name()    const { return stat_name;   }
	~qstatus() { freepk(stat_packet); }
};

//-----------------------------------------------------------------------------
// qspawnstatus - SPAWN Reply event object. This Event is thrown whenever
// the nodeserver sends a packet containing a spawn status.
//-----------------------------------------------------------------------------
class qspawnstatus : public qstatus
{
public:
	qspawnstatus(qpacket* p);
};

//-----------------------------------------------------------------------------
// qtaskstatus - TASK Status Event object. This Event is thrown whenever
// the nodeserver sends a packet containing a task status.
//-----------------------------------------------------------------------------
class qtaskstatus : public qstatus
{
public:
	qtaskstatus(qpacket* p);
};

#endif
