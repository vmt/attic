//-----------------------------------------------------------------------------
// bin.cc - Task Bin Implementation (bin.h)
//-----------------------------------------------------------------------------
#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <error.hh>
#include <bin.hh>

using namespace std;

//-----------------------------------------------------------------------------
// Bin() - Constructor for the Bin object. 
//-----------------------------------------------------------------------------
Bin::Bin(const char* file) : binCount(0)
{	
	xmlDocPtr  doc;
	xmlNodePtr cur;
	xmlChar*   key;

	if ((doc = xmlParseFile(file)) == NULL ) 
		throw Error(eXml, "parser failed.");
	if ((cur = xmlDocGetRootElement(doc)) == NULL) {
		xmlFreeDoc(doc);
		throw Error(eXml, "parser failed.");
	}
	if (xmlStrcmp(cur->name, (const xmlChar *)"bin")) {
		xmlFreeDoc(doc);
		throw Error(eXml, "root is not <bin>.");
	}
	if ( ! (key = xmlGetProp(cur, (xmlChar*)"tasks")))
		throw Error(eXml, "task count cannot be 0.");
	binSize = atoi((const char*)key);
	binTAB  = new Bin::Task[binSize];
	xmlFree(key);	
	xmlParseBin(doc, cur, binSize);
	xmlFreeDoc(doc);
}

//-----------------------------------------------------------------------------
// xmlParseBin() - XML Parser for <bin></bin>
//-----------------------------------------------------------------------------
void Bin::xmlParseBin(xmlDocPtr doc, xmlNodePtr cur, unsigned int tasks)
{
	cur = cur->xmlChildrenNode;
	while (cur != NULL && tasks) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"task"))) {
			xmlParseTask(doc, cur, binTAB[binCount].name, 
				binTAB[binCount].file );
			--tasks;
			++binCount;
		}
		cur = cur->next;
	}
}

//-----------------------------------------------------------------------------
// xmlParseTask() - XML Parser for <task></task>
//-----------------------------------------------------------------------------
void Bin::xmlParseTask(xmlDocPtr doc, xmlNodePtr cur, string& name, 
			string& bfile)
{
	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		xmlChar *key = NULL;
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name"))) {
			key = xmlNodeListGetString(doc, 
					cur->xmlChildrenNode, 1);
			name = (const char*)key;
			xmlFree(key);
		}
		else
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"file"))) {
			key = xmlNodeListGetString(doc, 
					cur->xmlChildrenNode, 1);
			bfile = (const char*)key;
			xmlFree(key);
		}
		cur = cur->next;		
	}
}

//-----------------------------------------------------------------------------
// file(name) - Returns file=>name
//-----------------------------------------------------------------------------
const char* Bin::file(const string& name)
{
	register Bin::Task* bref = binTAB;
	register Bin::Task* bend = binTAB + binSize;

	for (; bref < bend; ++bref) {
		if (name == bref->name)
			return bref->file.c_str();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ~Bin() - Bin destructor
//-----------------------------------------------------------------------------
Bin::~Bin()
{
	delete[] binTAB;
}
