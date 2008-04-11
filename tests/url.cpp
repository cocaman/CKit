/*
 * This is a test program that works the CKURL.
 */

#include <iostream>
#include <stdio.h>

#include "CKURL.h"

int main(int argc, char *argv[]) {
//	bool	error = false;

	CKURL	a("http://peabody.local/~drbob/info.php");
	a.setField("level", "tL");
	a.setField("name", "Bob Beaty");
	a.setField("code", "H31p m3!");
	std::cout << "[url] " << a.toString() << std::endl;

	CKURL	b("http://peabody.local/~drbob/info.php");
	std::cout << "[url] " << b.toString() << std::endl;
	b.doGET();
	std::cout << "  ...got " << b.getResults().size() << " chars back" << std::endl;
	std::cout << "========== data: ==========" << std::endl;
//	std::cout << b.getResults() << std::endl;
	std::cout << "===========================" << std::endl;

	b.doPOST();
	std::cout << "  ...got " << b.getResults().size() << " chars back" << std::endl;
	
//	if (error) {
//		std::cout << "FAILURE" << std::endl;
//	} else {
//		std::cout << "SUCCESS" << std::endl;
//	}
}
