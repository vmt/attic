//-----------------------------------------------------------------------------
// taskid.hh - Task Identifier object.
//-----------------------------------------------------------------------------
#ifndef _QVM_TASKID_H_
#define _QVM_TASKID_H_

#include <iostream>
#include <protocol.hh>
#include <qvm/types.hh>

struct qtaskid
{
	qnid	nid;
	qtid	tid;

	inline qtaskid();
	inline qtaskid& operator = (const qtaskid&);
	inline bool operator ==(const qtaskid&);
	inline bool operator !=(const qtaskid&);
};

//-----------------------------------------------------------------------------
// constructor.
//-----------------------------------------------------------------------------
qtaskid::qtaskid() : nid(0), tid(0) 
{ 

}

//-----------------------------------------------------------------------------
// operators overloaded
//-----------------------------------------------------------------------------
qtaskid& qtaskid::operator = (const qtaskid& i) {
	nid = i.nid; 
	tid = i.tid;
	return *this;
}

bool qtaskid::operator == (const qtaskid& i) 
{
	if (nid == i.nid && tid == i.tid)
		return true;
	return false;
}		

bool qtaskid::operator != (const qtaskid& i) 
{
	return ! operator == (i);
}		

//-----------------------------------------------------------------------------
// ostream << overloaded
//-----------------------------------------------------------------------------
inline std::ostream& operator << (std::ostream& os, const qtaskid& i)
{
	return (os << int(i.nid) << ':' << int(i.tid));
}

#endif
