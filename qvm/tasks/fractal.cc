#include <iostream>
#include <cmath>
#include <cfloat>
#include <cerrno>

char FRAC_BUFFER[1600][1800];

void compute_line(int lineno, int width, int height, 
		 int iterations, void (*frac_point)(int, int))
{
	double x_min = -2.1;
	double y_min = -1;
	double x_max = 1;
	double y_max = 1.3;

	double x_inc = (x_max - x_min) / width;
	double y_inc = (y_max - y_min) / height;
	
	double C_y = y_max - (double(lineno) * y_inc);
	double C_x = x_min;

	for (int j = 0; j < width; ++j ) {

		double x = 0, xn = 0;
		double y = 0, yn = 0;
		double abs = 0;
		int k = 0;
		for (; k < iterations; ++k) {

			xn = x*x - y*y + C_x;
			yn = 2*x*y + C_y; 
			abs= sqrt(xn*xn + yn*yn);

			if (std::abs(abs) == HUGE_VAL ||
			    std::abs(abs) ==-HUGE_VAL  )
					break;

			x = xn;
			y = yn;
		}

		if (k == iterations) {
			FRAC_BUFFER[lineno][j] = '#';
			
		}
			//frac_point(j, lineno(;
		else  { FRAC_BUFFER[lineno][j] = ' ';
			cout << " ==> "<<lineno << " "<<j <<endl;
		}
	
		C_x = C_x + x_inc;
	}
}

int main()
{
	int width = 800;
	int height= 400;
	int iter = 100;

	for (int i = 0; i < height; ++i) {
		compute_line(i, width, height, iter, 0);
		//cout << endl;
	}
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j)
			cout << FRAC_BUFFER[i][j];
		cout << endl;
	}
}

/*
int main()
{
	int width = 800;
	int height= 400;
	int iter;

	double x_min = -2.1;
	double y_min = -1.3;
	double x_max = 1;
	double y_max = 1.3;

	double x_inc = (x_max - x_min) / width;
	double y_inc = (y_max - y_min) / height;

	double C_x = 0;
	double C_y = 0;
	double Z_x = 0;
	double Z_y = 0;
	double Z_abs = 0;
	
	cin >> iter;
	
	C_y = y_max;

	for (int i = 0; i < height ; ++i) {
		
		C_x = x_min;

		for (int j = 1; j < width; ++j ) {

			double x = 0, xn = 0;
			double y = 0, yn = 0;
			double abs=0;

			int k = 0;
			for (; k < iter; ++k) {

				xn = x*x - y*y + C_x;
				yn = 2*x*y + C_y; 
				abs= sqrt(xn*xn + yn*yn);

				if (std::abs(abs) == HUGE_VAL ||
				    std::abs(abs) ==-HUGE_VAL  )
					break;

				x = xn;
				y = yn;
			}

			if (k == iter) {
				cout << "#";
			} else  cout << " ";
	
			C_x = C_x + x_inc;
		}

		cout << endl;

		C_y = C_y - y_inc;
	}

}
*/
