#include <iostream>
#include <string>
#include <unistd.h>
#include <qvm.h>
#include "frac.h"

using namespace std;

qtag streamtag = 123;
qtaskid masterid;

qint F_width;
qint F_height;
qint F_line_interval;
qint F_line_start;
qint F_iterations;

qstream* stream = 0;

void waituser()
{
	char c;
	cout << ": ";
	cin >> c;
}

void frac_point(int x, int y)
{
	cout << "==> "<<x<<" "<<y <<endl;

	stream->put(x);
	stream->put(y);
}

int main(int argc, char **argv)
{
	 try {
		// initialize task in vm
		qtask qt(argc, argv);

		cout << "My ID = " <<qt.mytid() <<endl;
		cout << "My Parent's ID = " <<qt.myptid() <<endl;

		// open stream with parent	
		stream = qt.mkstream(qt.myptid(), 1);

		// get fractal details
		stream->get(F_width);
		stream->get(F_height);

		// get my share of work
		stream->get(F_line_interval);
		stream->get(F_line_start);
		stream->get(F_iterations);

		// print details
		croak("Fractal details..");
		croak("Width  = "<<F_width);
		croak("Height = "<<F_height);
		croak("My work..");
		croak("Lines in interval = "<<F_line_interval);
		croak("Starting from  = "<<F_line_start);
		croak("Iterations  = "<<F_iterations);

		waituser();

		for (int i = F_line_start; i < F_height; i += F_line_interval) {
			compute_line(i, F_width, F_height, F_iterations,
				     frac_point);
		}

		frac_point(0,0);
		stream->flush();
		sleep(10);

	} catch(Exception& e) {
		cerr<<e<<endl;
  	} catch(...) {
		cerr<<"Unhandled exception caught."<< endl;
  	}


}
