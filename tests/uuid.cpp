/*
 * This is a test program that generates a UUID and then tries to use the
 * string value (encoded value) to crerate another one and test for equality.
 * If all goes well, then they should be equal.
 */

#include <iostream>
#include <stdio.h>

#include "CKUUID.h"

int main(int argc, char *argv[]) {
	bool	error = false;

	CKUUID	a = CKUUID::newUUID();
	std::cout << "A: " << a.getStringValue() << std::endl;
	std::cout << "   " << a.getStringValueInDCEFormat() << std::endl;
	printf("date: %15.6lf\n", a.getDateCreated());

	CKUUID	b = CKUUID::uuidWithString(a.getStringValue());
	if (a == b) {
		std::cout << "equal" << std::endl;
	} else {
		error = true;
		std::cout << "NOT equal" << std::endl;
	}

	CKUUID	c = CKUUID::newUUIDForClass("CKUUID");
	std::cout << "C: " << c.getStringValue() << std::endl;
	std::cout << "   " << c.getStringValueInDCEFormat() << std::endl;
	printf("date: %15.6lf\n", c.getDateCreated());

	CKUUID	d = CKUUID::uuidWithString(c.getStringValue());
	if (c == d) {
		std::cout << "equal" << std::endl;
	} else {
		error = true;
		std::cout << "NOT equal" << std::endl;
	}

	if (error) {
		std::cout << "FAILURE" << std::endl;
	} else {
		std::cout << "SUCCESS" << std::endl;
	}
}
