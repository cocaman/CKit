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
	n->putVarAtPath("MainC/SubA/title2/subbookQ/symbolW/price", CKVariant((double) 1.3));
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

	CKDataNode	*d = NULL;
	d = n->findChild("MainC");
	if (d != NULL) {
		d = d->findChild("SubA");
		if (d != NULL) {
			d = d->findChild("title2");
			if (d != NULL) {
				d = d->findChild("subbookQ");
				if (d != NULL) {
					d = d->findChild("symbolW");
					if (d != NULL) {
						std::cout << "Path to 'symbolW' should be: "
							"'Sectors/MainC/SubA/title2/subbookQ/symbolW':" <<
							std::endl << "   " << d->getPath() << std::endl;
					}
				}
			}
		}
	}

	std::cout << std::endl;
	std::cout << "Unique Leaf Node Names:" << std::endl;
	std::cout << "-----------------------" << std::endl;
	std::vector<std::string>	leafs = n->getUniqueLeafNodeNames();
	for (unsigned int l = 0; l < leafs.size(); l++) {
		std::cout << "  " << leafs[l] << std::endl;
	}
	std::cout << std::endl;

	CKDataNode::deleteNodeDeep(n);
	if (n == NULL) {
		std::cout << "...cleaned up successfully." << std::endl;
	} else {
		std::cout << "PROBLEM! Clean-up was NOT successful!" << std::endl;
	}
	std::cout << std::endl;
}
