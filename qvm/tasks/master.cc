//-----------------------------------------------------------------------------
// Demo - Master-Slave: Master
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <unistd.h>
#include <qvm.h>

using namespace std;

qtag	slavetag;
qtaskid slaveid;
bool	slavealive = false;

//-----------------------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------------------
class spawneventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::OK &&
			status.tag() == slavetag) {
			// get child parameters
			slaveid = status.id();
			slavealive = true;
		}
		else
		if (status.status() == qstatus::FAILED) {
			croak("SPAWN FAILED "<<status.id()<<" "<<
			      status.name());
		}
	}
};

class taskeventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::QUIT &&
			slaveid == status.id())  {
			slavealive = false;
		}
	}
};

void waituser()
{
	char c;
	cout << ": ";
	cin >> c;
}

int main(int argc, char **argv)
{
 try {

	// create the task object
	qtask task(argc, argv);

	// assign an arbitrary tag to slave
	slavetag = 2;

	// register event handlers
	spawneventhandler spawnEHndlr;
	taskeventhandler taskEHndlr;
	task.setspawneventhandler(&spawnEHndlr);
	task.settaskeventhandler(&taskEHndlr);

	cout << "MASTER" << endl;
	cout << "My ID = "<<task.mytid()<<endl;
	cout << "My Parent's ID = "<<task.myptid()<<endl;

	waituser();

	// spawn slave
	task.spawn("slave", slavetag);

	cout << "Waiting for slave to start..." <<endl;
	// wait for slave to become alive.
	while (! slavealive)
		task.probe();
	cout << "Slave is alive." <<endl;

	// make two streams
	qstream& stream1 = *task.mkstream(slaveid, 1);
	qstream& stream2 = *task.mkstream(slaveid, 2);


	cout << "Sending slave the hello message.."<<endl;
	waituser();

	// send hello over stream 1
	stream1 << "Hello slave !";
	stream1.flush();

	cout << "Testing stream 2" <<endl;
	cout << "Sending 1000 numbers.. "<<endl;

	waituser();

	for (qint i = 0; i < 1000; ++i)
		stream2 << i;
	stream2.flush();

	cout << "Waiting for slave to quit.." <<endl;
	while (slavealive)
		task.probe();
	cout << "Slave quit"<<endl;

	// block
	task.probe();

  } catch(Exception& e) {
	cout << e << endl;
  } catch(...) {
	cout << "Unhandled exception caught." << endl;
  }
}
