#include <string>
#include <assert.h>
#include <protocol.hh>
#include <qvm/types.hh>
#include <qvm/status.hh>

using namespace std;

//-----------------------------------------------------------------------------
// qstatus() - qstatus constructor
//-----------------------------------------------------------------------------
qstatus::qstatus(qpacket* p): stat_packet(p)
{
	stat_tid.nid = p->h_fnid();
	stat_tid.tid = p->h_ftid();
	stat_tag     = p->h_utag();
	stat_code    = qstatus::NONE;
}

//-----------------------------------------------------------------------------
// qspawnstatus() - qspawnstatus constructor
//-----------------------------------------------------------------------------
qspawnstatus::qspawnstatus(qpacket* p): qstatus(p) 
{
	p->unpack(stat_name);
	p->unpack(stat_tag);

	switch (p->h_itag()) {
	case P_SPAWNED: 
		stat_code = qstatus::OK;
		break;
	case P_SPAWN_FAILED: 
		stat_code = qstatus::FAILED; 
		break;
	}
}

//-----------------------------------------------------------------------------
// qtaskstatus() - qtaskstatus constructor
//-----------------------------------------------------------------------------
qtaskstatus::qtaskstatus(qpacket* p): qstatus(p) 
{
	switch (p->h_itag()) {
	case P_INVTASK: 
		stat_code = qstatus::INVALID; 
		break;
	case P_TASKQUIT: 
		stat_code = qstatus::QUIT; 
		break;
	case P_TIMEOUT: 
		stat_code = qstatus::TIMEOUT; 
		break;
	}
}
