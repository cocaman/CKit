/*
 * This is a test program that exercises the CKString.
 */

#include <iostream>
#include <stdio.h>
#include <string>
#include <map>

#include "CKString.h"

int main(int argc, char *argv[]) {
	CKString	line = "When I went walking one day ";

	std::cout << line << std::endl;
	
	line.append("I saw a very large fish. It was so big that it needed another "
				"pond to live in. I tried to take it to a pond I knew of, but "
				"it was too heavy for me.");
	line += 4;

	std::cout << line << std::endl;
	
	line = std::string("hi");
	line += " I can do this";
	line.prepend("Yo! ");

	std::cout << line << std::endl;
	std::cout << "size=" << line.size() << "  length=" << line.length() <<
		"  capacity=" << line.capacity() << std::endl;
	std::cout << "line[2]=" << line[2] << std::endl;
	
	CKString	s;
	s = "** " + line + " **";
	std::cout << s << std::endl;
	s = line;
	std::cout << s << std::endl;

	line = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::cout << line << std::endl;
	std::cout << line.substr(2) << std::endl;
	std::cout << line.substr(2, 2) << std::endl;
	std::cout << line.substrTo("IJK") << std::endl;
	std::cout << line.substrFrom("IJK") << std::endl;
	std::cout << line.substrBefore("IJK") << std::endl;
	std::cout << line.substrAfter("IJK") << std::endl;
	std::cout << line.left(5) << std::endl;
	std::cout << line.right(3) << std::endl;
	std::cout << line.mid(3,5) << std::endl;
	std::cout << line.replace('O','*') << " " << line << std::endl;
	std::cout << line.findLast("TU") << std::endl;
	
	std::map<CKString, CKString>	lookup;
	lookup["one"] = "ONE";
	lookup["two"] = "TWO";
	lookup["three"] = "THREE";
	lookup["one_two"] = "ONE_TWO";
	std::map<CKString, CKString>::iterator	lu;
	for (lu = lookup.begin(); lu != lookup.end(); ++lu) {
		std::cout << lu->first << "=>" << lu->second << std::endl;
	}
	
	CKString	one("one");
	CKString	one_two("one_two");
	if ((one == one_two) || (one_two == one)) {
		std::cout << "bad equality" << std::endl;
	} else {
		std::cout << "good equality" << std::endl;
	}
	if (one < one_two) {
		std::cout << one << " < " << one_two << std::endl;
	} else {
		std::cout << one << " !< " << one_two << std::endl;
	}
	if (one_two < one) {
		std::cout << one_two << " < " << one << std::endl;
	} else {
		std::cout << one_two << " !< " << one << std::endl;
	}
}
