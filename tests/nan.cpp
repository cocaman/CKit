/*
 * This program simply exercises the NAN value and isnan() method on
 * all the platforms so that I can be assured that it's working
 * properly on all platforms.
 */

#include <iostream>

#include "CKTable.h"

int main(int argc, char *argv[]) {
	double	x = 1.0;
	std::cout << "x = " << x << " ...it is " << (isnan(x) ? "" : "not") << " NAN" << std::endl;
	x = NAN;
	std::cout << "x = " << x << " ...it is " << (isnan(x) ? "" : "not") << " NAN" << std::endl;
}
