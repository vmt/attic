//-----------------------------------------------------------------------------
// error.hh - Exception Handling
//-----------------------------------------------------------------------------
#ifndef _ERROR_HH_
#define _ERROR_HH_

#include <string>
#include <cerrno>

//-----------------------------------------------------------------------------
// Exception Object
//-----------------------------------------------------------------------------
class Exception
{
private:
	int		enumber;
	std::string	emessage;
	const char*	efile;
	unsigned int	eline;
public:
	inline Exception():
		enumber(0), efile(0), eline(0) {}
	inline Exception(int n, const std::string& m, const char* f, 
			 unsigned int l):
		enumber(n), emessage(m), efile(f), eline(l) {}
	inline int code() const { 
		return(enumber); 
	}
	inline const std::string& mesg() const { 
		return(emessage); 
	}
	inline const char* file() const { 
		return(efile); 
	}
	inline unsigned int line() const { 
		return(eline); 
	}	
};

inline std::ostream& operator << (std::ostream& os, const Exception& e)
{
	if (e.file()) {
		os << e.file() << ": " <<e.line() << e.mesg() << endl;
	} else {
		os << e.mesg() << endl;
	}
	return os;
}

#define Error(c, m) Exception(c, m, __FILE__, __LINE__)
#define LibError()  Exception(errno, strerror(errno), __FILE__, __LINE__)
#define croak(m)    (std::cerr<<"==> "<<m<<endl)
#endif
