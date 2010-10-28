//-----------------------------------------------------------------------------
// Demo - Master-Slave: Slave
//-----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <unistd.h>
#include <qvm.h>

using namespace std;

void waituser()
{
	char c;
	cout << ": ";
	cin >> c;
}

int main(int argc, char **argv)
{

 try {
	string mesg;

	// make task objects
	qtask task(argc, argv);

	cout << "SLAVE" << endl;
	cout << "My ID = "<<task.mytid()<<endl;
	cout << "My Parent's ID = "<<task.myptid()<<endl;

	waituser();

	// make two stream objects
	qstream& stream1 = *task.mkstream(task.myptid(), 1);
	qstream& stream2 = *task.mkstream(task.myptid(), 2);

	// recv hello message from master
	stream1 >> mesg;
	cout << "Master says: "<<mesg<<endl;

	cout << "Testing stream 2" <<endl;
	cout << "Recving 1000 numbers.. "<<endl;

	waituser();

	qint j;
	for (qint i = 0; i < 1000;) {
		if (stream2.get_nb(j)) {
			cout << j << endl;
			++i;
		}
	}
	
	// block for signals;
	task.probe();

  } catch(Exception& e) {
	cout << e<<endl;
  } catch(...) {
	cout << "Unhandled exception caught." << endl;;	
  }
}
