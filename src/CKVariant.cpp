/*
 * CKVariant.cpp - this file defines a class that can be used to represent a
 *                 general data value. The power of this class is that all the
 *                 different kinds of values that this object can rperesent can
 *                 then be treated as a single data type and thus really
 *                 simplify dealing with tables of different types of data.
 *
 * $Id: CKVariant.cpp,v 1.18 2005/01/20 15:55:08 drbob Exp $
 */

//	System Headers
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <strings.h>

//	Third-Party Headers
#include <CKException.h>

//	Other Headers
#include "CKVariant.h"
#include "CKTable.h"
#include "CKTimeSeries.h"
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


CKVariant::CKVariant( const CKString *aStringValue ) :
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
 * This form of the constructor understands that the value that's
 * intended to be stored here is a CKTimeSeries, and the value
 * provided is what's to be stored. The value argument will not be
 * touched in this constructor as we'll be making a copy of the
 * contents for local use.
 */
CKVariant::CKVariant( const CKTimeSeries *aTimeSeriesValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setTimeSeriesValue(aTimeSeriesValue);
}


/*
 * This form of the constructor understands that the value that's
 * intended to be stored here is a CKPrice, and the value provided
 * is what's to be stored. The value argument will not be touched
 * in this constructor as we'll be making a copy of the contents
 * for local use.
 */
CKVariant::CKVariant( const CKPrice *aPriceValue ) :
	mType(eUnknownVariant),
	mStringValue(NULL)
{
	setPriceValue(aPriceValue);
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
	clearValue();
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
			clearValue();
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
		case eTimeSeriesVariant:
			setTimeSeriesValue(anOther.getTimeSeriesValue());
			break;
		case ePriceVariant:
			setPriceValue(anOther.getPriceValue());
			break;
	}

	return *this;
}


/*
 * When we want to make a simple assignment to a CKVariant, these
 * operators will make it easy to put the important data types in
 * to the variant.
 */
CKVariant & CKVariant::operator=( const char *aString )
{
	setStringValue(aString);
	return *this;
}


CKVariant & CKVariant::operator=( int aValue )
{
	setDoubleValue((double)aValue);
	return *this;
}


CKVariant & CKVariant::operator=( long aDateValue )
{
	setDateValue(aDateValue);
	return *this;
}


CKVariant & CKVariant::operator=( double aValue )
{
	setDoubleValue(aValue);
	return *this;
}


CKVariant & CKVariant::operator=( const CKString & aString )
{
	setStringValue(aString.c_str());
	return *this;
}


CKVariant & CKVariant::operator=( const CKTable & aTable )
{
	setTableValue(&aTable);
	return *this;
}


CKVariant & CKVariant::operator=( const CKTimeSeries & aTimeSeries )
{
	setTimeSeriesValue(&aTimeSeries);
	return *this;
}


CKVariant & CKVariant::operator=( const CKPrice & aPrice )
{
	setPriceValue(&aPrice);
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
	switch (aType) {
		case eUnknownVariant:
			/*
			 * OK... we need to see what this data is and act on that.
			 * Thankfully, we have some helper functions for this.
			 */
			if (isTable(aValue)) {
				// this could be a table, price or timeseries... check the value
				double	v = strtod(&(aValue[1]), (char **)NULL);
				if (v != floor(v)) {
					// make a price from the string representation
					CKPrice		price(aValue);
					// ...and use that as the value
					setPriceValue(&price);
				} else if ((v > 19760000) && (v < 20100000)) {
					// make a time series from the string representation
					CKTimeSeries	ts(aValue);
					// ...and use that as the value
					setTimeSeriesValue(&ts);
				} else {
					// make a table from the string representation
					CKTable		tbl(aValue);
					// ...and use that as the value
					setTableValue(&tbl);
				}
			} else if (isDate(aValue)) {
				setDateValue(strtol(aValue, (char **)NULL, 10));
			} else if (isDouble(aValue)) {
				setDoubleValue(strtod(aValue, (char **)NULL));
			} else {
				// everything else is a string
				setStringValue(aValue);
			}
			break;
		case eStringVariant:
			setStringValue(aValue);
			break;
		case eNumberVariant:
			setDoubleValue(strtod(aValue, (char **)NULL));
			break;
		case eDateVariant:
			setDateValue(strtol(aValue, (char **)NULL, 10));
			break;
		case eTableVariant:
			{
				// make a table from the string representation
				CKTable	tbl(aValue);
				// ...and use that as the value
				setTableValue(&tbl);
			}
			break;
		case eTimeSeriesVariant:
			{
				// make a time series from the string representation
				CKTimeSeries	ts(aValue);
				// ...and use that as the value
				setTimeSeriesValue(&ts);
			}
			break;
		case ePriceVariant:
			{
				// make a price from the string representation
				CKPrice		price(aValue);
				// ...and use that as the value
				setPriceValue(&price);
			}
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
	clearValue();
	// next, if we have something to set, then create space for it
	if (aStringValue != NULL) {
		mStringValue = new CKString(aStringValue);
		if (mStringValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setStringValue"
				"(const char *) - the space to hold this string value could "
				"not be created. This is a serious allocation error.");
		}
	}
	// ...and don't forget to set the type of data we have now
	mType = eStringVariant;
}


void CKVariant::setStringValue( const CKString *aStringValue )
{
	// first, see if we need to delete what's might already be here
	clearValue();
	// next, if we have something to set, then create space for it
	if (aStringValue != NULL) {
		mStringValue = new CKString(*aStringValue);
		if (mStringValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setStringValue"
				"(const char *) - the space to hold this string value could "
				"not be created. This is a serious allocation error.");
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
	clearValue();
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
	clearValue();
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
	clearValue();
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
 * This sets the value stored in this instance as a time series,
 * but a local copy will be made so that the caller doesn't have
 * to worry about holding on to the parameter, and is free to
 * delete it.
 */
void CKVariant::setTimeSeriesValue( const CKTimeSeries *aTimeSeriesValue )
{
	// first, see if we need to delete what's might already be here
	clearValue();
	// next, if we have something to set, then create space for it
	if (aTimeSeriesValue != NULL) {
		mTimeSeriesValue = new CKTimeSeries(*aTimeSeriesValue);
		if (mTimeSeriesValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setTimeSeries"
				"Value(const CKTimeSeries *) - the copy of this time series "
				"value could not be created. This is a serious allocation "
				"error.");
		}
	}
	// ...and don't forget to set the type of data we have now
	mType = eTimeSeriesVariant;
}


/*
 * This sets the value stored in this instance as a price (native
 * and USD), but a local copy will be made so that the caller
 * doesn't have to worry about holding on to the parameter, and
 * is free to delete it.
 */
void CKVariant::setPriceValue( const CKPrice *aPriceValue )
{
	// first, see if we need to delete what's might already be here
	clearValue();
	// next, if we have something to set, then create space for it
	if (aPriceValue != NULL) {
		mPriceValue = new CKPrice(*aPriceValue);
		if (mPriceValue == NULL) {
			throw CKException(__FILE__, __LINE__, "CKVariant::setPriceValue("
				"const CKPrice *) - the copy of this price value could not be "
				"created. This is a serious allocation error.");
		}
	}
	// ...and don't forget to set the type of data we have now
	mType = ePriceVariant;
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
const CKString *CKVariant::getStringValue() const
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


/*
 * This method returns the actual time series value of the data that
 * this instance is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKTimeSeries *CKVariant::getTimeSeriesValue() const
{
	// make sure it's something that can be done
	if (mType != eTimeSeriesVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getTimeSeriesValue() - "
			"the data contained in this instance is not a time series and "
			"therefore we can't get a time series value from it.");
	}
	return mTimeSeriesValue;
}


/*
 * This method returns the actual price value of the data that
 * this instance is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKPrice *CKVariant::getPriceValue() const
{
	// make sure it's something that can be done
	if (mType != ePriceVariant) {
		throw CKException(__FILE__, __LINE__, "CKVariant::getPriceValue() - "
			"the data contained in this instance is not a price and "
			"therefore we can't get a price value from it.");
	}
	return mPriceValue;
}


/*
 * This method can be used to clear out any existing value in the
 * variant and reset it to it's "unknown" state. This is useful if
 * you want to clean up the memory used by the variant in preparation
 * for setting it to a new value.
 */
void CKVariant::clearValue()
{
	// first, free up any memory used by the current value
	switch(mType) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			if (mStringValue != NULL) {
				delete mStringValue;
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
		case eTimeSeriesVariant:
			if (mTimeSeriesValue != NULL) {
				delete mTimeSeriesValue;
				mTimeSeriesValue = NULL;
			}
			break;
		case ePriceVariant:
			if (mPriceValue != NULL) {
				delete mPriceValue;
				mPriceValue = NULL;
			}
			break;
	}

	// don't forget to set it to 'unknown'
	mType = eUnknownVariant;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * When parsing the incoming data, it's important to be able
 * to tell what the data coming back is. That's the purpose of this
 * function - if the data (string) can be represented as a double
 * without problems then we return true, otherwise we return false.
 */
bool CKVariant::isDouble( const char *aValue )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (aValue == NULL) {
			error = true;
		}
	}

	// find the first character that is NOT a digit
	if (!error) {
		int 	len = strlen(aValue);
		char	c;
		for (int i = 0; i < len; i++) {
			c = aValue[i];
			if (!(isdigit(c) || (c == '.') || (c == '+') || (c == '-') ||
				  (c == 'e') || (c == 'E'))) {
				error = true;
				break;
			}
		}
	}

	return !error;
}


/*
 * When parsing the incoming data, it's important to be able
 * to tell what the data coming back is. That's the purpose of this
 * function - if the data (string) can be represented as an integer
 * without problems then we return true, otherwise we return false.
 */
bool CKVariant::isInteger( const char *aValue )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (aValue == NULL) {
			error = true;
		}
	}

	// find the first character that is NOT a digit
	if (!error) {
		int len = strlen(aValue);
		for (int i = 0; i < len; i++) {
			if (!isdigit(aValue[i])) {
				error = true;
				break;
			}
		}
	}

	return !error;
}


/*
 * When parsing the incoming data, it's important to be able
 * to tell what the data coming back is. That's the purpose of this
 * function - if the data (string) can be represented as an integer
 * of the form YYYYMMDD without problems then we return true,
 * otherwise we return false.
 */
bool CKVariant::isDate( const char *aValue )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (aValue == NULL) {
			error = true;
		}
	}

	// see if it's not an integer - if it isn't, then we're done
	if (!error) {
		if (!isInteger(aValue)) {
			error = true;
		}
	}

	// it's got to be YYYYMMDD which is 8 characters
	if (!error) {
		if (strlen(aValue) != 8) {
			error = true;
		}
	}

	// check the components of the value as well
	if (!error) {
		double	v = strtod(aValue, (char **)NULL);
		int		year = (int)(v/10000);
		int		month = (int)((v - year*10000)/100);
		int		day = (int)(v - (year*100 + month)*100);

		if ((year < 1980) || (year > 2010) ||
			(month < 1) || (month > 12) ||
			(day < 1) || (day > 31)) {
			error = true;
		}
	}

	return !error;
}


/*
 * When parsing the incoming data, it's important to be able
 * to tell what the data coming back is. That's the purpose of this
 * function - if the data (string) can be represented as a table
 * without problems then we return true, otherwise we return false.
 */
bool CKVariant::isTable( const char *aValue )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (aValue == NULL) {
			error = true;
		}
	}

	// find the first character that is NOT a digit
	if (!error) {
		if ((aValue[0] != aValue[strlen(aValue) - 1]) ||
			isalnum(aValue[0]) ||
			(aValue[0] == '+') || (aValue[0] == '-') ||
			(aValue[1] != '2') || (strlen(aValue) < 15)) {
			error = true;
		}
	}

	return !error;
}


/*
 * This method returns a copy of the current value as contained in
 * a string. This is returned as a CKString just so it's easy to use.
 */
CKString CKVariant::getValueAsString() const
{
	// first, create a string and then set it's value
	CKString		retval;
	switch (mType) {
		case eUnknownVariant:
			retval += "<unknown>";
			break;
		case eStringVariant:
			retval += *mStringValue;
			break;
		case eNumberVariant:
			retval += mDoubleValue;
			break;
		case eDateVariant:
			retval += mDateValue;
			break;
		case eTableVariant:
			if (mTableValue == NULL) {
				retval += "NULL";
			} else {
				retval += mTableValue->toString();
			}
			break;
		case eTimeSeriesVariant:
			if (mTimeSeriesValue == NULL) {
				retval += "NULL";
			} else {
				retval += mTimeSeriesValue->toString();
			}
			break;
		case ePriceVariant:
			if (mPriceValue == NULL) {
				retval += "NULL";
			} else {
				retval += mPriceValue->toString();
			}
			break;
	}

	return retval;
}


/*
 * This method returns a STL std::string that is a nice, clean,
 * string representation of the data this instance is holding. It's
 * functionally equivalent to getValueAsString() except that it
 * returns a std::string on the stack so the user doesn't have to
 * mess with deleting the memory.
 */
std::string CKVariant::getValueAsSTLString() const
{
	return getValueAsString().stl_str();
}


/*
 * In order to simplify the move of this object from C++ to Java
 * it makes sense to encode the value's data into a (char *) that
 * can be converted to a Java String and then the Java object can
 * interpret it and "reconstitue" the object from this coding.
 */
CKString CKVariant::generateCodeFromValues() const
{
	CKString buff;
	switch (getType()) {
		case eUnknownVariant:
			buff.append("U:");
			break;
		case eStringVariant:
			buff.append("S:").append(*mStringValue);
			break;
		case eNumberVariant:
			buff.append("N:").append(mDoubleValue);
			break;
		case eDateVariant:
			buff.append("D:").append(mDateValue);
			break;
		case eTableVariant:
			if (mTableValue == NULL) {
				buff.append("U:");
			} else {
				buff.append("T:").append(mTableValue->generateCodeFromValues());
			}
			break;
		case eTimeSeriesVariant:
			if (mTimeSeriesValue == NULL) {
				buff.append("U:");
			} else {
				buff.append("L:").append(mTimeSeriesValue->generateCodeFromValues());
			}
			break;
		case ePriceVariant:
			if (mPriceValue == NULL) {
				buff.append("U:");
			} else {
				buff.append("P:").append(mPriceValue->generateCodeFromValues());
			}
			break;
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
void CKVariant::takeValuesFromCode( const CKString & aCode )
{
	switch (aCode[0]) {
		case 'U':
			clearValue();
			break;
		case 'S':
			setValueAsType(eStringVariant, aCode.substr(2).c_str());
			break;
		case 'N':
			setValueAsType(eNumberVariant, aCode.substr(2).c_str());
			break;
		case 'D':
			setValueAsType(eDateVariant, aCode.substr(2).c_str());
			break;
		case 'T':
			setValueAsType(eTableVariant, aCode.substr(2).c_str());
			break;
		case 'L':
			setValueAsType(eTimeSeriesVariant, aCode.substr(2).c_str());
			break;
		case 'P':
			setValueAsType(ePriceVariant, aCode.substr(2).c_str());
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
						if ((*mStringValue) != (*anOther.mStringValue)) {
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
			case eTimeSeriesVariant:
				// two NULLs match in my opinion
				if (mTimeSeriesValue == NULL) {
					if (anOther.mTimeSeriesValue != NULL) {
						equal = false;
					}
				} else {
					if (anOther.mTimeSeriesValue == NULL) {
						equal = false;
					} else {
						if ((*mTimeSeriesValue) != (*anOther.mTimeSeriesValue)) {
							equal = false;
						}
					}
				}
				break;
			case ePriceVariant:
				// two NULLs match in my opinion
				if (mPriceValue == NULL) {
					if (anOther.mPriceValue != NULL) {
						equal = false;
					}
				} else {
					if (anOther.mPriceValue == NULL) {
						equal = false;
					} else {
						if ((*mPriceValue) != (*anOther.mPriceValue)) {
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
 * the representation, so this returns a CKString.
 */
CKString CKVariant::toString() const
{
	CKString		retval;
	char			buff[128];
	// first, send out the type as if it were a 'cast' of the data
	switch (mType) {
		case eUnknownVariant:
			retval = "(unknown)??";
			break;
		case eStringVariant:
			retval = "(String)";
			if (mStringValue == NULL) {
				retval.append("NULL");
			} else {
				retval.append(*mStringValue);
			}
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
			retval = "(CKTable)";
			retval.append(mTableValue->toString());
			break;
		case eTimeSeriesVariant:
			retval = "(CKTimeSeries)";
			retval.append(mTimeSeriesValue->toString());
			break;
		case ePriceVariant:
			retval = "(CKPrice)";
			retval.append(mPriceValue->toString());
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
		case eTimeSeriesVariant:
		case ePriceVariant:
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
