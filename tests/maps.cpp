// system includes
#include <iostream>

// CKit includes
#include "CKString.h"
#include "CKBiMap.h"
#include "CKXRef.h"

int main(int argc, char *argv[]) {
	// make the map we'll be using for the tests.
	CKBiMap<int, CKString>	m;
	// add some values
	m.put(0, "zero");
	m.put(1, "one");
	m.put(2, "two");
	m.put(3, "three");
	m.put(4, "four");

	// get some values out
	std::cout << "2 -> " << m.get(2) << std::endl;
	std::cout << "two -> " << m.getKey("two") << std::endl;
	std::cout << "4 -> " << m.get(4) << std::endl;

	// shoot out everything
	std::cout << std::endl << " --- map ---" << std::endl;
	std::map<int, CKString>::iterator	i;
	for (i = m.keysBegin(); i != m.keysEnd(); ++i) {
		std::cout << i->first << " -> " << i->second << std::endl;
	}
	std::cout << " --- by values ---" << std::endl;
	std::map<CKString, int>::iterator	j;
	for (j = m.valuesBegin(); j != m.valuesEnd(); ++j) {
		std::cout << j->first << " -> " << j->second << std::endl;
	}

	// erase something and show the results
	std::cout << std::endl << "deleting '2'..." << std::endl;
	m.erase(2);
	std::cout << " --- map ---" << std::endl;
	for (i = m.keysBegin(); i != m.keysEnd(); ++i) {
		std::cout << i->first << " -> " << i->second << std::endl;
	}

	// make a cross-reference table
	CKXRef<int, CKString>	x;
	// add some values
	x.put(0, "zero");
	x.put(0, "zip");
	x.put(0, "nada");
	x.put(0, "zilch");
	x.put(1, "one");
	x.put(1, "spot");
	x.put(1, "single");
	x.put(1, "uno");
	x.put(2, "two");
	x.put(2, "pair");
	x.put(2, "couple");
	x.put(3, "three");
	x.put(3, "trio");
	x.put(3, "few");

	// shoot out everything
	std::cout << std::endl << " --- x-ref ---" << std::endl;
	std::map< int, std::set<CKString> >::iterator	k;
	for (k = x.keysBegin(); k != x.keysEnd(); ++k) {
		std::ostringstream	pair;
		pair << k->first << " -> [";
		std::set<CKString>::iterator	j;
		for (j = k->second.begin(); j != k->second.end(); ++j) {
			if (j != k->second.begin()) {
				pair << ", ";
			}
			pair << *j;
		}
		pair << "]";
		std::cout << pair.str() << std::endl;
	}

	// do just one cut forward...
	std::cout << std::endl << " --- forward ---" << std::endl;
	std::cout << "2 -> [";
	std::set<CKString>::iterator	f;
	for (f = x.valuesBegin(2); f != x.valuesEnd(2); ++f) {
		if (f != x.valuesBegin(2)) {
			std::cout << ", ";
		}
		std::cout << *f;
	}
	std::cout << "]" << std::endl;

	// do just one cut reverse...
	std::cout << std::endl << " --- reverse ---" << std::endl;
	std::cout << "single -> [";
	std::set<int>::iterator	r;
	for (r = x.keysBegin("single"); r != x.keysEnd("single"); ++r) {
		if (r != x.keysBegin("single")) {
			std::cout << ", ";
		}
		std::cout << *r;
	}
	std::cout << "]" << std::endl;

	// delete one of the values and see what happens
	std::cout << std::endl << "deleting the 'value' of 'single'..." << std::endl;
	x.eraseValue("single");
	std::cout << " --- forward ---" << std::endl;
	for (k = x.keysBegin(); k != x.keysEnd(); ++k) {
		std::ostringstream	pair;
		pair << k->first << " -> [";
		std::set<CKString>::iterator	j;
		for (j = k->second.begin(); j != k->second.end(); ++j) {
			if (j != k->second.begin()) {
				pair << ", ";
			}
			pair << *j;
		}
		pair << "]";
		std::cout << pair.str() << std::endl;
	}

	// delete one of the keys and see what happens
	std::cout << std::endl << "deleting the 'key' of 2..." << std::endl;
	x.erase(2);
	std::cout << " --- forward ---" << std::endl;
	for (k = x.keysBegin(); k != x.keysEnd(); ++k) {
		std::ostringstream	pair;
		pair << k->first << " -> [";
		std::set<CKString>::iterator	j;
		for (j = k->second.begin(); j != k->second.end(); ++j) {
			if (j != k->second.begin()) {
				pair << ", ";
			}
			pair << *j;
		}
		pair << "]";
		std::cout << pair.str() << std::endl;
	}

	return 0;
}
