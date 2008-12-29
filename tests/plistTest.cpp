#include <iostream>

#include "CKPListDataNode.h"
#include "CKString.h"

int main(int argc, char *argv[]) {
	// first, make a simple node
	CKPListDataNode		n("slapper.conf");

	std::cout << "Root node:" << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << n << std::endl;

	std::cout << std::endl;
	std::cout << "Data Values:" << std::endl;
	std::cout << "-----------" << std::endl;
	std::cout << n["backup/filename"] << std::endl;
	std::cout << n["backup/interval_mins"] << std::endl;
	std::cout << n["amba/logfile"] << std::endl;
//	std::cout << n["weight"] << std::endl;
//	std::cout << n["color"] << std::endl;
//	std::cout << n["chassis/year"] << std::endl;

	std::cout << std::endl;
	std::cout << "PList:" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << n.toPListString() << std::endl;

	std::cout << std::endl;
}
