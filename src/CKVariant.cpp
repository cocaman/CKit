/*
 * CKVariant.cpp - this file defines a class that can be used to represent a
 *                 general data value. The power of this class is that all the
 *                 different kinds of values that this object can rperesent can
 *                 then be treated as a single data type and thus really 
 *                 simplify dealing with tables of different types of data.
 * 
 * $Id: CKVariant.cpp,v 1.1 2003/12/16 18:09:13 drbob Exp $
 */

//	System Headers
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>

//	Third-Party Headers
#include <CKException.h>

//	Other Headers
#include "CKVariant.h"
#include "CKTable.h"

//	Forward Declarations

//	Private Constants
/*
 * We need to check the error number after a few commands and this is
 * the way we need to add support for it.
 */
int errno;

//	Private Datatypes

//	Private Data Constants



/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that defaults to a String but
 * contains nothing. After this, you're certainly going to have to 
 * set the value's type and contents.
 */
CKVariant::CKVariant() :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
}


/*
 * This form of the constructor is nice in that it allows us to take
 * the values right away and then the user doesn't have to worry
 * about setting the values right after creation. This just makes
 * things a lot nicer. The passed-in value will be converted to the
 * proper datatype as defined by the type parameter. The value
 * parameter will not be touched in this operation and memory
 * management for the value arg will remain with the caller.
 */
CKVariant::CKVariant( CKVariantType aType, const char *aValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setValueAsType(aType, aValue);
}


/*
 * This form of the constructor understands that the value that's
 * intended to be stored here is a String, and the value provided
 * is what's to be stored. The string value will be copied to the
 * local storage in this constructor so the management remains
 * with the caller.
 */
CKVariant::CKVariant( const char *aStringValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setStringValue(aStringValue);
}


/*
 * This form of the constructor understands that the value that's
 * intended to be stored here is a Date (long) of the form YYYYMMDD,
 * and the value provided is what's to be stored.
 */
CKVariant::CKVariant( long aDateValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setDateValue(aDateValue);
}


/*
 * This form of the constructor understands that the value that's
 * intended to be stored here is a double, and the value provided
 * is what's to be stored.
 */
CKVariant::CKVariant( double aDoubleValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setDoubleValue(aDoubleValue);
}


/*
 * This form of the constructor understands that the value that's
 * intended to be stored here is a CKTable, and the value provided
 * is what's to be stored. The value argument will not be touched
 * in this constructor as we'll be making a copy of the contents
 * for local use.
 */
CKVariant::CKVariant( const CKTable *aTableValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setTableValue(aTableValue);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKVariant::CKVariant( const CKVariant & anOther ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKVariant::~CKVariant()
{
	switch (getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			if (mStringValue != NULL) {
				delete [] mStringValue;
				mStringValue = NULL;
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			if (mTableValue != NULL) {
				delete mTableValue;
				mTableValue = NULL;
			}
			break;
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKVariant & CKVariant::operator=( const CKVariant & anOther )
{
	switch (anOther.getType()) {
		case eUnknownVariant:
			setStringValue(NULL);
			break;
		case eStringVariant:
			setStringValue(anOther.getStringValue());
			break;
		case eNumberVariant:
			setDoubleValue(anOther.getDoubleValue());
			break;
		case eDateVariant:
			setDateValue(anOther.getDateValue());
			break;
		case eTableVariant:
			setTableValue(anOther.getTableValue());
			break;
	}

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the value of this instance based on the type
 * of data that's represented in the supplied string. This string
 * will *not* be modified or retained in any way, as a copy will be
 * made for this instance, if it's necessary to have one. This is
 * the most general form of the setter for this instance.
 *
 * If the type eUnknown is passed in, then this method will do it's
 * best to determine if the value represented in the supplied
 * string is a date (format YYYYMMDD), a double, a string, or a
 * table and then act accordingly.
 */
void CKVariant::setValueAsType( CKVariantType aType, const char *aValue )
{
	double	val;

	switch (aType) {
		case eUnknownVariant:
			/*
			 * OK... we need to see if this can be successfully
			 * converted to a number. If it can, then it's likely
			 * a date (YYYYMMDD) or a double. If it fails, then it's
			 * a string for sure. If it's a number, then we look to
			 * see it's length. If it's 8, and the first four digits
			 * are in the range 1900-2100, the next two are in the
			 * range 01-12, and the last two are in the range 01-31,
			 * then it's likely a date.
			 */
			val = atof(aValue);
			if (errno != 0) {
				/*
				 * OK, we have a problem converting a number, so it's
				 * likely that this is a string. Now we need to see if
				 * it's a 'bulk' table format, or just a string.
				 */
				if ((aValue[0] != aValue[strlen(aValue) - 1]) ||
					isalnum(aValue[0]) ||
					(aValue[0] == '+') || (aValue[0] == '-') ||
					(aValue[1] != '2') || (strlen(aValue) < 15)) {
					// OK, looks like a string for sure
					setStringValue(aValue);
				} else {
					// make a table from the string representation
					CKTable	tbl(aValue);
					// ...and use that as the value
					setTableValue(&tbl);
				}
			} else {
				// could be a date, do a few more tests to be sure
				if ((val != floor(val)) ||
					(strlen(aValue) != 8) ||
					(floor(val/10000) < 1900) ||
					(floor(val/10000) > 2100) ||
					(val - floor(val/10000)*10000 <  101) ||
					(val - floor(val/10000)*10000 > 1231)) {
					// failed a date test, so leave it as a double
					setDoubleValue(val);
				} else {
					// likely a date, so keep it as such
					setDateValue((long)val);
				}
			}
			break;
		case eStringVariant:
			setStringValue(aValue);
			break;
		case eNumberVariant:
			setDoubleValue(atof(aValue));
			break;
		case eDateVariant:
			setDateValue(atol(aValue));
			break;
		case eTableVariant:
			// make a table from the string representation
			CKTable	tbl(aValue);
			// ...and use that as the value
			setTableValue(&tbl);
			break;
	}
}


/*
 * This sets the value stored in this instance as a string, but a local
 * copy will be made so that the caller doesn't have to worry about
 * holding on to the parameter, and is free to delete it.
 */
void CKVariant::setStringValue( const char *aStringValue )
{
	// first, see if we need to delete what's might already be here
	if (mType == eStringVariant) {
		if (mStringValue != NULL) {
			delete [] mStringValue;
			mStringValue = NULL;
		}
	}
	// next, if we have something to set, then create space for it
	if (aStringValue != NULL) {
		mStringValue = new char[strlen(aStringValue) + 1];
		if (mStringValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setStringValue"
				"(const char *) - the space to hold this string value could "
				"not be created. This is a serious allocation error.");
		} else {
			// go ahead and copy it in
			strcpy( mStringValue, aStringValue );
		}
	}
	// ...and don't forget to set the type of data we have now
	mType = eStringVariant;
}


/*
 * This method sets the value stored in this instance as a date of the
 * form YYYYMMDD - stored as a long.
 */
void CKVariant::setDateValue( long aDateValue )
{
	// first, see if we need to delete what's might already be here
	if (mType == eStringVariant) {
		if (mStringValue != NULL) {
			delete [] mStringValue;
			mStringValue = NULL;
		}
	}
	// next, set what we have been given
	mDateValue = aDateValue;
	// ...and don't forget to set the type of data we have now
	mType = eDateVariant;
}


/*
 * This method sets the value stored in this instance as a double.
 */
void CKVariant::setDoubleValue( double aDoubleValue )
{
	// first, see if we need to delete what's might already be here
	if (mType == eStringVariant) {
		if (mStringValue != NULL) {
			delete [] mStringValue;
			mStringValue = NULL;
		}
	}
	// next, set what we have been given
	mDoubleValue = aDoubleValue;
	// ...and don't forget to set the type of data we have now
	mType = eNumberVariant;
}


/*
 * This sets the value stored in this instance as a table, but a
 * local copy will be made so that the caller doesn't have to worry
 * about holding on to the parameter, and is free to delete it.
 */
void CKVariant::setTableValue( const CKTable *aTableValue )
{
	// first, see if we need to delete what's might already be here
	if (mType == eStringVariant) {
		if (mStringValue != NULL) {
			delete [] mStringValue;
			mStringValue = NULL;
		}
	}
	// next, if we have something to set, then create space for it
	if (aTableValue != NULL) {
		mTableValue = new CKTable(*aTableValue);
		if (mTableValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setTableValue"
				"(const CKTable *) - the copy of this table value could "
				"not be created. This is a serious allocation error.");
		}
	}
	// ...and don't forget to set the type of data we have now
	mType = eTableVariant;
}


/*
 * This method returns the enumerated type of the data that this
 * instance is currently holding.
 */
CKVariantType CKVariant::getType() const
{
	return mType;
}


/*
 * This method will return the integer value of the data stored in this
 * instance - if the type is numeric. If the data isn't numeric an 
 * exception will be thrown as it's assumed that the user should make
 * sure that this instance is numeric *before* calling this method.
 */
int CKVariant::getIntValue() const
{
	// make sure it's something that can be done
	if (mType != eNumberVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getIntValue() - the "
			"data contained in this instance is not numeric and therefore "
			"we can't get an integer from it.");
	}
	// return what it is they're asking for
	return (int) mDoubleValue;
}


/*
 * This method will return the double value of the data stored in this
 * instance - if the type is numeric. If the data isn't numeric an 
 * exception will be thrown as it's assumed that the user should make
 * sure that this instance is numeric *before* calling this method.
 */
double CKVariant::getDoubleValue() const
{
	// make sure it's something that can be done
	if (mType != eNumberVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getDoubleValue() - "
			"the data contained in this instance is not numeric and therefore "
			"we can't get a double from it.");
	}
	return mDoubleValue;
}


/*
 * This method will return the date value of the data stored in this
 * instance in a long of the form YYYYMMDD - if the type is date. If
 * the data isn't date an exception will be thrown as it's assumed
 * that the user should make sure that this instance is date *before*
 * calling this method.
 */
long CKVariant::getDateValue() const
{
	// make sure it's something that can be done
	if (mType != eDateVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getDateValue() - "
			"the data contained in this instance is not a date and therefore "
			"we can't get a date value from it.");
	}
	return mDateValue;
}


/*
 * This method returns the actual string value of the data that
 * this instance is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy,
 * or call the getValueAsString() method that returns a copy.
 */
const char *CKVariant::getStringValue() const
{
	// make sure it's something that can be done
	if (mType != eStringVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getStringValue() - "
			"the data contained in this instance is not a string and therefore "
			"we can't get a string value from it.");
	}
	return mStringValue;
}


/*
 * This method returns the actual table value of the data that
 * this instance is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKTable *CKVariant::getTableValue() const
{
	// make sure it's something that can be done
	if (mType != eTableVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getTableValue() - "
			"the data contained in this instance is not a table and therefore "
			"we can't get a table value from it.");
	}
	return mTableValue;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method returns a copy of the current value as contained in 
 * a string and it is the responsibility of the caller to call
 * 'delete []' on the results. It's also possible that this method
 * will return NULL, so you have better check the return value
 * before assuming anything.
 */
char *CKVariant::getValueAsString() const
{
	char	*retval = NULL;

	// first, create a string and then stream the value into it
	std::ostringstream	buff;
	switch (mType) {
		case eUnknownVariant:
			buff << "<unknown>";
			break;
		case eStringVariant:
			buff << mStringValue;
			break;
		case eNumberVariant:
			buff << mDoubleValue;
			break;
		case eDateVariant:
			buff << mDateValue;
			break;
		case eTableVariant:
			buff << mTableValue->toString();
			break;
	}
	// now we need to create a copy of the string
	const char	*guts = buff.str().c_str();
	int			gutsLen = strlen(guts);
	if (guts != NULL) {
		retval = new char[gutsLen + 1];
		if (retval == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::getValueAsString"
				"() - the space to hold the string representation of this "
				"value could not be created. This is a serious allocation "
				"error.");
		} else {
			// copy the std::string's value into the new (char *) array
			strncpy( retval, guts, gutsLen );
			// ...and make sure to NULL terminate it
			retval[gutsLen] = '\0';
		}
	}

	return retval;
}


/*
 * In order to simplify the move of this object from C++ to Java
 * it makes sense to encode the value's data into a (char *) that
 * can be converted to a Java String and then the Java object can
 * interpret it and "reconstitue" the object from this coding.
 *
 * This method returns a character array that the caller is
 * responsible for calling 'delete []' on. This is useful as these
 * codes are used outside the scope of this class and so a copy
 * is far more useful.
 */
char *CKVariant::generateCodeFromValues() const
{
	char	*retval = NULL;

	std::ostringstream buff;
	switch (getType()) {
		case eUnknownVariant:
			buff << "U:";
			break;
		case eStringVariant:
			buff << "S:" << mStringValue;
			break;
		case eNumberVariant:
			buff << "N:" << mDoubleValue;
			break;
		case eDateVariant:
			buff << "D:" << mDateValue;
			break;
		case eTableVariant:
			buff << "T:" << mTableValue->generateCodeFromValues();
			break;
	}

	// now create a new buffer to hold all this
	const char	*guts = buff.str().c_str();
	int			gutsLen = strlen(guts);
	if (guts != NULL) {
		retval = new char[gutsLen + 1];
		if (retval == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::generateCodeFromValues"
				"() - the space to hold the codified representation of this "
				"value could not be created. This is a serious allocation "
				"error.");
		} else {
			// copy over the string's contents
			strncpy(retval, guts, gutsLen);
			// ...and make sure to NULL terminate it
			retval[gutsLen] = '\0';
		}
	}
	
	return retval;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this value. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKVariant::takeValuesFromCode( const char *aCode )
{
	switch (aCode[0]) {
		case 'U':
			setValueAsType(eUnknownVariant, &(aCode[2]));
			break;
		case 'S':
			setValueAsType(eStringVariant, &(aCode[2]));
			break;
		case 'N':
			setValueAsType(eNumberVariant, &(aCode[2]));
			break;
		case 'D':
			setValueAsType(eDateVariant, &(aCode[2]));
			break;
		case 'T':
			setValueAsType(eTableVariant, &(aCode[2]));
			break;
	}
}


/*
 * This method checks to see if the two CKVariants are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKVariant::operator==( const CKVariant & anOther ) const
{
	bool		equal = true;

	// first, see if the types match
	if (mType != anOther.mType) {
		equal = false;
	}

	// now, see if the values match
	if (equal) {
		switch (mType) {
			case eUnknownVariant:
				// two unknowns are equal by definition
				break;
			case eStringVariant:
				// two NULLs match in my opinion
				if (mStringValue == NULL) {
					if (anOther.mStringValue != NULL) {
						equal = false;
					}
				} else {
					if (anOther.mStringValue == NULL) {
						equal = false;
					} else {
						if (strcmp(mStringValue, anOther.mStringValue) != 0) {
							equal = false;
						}
					}
				}
				break;
			case eNumberVariant:
				if (mDoubleValue != anOther.mDoubleValue) {
					equal = false;
				}
				break;
			case eDateVariant:
				if (mDateValue != anOther.mDateValue) {
					equal = false;
				}
				break;
			case eTableVariant:
				// two NULLs match in my opinion
				if (mTableValue == NULL) {
					if (anOther.mTableValue != NULL) {
						equal = false;
					}
				} else {
					if (anOther.mTableValue == NULL) {
						equal = false;
					} else {
						if ((*mTableValue) != (*anOther.mTableValue)) {
							equal = false;
						}
					}
				}
				break;
		}
	}

	return equal;
}


/*
 * This method checks to see if the two CKVariants are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKVariant::operator!=( const CKVariant & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice human-readable
 * form of the contents of this instance. Most of the time this means
 * that it's used for debugging, but it could be used for just about anything.
 * In these cases, it's nice not to have to worry about the ownership of
 * the representation, so this returns a std::string.
 */
std::string CKVariant::toString() const
{
	std::string		retval;
	char			buff[128];
	// first, send out the type as if it were a 'cast' of the data
	switch (mType) {
		case eUnknownVariant:
			retval = "(unknown)??";
			break;
		case eStringVariant:
			retval = "(String)";
			retval.append(mStringValue);
			break;
		case eNumberVariant:
			snprintf(buff, 127, "(Number)%f", mDoubleValue);
			retval = buff;
			break;
		case eDateVariant:
			snprintf(buff, 127, "(Date)%ld", mDateValue);
			retval = buff;
			break;
		case eTableVariant:
			retval = "(BBGTable)";
			retval.append(mTableValue->toString());
			break;
	}

	return retval;
}


/*
 * This method sets the 'type' of the data that's being stored in this
 * instance. This is really an implementation method as the setters
 * of specific values really control the type, but in the interests
 * of encapsulation, I wanted to add this setter to the class.
 */
void CKVariant::setType( CKVariantType aType )
{
	switch (aType) {
		case eStringVariant:
		case eNumberVariant:
		case eDateVariant:
		case eTableVariant:
			mType = aType;
			break;
		default:
			std::cout << "CKVariant::setType(CKVariantType) - the "
				"supplied type: " << aType << " is not understood at this "
				" time. Please alert the developers." << std::endl;
	}
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKVariant & anItem )
{
	aStream << anItem.toString();

	return aStream;
}
