//-----------------------------------------------------------------------------
// QVM Exception Object Definition
//-----------------------------------------------------------------------------
#ifndef _QVMLIB_ERROR_HH_
#define _QVMLIB_ERROR_HH_

#include <string>
#include <error.hh>

//-----------------------------------------------------------------------------
// QVM Exception class
//-----------------------------------------------------------------------------
class qerror : public Exception
{
public:
	qerror (int code, const std::string& mesg, 
		   const char* file, int line):
		Exception(code, mesg, file, line) 
	{}

	enum code
	{
		eInvalidCmdLine,
		eSocket
	};
};

#endif
