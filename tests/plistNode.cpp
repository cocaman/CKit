#include <iostream>

#include "CKPListDataNode.h"
#include "CKString.h"

int main(int argc, char *argv[]) {
	// first, make a simple node
	CKPListDataNode		n("test.plist");

	std::cout << "Root node:" << std::endl;
	std::cout << "----------" << std::endl;
	std::cout << n << std::endl;
	
	std::cout << std::endl;
	std::cout << "Data Values:" << std::endl;
	std::cout << "-----------" << std::endl;
	std::cout << n["name"] << std::endl;
	std::cout << n["size"] << std::endl;
	std::cout << n["weight"] << std::endl;
	std::cout << n["color"] << std::endl;
	std::cout << n["chassis/year"] << std::endl;
	
	std::cout << std::endl;
	std::cout << "PList:" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << n.toPListString() << std::endl;

	std::cout << std::endl;
}
