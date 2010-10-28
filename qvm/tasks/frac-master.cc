#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <qvm.h>

using namespace std;

quint NR_SLAVES;

bool	slavesready = false;
qtaskid	slaveids[10];
quint	readycount = 0;


class spawneventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::OK &&
			status.tag() < NR_SLAVES) {
			// save the id
			slaveids[status.tag()] = status.id();
			// check if all slaves are ready
			if (++readycount == NR_SLAVES)
				slavesready = true;
			cout << "Slave "<<status.id()<<" ready.";
		}
		else
		if (status.status() == qstatus::FAILED) {
			croak("SPAWN FAILED "<<status.id()<<" "<<
			      status.name());
			sleep(4);
			exit(1) ;
		}
	}
};

class taskeventhandler : public qeventhandler {  
public: void entry(qstatus& status)
	{
		if (status.status() == qstatus::QUIT)  {
			if (--readycount == 0)
				slavesready = false;
		}
	}
};

char FRAC_BUFFER[1600][1800];

int main(int argc, char **argv)
{
	qint F_width, F_height, F_iterations;

 try {
	// make task object
	qtask task(argc, argv);

	// register event handlers
	spawneventhandler spawnEHndlr;
	taskeventhandler taskEHndlr;
	task.setspawneventhandler(&spawnEHndlr);
	task.settaskeventhandler(&taskEHndlr);
	
	cout << "Mandelbrot Set Generator"<<endl;
	cout << "My ID = " <<task.mytid() <<endl;
	cout << "My Parent's ID = " <<task.myptid() <<endl;
	cout << "Width ? : "; cin >> F_width;
	cout << "Height? : "; cin >> F_height;
	cout << "Iterations ? : "; cin >> F_iterations;
	cout << "Number of slaves to use ? : "; cin >> NR_SLAVES;

	// spawn slaves
	for (Ttag i = 0; i < NR_SLAVES; ++i)
		task.spawn("frac-slave", i);

	croak("Waiting for slaves to become ready...");
	while (! slavesready)
		task.probe();
	croak("Slaves are ready.");

	// make stream, for each slave and send details
	qstream **streams = new qstream*[NR_SLAVES];
	for (Ttag i = 0; i < NR_SLAVES; ++i) {
		streams[i] = task.mkstream(slaveids[i], 1);
		
		*streams[i] << qint(F_width);
		*streams[i] << qint(F_height);
		*streams[i] << qint(NR_SLAVES);
		*streams[i] << qint(i);
		*streams[i] << qint(F_iterations);

		streams[i]->flush();
	}

	// init buffer
	for (int i = 0; i < F_height; ++i) {
		for (int j = 0; j < F_width; ++j)
			FRAC_BUFFER[i][j] = ' ';
	}

	// get data from slaves
	cout << "Recving data from slaves..."<<endl;
	while (slavesready) {
		qint x, y;
		quint i = 0;
		for (; i < NR_SLAVES; ++i) {
			if (streams[i]->get_nb(x)) {
				streams[i]->get(y);
				cout << "==> "<<x<<" "<<y<<endl;
				if ( x &&  y) 
					FRAC_BUFFER[x][y] = '#';
			} 
		}	
	}

	ofstream outf("frac.txt");

	for (int i = 0; i < F_height; ++i) {
		for (int j = 0; j < F_width; ++j)
			outf << FRAC_BUFFER[i][j];
		outf << endl;
	}
	croak("Output !");
	outf.close();
  }
  catch(Exception& e) {
	cout << e<<endl;
  }
  catch(...) {
	cout << "Unhandled exception caught." << endl;;	
  }

}
