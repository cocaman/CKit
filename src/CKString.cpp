/*
 * CKString.cpp - this file implements a class that can be used to represent a
 *                general string. This is an alternative to the STL
 *                std::string because there are several problems with STL and
 *                GCC, and rather than fight those problems, it's easier to
 *                make an object with the subset of features that we really
 *                need and leave out the problems that STL brings.
 *
 * $Id: CKString.cpp,v 1.1 2004/09/14 15:39:50 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

//	Third-Party Headers
#include <CKException.h>

//	Other Headers
#include "CKString.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that creates a nice, empty
 * string ready for the user to dump data into as needed.
 */
CKString::CKString() :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// make sure we start out with the correct initial buffer
	resize(mInitialCapacity);
}


/*
 * This form of the constructor takes an initial buffer capacity
 * and this overrides the default for this class. This is useful
 * when you know the size of the string that will be stored and
 * you don't want to have the overhead of allocating space as
 * the string grows. You can also specify the default increment
 * capacity for this class, if you need to, but it defaults to the
 * default for the class.
 */
CKString::CKString( int anInitialCapacity, int aCapacityIncrement ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// get the values from the arguments
	mInitialCapacity = anInitialCapacity;
	mCapacityIncrement = aCapacityIncrement;

	// make sure we start out with the correct initial buffer
	resize(mInitialCapacity);
}


/*
 * These forms of the constructor take a simple NULL-terminated
 * string and create a new CKString from this. The arguments are
 * left unchanged, and it remains the caller's responsibility to
 * manage the memory of these arrays.
 */
CKString::CKString( char *aString ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the operator=() do all the work for me
	*this = aString;
}


CKString::CKString( const char *aString ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the operator=() do all the work for me
	*this = aString;
}


/*
 * These forms of the constructor take an STL std::string and
 * create a new CKString from this. This is a convenience/bridge
 * method to continued use of STL while working this class into
 * the codebase.
 */
CKString::CKString( std::string & aString ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the operator=() do all the work for me
	*this = aString;
}


CKString::CKString( const std::string & aString ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the operator=() do all the work for me
	*this = aString;
}


/*
 * These forms of the constructor take a simple NULL-terminated
 * string and a starting and (optional) length parameter. The
 * idea is that these forms will create a new CKString from the
 * substring of the argument starting at the index 'aStartIndex'
 * which is zero-biased, and including 'aLength' of characters.
 * If 'aLength' is omitted the default is to include to the end
 * of the source string.
 */
CKString::CKString( char *aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString(aString, aStartIndex, aLength);
}


CKString::CKString( const char *aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString((char *)aString, aStartIndex, aLength);
}


/*
 * These forms of the constructor take another CKString
 * and a starting and (optional) length parameter. The
 * idea is that these forms will create a new CKString from the
 * substring of the argument starting at the index 'aStartIndex'
 * which is zero-biased, and including 'aLength' of characters.
 * If 'aLength' is omitted the default is to include to the end
 * of the source string.
 */
CKString::CKString( CKString & aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString(aString.mString, aStartIndex, aLength);
}



CKString::CKString( const CKString & aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString((char *)aString.mString, aStartIndex, aLength);
}


/*
 * These forms of the constructor take a STL std::string
 * and a starting and (optional) length parameter. The
 * idea is that these forms will create a new CKString from the
 * substring of the argument starting at the index 'aStartIndex'
 * which is zero-biased, and including 'aLength' of characters.
 * If 'aLength' is omitted the default is to include to the end
 * of the source string.
 */
CKString::CKString( std::string & aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString((char *)aString.c_str(), aStartIndex, aLength);
}


CKString::CKString( const std::string & aString, int aStartIndex, int aLength ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// simply call the substring initializer
	initWithSubString((char *)aString.c_str(), aStartIndex, aLength);
}


/*
 * This form of the constructor takes a single character and a
 * repeat count and creates a new CKString of that many of that
 * single character. Nice if you have to create a dividing line
 * and need 50 '-' characters all in a row.
 */
CKString::CKString( char aChar, int aRepeatCount ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// based on the length they want, set our capacity and size
	mSize = aRepeatCount;
	if (mSize >= mInitialCapacity) {
		mInitialCapacity += mSize;
	}

	// now create the correctly sized buffer for holding this guy
	mString = new char[mInitialCapacity];
	if (mString == NULL) {
		std::ostringstream	msg;
		msg << "CKString::CKString(char, int) - the initial storage for this "
			"string was to be " << mInitialCapacity << " chars, but the creation "
			"failed. Please look into this allocation error as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// clear out all of it first
		bzero(mString, mInitialCapacity);
		// now see if we need to copy anything into this buffer
		for (int i = 0; i < mSize; i++) {
			mString[i] = aChar;
		}
		// ...and set the capacity of this guy as well
		mCapacity = mInitialCapacity;
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKString::CKString( CKString & anOther ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the '=' operator take care of this for me
	this->operator=(anOther);
}


CKString::CKString( const CKString & anOther ) :
	mString(NULL),
	mSize(0),
	mCapacity(0),
	mInitialCapacity(DEFAULT_STARTING_SIZE),
	mCapacityIncrement(DEFAULT_INCREMENT_SIZE)
{
	// let the '=' operator take care of this for me
	this->operator=((CKString &)anOther);
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKString::~CKString()
{
	// delete whatever we have in use right now
	if (mString != NULL) {
		delete [] mString;
		mString = NULL;
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKString & CKString::operator=( CKString & anOther )
{
	// if we already have something then we need to drop it
	if (mString != NULL) {
		delete [] mString;
		mString = NULL;
		mSize = 0;
		mCapacity = 0;
	}

	// copy in all the easy things from the other guy
	mSize = anOther.mSize;
	mCapacity = anOther.mCapacity;
	mInitialCapacity = anOther.mInitialCapacity;
	mCapacityIncrement = anOther.mCapacityIncrement;

	// now create the correctly sized buffer for holding this guy
	mString = new char[mCapacity];
	if (mString == NULL) {
		std::ostringstream	msg;
		msg << "CKString::operator=(CKString &) - the initial storage for this "
			"string was to be " << mCapacity << " chars, but the creation "
			"failed. Please look into this allocation error as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// clear out all of it first
		bzero(mString, mCapacity);
		// now copy everything into this buffer
		memcpy(mString, anOther.mString, mSize);
	}

	return *this;
}


CKString & CKString::operator=( const CKString & anOther )
{
	// do a simple cast to remove the 'const'
	return this->operator=((CKString &)anOther);
}


/*
 * These forms of the '=' operator are for easy assignment to the
 * CKString. They try to cover what you're likely to be doing with
 * the string, but more can always be added if they are insufficient.
 */
CKString & CKString::operator=( std::string & anSTLString )
{
	return this->operator=((char *)anSTLString.c_str());
}


CKString & CKString::operator=( const std::string & anSTLString )
{
	return this->operator=((char *)anSTLString.c_str());
}


CKString & CKString::operator=( char *aCString )
{
	// if we already have something then we need to drop it
	if (mString != NULL) {
		delete [] mString;
		mString = NULL;
		mSize = 0;
		mCapacity = 0;
	}

	// if we have something, then get it's size and use it
	if (aCString != NULL) {
		mSize = strlen(aCString);
		if (mSize >= mInitialCapacity) {
			mInitialCapacity += mSize;
		}
	}

	// now create the correctly sized buffer for holding this guy
	mString = new char[mInitialCapacity];
	if (mString == NULL) {
		std::ostringstream	msg;
		msg << "CKString::operator=(char *) - the initial storage for this "
			"string was to be " << mInitialCapacity << " chars, but the creation "
			"failed. Please look into this allocation error as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// clear out all of it first
		bzero(mString, mInitialCapacity);
		// now see if we need to copy anything into this buffer
		if (aCString != NULL) {
			memcpy(mString, aCString, mSize);
		}
		// ...and set the capacity of this guy as well
		mCapacity = mInitialCapacity;
	}

	return *this;
}


CKString & CKString::operator=( const char *aCString )
{
	// do a simple cast to get rid of the 'const'
	return this->operator=((char *)aCString);
}


CKString & CKString::operator=( char aChar )
{
	// make a simple character array
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	// now use the operator=() to set this guy
	return this->operator=(c);
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * These methods add the different kinds of strings to the
 * end of the existing string and return 'true' if successful,
 * or 'false' if not. There are quite a few to be as generally
 * useful as possible.
 */
bool CKString::append( CKString & aString )
{
	return append((char *)aString.mString);
}


bool CKString::append( const CKString & aString )
{
	return append((char *)aString.mString);
}


bool CKString::append( char *aCString )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::append(char *) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure we have something to do, and get the length
	int		newChars = 0;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::append(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// this is what we have to add to the buffer
			newChars = strlen(aCString);
		}
	}

	// see if we need to make more room in the buffer
	if (!error) {
		if ((mSize + newChars + 1) >= mCapacity) {
			// make room for it all and then a little growth
			char	*more = new char[mSize + newChars + mCapacityIncrement];
			if (more == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::append(char *) - the existing buffer of " <<
					mCapacity << " chars was not sufficient to hold the " <<
					(mSize + newChars + 1) << " characters, and while trying to "
					"create more space we failed. This is a serious allocation "
					"error that needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// clear this puppy out
				bzero(more, (mSize + newChars + mCapacityIncrement));
				// move over all the old data
				memcpy(more, mString, mSize);
				// delete the old buffer as it's used up
				delete [] mString;
				mString = NULL;
				// save the new buffer for the string
				mString = more;
			}
		}
	}

	// now copy over the new characters to the string
	if (!error) {
		strncpy(&(mString[mSize]), aCString, newChars);
		mSize += newChars;
	}

	return !error;
}


bool CKString::append( const char *aCString )
{
	return append((char *)aCString);
}


bool CKString::append( std::string & aSTLString )
{
	return append((char *)aSTLString.c_str());
}


bool CKString::append( const std::string & aSTLString )
{
	return append((char *)aSTLString.c_str());
}


/*
 * These methods add the string representation of the different
 * kinds of base data objects to the end of the existing string
 * and return 'true' if successful, or 'false' if not. There are
 * quite a few to be as generally useful as possible.
 */
bool CKString::append( char aChar )
{
	// make a simple buffer to add this guy in
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return append(c);
}


bool CKString::append( int anInteger, int aNumOfDigits )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumOfDigits > 0) {
		snprintf(c, 79, "%*d", aNumOfDigits, anInteger);
	} else {
		snprintf(c, 79, "%d", anInteger);
	}
	return append(c);
}


bool CKString::append( long aLong, int aNumOfDigits )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumOfDigits > 0) {
		snprintf(c, 79, "%*ld", aNumOfDigits, aLong);
	} else {
		snprintf(c, 79, "%ld", aLong);
	}
	return append(c);
}


bool CKString::append( double aDouble )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	snprintf(c, 79, "%f", aDouble);
	return append(c);
}


/*
 * These methods add the different kinds of strings to the
 * beginning of the existing string and return 'true' if successful,
 * or 'false' if not. There are quite a few to be as generally
 * useful as possible.
 */
bool CKString::prepend( CKString & aString )
{
	return prepend(aString.mString);
}


bool CKString::prepend( const CKString & aString )
{
	return prepend((char *)aString.mString);
}


bool CKString::prepend( char *aCString )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::prepend(char *) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure we have something to do, and get the length
	int		newChars = 0;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::prepend(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// this is what we have to add to the buffer
			newChars = strlen(aCString);
		}
	}

	// see if we need to make more room in the buffer
	if (!error) {
		if ((mSize + newChars + 1) >= mCapacity) {
			// make room for it all and then a little growth
			char	*more = new char[mSize + newChars + mCapacityIncrement];
			if (more == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::prepend(char *) - the existing buffer of " <<
					mCapacity << " chars was not sufficient to hold the " <<
					(mSize + newChars + 1) << " characters, and while trying to "
					"create more space we failed. This is a serious allocation "
					"error that needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// clear this puppy out
				bzero(more, (mSize + newChars + mCapacityIncrement));
				// move over all the old data
				memcpy(more, mString, mSize);
				// delete the old buffer as it's used up
				delete [] mString;
				mString = NULL;
				// save the new buffer for the string
				mString = more;
			}
		}
	}

	// now copy over the new characters to the string
	if (!error) {
		// first, move the existing string over the right amount
		memmove(&(mString[newChars]), mString, mSize);
		// now copy in the front the new characters
		strncpy(mString, aCString, newChars);
		mSize += newChars;
	}

	return !error;
}


bool CKString::prepend( const char *aCString )
{
	return prepend((char *)aCString);
}


bool CKString::prepend( std::string & aSTLString )
{
	return prepend((char *)aSTLString.c_str());
}


bool CKString::prepend( const std::string & aSTLString )
{
	return prepend((char *)aSTLString.c_str());
}


/*
 * These methods add the string representation of the different
 * kinds of base data objects to the beginning of the existing string
 * and return 'true' if successful, or 'false' if not. There are
 * quite a few to be as generally useful as possible.
 */
bool CKString::prepend( char aChar )
{
	// make a simple buffer to add this guy in
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return prepend(c);
}


bool CKString::prepend( int anInteger, int aNumOfDigits )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumOfDigits > 0) {
		snprintf(c, 79, "%*d", aNumOfDigits, anInteger);
	} else {
		snprintf(c, 79, "%d", anInteger);
	}
	return prepend(c);
}


bool CKString::prepend( long aLong, int aNumOfDigits )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumOfDigits > 0) {
		snprintf(c, 79, "%*ld", aNumOfDigits, aLong);
	} else {
		snprintf(c, 79, "%ld", aLong);
	}
	return prepend(c);
}


bool CKString::prepend( double aDouble )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	snprintf(c, 79, "%f", aDouble);
	return prepend(c);
}


/*
 * This method returns the current size of the string in characters.
 * This is the same as the call to length(), but it matches some of
 * the STL-isms a bit and is therefore really a convenience method.
 */
int CKString::size()
{
	return mSize;
}


int CKString::size() const
{
	return mSize;
}


/*
 * This method returns the number of characters in the string and
 * is the same as the call to size(). It's simply here as another
 * way to get the same data.
 */
int CKString::length()
{
	return mSize;
}


int CKString::length() const
{
	return mSize;
}


/*
 * This method returns true if the representation of the string
 * is really empty. This may not mean that the instance is without
 * storage allocated, it only means that there's no information in
 * that storage.
 */
bool CKString::empty()
{
	return (mSize == 0);
}


bool CKString::empty() const
{
	return (mSize == 0);
}


/*
 * This method returns the total currently allocated capacity of
 * this string. This is always going to be greater than the size
 * of the string as there is meant to be a little breathing room
 * at the end of the string.
 */
int CKString::capacity()
{
	return mCapacity;
}


int CKString::capacity() const
{
	return mCapacity;
}


/*
 * This method can be used to clear up *most* of the excess free
 * space that this CKString is using. It won't get any smaller
 * than the initial size, but it's possible to free up a lot of
 * space if this guy got very big and now is quite small.
 */
bool CKString::compact()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::compact() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if there's "excess" space
	if (!error) {
		if ((mCapacity > mInitialCapacity) &&
			(mCapacity > (mSize + mCapacityIncrement))) {
			// OK, let's create just what we need and move into that
			char	*less = new char[mSize + mCapacityIncrement];
			if (less == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::compact() - the new, smaller buffer of " <<
					(mSize + mCapacityIncrement) << " chars could not be created "
					"This is a serious allocation problem that needs to be "
					"looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// clear this puppy out
				bzero(less, (mSize + mCapacityIncrement));
				// move over all the old data
				memcpy(less, mString, mSize);
				// delete the old buffer as it's used up
				delete [] mString;
				mString = NULL;
				// save the new buffer for the string
				mString = less;
			}
		}
	}

	return !error;
}


bool CKString::compact() const
{
	return ((CKString *)this)->compact();
}


/*
 * This method returns the actual pointer to the C-String that
 * represents the contents of this CKString. Since this is the
 * real McCoy, if you wish to keep it around for something, please
 * make a copy of it right away as there is no guarantees as to
 * what might happen to it if the string grows or shrinks.
 */
const char *CKString::c_str()
{
	return mString;
}


const char *CKString::c_str() const
{
	return mString;
}


/*
 * This method returns an STL std::string that contains a copy of
 * the contents of this CKString. Since it's a copy, the caller is
 * free to use it as he sees fit.
 */
std::string CKString::stl_str()
{
	return std::string(mString);
}


std::string CKString::stl_str() const
{
	return std::string(mString);
}


/*
 * This method is a simple indexing operator so that we can easily
 * get the individual characters in the CKString. If the argument
 * is -1, then the default is to get the *LAST* non-NULL
 * character in the string. This is nice in that we're often
 * looking at the last character and getting the length for no
 * other reason.
 */
char CKString::operator[]( int aPosition )
{
	bool		error = false;
	char		retval = '\0';

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::operator[](int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// check to see that it's the in the right range
	if (!error) {
		if (aPosition == -1) {
			retval = mString[mSize - 1];
		} else if ((aPosition < 0) || (aPosition >= mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::operator[](int) - the CKString's storage has " <<
				mSize << " characters in it and that means that character "
				"position " << aPosition << " is an illegal value. Please make "
				"sure to use only valid positions.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			retval = mString[aPosition];
		}
	}

	return retval;
}


char CKString::operator[]( int aPosition ) const
{
	return ((CKString *)this)->operator[](aPosition);
}


/********************************************************
 *
 *                Manipulation Methods
 *
 ********************************************************/
/*
 * These operators add the different kinds of strings to the
 * beginning of the existing string and return 'true' if successful,
 * or 'false' if not. There are quite a few to be as generally
 * useful as possible.
 */
CKString & CKString::operator+=( CKString & aString )
{
	append(aString.mString);
	return *this;
}


CKString & CKString::operator+=( const CKString & aString )
{
	append((char *)aString.mString);
	return *this;
}


CKString & CKString::operator+=( char *aCString )
{
	append(aCString);
	return *this;
}


CKString & CKString::operator+=( const char *aCString )
{
	append((char *)aCString);
	return *this;
}


CKString & CKString::operator+=( std::string & aSTLString )
{
	append((char *)aSTLString.c_str());
	return *this;
}


CKString & CKString::operator+=( const std::string & aSTLString )
{
	append((char *)aSTLString.c_str());
	return *this;
}


/*
 * These operators add the string representation of the different
 * kinds of base data objects to the beginning of the existing string
 * and return 'true' if successful, or 'false' if not. There are
 * quite a few to be as generally useful as possible.
 */
CKString & CKString::operator+=( int anInteger )
{
	append(anInteger);
	return *this;
}


CKString & CKString::operator+=( long aLong )
{
	append(aLong);
	return *this;
}


CKString & CKString::operator+=( double aDouble )
{
	append(aDouble);
	return *this;
}


CKString & CKString::operator+=( char aChar )
{
	append(aChar);
	return *this;
}


/*
 * These operators are the general-purpose concatenation operators
 * that you'd expect to have in a nice string class. With these
 * you can pretty much build up a CKString in just about any way
 * you can imagine.
 */
CKString operator+( CKString & aString, CKString & anOther )
{
	CKString	retval(aString);
	retval.append(anOther.mString);
	return retval;
}


CKString operator+( const CKString & aString, CKString & anOther )
{
	CKString	retval(aString);
	retval.append(anOther.mString);
	return retval;
}


CKString operator+( CKString & aString, const CKString & anOther )
{
	CKString	retval(aString);
	retval.append((char *)anOther.mString);
	return retval;
}


CKString operator+( const CKString & aString, const CKString & anOther )
{
	CKString	retval(aString);
	retval.append((char *)anOther.mString);
	return retval;
}


CKString operator+( CKString & aString, char *aCString )
{
	CKString	retval(aString);
	retval.append(aCString);
	return retval;
}


CKString operator+( CKString & aString, const char *aCString )
{
	CKString	retval(aString);
	retval.append((char *)aCString);
	return retval;
}


CKString operator+( const CKString & aString, char *aCString )
{
	CKString	retval(aString);
	retval.append(aCString);
	return retval;
}


CKString operator+( const CKString & aString, const char *aCString )
{
	CKString	retval(aString);
	retval.append((char *)aCString);
	return retval;
}


CKString operator+( char *aCString, CKString & aString )
{
	CKString	retval(aCString);
	retval.append(aString.mString);
	return retval;
}


CKString operator+( char *aCString, const CKString & aString )
{
	CKString	retval(aCString);
	retval.append((char *)aString.mString);
	return retval;
}


CKString operator+( const char *aCString, CKString & aString )
{
	CKString	retval((char *)aCString);
	retval.append(aString.mString);
	return retval;
}


CKString operator+( const char *aCString, const CKString & aString )
{
	CKString	retval((char *)aCString);
	retval.append((char *)aString.mString);
	return retval;
}


CKString operator+( CKString & aString, std::string & anSTLString )
{
	CKString	retval(aString);
	retval.append((char *)anSTLString.c_str());
	return retval;
}


CKString operator+( CKString & aString, const std::string & anSTLString )
{
	CKString	retval(aString);
	retval.append((char *)anSTLString.c_str());
	return retval;
}


CKString operator+( const CKString & aString, std::string & anSTLString )
{
	CKString	retval(aString);
	retval.append((char *)anSTLString.c_str());
	return retval;
}


CKString operator+( const CKString & aString, const std::string & anSTLString )
{
	CKString	retval(aString);
	retval.append((char *)anSTLString.c_str());
	return retval;
}


CKString operator+( std::string & anSTLString, CKString & aString )
{
	CKString	retval(anSTLString.c_str());
	retval.append(aString.mString);
	return retval;
}


CKString operator+( std::string & anSTLString, const CKString & aString )
{
	CKString	retval(anSTLString.c_str());
	retval.append((char *)aString.mString);
	return retval;
}


CKString operator+( const std::string & anSTLString, CKString & aString )
{
	CKString	retval((char *)anSTLString.c_str());
	retval.append(aString.mString);
	return retval;
}


CKString operator+( const std::string & anSTLString, const CKString & aString )
{
	CKString	retval((char *)anSTLString.c_str());
	retval.append((char *)aString.mString);
	return retval;
}


CKString operator+( CKString & aString, int anInteger )
{
	CKString	retval(aString);
	retval.append(anInteger);
	return retval;
}


CKString operator+( const CKString & aString, int anInteger )
{
	CKString	retval(aString);
	retval.append(anInteger);
	return retval;
}


CKString operator+( int anInteger, CKString & aString )
{
	CKString	retval(anInteger);
	retval.append(aString.mString);
	return retval;
}


CKString operator+( int anInteger, const CKString & aString )
{
	CKString	retval(anInteger);
	retval.append((char *)aString.mString);
	return retval;
}


CKString operator+( CKString & aString, long aLong )
{
	CKString	retval(aString);
	retval.append(aLong);
	return retval;
}


CKString operator+( const CKString & aString, long aLong )
{
	CKString	retval(aString);
	retval.append(aLong);
	return retval;
}


CKString operator+( long aLong, CKString & aString )
{
	CKString	retval(aString);
	retval.prepend(aLong);
	return retval;
}


CKString operator+( long aLong, const CKString & aString )
{
	CKString	retval(aString);
	retval.prepend(aLong);
	return retval;
}


CKString operator+( CKString & aString, double aDouble )
{
	CKString	retval(aString);
	retval.append(aDouble);
	return retval;
}


CKString operator+( const CKString & aString, double aDouble )
{
	CKString	retval(aString);
	retval.append(aDouble);
	return retval;
}


CKString operator+( double aDouble, CKString & aString )
{
	CKString	retval(aString);
	retval.prepend(aDouble);
	return retval;
}


CKString operator+( double aDouble, const CKString & aString )
{
	CKString	retval(aString);
	retval.prepend(aDouble);
	return retval;
}


CKString operator+( CKString & aString, char aChar )
{
	CKString	retval(aString);
	retval.append(aChar);
	return retval;
}


CKString operator+( const CKString & aString, char aChar )
{
	CKString	retval(aString);
	retval.append(aChar);
	return retval;
}


CKString operator+( char aChar, CKString & aString )
{
	CKString	retval(aChar);
	retval.append(aString.mString);
	return retval;
}


CKString operator+( char aChar, const CKString & aString )
{
	CKString	retval(aChar);
	retval.append((char *)aString.mString);
	return retval;
}


/********************************************************
 *
 *                Text Handling Methods
 *
 ********************************************************/
/*
 * This method goes through all the characters in the string
 * and makes sure that they are all uppercase. It's really
 * pretty simple, but it's awfully handy not to have to implement
 * this in all the projects.
 */
bool CKString::toUpper()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::toUpper() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now just call toupper() on all the characters
	if (!error) {
		for (int i = 0; i < mSize; i++) {
			mString[i] = toupper((int)mString[i]);
		}
	}

	return !error;
}


bool CKString::toUpper() const
{
	return ((CKString *)this)->toUpper();
}


/*
 * This method goes through all the characters in the string
 * and makes sure that they are all lowercase. It's really
 * pretty simple, but it's awfully handy not to have to implement
 * this in all the projects.
 */
bool CKString::toLower()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::toLower() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now just call tolower() on all the characters
	if (!error) {
		for (int i = 0; i < mSize; i++) {
			mString[i] = tolower((int)mString[i]);
		}
	}

	return !error;
}


bool CKString::toLower() const
{
	return ((CKString *)this)->toLower();
}


/*
 * This method returns a new CKString based on the substring
 * of the current string defined to start at 'aStartingPos'
 * and including 'aLength' number of characters (all positions
 * in the string are zero-biased). If this substring does not
 * exist in the string due to position or length, then an
 * excepton will be thrown. If the length is left off, then
 * the "end of the string" is the default behavior.
 */
CKString CKString::substr( int aStartingPos, int aLength )
{
	bool		error = false;
	CKString	retval;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substr(int, int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the substring exists
	if (!error) {
		if ((aStartingPos < 0) || (aStartingPos >= mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substr(int, int) - the provided starting position "
				"of " << aStartingPos << " is not contained in this string. "
				"Please make sure that you ask for a substring that's within the "
				"limits of this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if ((aLength >= 0) && ((aStartingPos + aLength) > mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substr(int, int) - the requested length of " <<
				aLength << " characters starting at the starting position of " <<
				aStartingPos << " is not contained in this string. Please make "
				"sure that you ask for a substring that's within the limits of "
				"this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make room in the return value for this substring
	if (!error) {
		int		newSize = aLength;
		if (aLength == -1) {
			newSize = mSize - aStartingPos;
		}
		if (retval.mSize < (newSize + mCapacityIncrement)) {
			if (!retval.resize(newSize + mCapacityIncrement)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::substr(int, int) - while trying to resize the "
					"return value to contain at least " << newSize << " characters "
					"a problem was encountered. Please check the logs for a "
					"possible cause.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
		// now if we're here, then we just need to copy over the substring
		strncpy(retval.mString, &(mString[aStartingPos]), newSize);
		retval.mSize = newSize;
	}

	return retval;
}


CKString CKString::substr( int aStartingPos, int aLength ) const
{
	return ((CKString *)this)->substr(aStartingPos, aLength);
}


/*
 * Often times, you want to get the first part of a string up
 * to, and including, a certain character or string. This method
 * does just that. If the character or string in question is not
 * in the current string, then an empty string will be returned.
 */
CKString CKString::substrTo( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return substrTo(c);
}


CKString CKString::substrTo( char aChar ) const
{
	return ((CKString *)this)->substrTo(aChar);
}


CKString CKString::substrTo( char *aCString )
{
	bool		error = false;
	CKString	retval;

	// make sure we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substrTo(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if the substring exists in this string
	if (!error) {
		int		pos = find(aCString);
		if (pos >= 0) {
			retval = substr(0, (pos + strlen(aCString)));
		}
	}

	return retval;
}


CKString CKString::substrTo( char *aCString ) const
{
	return ((CKString *)this)->substrTo(aCString);
}


CKString CKString::substrTo( const char *aCString )
{
	return substrTo((char *)aCString);
}


CKString CKString::substrTo( const char *aCString ) const
{
	return ((CKString *)this)->substrTo((char *)aCString);
}


CKString CKString::substrTo( std::string & anSTLString )
{
	return substrTo(anSTLString.c_str());
}


CKString CKString::substrTo( std::string & anSTLString ) const
{
	return ((CKString *)this)->substrTo(anSTLString.c_str());
}


CKString CKString::substrTo( const std::string & anSTLString )
{
	return substrTo((char *)anSTLString.c_str());
}


CKString CKString::substrTo( const std::string & anSTLString ) const
{
	return ((CKString *)this)->substrTo((char *)anSTLString.c_str());
}


CKString CKString::substrTo( CKString & aString )
{
	return substrTo(aString.mString);
}


CKString CKString::substrTo( CKString & aString ) const
{
	return ((CKString *)this)->substrTo(aString.mString);
}


CKString CKString::substrTo( const CKString & aString )
{
	return substrTo((char *)aString.mString);
}


CKString CKString::substrTo( const CKString & aString ) const
{
	return ((CKString *)this)->substrTo((char *)aString.mString);
}


/*
 * Often times, you want to get the last part of a string starting
 * with, and including, a certain character or string. This method
 * does just that. If the character or string in question is not
 * in the current string, then an empty string will be returned.
 */
CKString CKString::substrFrom( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return substrFrom(c);
}


CKString CKString::substrFrom( char aChar ) const
{
	return ((CKString *)this)->substrFrom(aChar);
}


CKString CKString::substrFrom( char *aCString )
{
	bool		error = false;
	CKString	retval;

	// make sure we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substrFrom(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if the substring exists in this string
	if (!error) {
		int		pos = find(aCString);
		if (pos >= 0) {
			retval = substr(pos);
		}
	}

	return retval;
}


CKString CKString::substrFrom( char *aCString ) const
{
	return ((CKString *)this)->substrFrom(aCString);
}


CKString CKString::substrFrom( const char *aCString )
{
	return substrFrom((char *)aCString);
}


CKString CKString::substrFrom( const char *aCString ) const
{
	return ((CKString *)this)->substrFrom((char *)aCString);
}


CKString CKString::substrFrom( std::string & anSTLString )
{
	return substrFrom(anSTLString.c_str());
}


CKString CKString::substrFrom( std::string & anSTLString ) const
{
	return ((CKString *)this)->substrFrom(anSTLString.c_str());
}


CKString CKString::substrFrom( const std::string & anSTLString )
{
	return substrFrom((char *)anSTLString.c_str());
}


CKString CKString::substrFrom( const std::string & anSTLString ) const
{
	return ((CKString *)this)->substrFrom((char *)anSTLString.c_str());
}


CKString CKString::substrFrom( CKString & aString )
{
	return substrFrom(aString.mString);
}


CKString CKString::substrFrom( CKString & aString ) const
{
	return ((CKString *)this)->substrFrom(aString.mString);
}


CKString CKString::substrFrom( const CKString & aString )
{
	return substrFrom((char *)aString.mString);
}


CKString CKString::substrFrom( const CKString & aString ) const
{
	return ((CKString *)this)->substrFrom((char *)aString.mString);
}


/*
 * Often times, you want to get the first part of a string up
 * to, but NOT including, a certain character or string. This
 * method does just that. If the character or string in question
 * is not in the current string, then an empty string will be
 * returned.
 */
CKString CKString::substrBefore( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return substrBefore(c);
}


CKString CKString::substrBefore( char aChar ) const
{
	return ((CKString *)this)->substrBefore(aChar);
}


CKString CKString::substrBefore( char *aCString )
{
	bool		error = false;
	CKString	retval;

	// make sure we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substrBefore(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if the substring exists in this string
	if (!error) {
		int		pos = find(aCString);
		if (pos > 0) {
			retval = substr(0, pos);
		}
	}

	return retval;
}


CKString CKString::substrBefore( char *aCString ) const
{
	return ((CKString *)this)->substrBefore(aCString);
}


CKString CKString::substrBefore( const char *aCString )
{
	return substrBefore((char *)aCString);
}


CKString CKString::substrBefore( const char *aCString ) const
{
	return ((CKString *)this)->substrBefore((char *)aCString);
}


CKString CKString::substrBefore( std::string & anSTLString )
{
	return substrBefore(anSTLString.c_str());
}


CKString CKString::substrBefore( std::string & anSTLString ) const
{
	return ((CKString *)this)->substrBefore(anSTLString.c_str());
}


CKString CKString::substrBefore( const std::string & anSTLString )
{
	return substrBefore((char *)anSTLString.c_str());
}


CKString CKString::substrBefore( const std::string & anSTLString ) const
{
	return ((CKString *)this)->substrBefore((char *)anSTLString.c_str());
}


CKString CKString::substrBefore( CKString & aString )
{
	return substrBefore(aString.mString);
}


CKString CKString::substrBefore( CKString & aString ) const
{
	return ((CKString *)this)->substrBefore(aString.mString);
}


CKString CKString::substrBefore( const CKString & aString )
{
	return substrBefore((char *)aString.mString);
}


CKString CKString::substrBefore( const CKString & aString ) const
{
	return ((CKString *)this)->substrBefore((char *)aString.mString);
}


/*
 * Often times, you want to get the last part of a string starting
 * with, but NOT including, a certain character or string. This
 * method does just that. If the character or string in question
 * is not in the current string, then an empty string will be
 * returned.
 */
CKString CKString::substrAfter( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return substrAfter(c);
}


CKString CKString::substrAfter( char aChar ) const
{
	return ((CKString *)this)->substrAfter(aChar);
}


CKString CKString::substrAfter( char *aCString )
{
	bool		error = false;
	CKString	retval;

	// make sure we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substrAfter(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if the substring exists in this string
	if (!error) {
		int		pos = find(aCString);
		if (pos >= 0) {
			retval = substr(pos + strlen(aCString));
		}
	}

	return retval;
}


CKString CKString::substrAfter( char *aCString ) const
{
	return ((CKString *)this)->substrAfter(aCString);
}


CKString CKString::substrAfter( const char *aCString )
{
	return substrAfter((char *)aCString);
}


CKString CKString::substrAfter( const char *aCString ) const
{
	return ((CKString *)this)->substrAfter((char *)aCString);
}


CKString CKString::substrAfter( std::string & anSTLString )
{
	return substrAfter(anSTLString.c_str());
}


CKString CKString::substrAfter( std::string & anSTLString ) const
{
	return ((CKString *)this)->substrAfter(anSTLString.c_str());
}


CKString CKString::substrAfter( const std::string & anSTLString )
{
	return substrAfter((char *)anSTLString.c_str());
}


CKString CKString::substrAfter( const std::string & anSTLString ) const
{
	return ((CKString *)this)->substrAfter((char *)anSTLString.c_str());
}


CKString CKString::substrAfter( CKString & aString )
{
	return substrAfter(aString.mString);
}


CKString CKString::substrAfter( CKString & aString ) const
{
	return ((CKString *)this)->substrAfter(aString.mString);
}


CKString CKString::substrAfter( const CKString & aString )
{
	return substrAfter((char *)aString.mString);
}


CKString CKString::substrAfter( const CKString & aString ) const
{
	return ((CKString *)this)->substrAfter((char *)aString.mString);
}


/*
 * Straight out of BASIC, this method returns the 'n' left-most
 * characters in this string as a new CKString. If 'n' characters
 * do not exist in this string, an exception will be thrown, so
 * please check to see what you're asking for.
 */
CKString CKString::left( int aNumOfChars )
{
	bool		error = false;
	CKString	retval;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::left(int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the substring exists
	if (!error) {
		if (aNumOfChars > mSize) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::left(int) - the number of characters to return: " <<
				aNumOfChars << " is not contained in this string. Please make "
				"sure that you ask for a substring that's within the limits of "
				"this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make the substring
	if (!error) {
		retval = substr(0, aNumOfChars);
	}

	return retval;
}


CKString CKString::left( int aNumOfChars ) const
{
	return ((CKString *)this)->left(aNumOfChars);
}


/*
 * Straight out of BASIC, this method returns the 'n' right-most
 * characters in thCKString::is string as a new CKString. If 'n' characters
 * do not exist in this string, an exception will be thrown, so
 * please check to see what you're asking for.
 */
CKString CKString::right( int aNumOfChars )
{
	bool		error = false;
	CKString	retval;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::right(int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the substring exists
	if (!error) {
		if (aNumOfChars > mSize) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::right(int) - the number of characters to return: " <<
				aNumOfChars << " is not contained in this string. Please make "
				"sure that you ask for a substring that's within the limits of "
				"this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make the substring
	if (!error) {
		retval = substr(mSize - aNumOfChars);
	}

	return retval;
}


CKString CKString::right( int aNumOfChars ) const
{
	return ((CKString *)this)->right(aNumOfChars);
}


/*
 * Straight out of BASIC, this method returns a new substring
 * starting at 'aStartPos' through (including) 'anEndPos' -
 * where both are zero-biased index values. This is a simple
 * way to get a substring that fits the old BASIC calling
 * convention.
 */
CKString CKString::mid( int aStartPos, int anEndPos )
{
	bool		error = false;
	CKString	retval;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::mid(int, int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the substring exists
	if (!error) {
		if ((aStartPos < 0) || (anEndPos >= mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::mid(int, int) - the substring defined by the "
				"positions: " << aStartPos << " to " << anEndPos << " is not "
				"contained in this string. Please make sure that you ask for a "
				"substring that's within the limits of this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make the substring
	if (!error) {
		retval = substr(aStartPos, (anEndPos - aStartPos + 1));
	}

	return retval;
}


CKString CKString::mid( int aStartPos, int anEndPos ) const
{
	return ((CKString *)this)->mid(aStartPos, anEndPos);
}


/*
 * This method is a 'global search and replace' on a character
 * level for this string. It will go through the entire string
 * searching for all occurrences of 'anOld' character and replace
 * each with 'aNew' character. The number of times this happens
 * is the return value for the method.
 */
int CKString::replace( char anOld, char aNew )
{
	bool		error = false;
	int			retval = 0;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::replace(char, char) - the CKString's storage is "
				"NULL and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now we just need to look for the old and put in the new
	if (!error) {
		for (int i = 0; i < mSize; i++) {
			if (mString[i] == anOld) {
				mString[i] = aNew;
				retval++;
			}
		}
	}

	return retval;
}


int CKString::replace( char anOld, char aNew ) const
{
	return ((CKString *)this)->replace(anOld, aNew);
}


/*
 * These methods all search for the FIRST occurrence of the
 * argument in the current string and return the starting
 * position of the match if one is found, or a -1 if there
 * is nothing in the current string that matches the  argument.
 */
int CKString::find( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return find(c);
}


int CKString::find( char aChar ) const
{
	return ((CKString *)this)->find(aChar);
}


int CKString::find( char *aCString )
{
	bool		error = false;
	int			retval = -1;

	// make sure we have something to do
	int		matchLen = -1;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::find(char *) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get the length of this argument now for use later
			matchLen = strlen(aCString);
			if (matchLen == 0) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::find(char *) - the passed-in C-String is empty "
					"and that means that there's nothing I can do. Please make "
					"sure that the argument is not empty before calling this "
					"method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::find(char *) - the CKString's storage is NULL and "
				"that means that there's been a terrible data corruption problem. "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to scan through the local string looking for the first
	 * occurrence of the provided C-string and we need to do it as fast as
	 * possible as this is going to be a big bottleneck if we aren't
	 * careful.
	 */
	if (!error) {
		bool		match = false;
		for (int i = 0; i < mSize; i++) {
			// see if the next char matches the first in  the target
			if (mString[i] == aCString[0]) {
				// OK, we have a tentative match, check the others
				match = true;
				for (int j = 1; j < matchLen; j++) {
					if (mString[i+j] != aCString[j]) {
						// sorry, not everyone matched
						match = false;
						break;
					}
				}
				// was the match 100% successful?
				if (match == true) {
					retval = i;
					break;
				}
			}
		}
	}

	return retval;
}


int CKString::find( char *aCString ) const
{
	return ((CKString *)this)->find(aCString);
}


int CKString::find( const char *aCString )
{
	return find((char *)aCString);
}


int CKString::find( const char *aCString ) const
{
	return ((CKString *)this)->find((char *)aCString);
}


int CKString::find( CKString & aString )
{
	return find(aString.mString);
}


int CKString::find( const CKString & aString )
{
	return find((char *)aString.mString);
}


int CKString::find( CKString & aString ) const
{
	return ((CKString *)this)->find(aString.mString);
}


int CKString::find( const CKString & aString ) const
{
	return ((CKString *)this)->find((char *)aString.mString);
}


int CKString::find( std::string & anSTLString )
{
	return find(anSTLString.c_str());
}


int CKString::find( const std::string & anSTLString )
{
	return find((char *)anSTLString.c_str());
}


int CKString::find( std::string & anSTLString ) const
{
	return ((CKString *)this)->find(anSTLString.c_str());
}


int CKString::find( const std::string & anSTLString ) const
{
	return ((CKString *)this)->find((char *)anSTLString.c_str());
}


/*
 * These methods all search for the LAST occurrence of the
 * argument in the current string and return the starting
 * position of the match if one is found, or a -1 if there
 * is nothing in the current string that matches the  argument.
 */
int CKString::findLast( char aChar )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return findLast(c);
}


int CKString::findLast( char aChar ) const
{
	return ((CKString *)this)->findLast(aChar);
}


int CKString::findLast( char *aCString )
{
	bool		error = false;
	int			retval = -1;

	// make sure we have something to do
	int		matchLen = -1;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::findLast(char *) - the passed-in C-String is NULL "
				"and that means that there's nothing I can do. Please make sure "
				"that the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get the length of this argument now for use later
			matchLen = strlen(aCString);
			if (matchLen == 0) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::findLast(char *) - the passed-in C-String is "
					"empty and that means that there's nothing I can do. Please "
					"make sure that the argument is not empty before calling "
					"this method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::findLast(char *) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to scan through the local string looking for the first
	 * occurrence of the provided C-string and we need to do it as fast as
	 * possible as this is going to be a big bottleneck if we aren't
	 * careful.
	 */
	if (!error) {
		bool		match = false;
		int			last = matchLen - 1;
		for (int i = (mSize - 1); i >= 0; i--) {
			// see if the next char matches the first in  the target
			if (mString[i] == aCString[last]) {
				// OK, we have a tentative match, check the others
				match = true;
				for (int j = 1; j < matchLen; j++) {
					if (mString[i - j] != aCString[last - j]) {
						// sorry, not everyone matched
						match = false;
						break;
					}
				}
				// was the match 100% successful?
				if (match == true) {
					retval = i - last;
					break;
				}
			}
		}
	}

	return retval;
}


int CKString::findLast( char *aCString ) const
{
	return ((CKString *)this)->findLast(aCString);
}


int CKString::findLast( const char *aCString )
{
	return findLast((char *)aCString);
}


int CKString::findLast( const char *aCString ) const
{
	return ((CKString *)this)->findLast((char *)aCString);
}


int CKString::findLast( CKString & aString )
{
	return findLast(aString.mString);
}


int CKString::findLast( const CKString & aString )
{
	return findLast((char *)aString.mString);
}


int CKString::findLast( CKString & aString ) const
{
	return ((CKString *)this)->findLast(aString.mString);
}


int CKString::findLast( const CKString & aString ) const
{
	return ((CKString *)this)->findLast((char *)aString.mString);
}


int CKString::findLast( std::string & anSTLString )
{
	return findLast(anSTLString.c_str());
}


int CKString::findLast( const std::string & anSTLString )
{
	return findLast((char *)anSTLString.c_str());
}


int CKString::findLast( std::string & anSTLString ) const
{
	return ((CKString *)this)->findLast(anSTLString.c_str());
}


int CKString::findLast( const std::string & anSTLString ) const
{
	return ((CKString *)this)->findLast((char *)anSTLString.c_str());
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKStrings are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKString::operator==( CKString & anOther )
{
	return operator==(anOther.mString);
}


bool CKString::operator==( const CKString & anOther )
{
	return operator==((char *)anOther.mString);
}


bool CKString::operator==( CKString & anOther ) const
{
	return ((CKString *)this)->operator==(anOther.mString);
}


bool CKString::operator==( const CKString & anOther ) const
{
	return ((CKString *)this)->operator==((char *)anOther.mString);
}


/*
 * These operators check to see if the CKString is equal to a simple
 * NULL-terminated C-string. This is nice in that we don't have to
 * hassle with converting all string constants to CKStrings and then
 * do the comparison.
 */
bool CKString::operator==( char *aCString )
{
	bool		equal = true;

	// first, check for logical sanity
	if (equal) {
		if (aCString == NULL) {
			equal = false;
		}
	}

	// next, see if the sizes match
	if (mSize != (int)strlen(aCString)) {
		equal = false;
	}

	// check the buffer contents
	if (equal && (mSize > 0)) {
		if (strncmp(mString, aCString, mSize) != 0) {
			equal = false;
		}
	}

	return equal;
}


bool CKString::operator==( const char *aCString )
{
	return operator==((char *)aCString);
}


bool CKString::operator==( char *aCString ) const
{
	return ((CKString *)this)->operator==(aCString);
}


bool CKString::operator==( const char *aCString ) const
{
	return ((CKString *)this)->operator==((char *)aCString);
}


/*
 * These operators check to see if the CKString is equal to an STL
 * string. This is nice in that we don't have to hassle with converting
 * all STL std::string to CKStrings and then do the comparison.
 */
bool CKString::operator==( std::string & anSTLString )
{
	return operator==(anSTLString.c_str());
}


bool CKString::operator==( const std::string & anSTLString )
{
	return operator==((char *)anSTLString.c_str());
}


bool CKString::operator==( std::string & anSTLString ) const
{
	return ((CKString *)this)->operator==(anSTLString.c_str());
}


bool CKString::operator==( const std::string & anSTLString ) const
{
	return ((CKString *)this)->operator==((char *)anSTLString.c_str());
}


/*
 * This method checks to see if the two CKStrings are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKString::operator!=( CKString & anOther )
{
	return !operator==(anOther.mString);
}


bool CKString::operator!=( const CKString & anOther )
{
	return !operator==((char *)anOther.mString);
}


bool CKString::operator!=( CKString & anOther ) const
{
	return !((CKString *)this)->operator==(anOther.mString);
}


bool CKString::operator!=( const CKString & anOther ) const
{
	return !((CKString *)this)->operator==((char *)anOther.mString);
}


/*
 * These operators check to see if the CKString is not equal to a
 * simple NULL-terminated C-string. This is nice in that we don't have
 * to hassle with converting all string constants to CKStrings and
 * then do the comparison.
 */
bool CKString::operator!=( char * aCString )
{
	return !operator==(aCString);
}


bool CKString::operator!=( const char * aCString )
{
	return !operator==((char *)aCString);
}


bool CKString::operator!=( char * aCString ) const
{
	return !((CKString *)this)->operator==(aCString);
}


bool CKString::operator!=( const char * aCString ) const
{
	return !((CKString *)this)->operator==((char *)aCString);
}


/*
 * These operators check to see if the CKString is not equal to an STL
 * string. This is nice in that we don't have to hassle with converting
 * all STL std::string to CKStrings and then do the comparison.
 */
bool CKString::operator!=( std::string & anSTLString )
{
	return !operator==(anSTLString.c_str());
}


bool CKString::operator!=( const std::string & anSTLString )
{
	return !operator==((char *)anSTLString.c_str());
}


bool CKString::operator!=( std::string & anSTLString ) const
{
	return !((CKString *)this)->operator==(anSTLString.c_str());
}


bool CKString::operator!=( const std::string & anSTLString ) const
{
	return !((CKString *)this)->operator==((char *)anSTLString.c_str());
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this string. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a std::string.
 */
std::string CKString::toString() const
{
	return stl_str();
}


/*
 * This method is used in the constructors that take a substring of
 * a C-String, because the code would be repeated far too often and
 * lead to maintenance problems if we didn't encapsulate it here.
 */
void CKString::initWithSubString( char *aCString, int aStartIndex, int aLength )
{
	// make sure there's something to do
	if (aStartIndex < 0) {
		std::ostringstream	msg;
		msg << "CKString::initWithSubString(char *, int, int) - the starting "
			"index of this new string is to be: " << aStartIndex << " which is "
			"an illegal value. Please make sure that it's >=0.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// if we have something, then get it's size and use it
	if (aCString != NULL) {
		// get the size based on what we're being asked to get
		if (aLength < 0) {
			mSize = strlen(aCString) - aStartIndex;
		} else {
			mSize = aLength;
		}
		// check to see that it's making sense
		if (mSize < 0) {
			std::ostringstream	msg;
			msg << "CKString::initWithSubString(char *, int, int) - the initial "
				"size of this string was to be " << mSize << " chars based on "
				"your start index of: " << aStartIndex << " and a length of: " <<
				aLength << ", but the creation failed. Please look into this "
				"allocation error as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
		// now make sure that we have at least a little buffer on this guy
		if (mSize >= mInitialCapacity) {
			mInitialCapacity += mSize;
		}
	}

	// now create the correctly sized buffer for holding this guy
	mString = new char[mInitialCapacity];
	if (mString == NULL) {
		std::ostringstream	msg;
		msg << "CKString::initWithSubString(char *, int, int) - the initial "
			"storage for this string was to be " << mInitialCapacity << " chars, "
			"but the creation failed. Please look into this allocation error as "
			"soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// clear out all of it first
		bzero(mString, mInitialCapacity);
		// now see if we need to copy anything into this buffer
		if (aCString != NULL) {
			memcpy(mString, &(aCString[aStartIndex]), mSize);
		}
		// ...and set the capacity of this guy as well
		mCapacity = mInitialCapacity;
	}
}


/*
 * When the string needs to resize itself, this method is the best
 * way to do it. If the passed-in size is greater than the current
 * cappcity, the existing string will be copied over and the
 * excess will be filled with NULLs. If the size is smaller than
 * the current capacity, then the excess will be truncated, but
 * we'll always maintain a NULL-terminated string as that's a core
 * tenet of the data structure.
 */
bool CKString::resize( int aSize )
{
	bool		error = false;

	// first, make sure the size is something reasonable
	if (!error) {
		if (aSize <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::resize(int) - the requested size for this string "
				"is: " << aSize << " which is clearly an illegal value. The "
				"operation is aborted.";
				throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// we need to create a new buffer that's the requested size
	char	*resultant = NULL;
	if (!error) {
		resultant = new char[aSize];
		if (resultant == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::resize(int) - while trying to create a new buffer "
				"of " << aSize << " characters, an allocation error occurred. "
				"Please look into this as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// clear it all out right away
			bzero(resultant, aSize);
		}
	}

	/*
	 * Now we need to update this instance with the new buffer. First,
	 * we need to copy over any data from the existing string (if there
	 * is one) to this new buffer, and then update all the attributes
	 * about the string itself.
	 */
	if (!error) {
		// first, see if we have something to move into this new string
		if (mString != NULL) {
			// copy over just what will fit in the new string
			strncpy(resultant, mString, (aSize-1));
			// ...and delete the old string
			delete [] mString;
			mString = NULL;
		}
		// next, update all the ivars that have been impacted
		mString = resultant;
		mSize = strlen(resultant);
		mCapacity = aSize;
	}

	return !error;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKString & aString )
{
	aStream << aString.toString();

	return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CKString & aString )
{
	aStream << ((CKString &)aString).toString();

	return aStream;
}


/*
 * Sometimes it's useful to read an input stream into a CKString. This
 * operator<<() does just that.
 */
std::ostream & operator<<( CKString & aString, std::ostream & aStream )
{
	return aStream;
}


std::ostream & operator<<( const CKString & aString, std::ostream & aStream )
{
	return aStream;
}
