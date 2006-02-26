/*
 * This is a test program that exercises the CKTimeSeries.
 */

#include <iostream>
#include <stdio.h>

#include "CKTimeSeries.h"

int main(int argc, char *argv[]) {
	CKTimeSeries	a;

	a.put(20041229.000000, 1.0);
	a.put(20041230.000000, 1.5);
	a.put(20041231.000000, 1.0);
	a.put(20050102.000000, 2.0);
	a.put(20050103.000000, 2.5);
	a.put(20050104.000000, 3.5);
	a.put(20050105.000000, 2.5);
	a.put(20050106.000000, 1.5);
	std::cout << a << std::endl;
	std::cout << "sum()=" << a.sum() << std::endl;
	std::cout << "sum(20050101, 20050105)=" << a.sum((double)20050101, (double)20050105) << std::endl;
	a.eraseDates(20050101, 20050105);
	std::cout << "delete (20050101, 20050105)=" << a << std::endl;
}
