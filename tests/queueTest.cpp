/*
 * This is a test program that exercises the CKVector.
 */

#include <iostream>
#include <stdio.h>

#include "CKLIFOQueue.h"

int main(int argc, char *argv[]) {
	CKLIFOQueue<int>	a;

	std::cout << "a.size = " << a.size() << std::endl;
	std::cout << "a.capacity = " << a.capacity() << std::endl;

	a.push(2);
	a.push(4);
	a.push(3);
	std::cout << "a had: " << a.popSomething() << std::endl;
	while (!a.empty()) {
		std::cout << "a had: " << a.pop() << std::endl;
	}
}
