#include <iostream>

#include "CKTable.h"

int main(int argc, char *argv[]) {
	CKTable		a(3, 4);
	a.setColumnHeader(0, "TICKER");
	a.setColumnHeader(1, "LAST");
	a.setColumnHeader(2, "CLOSE");
	a.setColumnHeader(3, "OPEN");
	a.setStringValue(0, 0, "IBM");
	a.setDoubleValue(0, 1, 55.6);
	a.setDoubleValue(0, 2, 58.0);
	a.setDoubleValue(0, 3, 54.3);
	a.setStringValue(1, 0, "AAPL");
	a.setDoubleValue(1, 1, 23.6);
	a.setDoubleValue(1, 2, 22.0);
	a.setDoubleValue(1, 3, 24.3);
	a.setStringValue(2, 0, "MSFT");
	a.setDoubleValue(2, 1, 15.6);
	a.setDoubleValue(2, 2, 18.0);
	a.setDoubleValue(2, 3, 14.3);
	std::cout << "A:" << std::endl;
	std::cout << a << std::endl;

	CKTable		b(3, 4);
	b.setColumnHeader(0, "TICKER");
	b.setColumnHeader(1, "LAST");
	b.setColumnHeader(2, "CLOSE");
	b.setColumnHeader(3, "OPEN");
	b.setStringValue(0, 0, "BBY");
	b.setDoubleValue(0, 1, 45.6);
	b.setDoubleValue(0, 2, 48.0);
	b.setDoubleValue(0, 3, 44.3);
	b.setStringValue(1, 0, "HD");
	b.setDoubleValue(1, 1, 33.6);
	b.setDoubleValue(1, 2, 32.0);
	b.setDoubleValue(1, 3, 34.3);
	b.setStringValue(2, 0, "LOW");
	b.setDoubleValue(2, 1, 65.6);
	b.setDoubleValue(2, 2, 68.0);
	b.setDoubleValue(2, 3, 64.3);
	std::cout << "B:" << std::endl;
	std::cout << b << std::endl;
	
	b.merge(a);
	std::cout << "A + B:" << std::endl;
	std::cout << b << std::endl;
}
