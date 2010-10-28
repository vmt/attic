//-----------------------------------------------------------------------------
// bin.hh - Task Bin (bin.cc)
//-----------------------------------------------------------------------------
#ifndef _BIN_HH_
#define _BIN_HH_

#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

//-----------------------------------------------------------------------------
// Bin - Task Bin Object. Essentially a hash table.
//-----------------------------------------------------------------------------
class Bin
{
private:
	int binSize;
	int binCount;	

	enum Error {
		eXml
	};

	struct Task {
		std::string name;
		std::string file;
	} *binTAB;

	void xmlParseTask(xmlDocPtr, xmlNodePtr, std::string&, std::string&);
	void xmlParseBin(xmlDocPtr,  xmlNodePtr, unsigned int);
public:

	Bin(const char* file);
	~Bin();
	const char* file(const std::string&);
};
#endif
