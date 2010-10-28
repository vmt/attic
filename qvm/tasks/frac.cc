#include <iostream>
#include <cmath>

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

		if (k == iterations) 
			frac_point(lineno, j);
		
	
		C_x = C_x + x_inc;
	}
}
