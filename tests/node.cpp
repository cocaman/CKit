#include <iostream>

#include "CKDataNode.h"

int main(int argc, char *argv[]) {
	// first, make a simple node
	CKDataNode		*n = new CKDataNode();
	n->setName("Root");
	n->putVar("size", CKVariant((double) 1.1));
	n->putVar("color", CKVariant("blue"));
	std::cout << "Basic:" << std::endl;
	std::cout << "------" << std::endl;
	std::cout << (*n) << std::endl;
	CKDataNode::deleteNodeDeep(n);
	if (n == NULL) {
		std::cout << "...cleaned up successfully." << std::endl;
	} else {
		std::cout << "PROBLEM! Clean-up was NOT successful!" << std::endl;
	}
	std::cout << std::endl;

	// now let's try a little more complex example
	n = new CKDataNode();
	n->setName("Sectors");
	n->putVarAtPath("MainA/SubA/price", CKVariant((double) 10.1));
	n->putVarAtPath("MainA/SubB/price", CKVariant((double) 10.2));
	n->putVarAtPath("MainA/SubC/price", CKVariant((double) 10.3));
	n->putVarAtPath("MainB/SubA/price", CKVariant((double) 11.1));
	n->putVarAtPath("MainB/SubB/price", CKVariant((double) 11.2));
	n->putVarAtPath("MainB/SubC/price", CKVariant((double) 11.3));
	n->putVarAtPath("MainB/\"R/V\"/price", CKVariant((double) 22.2));
	std::cout << "Sectors:" << std::endl;
	std::cout << "-------" << std::endl;
	std::cout << (*n) << std::endl;

	CKVariant	*a;
	a = n->getVarAtPath("MainA/SubC/price");
	std::cout << "MainA/SubC/price = " << (*a) << std::endl;
	a = n->getVarAtPath("MainA/\"R/V\"/price");
	if (a == NULL) {
		std::cout << "MainA/\"R/V\"/price = " << "NULL" << std::endl;
	} else {
		std::cout << "MainA/\"R/V\"/price = " << (*a) << std::endl;
	}
	a = n->getVarAtPath("MainB/\"R/V\"/price");
	if (a == NULL) {
		std::cout << "MainB/\"R/V\"/price = " << "NULL" << std::endl;
	} else {
		std::cout << "MainB/\"R/V\"/price = " << (*a) << std::endl;
	}
	CKDataNode::deleteNodeDeep(n);
	if (n == NULL) {
		std::cout << "...cleaned up successfully." << std::endl;
	} else {
		std::cout << "PROBLEM! Clean-up was NOT successful!" << std::endl;
	}
	std::cout << std::endl;
}
