/*
 * This trys to reproduce the bug I was seeing in the initialization
 * of the CKString while in a CKVector in a std::map.
 */
#include <iostream>
#include <map>
#include "CKVector.h"
#include "CKString.h"

typedef struct tuple_t {
	CKString	one;
	CKString	two;
	CKString	three;

	// constructors
	tuple_t() :
		one(),
		two(),
		three()
	{
	}

	tuple_t( const CKString & aOne, const CKString & aTwo, const CKString & aThree ) :
		one(aOne),
		two(aTwo),
		three(aThree)
	{
	}

	tuple_t( const tuple_t & anOther ) :
		one(),
		two(),
		three()
	{
		*this = anOther;
	}

	virtual ~tuple_t()
	{
	}

	tuple_t & operator=( const tuple_t & anOther )
	{
		one = anOther.one;
		two = anOther.two;
		three = anOther.three;

		return *this;
	}

	bool operator==( const tuple_t & anOther )
	{
		bool	equal = true;
		if ((one != anOther.one) ||
			(two != anOther.two) ||
			(three != anOther.three)) {
			equal = false;
		}
		return equal;
	}

	bool operator!=( const tuple_t & anOther )
	{
		return !operator==(anOther);
	}

	CKString toString() const
	{
		CKString	retval("[one=");
		retval.append(one).append(", two=").append(two).append(", three=").append(three).append("]");
		return retval;
	}
} tuple;

typedef CKVector<tuple> TList;
typedef std::map<CKString, TList> TListMap;

int main(int argc, char *argv[]) {
	// make the map - no entries
	TListMap	myMap;
	// this is the first key we'll be using
	CKString	myKey("key");
	// create the first map entry
	TList	& list = myMap[myKey];
	// create a tuple to place on the list
	tuple	t("a", "b", "c");
	std::cout << "tuple = " << t.toString() << std::endl;
	// put the tuple on the list
	std::cout << "list has " << list.size() << " elems with capacity of " << list.capacity() << std::endl;
	if (!list.contains(t)) {
		std::cout << "tuple not in list, adding..." << std::endl;
		list.addToEnd(t);
	}
	// print out the map
	std::cout << "here's the map now: ";
	TListMap::iterator	mi;
	for (mi = myMap.begin(); mi != myMap.end(); ++mi) {
		std::cout << mi->first << " = [";
		for (int i = 0; i < mi->second.size(); ++i) {
			if (i != 0) {
				std::cout << ", ";
			}
			std::cout << mi->second[i].toString();
		}
		std::cout << "]" << std::endl;
	}
}
