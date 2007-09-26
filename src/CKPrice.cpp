/*
 * CKPrice.cpp - this file implements a class that is used in the Factory++ to
 *               represent prices from the MM Server. As it turns out, the
 *               MM Server delivers both $US and Native prices for almost all
 *               the important prices and values. This object makes it easy
 *               to get at these guys.
 *
 * $Id: CKPrice.cpp,v 1.4 2007/09/26 19:33:46 drbob Exp $
 */

//	System Headers
#include <stdio.h>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"
#include "CKTable.h"
#include "CKPrice.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that assumes nothing about the
 * price/value that's being stored. It just sets things up ready
 * to use.
 */
CKPrice::CKPrice() :
	mUSD(NAN),
	mNative(NAN)
{
	// the default values are everything we need
}


/*
 * This form of the constructor is nice in that it allows us to take
 * the values right away and then the user doesn't have to worry
 * about setting the values right after creation. If only the $US
 * value is known, then the Native value defaults to NAN which can
 * be checked for.
 */
CKPrice::CKPrice( double aUSD, double aNative ) :
	mUSD(NAN),
	mNative(NAN)
{
	mUSD = aUSD;
	mNative = aNative;
}


/*
 * This constructor is interesting in that it takes the data as
 * it comes from another CKPrice's generateCodeFromValues()
 * method and parses it into a price directly. This is very useful
 * for serializing the price's data from one host to another across
 * a socket, for instance.
 */
CKPrice::CKPrice( const CKString & aCode ) :
	mUSD(NAN),
	mNative(NAN)
{
	// first, make sure we have something to do
	if (aCode.empty()) {
		std::ostringstream	msg;
		msg << "CKPrice::CKPrice(const char *) - the provided argument is empty "
			"and that means that nothing can be done. Please make sure that the "
			"argument is not empty before calling this constructor.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// load in the values from the code
		takeValuesFromCode(aCode);
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKPrice::CKPrice( const CKPrice & anOther ) :
	mUSD(NAN),
	mNative(NAN)
{
	// we can use the '=' operator to do most of this for us
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKPrice::~CKPrice()
{
	// this guy doesn't have to worry about deleting anything
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKPrice & CKPrice::operator=( const CKPrice & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		// copy over the values
		mUSD = anOther.mUSD;
		mNative = anOther.mNative;
	}
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * These are the setters for the USD and Native values for this
 * guy. They both need to be set either by the constructor or
 * individually, and the data from the MM Server ensures that both
 * will be set properly unless there's a serious problem with the
 * data.
 */
void CKPrice::setUSD( double aUSD )
{
	mUSD = aUSD;
}


void CKPrice::setUSD( double aUSD ) const
{
	((CKPrice *)this)->setUSD(aUSD);
}


void CKPrice::setNative( double aNative )
{
	mNative = aNative;
}


void CKPrice::setNative( double aNative ) const
{
	((CKPrice *)this)->setNative(aNative);
}


/*
 * These are the getters for the USD and Native values of this
 * guy. They should be set before they are used, as there's no
 * exchange rate conversion being done by this guy.
 */
double CKPrice::getUSD() const
{
	return mUSD;
}


double CKPrice::getNative() const
{
	return mNative;
}


/********************************************************
 *
 *                Simple Math Methods
 *
 ********************************************************/
/*
 * These methods allow the user to add values to this price,
 * in the first case, it's a constant value but in the
 * second it's another price.
 */
bool CKPrice::add( double anOffset )
{
	mUSD += anOffset;
	mNative += anOffset;
	return true;
}


bool CKPrice::add( CKPrice & anOther )
{
	mUSD += anOther.mUSD;
	mNative += anOther.mNative;
	return true;
}


bool CKPrice::add( const CKPrice & anOther )
{
	return add((CKPrice &)anOther);
}


/*
 * These methods allow the user to subtract values from this
 * price, in the first case, it's a constant value but
 * in the second it's another price.
 */
bool CKPrice::subtract( double anOffset )
{
	mUSD -= anOffset;
	mNative -= anOffset;
	return true;
}


bool CKPrice::subtract( CKPrice & anOther )
{
	mUSD -= anOther.mUSD;
	mNative -= anOther.mNative;
	return true;
}


bool CKPrice::subtract( const CKPrice & anOther )
{
	return subtract((CKPrice &)anOther);
}


/*
 * These method allows the user to multiply a constant value to
 * both components of the price or it multiplies each by it's
 * respective partner in the other price.
 */
bool CKPrice::multiply( double aFactor )
{
	mUSD *= aFactor;
	mNative *= aFactor;
	return true;
}


bool CKPrice::multiply( CKPrice & anOther )
{
	mUSD *= anOther.mUSD;
	mNative *= anOther.mNative;
	return true;
}


bool CKPrice::multiply( const CKPrice & anOther )
{
	return multiply((CKPrice &)anOther);
}


/*
 * These method allows the user to divide each component of
 * the price by a constant or it divides each by it's respective
 * partner in the other price.
 */
bool CKPrice::divide( double aDivisor )
{
	mUSD /= aDivisor;
	mNative /= aDivisor;
	return true;
}


bool CKPrice::divide( CKPrice & anOther )
{
	mUSD /= anOther.mUSD;
	mNative /= anOther.mNative;
	return true;
}


bool CKPrice::divide( const CKPrice & anOther )
{
	return divide((CKPrice &)anOther);
}


/*
 * This method simply takes the inverse of each value in the price
 * so that x -> 1/x for each value. This is marginally useful
 * I'm thinking, but I added it here to be a little more complete.
 */
bool CKPrice::inverse()
{
	mUSD = 1.0/mUSD;
	mNative = 1.0/mNative;
	return true;
}


/*
 * These are the operator equivalents of the simple mathematical
 * operations on the price. They are here as an aid to the
 * developer of analytic functions based on these guys.
 */
CKPrice & CKPrice::operator+=( double anOffset )
{
	add(anOffset);
	return *this;
}


CKPrice & CKPrice::operator+=( CKPrice & aPrice )
{
	add(aPrice);
	return *this;
}


CKPrice & CKPrice::operator+=( const CKPrice & aPrice )
{
	add((CKPrice &)aPrice);
	return *this;
}


CKPrice & CKPrice::operator-=( double anOffset )
{
	subtract(anOffset);
	return *this;
}


CKPrice & CKPrice::operator-=( CKPrice & aPrice )
{
	subtract(aPrice);
	return *this;
}


CKPrice & CKPrice::operator-=( const CKPrice & aPrice )
{
	subtract((CKPrice &)aPrice);
	return *this;
}


CKPrice & CKPrice::operator*=( double aFactor )
{
	multiply(aFactor);
	return *this;
}


CKPrice & CKPrice::operator/=( double aDivisor )
{
	divide(aDivisor);
	return *this;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * In order to simplify the move of this object from C++ to Java
 * it makes sense to encode the value's data into a CKString that
 * can be converted to a Java String and then the Java object can
 * interpret it and "reconstitue" the object from this coding.
 */
CKString CKPrice::generateCodeFromValues() const
{
	// start by getting a buffer to build up this value
	CKString buff;

	// first, send out the USD value and then the Native value
	buff.append("\x01").append(mUSD);
	buff.append("\x01").append(mNative).append("\x01");

	/*
	 * OK, it's now in a simple character array that we can scan to check
	 * for acceptable delimiter values. What we'll do is to check the string
	 * for the existence of a series of possible delimiters, and as soon as
	 * we find one that's not used in the string we'll use that guy.
	 */
	if (!CKTable::chooseAndApplyDelimiter(buff)) {
		throw CKException(__FILE__, __LINE__, "CKPrice::generateCodeFrom"
			"Values() - while trying to find an acceptable delimiter for "
			"the data in the price we ran out of possibles before finding "
			"one that wasn't being used in the text of the code. This is "
			"a serious problem that the developers need to look into.");
	}

	return buff;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this value. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKPrice::takeValuesFromCode( const CKString & aCode )
{
	// first, see if we have anything to do
	if (aCode.empty()) {
		throw CKException(__FILE__, __LINE__, "CKPrice::takeValuesFromCode("
			"const CKString &) - the passed-in code is empty which means that "
			"there's nothing I can do. Please make sure that the argument is "
			"not empty before calling this method.");
	}

	/*
	 * The data is character-delimited and the delimiter is
	 * the first character of the field data. All subsequent
	 * values will be delimited by this character. We need to
	 * get it.
	 */
	char	delim = aCode[0];
	// ...and parse this guy into chunks
	int		bit = 0;
	CKStringList	chunks = CKStringList::parseIntoChunks(
									aCode.substr(1, aCode.size()-2), delim);
	if (chunks.size() < 2) {
		std::ostringstream	msg;
		msg << "CKPrice::takeValuesFromCode(const CKString &) - the code: '" <<
			aCode << "' does not represent a valid price encoding. Please "
			"check on it's source as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * Next thing is the USD and then the Native value (both doubles)
	 */
	mUSD = chunks[bit++].doubleValue();
	mNative = chunks[bit++].doubleValue();
}


/*
 * This method checks to see if the two CKPrices are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKPrice::operator==( const CKPrice & anOther ) const
{
	bool		equal = true;

	// see if the values match
	if ((mUSD != anOther.mUSD) ||
		(mNative != anOther.mNative)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKPrices are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKPrice::operator!=( const CKPrice & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKPrice::toString() const
{
	// need to write them to a buffer first
	char	buff[256];
	// now get busy on the looping
	snprintf(buff, 255, "(USD=%lf, Native=%lf)", mUSD, mNative);

	return CKString(buff);
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKPrice & anItem )
{
	aStream << anItem.toString();

	return aStream;
}


/*
 * These are the operators for creating new price data from
 * one or two existing prices. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKPrice operator+( CKPrice & aPrice, double aValue )
{
	CKPrice		retval(aPrice);
	retval += aValue;
	return retval;
}


CKPrice operator+( double aValue, CKPrice & aPrice )
{
	return operator+(aPrice, aValue);
}


CKPrice operator+( CKPrice & aPrice, CKPrice & anotherPrice )
{
	CKPrice		retval(aPrice);
	retval += anotherPrice;
	return retval;
}


CKPrice operator-( CKPrice & aPrice, double aValue )
{
	CKPrice		retval(aPrice);
	retval -= aValue;
	return retval;
}


CKPrice operator-( double aValue, CKPrice & aPrice )
{
	CKPrice		retval(aPrice);
	retval *= -1.0;
	retval += aValue;
	return retval;
}


CKPrice operator-( CKPrice & aPrice, CKPrice & anotherPrice )
{
	CKPrice		retval(aPrice);
	retval -= anotherPrice;
	return retval;
}


CKPrice operator*( CKPrice & aPrice, double aValue )
{
	CKPrice		retval(aPrice);
	retval *= aValue;
	return retval;
}


CKPrice operator*( double aValue, CKPrice & aPrice )
{
	return operator*(aPrice, aValue);
}


CKPrice operator/( CKPrice & aPrice, double aValue )
{
	CKPrice		retval(aPrice);
	retval /= aValue;
	return retval;
}


CKPrice operator/( double aValue, CKPrice & aPrice )
{
	CKPrice		retval(aPrice);
	retval.mUSD = 1.0/retval.mUSD;
	retval.mNative = 1.0/retval.mNative;
	retval *= aValue;
	return retval;
}
