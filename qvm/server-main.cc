#include <iostream>
#include <signal.h>
#include <server.hh>
#include <error.hh>

using namespace std;

VirtualMachineServer *vms;

extern "C" void sigint(int)   {	vms->sigint(); }

//-----------------------------------------------------------------------------
// main() - NODE server main
// ./vmns <conf> <port>
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	try {

	vms = new VirtualMachineServer("node.xml", 8000);

	if (signal(SIGINT, sigint)    == SIG_ERR) 
		cout << (strerror(errno)) << endl;
		vms->work();
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
