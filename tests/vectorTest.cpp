/*
 * This is a test program that exercises the CKVector.
 */

#include <iostream>
#include <stdio.h>

#include "CKVector.h"

int main(int argc, char *argv[]) {
	CKVector<int>	a;

	std::cout << "a.size = " << a.size() << std::endl;
	std::cout << "a.capacity = " << a.capacity() << std::endl;

	a.addToEnd(2);
	a.addToEnd(4);
	a[4] = 5;
	for (int i = 0; i < a.size(); i++) {
		std::cout << "a[" << i << "] = " << a[i] << std::endl;
	}
}
