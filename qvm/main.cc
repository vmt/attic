#include <iostream>
#include <signal.h>
#include <sock.hh>
#include <nsrv.hh>
#include <error.hh>

using namespace std;

VirtualNodeServer *vmns;


extern "C" void sigalarm(int) {	vmns->sigTimer(); 
}
extern "C" void sigchild(int) {	vmns->sigChild(); 
}
extern "C" void sigint(int)   {	vmns->sigInt();   
}
extern "C" void sigabrt(int)  { vmns->sigInt();   
}

//-----------------------------------------------------------------------------
// main() - NODE server main
// ./vmns <conf> <port>
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "Usage: " <<argv[0] << " [port]" << endl;
		return -1;
	}

	unsigned port = atoi(argv[1]);
	seteuid(0);

	try {

	vmns = new VirtualNodeServer("bin.xml", "amd.localhost", 8000,
				port, 32);

	if (signal(SIGCHLD, sigchild) == SIG_ERR) 
		cout << (strerror(errno)) << endl;
	if (signal(SIGALRM, sigalarm) == SIG_ERR) 
		cout << (strerror(errno)) << endl;
	if (signal(SIGINT, sigint)    == SIG_ERR) 
		cout << (strerror(errno)) << endl;
	if (signal(SIGABRT, sigint)    == SIG_ERR) 
		cout << (strerror(errno)) << endl;

		vmns->work();
	}
	catch(Exception& e)
	{
		cout <<e.file() << ":" << e.line() << " "<<e.mesg();
	}
	catch(...)
	{
		cout << "Weird!";
	}
}
