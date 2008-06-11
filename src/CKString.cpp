/*
 * CKString.cpp - this file implements a class that can be used to represent a
 *                general string. This is an alternative to the STL
 *                std::string because there are several problems with STL and
 *                GCC, and rather than fight those problems, it's easier to
 *                make an object with the subset of features that we really
 *                need and leave out the problems that STL brings.
 *
 * $Id: CKString.cpp,v 1.33 2008/06/11 23:09:46 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <string.h>
#include <strings.h>
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
		memset(mString, aChar, mSize);
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
	/*
	 * We have to watch out for someone setting this string equal
	 * to itself. If they did, we'd wipe out what is in us for what's
	 * in the other, but since the other *is* us, we'd have nothing
	 * to put back in. So we have to make sure we're not doing this
	 * to ourselves.
	 */
	if (this != & anOther) {
		/*
		 * Next, let's see if what we need to take can already fit in what
		 * we have allocated. If so, then let's just copy it in, but if not,
		 * then we need to make a bigger buffer and copy it in.
		 */
		mSize = anOther.mSize;
		if (mSize >= mCapacity) {
			// make the new capacity just enough to hold this guy
			mCapacity = mSize + 1;

			// drop the old buffer, if we had one
			if (mString != NULL) {
				delete [] mString;
				mString = NULL;
			}

			// create the new one
			mString = new char[mCapacity];
			if (mString == NULL) {
				std::ostringstream	msg;
				msg << "CKString::operator=(CKString &) - the storage needed for "
					"this string is " << mCapacity << " chars, but the creation "
					"failed. Please look into this allocation error as soon as "
					"possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}

		// now let's clear out what we have and copy in the string
		bzero(mString, mCapacity);
		// now see if we need to copy anything into this buffer
		if (anOther.mString != NULL) {
			memcpy(mString, anOther.mString, mSize);
		}
	}

	return *this;
}


CKString & CKString::operator=( const CKString & anOther )
{
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
	/*
	 * First, let's see if what we need to take can already fit in what
	 * we have allocated. If so, then let's just copy it in, but if not,
	 * then we need to make a bigger buffer and copy it in.
	 */
	if (aCString != NULL) {
		mSize = strlen(aCString);
		if (mSize >= mCapacity) {
			// make the new capacity just enough to hold this guy
			mCapacity = mSize + 1;

			// drop the old buffer, if we had one
			if (mString != NULL) {
				delete [] mString;
				mString = NULL;
			}

			// create the new one
			mString = new char[mCapacity];
			if (mString == NULL) {
				std::ostringstream	msg;
				msg << "CKString::operator=(char *) - the storage needed for "
					"this string is " << mCapacity << " chars, but the creation "
					"failed. Please look into this allocation error as soon as "
					"possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	} else {
		// if we're getting a NULL, then we have no size anymore
		mSize = 0;
	}

	// now let's clear out what we have and copy in the string
	bzero(mString, mCapacity);
	// now see if we need to copy anything into this buffer
	if (aCString != NULL) {
		memcpy(mString, aCString, mSize);
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
CKString & CKString::append( CKString & aString )
{
	return append((char *)aString.mString, aString.mSize);
}


CKString & CKString::append( const CKString & aString )
{
	return append((char *)aString.mString, aString.mSize);
}


CKString & CKString::append( char *aCString, int aLength )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::append(char *, int) - the CKString's storage is NULL "
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
			msg << "CKString::append(char *, int) - the passed-in C-String is NULL and "
				"that means that there's nothing I can do. Please make sure that "
				"the argument is not NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// this is what we have to add to the buffer
			if (aLength >= 0) {
				newChars = aLength;
			} else {
				newChars = strlen(aCString);
			}
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
				msg << "CKString::append(char *, int) - the existing buffer of " <<
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
				mCapacity = mSize + newChars + mCapacityIncrement;
			}
		}
	}

	// now copy over the new characters to the string
	if (!error) {
		memcpy(&(mString[mSize]), aCString, newChars);
		mSize += newChars;
	}

	return *this;
}


CKString & CKString::append( const char *aCString, int aLength )
{
	return append((char *)aCString, aLength);
}


CKString & CKString::append( std::string & aSTLString )
{
	return append((char *)aSTLString.c_str());
}


CKString & CKString::append( const std::string & aSTLString )
{
	return append((char *)aSTLString.c_str());
}


/*
 * These methods add the string representation of the different
 * kinds of base data objects to the end of the existing string
 * and return 'true' if successful, or 'false' if not. There are
 * quite a few to be as generally useful as possible.
 */
CKString & CKString::append( char aChar )
{
	// make a simple buffer to add this guy in
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return append(c);
}


CKString & CKString::append( int anInteger, int aNumOfDigits )
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


CKString & CKString::append( long aLong, int aNumOfDigits )
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


CKString & CKString::append( double aDouble, int aNumDecPlaces )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumDecPlaces > 0) {
		snprintf(c, 79, "%.*f", aNumDecPlaces, aDouble);
	} else {
		snprintf(c, 79, "%.16g", aDouble);
	}
	return append(c);
}


CKString & CKString::append( void *anAddress )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	snprintf(c, 79, "%lx", (unsigned long)anAddress);
	return append(c);
}


/*
 * These methods add the different kinds of strings to the
 * beginning of the existing string and return 'true' if successful,
 * or 'false' if not. There are quite a few to be as generally
 * useful as possible.
 */
CKString & CKString::prepend( CKString & aString )
{
	return prepend(aString.mString);
}


CKString & CKString::prepend( const CKString & aString )
{
	return prepend((char *)aString.mString);
}


CKString & CKString::prepend( char *aCString, int aLength )
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
			if (aLength >= 0) {
				newChars = aLength;
			} else {
				newChars = strlen(aCString);
			}
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
				mCapacity = mSize + newChars + mCapacityIncrement;
			}
		}
	}

	// now copy over the new characters to the string
	if (!error) {
		// first, move the existing string over the right amount
		memmove(&(mString[newChars]), mString, mSize);
		// now copy in the front the new characters
		memcpy(mString, aCString, newChars);
		mSize += newChars;
	}

	return *this;
}


CKString & CKString::prepend( const char *aCString, int aLength )
{
	return prepend((char *)aCString, aLength);
}


CKString & CKString::prepend( std::string & aSTLString )
{
	return prepend((char *)aSTLString.c_str());
}


CKString & CKString::prepend( const std::string & aSTLString )
{
	return prepend((char *)aSTLString.c_str());
}


/*
 * These methods add the string representation of the different
 * kinds of base data objects to the beginning of the existing string
 * and return 'true' if successful, or 'false' if not. There are
 * quite a few to be as generally useful as possible.
 */
CKString & CKString::prepend( char aChar )
{
	// make a simple buffer to add this guy in
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return prepend(c);
}


CKString & CKString::prepend( int anInteger, int aNumOfDigits )
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


CKString & CKString::prepend( long aLong, int aNumOfDigits )
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


CKString & CKString::prepend( double aDouble, int aNumDecPlaces )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	if (aNumDecPlaces > 0) {
		snprintf(c, 79, "%.*g", aNumDecPlaces, aDouble);
	} else {
		snprintf(c, 79, "%.16g", aDouble);
	}
	return prepend(c);
}


CKString & CKString::prepend( void *anAddress )
{
	// make a simple buffer for this guy too
	char	c[80];
	bzero(c, 80);
	snprintf(c, 79, "%lx", (unsigned long)anAddress);
	return prepend(c);
}


/*
 * When you want to fill an existing string with a repeated value,
 * this is a good way to do it. It replaces what's in the string
 * and in it's place puts the requested number of copies of the
 * given character.
 */
CKString & CKString::fill( char aChar, int aCount )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::fill(char, int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if we need to make more room in the buffer
	if (!error) {
		if (aCount+1 >= mCapacity) {
			// make room for it all and then a little growth
			char	*more = new char[aCount+1];
			if (more == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::fill(char, int) - the existing buffer of " <<
					mCapacity << " chars was not sufficient to hold the " <<
					(aCount + 1) << " characters, and while trying to "
					"create more space we failed. This is a serious allocation "
					"error that needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// clear this puppy out
				bzero(more, (aCount + 1));
				// delete the old buffer as it's used up
				delete [] mString;
				mString = NULL;
				// save the new buffer for the string
				mString = more;
				mCapacity = aCount + 1;
			}
		}
	}

	// now copy over the new characters to the string
	if (!error) {
		memset(mString, aChar, aCount);
		mSize = aCount;
	}

	return *this;
}


CKString & CKString::fill( char aChar, int aCount ) const
{
	return ((CKString *)this)->fill(aChar, aCount);
}


/*
 * When you need to remove (erase) a section of the string, this
 * method is the one to call. You give it a starting index and
 * optionally a number of characters to delete and they will be
 * removed from the string. If the length is not supplied, then
 * the characters from 'aStartingIndex' to the end of the string
 * will be removed.
 */
bool CKString::erase( int aStartingIndex,  int aLength )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::erase(int, int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure what's being asked of us is possible
	if (!error) {
		if (aStartingIndex < 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::erase(int, int) - the provided starting index is: " <<
				aStartingIndex << " and that makes no sense. Please make sure that "
				"the index falls within the string's length.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if ((aLength > 0) && ((aStartingIndex + aLength) > mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::erase(int, int) - the provided starting index is: " <<
				aStartingIndex << " and the length is: " << aLength << " that "
				"combine to make a string longer than this string is. Please make "
				"Please make sure that the erased string exists in the current "
				"string's length.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to handle the erasure in one of two ways.... if it's
	 * in the middle of the string then it's a left shift, but it it's
	 * 'to the end', then it's a simply bzero() call. Both end up changing
	 * the size, so don't forget that.
	 */
	if (!error) {
		if (aLength < 0) {
			// it's a 'to the end' call
			bzero(&(mString[aStartingIndex]), (mSize - aStartingIndex));
			mSize = aStartingIndex;
		} else {
			// it's within the string, so it's a left shift
			memmove(&(mString[aStartingIndex]),
					&(mString[(aStartingIndex + aLength)]),
					(mSize - aStartingIndex - aLength));
			bzero(&(mString[(mSize - aLength)]), aLength);
			mSize -= aLength;
		}
	}

	return !error;
}


bool CKString::erase( int aStartingIndex,  int aLength ) const
{
	return ((CKString *)this)->erase(aStartingIndex, aLength);
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


/*
 * This method allows the user to clear out the buffer explicitly.
 * It's handy from time to time if you're going to be re-using the
 * same string again and again.
 */
bool CKString::clear()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::clear() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now clear it out and reset the size
	if (!error) {
		bzero(mString, mSize);
		mSize = 0;
	}

	return !error;
}


bool CKString::clear() const
{
	return ((CKString *)this)->clear();
}


/*
 * There are times that you might want to see the numeric
 * representation of the contents of this string. These methods
 * make that easy enough to do and take care of all the dirty
 * work for us.
 */
int CKString::intValue()
{
	int			retval = 0;
	if (mSize > 0) {
		retval = (int) strtol(mString, NULL, 10);
	}
	return retval;
}


int CKString::intValue() const
{
	return ((CKString *)this)->intValue();
}


int CKString::hexIntValue()
{
	int			retval = 0;
	if (mSize > 0) {
		retval = (int) strtol(mString, NULL, 16);
	}
	return retval;
}


int CKString::hexIntValue() const
{
	return ((CKString *)this)->hexIntValue();
}



long CKString::longValue()
{
	long		retval = 0;
	if (mSize > 0) {
		retval = strtol(mString, NULL, 10);
	}
	return retval;
}


long CKString::longValue() const
{
	return ((CKString *)this)->longValue();
}


double CKString::doubleValue()
{
	double		retval = NAN;
	if (mString != NULL) {
		retval = strtod(mString, (char **)NULL);
	}
	return retval;
}


double CKString::doubleValue() const
{
	return ((CKString *)this)->doubleValue();
}


/********************************************************
 *
 *                Manipulation Methods
 *
 ********************************************************/
/*
 * There will be times when the data in a string may not exactly
 * look like a string, but you need to make an *exact* copy
 * anyway. This method will take the capacity, size, and data
 * from the argument and duplicate them so that this guy is a
 * clone of the original.
 */
CKString & CKString::clone( CKString & anOther )
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::clone(CKString &) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if (anOther.mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::clone(CKString &) - the argument's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if we need to alter out buffer to match the other guy's
	if (!error) {
		if (mCapacity < anOther.mCapacity) {
			// make room for it all and then a little growth
			char	*him = new char[anOther.mCapacity];
			if (him == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKString::clone(CKString &) - the existing buffer of " <<
					mCapacity << " chars was not sufficient to hold the other "
					"value, and while trying to create more space we failed. "
					"This is a serious allocation error that needs to be looked "
					"into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// clear this puppy out
				bzero(him, anOther.mCapacity);
				// delete the old buffer as it's used up
				delete [] mString;
				mString = NULL;
				// save the new buffer for the string
				mString = him;
				mCapacity = anOther.mCapacity;
			}
		}
	}

	// now copy over the characters to the string
	if (!error) {
		memcpy(mString, anOther.mString, mCapacity);
		mSize = anOther.mSize;
	}

	return *this;
}


CKString & CKString::clone( const CKString & anOther )
{
	return clone((CKString &)anOther);
}


CKString & CKString::clone( CKString & anOther ) const
{
	return ((CKString *)this)->clone(anOther);
}


CKString & CKString::clone( const CKString & anOther ) const
{
	return ((CKString *)this)->clone((CKString &)anOther);
}


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
CKString & CKString::toUpper()
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

	return *this;
}


CKString & CKString::toUpper() const
{
	return ((CKString *)this)->toUpper();
}


/*
 * This method goes through all the characters in the string
 * and makes sure that they are all lowercase. It's really
 * pretty simple, but it's awfully handy not to have to implement
 * this in all the projects.
 */
CKString & CKString::toLower()
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

	return *this;
}


CKString & CKString::toLower() const
{
	return ((CKString *)this)->toLower();
}


/*
 * This method first copies the string and then goes through all
 * the characters in the string and makes sure that they are all
 * uppercase. It's really pretty simple, but it's awfully handy
 * not to have to implement this in all the projects. This is
 * nice in that it doesn't change the original string.
 */
CKString CKString::copyUpper()
{
	CKString	retval(*this);
	return retval.toUpper();
}


CKString CKString::copyUpper() const
{
	return ((CKString *)this)->copyUpper();
}


/*
 * This method first copies the string and then goes through all
 * the characters in the string and makes sure that they are all
 * lowercase. It's really pretty simple, but it's awfully handy
 * not to have to implement this in all the projects. This is
 * nice in that it doesn't change the original string.
 */
CKString CKString::copyLower()
{
	CKString	retval(*this);
	return retval.toLower();
}


CKString CKString::copyLower() const
{
	return ((CKString *)this)->copyLower();
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
				"of " << aStartingPos << " is not contained in this ";
			if (mSize == 0) {
				msg << " empty string. ";
			} else {
				msg << " string: '" << mString << "'. ";
			}
			msg << "Please make sure that you ask for a substring that's within "
				"the limits of this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if ((aLength >= 0) && ((aStartingPos + aLength) > mSize)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::substr(int, int) - the requested length of " <<
				aLength << " characters starting at the starting position of " <<
				aStartingPos << " is not contained in this ";
			if (mSize == 0) {
				msg << " empty string. ";
			} else {
				msg << " string: '" << mString << "'. ";
			}
			msg << "Please make sure that you ask for a substring that's within "
				"the limits of this string.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make room in the return value for this substring
	if (!error) {
		int		newSize = aLength;
		if (aLength == -1) {
			newSize = mSize - aStartingPos;
		}
		if (newSize >= retval.mCapacity) {
			// make the resize just fit this new data
			if (!retval.resize(newSize + 1)) {
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
		memcpy(retval.mString, &(mString[aStartingPos]), newSize);
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

	// now let's make the substring
	if (!error && (mSize > 0)) {
		retval = substr(0, (aNumOfChars > mSize ? mSize : aNumOfChars));
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

	// now let's make the substring
	if (!error && (mSize > 0)) {
		if (aNumOfChars >= mSize) {
			retval = *this;
		} else {
			retval = substr(mSize - aNumOfChars);
		}
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
 * argument in the current string *ON OR AFTER* the supplied
 * index and return the starting position of the match if one
 * is found, or a -1 if there is nothing in the current string
 * that matches the  argument.
 */
int CKString::find( char aChar, int aStartingIndex )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return find(c, aStartingIndex);
}


int CKString::find( char aChar, int aStartingIndex ) const
{
	return ((CKString *)this)->find(aChar, aStartingIndex);
}


int CKString::find( char *aCString, int aStartingIndex )
{
	bool		error = false;
	int			retval = -1;

	// make sure we have something to do
	int		matchLen = -1;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			// be nice and don't thrown an exception
		} else {
			// get the length of this argument now for use later
			matchLen = strlen(aCString);
			if (matchLen == 0) {
				error = true;
				// be nice and don't thrown an exception
			}
		}
		// see if we have anything to match
		if (mSize == 0) {
			error = true;
			// no need to throw an exception
		}
	}

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::find(char *, int) - the CKString's storage is NULL and "
				"that means that there's been a terrible data corruption problem. "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure the starting position isn't out of range
	if (!error) {
		if (aStartingIndex >= mSize) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::find(char *, int) - the provided starting index: " <<
				aStartingIndex << " lies outside the length of the string. Please "
				"make sure that it is within the string next time.";
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
		int			startPos = (aStartingIndex < 0 ? 0 : aStartingIndex);
		for (int i = startPos; i < mSize; i++) {
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


int CKString::find( char *aCString, int aStartingIndex ) const
{
	return ((CKString *)this)->find(aCString, aStartingIndex);
}


int CKString::find( const char *aCString, int aStartingIndex )
{
	return find((char *)aCString, aStartingIndex);
}


int CKString::find( const char *aCString, int aStartingIndex ) const
{
	return ((CKString *)this)->find((char *)aCString, aStartingIndex);
}


int CKString::find( CKString & aString, int aStartingIndex )
{
	return find(aString.mString, aStartingIndex);
}


int CKString::find( const CKString & aString, int aStartingIndex )
{
	return find((char *)aString.mString, aStartingIndex);
}


int CKString::find( CKString & aString, int aStartingIndex ) const
{
	return ((CKString *)this)->find(aString.mString, aStartingIndex);
}


int CKString::find( const CKString & aString, int aStartingIndex ) const
{
	return ((CKString *)this)->find((char *)aString.mString, aStartingIndex);
}


int CKString::find( std::string & anSTLString, int aStartingIndex )
{
	return find(anSTLString.c_str(), aStartingIndex);
}


int CKString::find( const std::string & anSTLString, int aStartingIndex )
{
	return find((char *)anSTLString.c_str(), aStartingIndex);
}


int CKString::find( std::string & anSTLString, int aStartingIndex ) const
{
	return ((CKString *)this)->find(anSTLString.c_str(), aStartingIndex);
}


int CKString::find( const std::string & anSTLString, int aStartingIndex ) const
{
	return ((CKString *)this)->find((char *)anSTLString.c_str(), aStartingIndex);
}


/*
 * These methods all search for the LAST occurrence of the
 * argument in the current string and return the starting
 * position of the match if one is found, or a -1 if there
 * is nothing in the current string that matches the  argument.
 */
int CKString::findLast( char aChar, int aStartingIndex )
{
	char	c[2];
	c[0] = aChar;
	c[1] = '\0';
	return findLast(c, aStartingIndex);
}


int CKString::findLast( char aChar, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast(aChar, aStartingIndex);
}


int CKString::findLast( char *aCString, int aStartingIndex )
{
	bool		error = false;
	int			retval = -1;

	// make sure we have something to do
	int		matchLen = -1;
	if (!error) {
		if (aCString == NULL) {
			error = true;
			// no need to throw an exception - it's no match
		} else {
			// get the length of this argument now for use later
			matchLen = strlen(aCString);
			if (matchLen == 0) {
				error = true;
				// no need to throw an exception - it's no match
			}
		}
		// see if we have anything to match
		if (mSize == 0) {
			error = true;
		}
	}

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::findLast(char *, int) - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure the starting position isn't a lod of hooey
	if (!error) {
		if (aStartingIndex >= mSize) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::findLast(char *, int) - the provided starting index: " <<
				aStartingIndex << " lies outside the length of the string. Please "
				"make sure that it is within the string next time.";
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
		bool	match = false;
		int		last = matchLen - 1;
		int		startPos = (aStartingIndex >= 0 ? aStartingIndex : (mSize - 1));
		for (int i = startPos; i >= 0; i--) {
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


int CKString::findLast( char *aCString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast(aCString, aStartingIndex);
}


int CKString::findLast( const char *aCString, int aStartingIndex )
{
	return findLast((char *)aCString, aStartingIndex);
}


int CKString::findLast( const char *aCString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast((char *)aCString, aStartingIndex);
}


int CKString::findLast( CKString & aString, int aStartingIndex )
{
	return findLast(aString.mString, aStartingIndex);
}


int CKString::findLast( const CKString & aString, int aStartingIndex )
{
	return findLast((char *)aString.mString, aStartingIndex);
}


int CKString::findLast( CKString & aString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast(aString.mString, aStartingIndex);
}


int CKString::findLast( const CKString & aString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast((char *)aString.mString, aStartingIndex);
}


int CKString::findLast( std::string & anSTLString, int aStartingIndex )
{
	return findLast(anSTLString.c_str(), aStartingIndex);
}


int CKString::findLast( const std::string & anSTLString, int aStartingIndex )
{
	return findLast((char *)anSTLString.c_str(), aStartingIndex);
}


int CKString::findLast( std::string & anSTLString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast(anSTLString.c_str(), aStartingIndex);
}


int CKString::findLast( const std::string & anSTLString, int aStartingIndex ) const
{
	return ((CKString *)this)->findLast((char *)anSTLString.c_str(), aStartingIndex);
}


/*
 * This method trims all the whitespace off the right-hand end of
 * the string so that the last character in the string is something
 * that visibly printable.
 */
CKString & CKString::trimRight()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::trimRight() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure we have something to do
	if (!error) {
		while ((mSize > 0) && isspace(mString[mSize-1])) {
			mString[--mSize] = '\0';
		}
	}

	return *this;
}


CKString & CKString::trimRight() const
{
	return ((CKString *)this)->trimRight();
}


/*
 * This method trims all the whitespace off the left-hand end of
 * the string so that the first character in the string is something
 * that visibly printable.
 */
CKString & CKString::trimLeft()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::trimLeft() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// make sure we have something to do
	if (!error) {
		// find out how many whitespace characters there are at the front
		int		cnt = 0;
		while ((cnt < mSize) && isspace(mString[cnt])) {
			cnt++;
		}
		// move all the data to the left that many characters
		memmove(mString, &(mString[cnt]), (mSize - cnt));
		bzero(&(mString[(mSize - cnt)]), cnt);
		mSize -= cnt;
	}

	return *this;
}


CKString & CKString::trimLeft() const
{
	return ((CKString *)this)->trimLeft();
}


/*
 * This method trims all the whitespace off the both ends of the
 * string so that string is freed of all the "junk".
 */
CKString & CKString::trim()
{
	bool		error = false;

	// make sure the buffer isn't corrupted
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::trimRight() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// do the right-hand trim first to make the move as small as possible
	if (!error) {
		while ((mSize > 0) && isspace(mString[mSize-1])) {
			mString[--mSize] = '\0';
		}
	}

	// now do the tougher left-hand trim
	if (!error) {
		// find out how many whitespace characters there are at the front
		int		cnt = 0;
		while ((cnt < mSize) && isspace(mString[cnt])) {
			cnt++;
		}
		// move all the data to the left that many characters
		memmove(mString, &(mString[cnt]), (mSize - cnt));
		bzero(&(mString[(mSize - cnt)]), cnt);
		mSize -= cnt;
	}

	return *this;
}


CKString & CKString::trim() const
{
	return ((CKString *)this)->trim();
}


/********************************************************
 *
 *                Codec Methods
 *
 ********************************************************/
/*
 * A very useful coding of binary data is the Base64 encoding where
 * each group of 6 bits is encoded to one of 64 ASCII-printable
 * characters. This is nice in that no matter what the form of the
 * data, it can be sent on an ASCII-only pipe and not encounter any
 * loss from the transmission. This method takes the data in this
 * instance and converts it to Base64 ASCII data based on the rules
 * for that encoding. This includes the 76-character line limit where
 * the line will be broken by a '\n'.
 */
CKString & CKString::convertToBase64()
{
	bool		error = false;

	// first, see if we have anything to do
	char		*byteData = NULL;
	int			byteCnt = 0;
	int			byteCapacity = 0;
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::convertToBase64() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get everything we need
			byteData = mString;
			byteCnt = mSize;
			byteCapacity = mCapacity;
			// see if there's anything to do
			if (byteCnt <= 0) {
				error = true;
				// not really an error, but there's no data so do nothing.
			}
			// ...and clean out the buffer for this instance
			mString = NULL;
			mSize = 0;
			mCapacity = 0;
		}
	}

	// let's resize the buffer for *this* guy to hold the encoded data
	if (!error) {
		if (!resize((int) (byteCnt * 1.25))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::convertToBase64() - the storage for the encoded "
				"date could not be created and that's a serious allocation "
				"problem. Please check on it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to go through the 'old' data three bytes at a time
	 * and convert each set of three into four Base64 characters and add
	 * those converted characters to the 'new' string.
	 */
	if (!error) {
		char	src1;
		char	src2;
		char	src3;
		char	dest1;
		char	dest2;
		char	dest3;
		char	dest4;
		for (int i = 0; i < byteCnt; i += 3) {
			// get the (up to) three source bytes
			src1 = byteData[i];
			src2 = 0x0;
			src3 = 0x0;
			if (i+1 < byteCnt) {
				src2 = byteData[i+1];
			}
			if (i+2 < byteCnt) {
				src3 = byteData[i+2];
			}

			// mask these into the four 6-bit chunks
			dest1 = (src1 & 0xfc) >> 2;
			dest2 = ((src1 & 0x3) << 4) | ((src2 & 0xf0) >> 4);
			dest3 = ((src2 & 0xf) << 2) | ((src3 & 0xc0) >> 6);
			dest4 = src3 & 0x3f;

			// now add the encoded values for these 4 chunks
			append(encodeBase64(dest1));
			append(encodeBase64(dest2));
			if (i+1 < byteCnt) {
				append(encodeBase64(dest3));
			} else {
				append('=');
			}
			if (i+2 < byteCnt) {
				append(encodeBase64(dest4));
			} else {
				append('=');
			}

			// if we've reached the line length limit, put a CRLF in there
			if ((i > 0) && (i % (76/4*3) == 0)) {
				append("\r\n");
			}
		}
	}

	// delete the old data that we have converted
	if (byteData != NULL) {
		delete [] byteData;
		byteData = NULL;
	}

	return *this;
}


/*
 * When data is received in Base64 encoding it can be placed in
 * this instance and then this method can be run to convert it to
 * the correct binary equivalence. It's simply an inversion method
 * for the Base64 encoding so that we can easily serialize this
 * data on ASCII channels.
 */
CKString & CKString::convertFromBase64()
{
	bool		error = false;

	// first, see if we have anything to do
	char		*charData = NULL;
	int			charCnt = 0;
	int			charCapacity = 0;
	if (!error) {
		if (mString == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::convertFromBase64() - the CKString's storage is NULL "
				"and that means that there's been a terrible data corruption "
				"problem. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get everything we need
			charData = mString;
			charCnt = mSize;
			charCapacity = mCapacity;
			// see if there's anything to do
			if (charCnt <= 0) {
				error = true;
				// not really an error, but there's no data so do nothing.
			}
			// ...and clean out the buffer for this instance
			mString = NULL;
			mSize = 0;
			mCapacity = 0;
		}
	}

	// let's resize the buffer for *this* guy to hold the decoded data
	if (!error) {
		if (!resize(charCnt)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKString::convertFromBase64() - the storage for the decoded "
				"date could not be created and that's a serious allocation "
				"problem. Please check on it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to go through the Base64 array of characters and
	 * filter out all those that aren't Base64 characters. This
	 * is because it's quite possible that CRLFs are in the byte stream
	 * and they need to be out of there before we do the decoding.
	 */
	if (!error) {
		for (int i = 0; i < charCnt; i++) {
			char	b = charData[i];
			if (isBase64Char(b)) {
				append(b);
			}
		}
		// switch the filtered data back to the source vector
		char	*tmp = charData;
		int		tmpSize = charCnt;
		int		tmpCapacity = charCapacity;
		charData = mString;
		charCnt = mSize;
		charCapacity = mCapacity;
		mString = tmp;
		mSize = tmpSize;
		mCapacity = tmpCapacity;
		// ...and clear out the char data
		clear();
		// ...also update the count of characters we have
		charCnt = strlen(charData);
	}

	/*
	 * Now we need to go through the 'old' data four characters at a
	 * time and convert each set of four into three bytes and add
	 * those converted characters to the 'new' string.
	 */
	if (!error) {
		char		char1;
		char		char2;
		char		char3;
		char		char4;
		char		byte1;
		char		byte2;
		char		byte3;
		char		byte4;
		for (int i = 0; i < charCnt; i += 4) {
			// get the (up to) four base64 characters
			char1 = charData[i];
			char2 = 'A';
			char3 = 'A';
			char4 = 'A';
			if (i+1 < charCnt) {
				char2 = charData[i+1];
			}
			if (i+2 < charCnt) {
				char3 = charData[i+2];
			}
			if (i+3 < charCnt) {
				char4 = charData[i+3];
			}

			// now decode them into the 6-bit binary chunks
			byte1 = decodeBase64(char1);
			byte2 = decodeBase64(char2);
			byte3 = decodeBase64(char3);
			byte4 = decodeBase64(char4);

			// now add in all the complete bytes that we got
			append((char) ((byte1 << 2) | (byte2 >> 4)));
			if (char3 != '=') {
				append((char) (((byte2 & 0xf) << 4) | (byte3 >> 2)));
			}
			if (char4 != '=') {
				append((char) (((byte3 & 0x3) << 6) | byte4));
			}
		}
	}

	// delete the old data that we have converted
	if (charData != NULL) {
		delete [] charData;
		charData = NULL;
	}

	return *this;
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
	if (equal) {
		if (mSize != (int)strlen(aCString)) {
			equal = false;
		}
	}

	// check the buffer contents
	if (equal && (mSize > 0)) {
		if (strcmp(mString, aCString) != 0) {
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
 * This method checks to see if the two CKStrings are equal to one
 * another ignoring any case differences between the two based on
 * the values they represent and *not* on the actual pointers
 * themselves. If they are equal (modulo case), then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKString::equalsIgnoreCase( CKString & anOther )
{
	return equalsIgnoreCase(anOther.mString);
}


bool CKString::equalsIgnoreCase( const CKString & anOther )
{
	return equalsIgnoreCase((char *)anOther.mString);
}


bool CKString::equalsIgnoreCase( CKString & anOther ) const
{
	return ((CKString *)this)->equalsIgnoreCase(anOther.mString);
}


bool CKString::equalsIgnoreCase( const CKString & anOther ) const
{
	return ((CKString *)this)->equalsIgnoreCase((char *)anOther.mString);
}


/*
 * These operators check to see if the CKString is equal to a simple
 * NULL-terminated C-string - modulo any case differences. This is
 * nice in that we don't have to hassle with converting all string
 * constants to CKStrings and then do the comparison.
 */
bool CKString::equalsIgnoreCase( char *aCString )
{
	bool		equal = true;

	// first, check for logical sanity
	if (equal) {
		if (aCString == NULL) {
			equal = false;
		}
	}

	// next, see if the sizes match
	if (equal) {
		if (mSize != (int)strlen(aCString)) {
			equal = false;
		}
	}

	// check the buffer contents
	if (equal && (mSize > 0)) {
		if (strcasecmp(mString, aCString) != 0) {
			equal = false;
		}
	}

	return equal;
}


bool CKString::equalsIgnoreCase( const char *aCString )
{
	return equalsIgnoreCase((char *)aCString);
}


bool CKString::equalsIgnoreCase( char *aCString ) const
{
	return ((CKString *)this)->equalsIgnoreCase(aCString);
}


bool CKString::equalsIgnoreCase( const char *aCString ) const
{
	return ((CKString *)this)->equalsIgnoreCase((char *)aCString);
}


/*
 * These operators check to see if the CKString is equal to an STL
 * string - modulo any case differences. This is nice in that we
 * don't have to hassle with converting all STL std::string to
 * CKStrings and then do the comparison.
 */
bool CKString::equalsIgnoreCase( std::string & anSTLString )
{
	return equalsIgnoreCase(anSTLString.c_str());
}


bool CKString::equalsIgnoreCase( const std::string & anSTLString )
{
	return equalsIgnoreCase((char *)anSTLString.c_str());
}


bool CKString::equalsIgnoreCase( std::string & anSTLString ) const
{
	return ((CKString *)this)->equalsIgnoreCase(anSTLString.c_str());
}


bool CKString::equalsIgnoreCase( const std::string & anSTLString ) const
{
	return ((CKString *)this)->equalsIgnoreCase((char *)anSTLString.c_str());
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
 * This method checks to see if the argument CKString is greather
 * than or equal to this string (thus this one being less than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<( CKString & anOther )
{
	return operator<(anOther.mString);
}


bool CKString::operator<( const CKString & anOther )
{
	return operator<((char *)anOther.mString);
}


bool CKString::operator<( CKString & anOther ) const
{
	return ((CKString *)this)->operator<(anOther.mString);
}


bool CKString::operator<( const CKString & anOther ) const
{
	return ((CKString *)this)->operator<((char *)anOther.mString);
}


/*
 * This method checks to see if the argument string is greather
 * than or equal to this string (thus this one being less than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<( char *aCString )
{
	bool		lessThan = false;

	if ((aCString != NULL) && (mSize > 0)) {
		if (strcmp(mString, aCString) < 0) {
			lessThan = true;
		}
	}

	return lessThan;
}


bool CKString::operator<( const char *aCString )
{
	return operator<((char *)aCString);
}


bool CKString::operator<( char *aCString ) const
{
	return ((CKString *)this)->operator<(aCString);
}


bool CKString::operator<( const char *aCString ) const
{
	return ((CKString *)this)->operator<((char *)aCString);
}


/*
 * This method checks to see if the argument STL string is greather
 * than or equal to this string (thus this one being less than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<( std::string & anSTLString )
{
	return operator<(anSTLString.c_str());
}


bool CKString::operator<( const std::string & anSTLString )
{
	return operator<((char *)anSTLString.c_str());
}


bool CKString::operator<( std::string & anSTLString ) const
{
	return ((CKString *)this)->operator<(anSTLString.c_str());
}


bool CKString::operator<( const std::string & anSTLString ) const
{
	return ((CKString *)this)->operator<((char *)anSTLString.c_str());
}


/*
 * This method checks to see if the argument CKString is greather
 * than this string (thus this one being less than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<=( CKString & anOther )
{
	return operator<=(anOther.mString);
}


bool CKString::operator<=( const CKString & anOther )
{
	return operator<=((char *)anOther.mString);
}


bool CKString::operator<=( CKString & anOther ) const
{
	return ((CKString *)this)->operator<=(anOther.mString);
}


bool CKString::operator<=( const CKString & anOther ) const
{
	return ((CKString *)this)->operator<=((char *)anOther.mString);
}


/*
 * This method checks to see if the argument string is greather
 * than this string (thus this one being less than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<=( char *aCString )
{
	bool		lessThanOrEqual = false;

	if ((aCString != NULL) && (mSize > 0)) {
		if (strcmp(mString, aCString) <= 0) {
			lessThanOrEqual = true;
		}
	}

	return lessThanOrEqual;
}


bool CKString::operator<=( const char *aCString )
{
	return operator<=((char *)aCString);
}


bool CKString::operator<=( char *aCString ) const
{
	return ((CKString *)this)->operator<=(aCString);
}


bool CKString::operator<=( const char *aCString ) const
{
	return ((CKString *)this)->operator<=((char *)aCString);
}


/*
 * This method checks to see if the argument STL string is greather
 * than this string (thus this one being less than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator<=( std::string & anSTLString )
{
	return operator<=(anSTLString.c_str());
}


bool CKString::operator<=( const std::string & anSTLString )
{
	return operator<=((char *)anSTLString.c_str());
}


bool CKString::operator<=( std::string & anSTLString ) const
{
	return ((CKString *)this)->operator<=(anSTLString.c_str());
}


bool CKString::operator<=( const std::string & anSTLString ) const
{
	return ((CKString *)this)->operator<=((char *)anSTLString.c_str());
}


/*
 * This method checks to see if the argument CKString is less
 * than or equal to this string (thus this one being greater than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>( CKString & anOther )
{
	return operator>(anOther.mString);
}


bool CKString::operator>( const CKString & anOther )
{
	return operator>((char *)anOther.mString);
}


bool CKString::operator>( CKString & anOther ) const
{
	return ((CKString *)this)->operator>(anOther.mString);
}


bool CKString::operator>( const CKString & anOther ) const
{
	return ((CKString *)this)->operator>((char *)anOther.mString);
}


/*
 * This method checks to see if the argument string is less
 * than or equal to this string (thus this one being greater than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>( char *aCString )
{
	bool		greaterThan = false;

	if ((aCString != NULL) && (mSize > 0)) {
		if (strcmp(mString, aCString) > 0) {
			greaterThan = true;
		}
	}

	return greaterThan;
}


bool CKString::operator>( const char *aCString )
{
	return operator>((char *)aCString);
}


bool CKString::operator>( char *aCString ) const
{
	return ((CKString *)this)->operator>(aCString);
}


bool CKString::operator>( const char *aCString ) const
{
	return ((CKString *)this)->operator>((char *)aCString);
}


/*
 * This method checks to see if the argument STL string is less
 * than or equal to this string (thus this one being greater than),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>( std::string & anSTLString )
{
	return operator>(anSTLString.c_str());
}


bool CKString::operator>( const std::string & anSTLString )
{
	return operator>((char *)anSTLString.c_str());
}


bool CKString::operator>( std::string & anSTLString ) const
{
	return ((CKString *)this)->operator>(anSTLString.c_str());
}


bool CKString::operator>( const std::string & anSTLString ) const
{
	return ((CKString *)this)->operator>((char *)anSTLString.c_str());
}


/*
 * This method checks to see if the argument CKString is less
 * than this string (thus this one being greater than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>=( CKString & anOther )
{
	return operator>=(anOther.mString);
}


bool CKString::operator>=( const CKString & anOther )
{
	return operator>=((char *)anOther.mString);
}


bool CKString::operator>=( CKString & anOther ) const
{
	return ((CKString *)this)->operator>=(anOther.mString);
}


bool CKString::operator>=( const CKString & anOther ) const
{
	return ((CKString *)this)->operator>=((char *)anOther.mString);
}


/*
 * This method checks to see if the argument string is less
 * than this string (thus this one being greater than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>=( char *aCString )
{
	bool		greaterThanOrEqual = false;

	if ((aCString != NULL) && (mSize > 0)) {
		if (strcmp(mString, aCString) >= 0) {
			greaterThanOrEqual = true;
		}
	}

	return greaterThanOrEqual;
}


bool CKString::operator>=( const char *aCString )
{
	return operator>=((char *)aCString);
}


bool CKString::operator>=( char *aCString ) const
{
	return ((CKString *)this)->operator>=(aCString);
}


bool CKString::operator>=( const char *aCString ) const
{
	return ((CKString *)this)->operator>=((char *)aCString);
}


/*
 * This method checks to see if the argument STL string is less
 * than this string (thus this one being greater than or equal),
 * based on the values they represent and *not* on the actual
 * pointers themselves.
 */
bool CKString::operator>=( std::string & anSTLString )
{
	return operator>=(anSTLString.c_str());
}


bool CKString::operator>=( const std::string & anSTLString )
{
	return operator>=((char *)anSTLString.c_str());
}


bool CKString::operator>=( std::string & anSTLString ) const
{
	return ((CKString *)this)->operator>=(anSTLString.c_str());
}


bool CKString::operator>=( const std::string & anSTLString ) const
{
	return ((CKString *)this)->operator>=((char *)anSTLString.c_str());
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this string. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a std::string.
 */
CKString CKString::toString() const
{
	return CKString(mString);
}


/*
 * There are times that it'd be nice to see the hex characters of
 * the string laid out so that you can actually see the binary data
 * in the string. This is the method that does that.
 */
CKString CKString::toHexString() const
{
	CKString	retval = "[";

	// put each byte in the return string
	CKString	ascii = " = ";
	for (int i = 0; i < mSize; i++) {
		if (i != 0) {
			retval.append(" ");
		}
		unsigned char	msb = mString[i] & 0xf0;
		unsigned char	lsb = mString[i] & 0x0f;
		switch (msb) {
			case 0x00: retval.append("0");	break;
			case 0x10: retval.append("1");	break;
			case 0x20: retval.append("2");	break;
			case 0x30: retval.append("3");	break;
			case 0x40: retval.append("4");	break;
			case 0x50: retval.append("5");	break;
			case 0x60: retval.append("6");	break;
			case 0x70: retval.append("7");	break;
			case 0x80: retval.append("8");	break;
			case 0x90: retval.append("9");	break;
			case 0xa0: retval.append("a");	break;
			case 0xb0: retval.append("b");	break;
			case 0xc0: retval.append("c");	break;
			case 0xd0: retval.append("d");	break;
			case 0xe0: retval.append("e");	break;
			case 0xf0: retval.append("f");	break;
		}
		switch (lsb) {
			case 0x00: retval.append("0");	break;
			case 0x01: retval.append("1");	break;
			case 0x02: retval.append("2");	break;
			case 0x03: retval.append("3");	break;
			case 0x04: retval.append("4");	break;
			case 0x05: retval.append("5");	break;
			case 0x06: retval.append("6");	break;
			case 0x07: retval.append("7");	break;
			case 0x08: retval.append("8");	break;
			case 0x09: retval.append("9");	break;
			case 0x0a: retval.append("a");	break;
			case 0x0b: retval.append("b");	break;
			case 0x0c: retval.append("c");	break;
			case 0x0d: retval.append("d");	break;
			case 0x0e: retval.append("e");	break;
			case 0x0f: retval.append("f");	break;
		}
		// now build up the ASCII string as well
		if (isprint(mString[i])) {
			ascii.append(mString[i]);
		} else {
			ascii.append('.');
		}
	}
	retval.append(ascii).append("]");

	return retval;
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
			int		cnt = (aSize > mCapacity ? mCapacity : aSize ) - 1;
			// copy over just what will fit in the new string
			memcpy(resultant, mString, cnt);
			// ...and delete the old string
			delete [] mString;
			mString = NULL;
		}
		// next, update all the ivars that have been impacted
		mString = resultant;
		mSize = (mSize < aSize ? mSize : (aSize-1));
		mCapacity = aSize;
	}

	return !error;
}


/*
 * This method takes the lower 6 bits of the passed-in byte and uses
 * the Based64 encoding map to convert it to an ASCII-printable
 * character expressed as a byte.
 */
char CKString::encodeBase64( char aByte )
{
	static char encoded[] = {
			0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
			0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54,
			0x55, 0x56, 0x57, 0x58, 0x59, 0x5a,
			0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
			0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74,
			0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
			0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
			0x2b, 0x2f
	};
	return encoded[0x3f & aByte];
}


/*
 * This method takes the Base64 character stored in the passed-in
 * byte and converts it to the proper lower-6-bits of the return
 * value.
 */
char CKString::decodeBase64( char aChar )
{
	char		retval = 0x00;
	if ((aChar >= 0x41) && (aChar <= 0x5a)) {
		retval = (char) (aChar - 0x41);
	} else if ((aChar >= 0x61) && (aChar <= 0x7a)) {
		retval = (char) (aChar - 0x61 + 26);
	} else if ((aChar >= 0x30) && (aChar <= 0x39)) {
		retval = (char) (aChar - 0x30 + 52);
	} else if (aChar == 0x2b) {
		retval = 62;
	} else {
		retval = 63;
	}
	return retval;
}


/*
 * This method looks at the Base64 character contained in the passed-in
 * byte and returns true if it's a valid Base64 character and
 * false if it's not. This is nice because the decoding has to
 * filter out all the non-Base64 characters during the decoding.
 */
bool CKString::isBase64Char( char aChar )
{
	bool		retval = false;
	if ((aChar >= 0x41) && (aChar <= 0x5a)) {
		retval = true;
	} else if ((aChar >= 0x61) && (aChar <= 0x7a)) {
		retval = true;
	} else if ((aChar >= 0x30) && (aChar <= 0x39)) {
		retval = true;
	} else if (aChar == 0x2b) {
		retval = true;
	} else if (aChar == 0x2f) {
		retval = true;
	} else if (aChar == 0x3d) {
		retval = true;
	}
	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKString & aString )
{
	const char *str = aString.c_str();
	if (str != NULL) {
		aStream << str;
	}

	return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CKString & aString )
{
	const char *str = aString.c_str();
	if (str != NULL) {
		aStream << str;
	}

	return aStream;
}




/*
 * ----------------------------------------------------------------------------
 * This is the low-level node in the doubly-linked list that will be used
 * to organize a list of strings. This is nice in that it's easy to use, easy
 * to deal with, and the destructor takes care of cleaning up the strings
 * itself.
 *
 * We base it off the CKString so that it appears to be a normal string in
 * all regards - save the ability to exist in a doubly-linked list.
 *
 * The reason for this is that the STL std::vector and stl::list are no good
 * in their implementations in GCC, so rather than try to fix that code, we
 * chose to write our own classes.
 * ----------------------------------------------------------------------------
 */
/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that really doesn't contain
 * anything. This isn't so bad, as the setters allow you to
 * populate this guy later with anything that you could want.
 */
CKStringNode::CKStringNode() :
	CKString(),
	mPrev(NULL),
	mNext(NULL)
{
}


/*
 * This is a "promotion" constructor that takes a string and
 * creates a new string node based on the data in that string.
 * This is important because it'll be an easy way to add strings
 * to the list.
 */
CKStringNode::CKStringNode( const CKString & anOther,
							CKStringNode *aPrev,
							CKStringNode *aNext ) :
	CKString(anOther),
	mPrev(aPrev),
	mNext(aNext)
{
}


CKStringNode::CKStringNode( const char * aCString,
							CKStringNode *aPrev,
							CKStringNode *aNext ) :
	CKString(aCString),
	mPrev(aPrev),
	mNext(aNext)
{
}


CKStringNode::CKStringNode( const std::string & anSTLString,
							CKStringNode *aPrev,
							CKStringNode *aNext ) :
	CKString(anSTLString),
	mPrev(aPrev),
	mNext(aNext)
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKStringNode::CKStringNode( const CKStringNode & anOther ) :
	CKString(),
	mPrev(NULL),
	mNext(NULL)
{
	// now we can use the '=' operator to do the rest of the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKStringNode::~CKStringNode()
{
	// the super takes are of deleting all but the pointers
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKStringNode & CKStringNode::operator=( const CKStringNode & anOther )
{
	// start by letting the super do it's copying
	CKString::operator=(anOther);
	// just copy in all the values from the other
	mPrev = anOther.mPrev;
	mNext = anOther.mNext;

	return *this;
}


/*
 * At times it's also nice to be able to set a string to this
 * node so that there's not a ton of casting in the code.
 */
CKStringNode & CKStringNode::operator=( const CKString & anOther )
{
	// just copy in the string value and leave the pointers alone
	CKString::operator=(anOther);

	return *this;
}


CKStringNode & CKStringNode::operator=( const char * aCString )
{
	// just copy in the string value and leave the pointers alone
	CKString::operator=(aCString);

	return *this;
}


CKStringNode & CKStringNode::operator=( const std::string & anSTLString )
{
	// just copy in the string value and leave the pointers alone
	CKString::operator=(anSTLString);

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * These are the simple setters for the links to the previous and
 * next nodes in the list. There's nothing special here, so we're
 * exposing them directly.
 */
void CKStringNode::setPrev( CKStringNode *aNode )
{
	mPrev = aNode;
}


void CKStringNode::setNext( CKStringNode *aNode )
{
	mNext = aNode;
}


/*
 * These are the simple getters for the links to the previous and
 * next nodes in the list. There's nothing special here, so we're
 * exposing them directly.
 */
CKStringNode *CKStringNode::getPrev()
{
	return mPrev;
}


CKStringNode *CKStringNode::getNext()
{
	return mNext;
}


/*
 * This method is used to 'unlink' the node from the list it's in.
 * This will NOT delete the node, merely take it out the the list
 * and now it becomes the responsibility of the caller to delete
 * this node, or add him to another list.
 */
void CKStringNode::removeFromList()
{
	// first, point the next's "prev" to the prev
	if (mNext != NULL) {
		mNext->mPrev = mPrev;
	}
	// next, point the prev's "next" to the next
	if (mPrev != NULL) {
		mPrev->mNext = mNext;
	}
	// finally, I'm not linked to *anyone* anymore
	mPrev = NULL;
	mNext = NULL;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKStringNodes are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKStringNode::operator==( const CKStringNode & anOther ) const
{
	bool		equal = true;

	// first, see if the strings are equal
	if (equal) {
		if (!CKString::operator==(anOther)) {
			equal = false;
		}
	}
	// ...now check the pointers
	if (equal) {
		if ((mPrev != anOther.mPrev) ||
			(mNext != anOther.mNext)) {
			equal = false;
		}
	}

	return equal;
}


/*
 * This method checks to see if the two CKStringNodes are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKStringNode::operator!=( const CKStringNode & anOther ) const
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
CKString CKStringNode::toString() const
{
	// put everything in between angle brackets to make it look nice
	CKString	retval = "<String='";
	retval.append(mString).append("', ");
	char	buff[80];
	bzero(buff, 80);
	snprintf(buff, 79, "Prev=%lx, Next=%lx>", (unsigned long)mPrev,
			(unsigned long)mNext);
	retval.append(buff);

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKStringNode & aNode )
{
	aStream << aNode.toString();

	return aStream;
}




/*
 * ----------------------------------------------------------------------------
 * This is the high-level interface to a list of CKString objects. It
 * is organized as a doubly-linked list of CKStringNodes and the interface
 * to the list if controlled by a nice CKFWMutex. This is a nice and clean
 * replacement to the STL std::list.
 * ----------------------------------------------------------------------------
 */
/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that really doesn't contain
 * anything. This isn't so bad, as the setters allow you to
 * populate this guy later with anything that you could want.
 */
CKStringList::CKStringList() :
	mHead(NULL),
	mTail(NULL),
	mMutex(),
	mConditional(mMutex)
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKStringList::CKStringList( CKStringList & anOther ) :
	mHead(NULL),
	mTail(NULL),
	mMutex(),
	mConditional(mMutex)
{
	// let the operator==() take care of this for me
	*this = anOther;
}


CKStringList::CKStringList( const CKStringList & anOther ) :
	mHead(NULL),
	mTail(NULL),
	mMutex(),
	mConditional(mMutex)
{
	// let the operator==() take care of this for me
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKStringList::~CKStringList()
{
	// we need to delete the head as long as there is one
	while (mHead != NULL) {
		CKStringNode	*next = mHead->mNext;
		delete mHead;
		mHead = next;
		if (mHead != NULL) {
			mHead->mPrev = NULL;
		}
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKStringList & CKStringList::operator=( CKStringList & anOther )
{
	// make sure we're not doing this to ourselves
	if (this != & anOther) {
		// first, clear out anything we might have right now
		clear();

		// now, do a deep copy of the source list
		copyToEnd(anOther);
	}

	return *this;
}


CKStringList & CKStringList::operator=( const CKStringList & anOther )
{
	this->operator=((CKStringList &) anOther);

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * These are the easiest ways to get at the head and tail of this
 * list. After that, the CKStringNode's getPrev() and getNext()
 * do a good job of moving you around the list.
 */
CKStringNode *CKStringList::getHead()
{
	return mHead;
}


CKStringNode *CKStringList::getHead() const
{
	return mHead;
}


CKStringNode *CKStringList::getTail()
{
	return mTail;
}


CKStringNode *CKStringList::getTail() const
{
	return mTail;
}


/*
 * These methods return copies of the first and last strings in the
 * list. This is useful if you want to look at the elements of the
 * list but don't want to do the size() - 1 for the last one. If the
 * list is empty then the empty string will be returned, so it's best
 * to check empty() before using the results of this method for sure.
 */
CKString CKStringList::getFirst()
{
	CKString	retval = "";
	if (mHead != NULL) {
		retval = *mHead;
	}
	return retval;
}


CKString CKStringList::getFirst() const
{
	return ((CKStringList *)this)->getFirst();
}


CKString CKStringList::getLast()
{
	CKString	retval = "";
	if (mTail != NULL) {
		retval = *mTail;
	}
	return retval;
}


CKString CKStringList::getLast() const
{
	return ((CKStringList *)this)->getLast();
}


/*
 * Because there may be times that the user wants to lock us up
 * for change, we're going to expose this here so it's easy for them
 * to iterate, for example.
 */
void CKStringList::lock()
{
	mMutex.lock();
}


void CKStringList::lock() const
{
	((CKStringList *)this)->lock();
}


void CKStringList::unlock()
{
	mMutex.unlock();
}


void CKStringList::unlock() const
{
	((CKStringList *)this)->unlock();
}


/*
 * This method is a simple indexing operator so that we can easily
 * get the individual strings in the list. If the argument
 * is -1, then the default is to get the *LAST* non-NULL
 * string in the list.
 */
CKString & CKStringList::operator[]( int aPosition )
{
	CKStringNode	*node = NULL;

	// first, see if the arg is -1, and if so, return the last one
	if (aPosition == -1) {
		node = mTail;
	} else {
		// first, lock up this guy against changes
		mMutex.lock();
		// all we need to do is count until we get there or the end
		int		cnt = 0;
		for (node = mHead; (cnt < aPosition) && (node != NULL); node = node->mNext) {
			cnt++;
		}
		// now we can release the lock
		mMutex.unlock();
	}

	// make sure that we have the node we're interested in
	if (node == NULL) {
		std::ostringstream	msg;
		msg << "CKStringList::operator[](int) - the requested index: " << aPosition <<
			" was not available in the list. Please make sure that you are asking "
			"for a valid index in the list.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return (CKString &)(*node);
}


CKString & CKStringList::operator[]( int aPosition ) const
{
	return ((CKStringList *)this)->operator[](aPosition);
}


/********************************************************
 *
 *                List Methods
 *
 ********************************************************/
/*
 * This method gets the size of the list in a thread-safe
 * way. This means that it will block until it can get the
 * lock on the data, so be warned.
 */
int CKStringList::size()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// all we need to do is count them all up
	int		cnt = 0;
	for (CKStringNode *node = mHead; node != NULL; node = node->mNext) {
		cnt++;
	}
	// now we can release the lock
	mMutex.unlock();

	return cnt;
}


int CKStringList::size() const
{
	return ((CKStringList *)this)->size();
}


/*
 * This is used to tell the caller if the list is empty. It's
 * faster than checking for a size() == 0.
 */
bool CKStringList::empty()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// all we need to do is count them all up
	bool	empty = false;
	if (mHead == NULL) {
		empty = true;
	}
	// now we can release the lock
	mMutex.unlock();

	return empty;
}


bool CKStringList::empty() const
{
	return ((CKStringList *)this)->empty();
}


/*
 * This method clears out the entire list and deletes all it's
 * contents. After this, all node pointers to nodes in this list
 * will be pointing to nothing, so watch out.
 */
void CKStringList::clear()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// we need to delete the head as long as there is one
	while (mHead != NULL) {
		CKStringNode	*next = mHead->mNext;
		delete mHead;
		mHead = next;
		if (mHead != NULL) {
			mHead->mPrev = NULL;
		}
	}
	// now make sure to reset the head and tail
	mHead = NULL;
	mTail = NULL;
	// now we can release the lock
	mMutex.unlock();
}


void CKStringList::clear() const
{
	((CKStringList *)this)->clear();
}


/*
 * When I want to add a string to the front or back of the list,
 * these are the simplest ways to do that. The passed-in string
 * is left untouched, and a copy is made of it at the proper point
 * in the list.
 */
void CKStringList::addToFront( CKString & aString )
{
	addToFront(aString.mString);
}


void CKStringList::addToFront( const CKString & aString )
{
	addToFront((char *)aString.mString);
}


void CKStringList::addToFront( CKString & aString ) const
{
	((CKStringList *)this)->addToFront(aString.mString);
}


void CKStringList::addToFront( const CKString & aString ) const
{
	((CKStringList *)this)->addToFront((char *)aString.mString);
}


void CKStringList::addToFront( std::string & anSTLString )
{
	addToFront(anSTLString.c_str());
}


void CKStringList::addToFront( const std::string & anSTLString )
{
	addToFront((char *)anSTLString.c_str());
}


void CKStringList::addToFront( std::string & anSTLString ) const
{
	((CKStringList *)this)->addToFront(anSTLString.c_str());
}


void CKStringList::addToFront( const std::string & anSTLString ) const
{
	((CKStringList *)this)->addToFront((char *)anSTLString.c_str());
}


void CKStringList::addToFront( char * aCString )
{
	// first, lock up this guy against changes
	mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aCString != NULL));

	// we need to create a new data point node based on this guy
	CKStringNode	*node = new CKStringNode(aCString, NULL, mHead);
	if (node == NULL) {
		// first we need to release the lock
		mMutex.unlock();
		// now we can throw the exception
		std::ostringstream	msg;
		msg << "CKStringList::addToFront(const char *) - a "
			"new string node could not be created for the passed in string: '" <<
			aCString << "' and that's a serious allocation problem that needs to "
			"be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// finish linking this guy into the list properly
		if (mHead == NULL) {
			mTail = node;
		} else {
			mHead->mPrev = node;
		}
		mHead = node;
	}

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// now we can release the lock
	mMutex.unlock();
}


void CKStringList::addToFront( const char * aCString )
{
	addToFront((char *)aCString);
}


void CKStringList::addToFront( char * aCString ) const
{
	((CKStringList *)this)->addToFront(aCString);
}


void CKStringList::addToFront( const char * aCString ) const
{
	((CKStringList *)this)->addToFront((char *)aCString);
}


void CKStringList::addToEnd( CKString & aString )
{
	addToEnd(aString.mString);
}


void CKStringList::addToEnd( const CKString & aString )
{
	addToEnd((char *)aString.mString);
}


void CKStringList::addToEnd( CKString & aString ) const
{
	((CKStringList *)this)->addToEnd(aString.mString);
}


void CKStringList::addToEnd( const CKString & aString ) const
{
	((CKStringList *)this)->addToEnd((char *)aString.mString);
}


void CKStringList::addToEnd( std::string & anSTLString )
{
	addToEnd(anSTLString.c_str());
}


void CKStringList::addToEnd( const std::string & anSTLString )
{
	addToEnd((char *)anSTLString.c_str());
}


void CKStringList::addToEnd( std::string & anSTLString ) const
{
	((CKStringList *)this)->addToEnd(anSTLString.c_str());
}


void CKStringList::addToEnd( const std::string & anSTLString ) const
{
	((CKStringList *)this)->addToEnd((char *)anSTLString.c_str());
}


void CKStringList::addToEnd( char *aCString )
{
	// first, lock up this guy against changes
	mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aCString != NULL));

	// we need to create a new data point node based on this guy
	CKStringNode	*node = new CKStringNode(aCString, mTail, NULL);
	if (node == NULL) {
		// first we need to release the lock
		mMutex.unlock();
		// now we can throw the exception
		std::ostringstream	msg;
		msg << "CKStringList::addToEnd(const char *) - a "
			"new string node could not be created for the passed in string: '" <<
			aCString << "' and that's a serious allocation problem that needs to "
			"be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// finish linking this guy into the list properly
		if (mTail == NULL) {
			mHead = node;
		} else {
			mTail->mNext = node;
		}
		mTail = node;
	}

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// now we can release the lock
	mMutex.unlock();
}


void CKStringList::addToEnd( const char *aCString )
{
	addToEnd((char *)aCString);
}


void CKStringList::addToEnd( char *aCString ) const
{
	((CKStringList *)this)->addToEnd(aCString);
}


void CKStringList::addToEnd( const char *aCString ) const
{
	((CKStringList *)this)->addToEnd((char *)aCString);
}


/*
 * These methods take control of the passed-in arguments and place
 * them in the proper place in the list. This is different in that
 * the control of the node is passed to the list, but that's why
 * we've created them... to make it easy to add in nodes by just
 * changing the links.
 */
void CKStringList::putOnFront( CKStringNode *aNode )
{
	// first, make sure we have something to do
	if (aNode != NULL) {
		// next, lock up this guy against changes
		mMutex.lock();

		// see if by adding this we're not empty
		bool	wakeUp = (mHead == NULL);

		// we simply need to link this bad boy into the list
		aNode->mPrev = NULL;
		aNode->mNext = mHead;
		if (mHead == NULL) {
			mTail = aNode;
		} else {
			mHead->mPrev = aNode;
		}
		mHead = aNode;

		// see if we need to wake any waiters on this guy
		if (wakeUp) {
			mConditional.wakeWaiters();
		}

		// now we can release the lock
		mMutex.unlock();
	}
}


void CKStringList::putOnFront( const CKStringNode *aNode )
{
	putOnFront((CKStringNode *)aNode);
}


void CKStringList::putOnFront( CKStringNode *aNode ) const
{
	((CKStringList *)this)->putOnFront(aNode);
}


void CKStringList::putOnFront( const CKStringNode *aNode ) const
{
	((CKStringList *)this)->putOnFront((CKStringNode *)aNode);
}


void CKStringList::putOnEnd( CKStringNode *aNode )
{
	// first, make sure we have something to do
	if (aNode != NULL) {
		// next, lock up this guy against changes
		mMutex.lock();

		// see if by adding this we're not empty
		bool	wakeUp = (mHead == NULL);

		// we simply need to link this bad boy into the list
		aNode->mPrev = mTail;
		aNode->mNext = NULL;
		if (mTail == NULL) {
			mHead = aNode;
		} else {
			mTail->mNext = aNode;
		}
		mTail = aNode;

		// see if we need to wake any waiters on this guy
		if (wakeUp) {
			mConditional.wakeWaiters();
		}

		// now we can release the lock
		mMutex.unlock();
	}
}


void CKStringList::putOnEnd( const CKStringNode *aNode )
{
	putOnEnd((CKStringNode *)aNode);
}


void CKStringList::putOnEnd( CKStringNode *aNode ) const
{
	((CKStringList *)this)->putOnEnd(aNode);
}


void CKStringList::putOnEnd( const CKStringNode *aNode ) const
{
	((CKStringList *)this)->putOnEnd((CKStringNode *)aNode);
}


/*
 * When you have a list that you want to add to this list, these
 * are the methods to use. It's important to note that the arguments
 * will NOT be altered - which is why this is called the 'copy' as
 * opposed to the 'splice'.
 */
void CKStringList::copyToFront( CKStringList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aList.mHead != NULL));

	/*
	 * I need to go through all the source data, but backwards because
	 * I'll be putting these new nodes on the *front* of the list, and
	 * if I go through the source in the forward order, I'll reverse the
	 * order of the elements in the source as I add them. So I'll go
	 * backwards... no biggie...
	 */
	for (CKStringNode *src = aList.mTail; src != NULL; src = src->mPrev) {
		// first, make a copy of this guy
		CKStringNode	*node = new CKStringNode(*src, NULL, mHead);
		if (node == NULL) {
			// first we need to release the locks
			aList.mMutex.unlock();
			mMutex.unlock();
			// now we can throw the exception
			std::ostringstream	msg;
			msg << "CKStringList::copyToFront(CKStringList &) - a "
				"new string node could not be created for the string: '" <<
				src << "' and that's a serious allocation problem that needs to "
				"be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}

		// now, add this guy to the front of the list
		if (mHead == NULL) {
			mTail = node;
		} else {
			mHead->mPrev = node;
		}
		mHead = node;
	}

	// now I can release the lock on the source locks
	aList.mMutex.unlock();

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// finally, I can release my own lock
	mMutex.unlock();
}


void CKStringList::copyToFront( const CKStringList & aList )
{
	copyToFront((CKStringList &)aList);
}


void CKStringList::copyToFront( CKStringList & aList ) const
{
	((CKStringList *)this)->copyToFront(aList);
}


void CKStringList::copyToFront( const CKStringList & aList ) const
{
	((CKStringList *)this)->copyToFront((CKStringList &)aList);
}


void CKStringList::copyToEnd( CKStringList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aList.mHead != NULL));

	/*
	 * I need to go through all the source data. I'll be putting these new
	 * nodes on the *end* of the list so the order is preserved.
	 */
	for (CKStringNode *src = aList.mHead; src != NULL; src = src->mNext) {
		// first, make a copy of this guy
		CKStringNode	*node = new CKStringNode(*src, mTail, NULL);
		if (node == NULL) {
			// first we need to release the locks
			aList.mMutex.unlock();
			mMutex.unlock();
			// now we can throw the exception
			std::ostringstream	msg;
			msg << "CKStringList::copyToEnd(CKStringList &) - a "
				"new string node could not be created for the node: " <<
				src << " and that's a serious allocation problem that needs to "
				"be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}

		// now, add this guy to the end of the list
		if (mTail == NULL) {
			mHead = node;
		} else {
			mTail->mNext = node;
		}
		mTail = node;
	}

	// now I can release the lock on the source locks
	aList.mMutex.unlock();

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// finally, I can release my own lock
	mMutex.unlock();
}


void CKStringList::copyToEnd( const CKStringList & aList )
{
	copyToEnd((CKStringList &)aList);
}


void CKStringList::copyToEnd( CKStringList & aList ) const
{
	((CKStringList *)this)->copyToEnd(aList);
}


void CKStringList::copyToEnd( const CKStringList & aList ) const
{
	((CKStringList *)this)->copyToEnd((CKStringList &)aList);
}


/*
 * When you have a list that you want to merge into this list, these
 * are the methods to use. It's important to note that the argument
 * lists will be EMPTIED - which is why this is called the 'splice'
 * as opposed to the 'copy'.
 */
void CKStringList::spliceOnFront( CKStringList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aList.mHead != NULL));

	// add the source, in total, to the head of this list
	if (mHead == NULL) {
		// take their list in toto - mine is empty
		mHead = aList.mHead;
		mTail = aList.mTail;
	} else {
		mHead->mPrev = aList.mTail;
		if (aList.mTail != NULL) {
			aList.mTail->mNext = mHead;
		}
		if (aList.mHead != NULL) {
			mHead = aList.mHead;
		}
	}
	// ...and empty the source list
	aList.mHead = NULL;
	aList.mTail = NULL;

	// now I can release the lock on the source locks
	aList.mMutex.unlock();

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// finally, I can release my own lock
	mMutex.unlock();
}


void CKStringList::spliceOnFront( const CKStringList & aList )
{
	spliceOnFront((CKStringList &)aList);
}


void CKStringList::spliceOnFront( CKStringList & aList ) const
{
	((CKStringList *)this)->spliceOnFront(aList);
}


void CKStringList::spliceOnFront( const CKStringList & aList ) const
{
	((CKStringList *)this)->spliceOnFront((CKStringList &)aList);
}


void CKStringList::spliceOnEnd( CKStringList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// see if by adding this we're not empty
	bool	wakeUp = ((mHead == NULL) && (aList.mHead != NULL));

	// add the source, in total, to the end of this list
	if (mTail == NULL) {
		// take their list in toto - mine is empty
		mHead = aList.mHead;
		mTail = aList.mTail;
	} else {
		mTail->mNext = aList.mHead;
		if (aList.mHead != NULL) {
			aList.mHead->mPrev = mTail;
		}
		if (aList.mTail != NULL) {
			mTail = aList.mTail;
		}
	}
	// ...and empty the source list
	aList.mHead = NULL;
	aList.mTail = NULL;

	// now I can release the lock on the source locks
	aList.mMutex.unlock();

	// see if we need to wake any waiters on this guy
	if (wakeUp) {
		mConditional.wakeWaiters();
	}

	// finally, I can release my own lock
	mMutex.unlock();
}


void CKStringList::spliceOnEnd( const CKStringList & aList )
{
	spliceOnEnd((CKStringList &)aList);
}


void CKStringList::spliceOnEnd( CKStringList & aList ) const
{
	((CKStringList *)this)->spliceOnEnd(aList);
}


void CKStringList::spliceOnEnd( const CKStringList & aList ) const
{
	((CKStringList *)this)->spliceOnEnd((CKStringList &)aList);
}


/*
 * This method removes the specified node from this list - actually,
 * it's just guaranteed to remove it from the list it's in as the
 * erasure simply removes this node from it's list and then deletes
 * it.
 */
void CKStringList::erase( CKStringNode * & aNode )
{
	if (aNode != NULL) {
		// lock this list up assuming it's in our list
		mMutex.lock();
		// we need to see if we are removing the head or tail
		if (aNode == mHead) {
			mHead = aNode->getNext();
		}
		if (aNode == mTail) {
			mTail = aNode->getPrev();
		}
		// next, we tell this guy to remove himself from the links
		CKStringNode	*next = aNode->getNext();
		aNode->removeFromList();
		// and delete him
		delete aNode;
		aNode = next;
		// finally, we can unlock the list
		mMutex.unlock();
	}
}


void CKStringList::erase( CKStringNode * & aNode ) const
{
	((CKStringList *)this)->erase(aNode);
}


void CKStringList::erase( CKString & aString )
{
	erase(aString.mString);
}


void CKStringList::erase( const CKString & aString )
{
	erase((char *)aString.mString);
}


void CKStringList::erase( CKString & aString ) const
{
	((CKStringList *)this)->erase(aString.mString);
}


void CKStringList::erase( const CKString & aString ) const
{
	((CKStringList *)this)->erase((char *)aString.mString);
}


void CKStringList::erase( char *aCString )
{
	bool		error = false;

	// first, make sure that we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			// no need to throw an exception, just stop
		}
	}

	// now we can scan all the elements in the list looking for matches
	if (!error) {
		// lock this list up against all changes
		mMutex.lock();

		// ...and scan through the list looking for matches to delete
		CKStringNode	*n = mHead;
		while (n != NULL) {
			if ((*((CKString *)n)) == aCString) {
				// we need to see if we are removing the head or tail
				if (n == mHead) {
					mHead = n->getNext();
				}
				if (n == mTail) {
					mTail = n->getPrev();
				}
				// next, we tell this guy to remove himself from the links
				CKStringNode	*next = n->getNext();
				n->removeFromList();
				// and delete him
				delete n;
				// get the next node in the list
				n = next;
			} else {
				// move to the next node to check
				n = n->getNext();
			}
		}

		// finally, we can unlock the list
		mMutex.unlock();
	}
}


void CKStringList::erase( const char *aCString )
{
	erase((char *)aCString);
}


void CKStringList::erase( char *aCString ) const
{
	((CKStringList *)this)->erase(aCString);
}


void CKStringList::erase( const char *aCString ) const
{
	((CKStringList *)this)->erase((char *)aCString);
}


void CKStringList::erase( std::string & anSTLString )
{
	erase(anSTLString.c_str());
}


void CKStringList::erase( const std::string & anSTLString )
{
	erase((char *)anSTLString.c_str());
}


void CKStringList::erase( std::string & anSTLString ) const
{
	((CKStringList *)this)->erase(anSTLString.c_str());
}


void CKStringList::erase( const std::string & anSTLString ) const
{
	((CKStringList *)this)->erase((char *)anSTLString.c_str());
}


/*
 * This method is useful in that it will tell us if this list
 * contains the provided string and that is a nice test if we
 * want to be making list of unique elements.
 */
bool CKStringList::contains( CKString & aString )
{
	return contains(aString.mString);
}


bool CKStringList::contains( const CKString & aString )
{
	return contains((char *)aString.mString);
}


bool CKStringList::contains( CKString & aString ) const
{
	return ((CKStringList *)this)->contains(aString.mString);
}


bool CKStringList::contains( const CKString & aString ) const
{
	return ((CKStringList *)this)->contains((char *)aString.mString);
}


bool CKStringList::contains( char *aCString )
{
	bool		error = false;
	bool		gotIt = false;

	// first, make sure that we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			// no need to throw an exception, just stop
		}
	}

	// now we can scan all the elements in the list looking for a match
	if (!error) {
		// lock this list up against all changes
		mMutex.lock();

		// ...and scan through the list looking for the first match
		CKStringNode	*n = mHead;
		while (!gotIt && (n != NULL)) {
			if ((*((CKString *)n)) == aCString) {
				gotIt = true;
				break;
			} else {
				// move to the next node to check
				n = n->getNext();
			}
		}

		// finally, we can unlock the list
		mMutex.unlock();
	}

	return gotIt;
}


bool CKStringList::contains( const char *aCString )
{
	return contains((char *)aCString);
}


bool CKStringList::contains( char *aCString ) const
{
	return ((CKStringList *)this)->contains(aCString);
}


bool CKStringList::contains( const char *aCString ) const
{
	return ((CKStringList *)this)->contains((char *)aCString);
}


bool CKStringList::contains( std::string & anSTLString )
{
	return contains(anSTLString.c_str());
}


bool CKStringList::contains( const std::string & anSTLString )
{
	return contains((char *)anSTLString.c_str());
}


bool CKStringList::contains( std::string & anSTLString ) const
{
	return ((CKStringList *)this)->contains(anSTLString.c_str());
}


bool CKStringList::contains( const std::string & anSTLString ) const
{
	return ((CKStringList *)this)->contains((char *)anSTLString.c_str());
}


/*
 * This method is useful in that it will locate the CKStringNode
 * in the list, if it exists, that matches the passed-in string.
 * This is a nice way to find a location in the list.
 */
CKStringNode *CKStringList::find( CKString & aString )
{
	return find(aString.mString);
}


CKStringNode *CKStringList::find( const CKString & aString )
{
	return find((char *)aString.mString);
}


CKStringNode *CKStringList::find( CKString & aString ) const
{
	return ((CKStringList *)this)->find(aString.mString);
}


CKStringNode *CKStringList::find( const CKString & aString ) const
{
	return ((CKStringList *)this)->find((char *)aString.mString);
}


CKStringNode *CKStringList::find( char *aCString )
{
	bool			error = false;
	CKStringNode	*retval = NULL;

	// first, make sure that we have something to do
	if (!error) {
		if (aCString == NULL) {
			error = true;
			// no need to throw an exception, just stop
		}
	}

	// now we can scan all the elements in the list looking for a match
	if (!error) {
		// lock this list up against all changes
		mMutex.lock();

		// ...and scan through the list looking for the first match
		CKStringNode	*n = mHead;
		while (n != NULL) {
			if ((*n) == aCString) {
				retval = n;
				break;
			} else {
				// move to the next node to check
				n = n->getNext();
			}
		}

		// finally, we can unlock the list
		mMutex.unlock();
	}

	return retval;
}


CKStringNode *CKStringList::find( const char *aCString )
{
	return find((char *)aCString);
}


CKStringNode *CKStringList::find( char *aCString ) const
{
	return ((CKStringList *)this)->find(aCString);
}


CKStringNode *CKStringList::find( const char *aCString ) const
{
	return ((CKStringList *)this)->find((char *)aCString);
}


CKStringNode *CKStringList::find( std::string & anSTLString )
{
	return find(anSTLString.c_str());
}


CKStringNode *CKStringList::find( const std::string & anSTLString )
{
	return find((char *)anSTLString.c_str());
}


CKStringNode *CKStringList::find( std::string & anSTLString ) const
{
	return ((CKStringList *)this)->find(anSTLString.c_str());
}


CKStringNode *CKStringList::find( const std::string & anSTLString ) const
{
	return ((CKStringList *)this)->find((char *)anSTLString.c_str());
}


/*
 * These methods remove the first and last strings from the list
 * and return them to the callers. The idea is that many times
 * when the processing of a list is done a line at a time and this
 * makes it easy to do this. If there are no lines in the list
 * this method will return the empty string, so it's important to
 * use the size() or empty() methods to see when to stop popping
 * off the strings.
 */
CKString CKStringList::popOffFront()
{
	bool		error = false;
	CKString	retval = "";

	// we have only one thing to remove - the head of the list - if it exists
	if (!error) {
		// lock this list up against all changes
		mMutex.lock();

		// see if we have anything to do
		if (mHead != NULL) {
			// get the node that we'll be removing
			CKStringNode	*n = mHead;
			// ...and get the string that's stored there for returning
			retval = *n;
			// move to the new head of the list
			mHead = mHead->getNext();
			// tell the node to remove itself nicely
			n->removeFromList();
			// and delete him
			delete n;
		}

		// finally, we can unlock the list
		mMutex.unlock();
	}

	return retval;
}


CKString CKStringList::popOffFront() const
{
	return ((CKStringList *)this)->popOffFront();
}


CKString CKStringList::popOffEnd()
{
	bool		error = false;
	CKString	retval = "";

	// we have only one thing to remove - the tail of the list - if it exists
	if (!error) {
		// lock this list up against all changes
		mMutex.lock();

		// see if we have anything to do
		if (mTail != NULL) {
			// get the node that we'll be removing
			CKStringNode	*n = mTail;
			// ...and get the string that's stored there for returning
			retval = *n;
			// move to the new tail of the list
			mTail = mTail->getPrev();
			// tell the node to remove itself nicely
			n->removeFromList();
			// and delete him
			delete n;
		}

		// finally, we can unlock the list
		mMutex.unlock();
	}

	return retval;
}


CKString CKStringList::popOffEnd() const
{
	return ((CKStringList *)this)->popOffEnd();
}


/*
 * These methods remove the first and last strings from the list
 * and return them to the callers. The idea is that many times
 * when the processing of a list is done a line at a time and this
 * makes it easy to do this. If there are no lines in the list
 * this method WILL BLOCK until there is something in the list
 * to return. This is an efficient way but can be dangerous
 * because of the blocking.
 */
CKString CKStringList::popSomethingOffFront()
{
	bool		error = false;
	CKString	retval = "";

	// make a test based on the head of our list
	CKStringListNotEmptyTest	tst(&mHead);
	// now wait until there's something on the list to get
	mConditional.lockAndTest(tst);

	// we have only one thing to remove - the head of the list - if it exists
	if (!error) {
		// see if we have anything to do
		if (mHead != NULL) {
			// get the node that we'll be removing
			CKStringNode	*n = mHead;
			// ...and get the string that's stored there for returning
			retval = *n;
			// move to the new head of the list
			mHead = mHead->getNext();
			// tell the node to remove itself nicely
			n->removeFromList();
			// and delete him
			delete n;
		}

		// finally, we can unlock the list
		mConditional.unlock();
	}

	return retval;
}


CKString CKStringList::popSomethingOffFront() const
{
	return ((CKStringList *)this)->popSomethingOffFront();
}


CKString CKStringList::popSomethingOffEnd()
{
	bool		error = false;
	CKString	retval = "";

	// make a test based on the head of our list
	CKStringListNotEmptyTest	tst(&mHead);
	// now wait until there's something on the list to get
	mConditional.lockAndTest(tst);

	// we have only one thing to remove - the tail of the list - if it exists
	if (!error) {
		// see if we have anything to do
		if (mTail != NULL) {
			// get the node that we'll be removing
			CKStringNode	*n = mTail;
			// ...and get the string that's stored there for returning
			retval = *n;
			// move to the new tail of the list
			mTail = mTail->getPrev();
			// tell the node to remove itself nicely
			n->removeFromList();
			// and delete him
			delete n;
		}

		// finally, we can unlock the list
		mConditional.unlock();
	}

	return retval;
}


CKString CKStringList::popSomethingOffEnd() const
{
	return ((CKStringList *)this)->popSomethingOffEnd();
}


/*
 * These methods remove up to 'aMaxCnt' lines from the front, or
 * end, of the list - depending on the method. The idea is that
 * many times in processing a list of "things", a thread needs to
 * grab a bunch of "things" and then process them. The individual
 * popSomethingOff...() methods are going to tie up the mutex used
 * to lock the list, so it's better to grab a 'bunch' at a time.
 * These methods will block until there is at least ONE line in the
 * list to process, so care needs to be taken to make sure that
 * the list is not empty before calling these - or you want to
 * block until there's something to do.
 */
CKStringList CKStringList::cutLinesOffFront( int aMaxCnt )
{
	bool			error = false;
	CKStringList	retval;

	// make a test based on the head of our list
	CKStringListNotEmptyTest	tst(&mHead);
	// now wait until there's something on the list to get
	mConditional.lockAndTest(tst);

	// we have only one thing to remove - the head of the list - if it exists
	if (!error) {
		// see if we have anything to do
		if (mHead != NULL) {
			// I am cutting the list off starting at the head
			retval.mHead = mHead;
			// move through the list to the cut line
			int		cnt = 0;
			for (CKStringNode *node = mHead; node != NULL; node = node->getNext()) {
				// stop moving if we have all the caller wants
				if (++cnt > aMaxCnt) {
					break;
				} else {
					// move the tail one more spot
					retval.mTail = node;
				}
			}
			// make the new head of the remaining list
			mHead = retval.mTail->getNext();
			// adjust the links on the remaining list
			if (mHead == NULL) {
				mTail = NULL;
			} else {
				mHead->setPrev(NULL);
			}
			// clean up the links on the cut-off list
			retval.mTail->setNext(NULL);
		}

		// finally, we can unlock the list
		mConditional.unlock();
	}

	return retval;
}


CKStringList CKStringList::cutLinesOffFront( int aMaxCnt ) const
{
	return ((CKStringList *)this)->cutLinesOffFront(aMaxCnt);
}


CKStringList CKStringList::cutLinesOffEnd( int aMaxCnt )
{
	bool			error = false;
	CKStringList	retval;

	// make a test based on the head of our list
	CKStringListNotEmptyTest	tst(&mTail);
	// now wait until there's something on the list to get
	mConditional.lockAndTest(tst);

	// we have only one thing to remove - the tail of the list - if it exists
	if (!error) {
		// see if we have anything to do
		if (mTail != NULL) {
			// I am cutting the list off starting at the tail
			retval.mTail = mTail;
			// move through the list to the cut line
			int		cnt = 0;
			for (CKStringNode *node = mTail; node != NULL; node = node->getPrev()) {
				// stop moving if we have all the caller wants
				if (++cnt > aMaxCnt) {
					break;
				} else {
					// move the head one more spot
					retval.mHead = node;
				}
			}
			// make the new tail of the remaining list
			mTail = retval.mHead->getPrev();
			// adjust the links on the remaining list
			if (mTail == NULL) {
				mHead = NULL;
			} else {
				mTail->setNext(NULL);
			}
			// clean up the links on the cut-off list
			retval.mHead->setPrev(NULL);
		}

		// finally, we can unlock the list
		mConditional.unlock();
	}

	return retval;
}


CKStringList CKStringList::cutLinesOffEnd( int aMaxCnt ) const
{
	return ((CKStringList *)this)->cutLinesOffEnd(aMaxCnt);
}


/*
 * This is the tokenizer/parser that wasn't in the STL string
 * class for some unknown reason. It takes a source and a
 * delimiter and breaks up the source into chunks that are
 * all separated by the delimiter string. Each chunk is put
 * into the returned vector for accessing by the caller. Since
 * the return value is created on the stack, the user needs to
 * save it if they want it to stay around.
 */
CKStringList CKStringList::parseIntoChunks( const CKString & aString,
											const CKString & aDelim )
{
	bool			error = false;
	CKStringList	retval;

	// first, see if we have anything to do
	if (!error) {
		if (aString.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKStringList::parseIntoChunks(const CKString &, "
				"const CKString &) - the length of the source string is 0 and "
				"that means that there's nothing for me to do. Please make sure "
				"that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	int		delimLength = 0;
	if (!error) {
		delimLength = aDelim.length();
		if (delimLength <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKStringList::parseIntoChunks(const CKString &, "
				"const CKString &) - the length of the delimiter string is 0 "
				"and that means that there's nothing for me to do. Please make "
				"sure that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, copy the source to a buffer so I can consume it in the process
	CKString		buff;
	if (!error) {
		buff = aString;
	}

	/*
	 * Now loop picking off the parts bettween the delimiters. Do this by
	 * finding the first delimiter, see if it's located at buff[0], and if
	 * so, then add an empty string to the vector, otherwise, get the
	 * substring up to that delimiter and place it at the end of the vector,
	 * removing it from the buffer as you do this. Then eat up the delimiter
	 * and do it all again. In the end, there will be one more bit and that
	 * will simply be added to the end of the vector.
	 */
	while (!error) {
		// find out wherre, if anyplace, the delimiter sits
		int		pos = buff.find(aDelim);
		if (pos == -1) {
			// nothing left to parse out, bail out
			break;
		} else if (pos == 0) {
			// add an empty string to the vector
			retval.addToEnd(CKString());
		} else {
			// pick off the substring up to the delimiter
			retval.addToEnd(buff.substr(0, pos));
			// ...and then delete them from the buffer
			buff.erase(0, pos);
		}

		// now strip off the delimiter from the buffer
		buff.erase(0, delimLength);
	}
	// if we didn't error out, then add the remaining buff to the end
	if (!error) {
		retval.addToEnd(buff);
	}

	return retval;
}


CKStringList CKStringList::parseIntoChunks( const CKString & aString,
											const char aDelim )
{
	CKString	delim((char) aDelim);
	return parseIntoChunks(aString, delim);
}


/*
 * This method is really useful when dealing with a list of
 * strings and wanting to make a single string out of them.
 * This might be in a debug print statement, it might also be
 * a way to "piece together" something. In any case, the elements
 * of the string list will be put into the resultant string each
 * one separated by the passed-in separator.
 */
CKString CKStringList::concatenate( const CKString & aSeparator )
{
	CKString	retval("");

	// simply iterate of all the elements and slap them together
	for (CKStringNode *node = mHead; node != NULL; node = node->mNext) {
		if (node != mHead) {
			retval.append(aSeparator);
		}
		retval.append(node->CKString::toString());
	}

	return retval;
}


CKString CKStringList::concatenate( const CKString & aSeparator ) const
{
	return ((CKStringList *)this)->concatenate(aSeparator);
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKStringLists are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKStringList::operator==( CKStringList & anOther )
{
	bool		equal = true;

	// first, lock up both lists against changes
	mMutex.lock();
	anOther.mMutex.lock();

	/*
	 * We need to compare each element in the list as strings and
	 * NOT as string nodes as the pointers will never be the same
	 * but the data will.
	 */
	CKStringNode	*me = mHead;
	CKStringNode	*him = anOther.mHead;
	while (equal) {
		// see if we're at the end
		if ((me == NULL) && (him == NULL)) {
			break;
		}

		// see if the two lists are of different lengths
		if (((me == NULL) && (him != NULL)) ||
			((me != NULL) && (him == NULL))) {
			equal = false;
			break;
		}

		// compare the values by data contents only
		if (!me->CKString::operator==(*(CKString*)him)) {
			equal = false;
			break;
		}

		// now move to the next point in each list
		me = me->mNext;
		him = him->mNext;
	}

	// now we're OK to unlock these lists and let them be free
	anOther.mMutex.unlock();
	mMutex.unlock();

	return equal;
}


bool CKStringList::operator==( const CKStringList & anOther )
{
	return this->operator==((CKStringList &) anOther);
}


bool CKStringList::operator==( CKStringList & anOther ) const
{
	return ((CKStringList *)this)->operator==(anOther);
}


bool CKStringList::operator==( const CKStringList & anOther ) const
{
	return ((CKStringList *)this)->operator==((CKStringList &) anOther);
}


/*
 * This method checks to see if the two CKStringLists are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKStringList::operator!=( CKStringList & anOther )
{
	return !(this->operator==(anOther));
}


bool CKStringList::operator!=( const CKStringList & anOther )
{
	return !(this->operator==((CKStringList &) anOther));
}


bool CKStringList::operator!=( CKStringList & anOther ) const
{
	return !(((CKStringList *)this)->operator==(anOther));
}


bool CKStringList::operator!=( const CKStringList & anOther ) const
{
	return !(((CKStringList *)this)->operator==((CKStringList &) anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKStringList::toString()
{
	// lock this guy up so he doesn't change
	mMutex.lock();

	CKString		retval = "[";
	// put each data point out on the output
	for (CKStringNode *node = mHead; node != NULL; node = node->mNext) {
		retval += node->CKString::toString();
		retval += "\n";
	}
	retval += "]";

	// unlock him now
	mMutex.unlock();

	return retval;
}


/*
 * Setting the head or the tail is a bit dicey and so we're not
 * going to let just anyone change these guys.
 */
void CKStringList::setHead( CKStringNode *aNode )
{
	mHead = aNode;
}


void CKStringList::setTail( CKStringNode *aNode )
{
	mTail = aNode;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKStringList & aList )
{
	aStream << aList.toString();

	return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CKStringList & aList )
{
	aStream << ((CKStringList &)aList).toString();

	return aStream;
}
