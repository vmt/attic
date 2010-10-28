#include <iostream>
#include <qvm.h>

using namespace std;

int main(int argc, char **argv)
{
	// create task object
	qtask task(argc, argv);

	// output details
	cout << "My ID  = "<<task.mytid() << endl;
	cout << "My Parent's ID = "<<task.myptid() << endl;

	// block for signals;
	task.probe();
}
