/*
 * This is a test program that verifies the Base64 encoding and decoding.
 */

#include <iostream>
#include <stdio.h>

#include "CKString.h"

int main(int argc, char *argv[]) {
	CKString	a("ABCABCABC");
	std::cout << "A: " << a << std::endl;
	a.convertToBase64();
	std::cout << "A: [Base64] " << a << std::endl;
	a.convertFromBase64();
	std::cout << "A: " << a << std::endl;
}
