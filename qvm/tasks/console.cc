//-----------------------------------------------------------------------------
// The Console
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <qvm.h>
#include <unistd.h>

using namespace std;

//-----------------------------------------------------------------------------
// Event handlers
//-----------------------------------------------------------------------------
class spawneventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::OK) 
			croak("SPAWNED "<<status.id()<<" "<<
			      status.name());
		else
		if (status.status() == qstatus::FAILED)
			croak("SPAWN FAILED "<<status.id()<<" "<<
			      status.name());
	}
};

class taskeventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::QUIT) 
			croak("QUIT "<<status.id());
	}
};

// buffer for talk messages
char talkbuffer[100];;

int main(int argc, char **argv)
{
 try {

	qstring cmd;
	qtask qt(argc, argv);

	cout << "QONSOLE for Node("<<int(qt.mytid().nid)<<")"<<endl;

	// register event handlers
	spawneventhandler spawnEHndlr;
	taskeventhandler taskEHndlr;
	qt.setspawneventhandler(&spawnEHndlr);
	qt.settaskeventhandler(&taskEHndlr);

	// command loop	
	while (cmd != "exit") {

		// the command prompt
		cout << "qonsole> ";
		cout.flush();
		cin >> cmd;

		// spawn command
		if (cmd == "spawn") {
			cin >> cmd;
			qt.spawn(cmd, 0);
		} 
		else if (cmd == "log") {

		}
		else if (cmd == "talk") {

		}
		else if (cmd == "help") {
			cout << "spawn <task-name>  - Spawn task"<<endl;
			cout << "log                - For logging events"<<endl;
		}
		else {
			cout << "Bad Command"<<endl;
		}

		// probe for events.
		qt.probe_nb();
	}
  } catch(Exception& e) {
	cout << e << endl;
	sleep(4);
  } catch(...) {
	cout << "Unhandled exceptionn caught." << endl;
	sleep(4);
  }
}
