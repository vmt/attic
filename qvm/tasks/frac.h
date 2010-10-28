#ifndef _FRAC_H_
#include <cmath>

extern void compute_line(int lineno, int width, int height, 
		 int iterations, void (*frac_point)(int, int));
#endif
