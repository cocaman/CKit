/*
 * CKVariant.h - this file defines a class that can be used to represent a
 *               general data value. The power of this class is that all the
 *               different kinds of values that this object can rperesent can
 *               then be treated as a single data type and thus really
 *               simplify dealing with tables of different types of data.
 *
 * $Id: CKVariant.h,v 1.14 2005/02/07 19:06:46 drbob Exp $
 */
#ifndef __CKVARIANT_H
#define __CKVARIANT_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKTimeSeries.h"
#include "CKString.h"

//	Forward Declarations
class CKTable;
class CKPrice;

//	Public Constants
/*
 * Since this data element can have different types of values, we need to
 * define what those different types are, and what their coded values will
 * be.
 */
enum CKVariantTypeEnum {
	eUnknownVariant = -1,
	eStringVariant = 0,
	eNumberVariant = 1,
	eDateVariant = 2,
	eTableVariant = 3,
	eTimeSeriesVariant = 4,
	ePriceVariant = 5
};
typedef CKVariantTypeEnum CKVariantType;

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKVariant
{
	public:
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
		CKVariant();
		/*
		 * This form of the constructor is nice in that it allows us to take
		 * the values right away and then the user doesn't have to worry
		 * about setting the values right after creation. This just makes
		 * things a lot nicer. The passed-in value will be converted to the
		 * proper datatype as defined by the type parameter. The value
		 * parameter will not be touched in this operation and memory
		 * management for the value arg will remain with the caller.
		 */
		CKVariant( CKVariantType aType, const char *aValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a String, and the value provided
		 * is what's to be stored. The string value will be copied to the
		 * local storage in this constructor so the management remains
		 * with the caller.
		 */
		CKVariant( const char *aStringValue );
		CKVariant( const CKString *aStringValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is an int (double), and the value
		 * provided is what's to be stored.
		 */
		CKVariant( int anIntValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a Date (long) of the form YYYYMMDD,
		 * and the value provided is what's to be stored.
		 */
		CKVariant( long aDateValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a double, and the value provided
		 * is what's to be stored.
		 */
		CKVariant( double aDoubleValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a CKTable, and the value provided
		 * is what's to be stored. The value argument will not be touched
		 * in this constructor as we'll be making a copy of the contents
		 * for local use.
		 */
		CKVariant( const CKTable *aTableValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a CKTimeSeries, and the value
		 * provided is what's to be stored. The value argument will not be
		 * touched in this constructor as we'll be making a copy of the
		 * contents for local use.
		 */
		CKVariant( const CKTimeSeries *aTimeSeriesValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a CKPrice, and the value provided
		 * is what's to be stored. The value argument will not be touched
		 * in this constructor as we'll be making a copy of the contents
		 * for local use.
		 */
		CKVariant( const CKPrice *aPriceValue );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKVariant( const CKVariant & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKVariant();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKVariant & operator=( CKVariant & anOther );
		CKVariant & operator=( const CKVariant & anOther );

		/*
		 * When we want to make a simple assignment to a CKVariant, these
		 * operators will make it easy to put the important data types in
		 * to the variant.
		 */
		CKVariant & operator=( const char *aString );
		CKVariant & operator=( int aValue );
		CKVariant & operator=( long aDateValue );
		CKVariant & operator=( double aValue );
		CKVariant & operator=( const CKString & aString );
		CKVariant & operator=( const CKTable & aTable );
		CKVariant & operator=( const CKTimeSeries & aTimeSeries );
		CKVariant & operator=( const CKPrice & aPrice );

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
		void setValueAsType( CKVariantType aType, const char *aValue );
		/*
		 * This sets the value stored in this instance as a string, but a
		 * local copy will be made so that the caller doesn't have to worry
		 * about holding on to the parameter, and is free to delete it.
		 */
		void setStringValue( const char *aStringValue );
		void setStringValue( const CKString *aStringValue );
		/*
		 * This method sets the value stored in this instance as a date of the
		 * form YYYYMMDD - stored as a long.
		 */
		void setDateValue( long aDateValue );
		/*
		 * This method sets the value stored in this instance as a double.
		 */
		void setDoubleValue( double aDoubleValue );
		/*
		 * This sets the value stored in this instance as a table, but a
		 * local copy will be made so that the caller doesn't have to worry
		 * about holding on to the parameter, and is free to delete it.
		 */
		void setTableValue( const CKTable *aTableValue );
		/*
		 * This sets the value stored in this instance as a time series,
		 * but a local copy will be made so that the caller doesn't have
		 * to worry about holding on to the parameter, and is free to
		 * delete it.
		 */
		void setTimeSeriesValue( const CKTimeSeries *aTimeSeriesValue );
		/*
		 * This sets the value stored in this instance as a price (native
		 * and USD), but a local copy will be made so that the caller
		 * doesn't have to worry about holding on to the parameter, and
		 * is free to delete it.
		 */
		void setPriceValue( const CKPrice *aPriceValue );

		/*
		 * This method returns the enumerated type of the data that this
		 * instance is currently holding.
		 */
		CKVariantType getType() const;

		/*
		 * This method will return the integer value of the data stored in this
		 * instance - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		int getIntValue() const;
		/*
		 * This method will return the double value of the data stored in this
		 * instance - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		double getDoubleValue() const;
		/*
		 * This method will return the date value of the data stored in this
		 * instance in a long of the form YYYYMMDD - if the type is date. If
		 * the data isn't date an exception will be thrown as it's assumed
		 * that the user should make sure that this instance is date *before*
		 * calling this method.
		 */
		long getDateValue() const;
		/*
		 * This method returns the actual string value of the data that
		 * this instance is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy,
		 * or call the getValueAsString() method that returns a copy.
		 */
		const CKString *getStringValue() const;
		/*
		 * This method returns the actual table value of the data that
		 * this instance is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTable *getTableValue() const;
		/*
		 * This method returns the actual time series value of the data that
		 * this instance is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTimeSeries *getTimeSeriesValue() const;
		/*
		 * This method returns the actual price value of the data that
		 * this instance is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKPrice *getPriceValue() const;

		/*
		 * This method can be used to clear out any existing value in the
		 * variant and reset it to it's "unknown" state. This is useful if
		 * you want to clean up the memory used by the variant in preparation
		 * for setting it to a new value.
		 */
		void clearValue();

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
		static bool isDouble( const char *aValue );
		/*
		 * When parsing the incoming data, it's important to be able
		 * to tell what the data coming back is. That's the purpose of this
		 * function - if the data (string) can be represented as an integer
		 * without problems then we return true, otherwise we return false.
		 */
		static bool isInteger( const char *aValue );
		/*
		 * When parsing the incoming data, it's important to be able
		 * to tell what the data coming back is. That's the purpose of this
		 * function - if the data (string) can be represented as an integer
		 * of the form YYYYMMDD without problems then we return true,
		 * otherwise we return false.
		 */
		static bool isDate( const char *aValue );
		/*
		 * When parsing the incoming data, it's important to be able
		 * to tell what the data coming back is. That's the purpose of this
		 * function - if the data (string) can be represented as a table
		 * without problems then we return true, otherwise we return false.
		 */
		static bool isTable( const char *aValue );

		/*
		 * This method returns a copy of the current value as contained in
		 * a string. This is returned as a CKString just so it's easy to use.
		 */
		CKString getValueAsString() const;
		/*
		 * This method returns a STL std::string that is a nice, clean,
		 * string representation of the data this instance is holding. It's
		 * functionally equivalent to getValueAsString() except that it
		 * returns a std::string on the stack so the user doesn't have to
		 * mess with deleting the memory.
		 */
		std::string getValueAsSTLString() const;
		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the value's data into a (char *) that
		 * can be converted to a Java String and then the Java object can
		 * interpret it and "reconstitue" the object from this coding.
		 */
		virtual CKString generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this value. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const CKString & aCode );
		/*
		 * This method checks to see if the two CKVariants are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKVariant & anOther ) const;
		/*
		 * This method checks to see if the two CKVariants are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKVariant & anOther ) const;
		/*
		 * These methods complete the inequality tests for the CKVariant and
		 * we need these as we might be doing a lot of testing and this makes
		 * it a lot easier than converting to the right type and then doing
		 * a low-level test.
		 */
		bool operator<( const CKVariant & anOther ) const;
		bool operator<=( const CKVariant & anOther ) const;
		bool operator>( const CKVariant & anOther ) const;
		bool operator>=( const CKVariant & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

		/*
		 * This method simply takes the inverse of the value so that
		 * x -> 1/x. This is useful in many cases, but in some it doesn't
		 * make sense (like strings), so skip those where it makes no
		 * sense.
		 */
		bool inverse();

		/*
		 * These operators allow us to use the variant as a "regular"
		 * variable in conditionals which is really important because
		 * we want this to fit into development like a regular scalar
		 * variable.
		 */
		bool operator==( const char *aCString ) const;
		bool operator==( const std::string & anSTLString ) const;
		bool operator==( const CKString & aString ) const;
		bool operator==( int aValue ) const;
		bool operator==( long aDateValue ) const;
		bool operator==( double aValue ) const;
		bool operator==( const CKTable & aTable ) const;
		bool operator==( const CKTimeSeries & aSeries ) const;
		bool operator==( const CKPrice & aPrice ) const;

		bool operator!=( const char *aCString ) const;
		bool operator!=( const std::string & anSTLString ) const;
		bool operator!=( const CKString & aString ) const;
		bool operator!=( int aValue ) const;
		bool operator!=( long aDateValue ) const;
		bool operator!=( double aValue ) const;
		bool operator!=( const CKTable & aTable ) const;
		bool operator!=( const CKTimeSeries & aSeries ) const;
		bool operator!=( const CKPrice & aPrice ) const;

		bool operator<( const char *aCString ) const;
		bool operator<( const std::string & anSTLString ) const;
		bool operator<( const CKString & aString ) const;
		bool operator<( int aValue ) const;
		bool operator<( long aDateValue ) const;
		bool operator<( double aValue ) const;
		bool operator<( const CKTable & aTable ) const;
		bool operator<( const CKTimeSeries & aSeries ) const;
		bool operator<( const CKPrice & aPrice ) const;

		bool operator<=( const char *aCString ) const;
		bool operator<=( const std::string & anSTLString ) const;
		bool operator<=( const CKString & aString ) const;
		bool operator<=( int aValue ) const;
		bool operator<=( long aDateValue ) const;
		bool operator<=( double aValue ) const;
		bool operator<=( const CKTable & aTable ) const;
		bool operator<=( const CKTimeSeries & aSeries ) const;
		bool operator<=( const CKPrice & aPrice ) const;

		bool operator>( const char *aCString ) const;
		bool operator>( const std::string & anSTLString ) const;
		bool operator>( const CKString & aString ) const;
		bool operator>( int aValue ) const;
		bool operator>( long aDateValue ) const;
		bool operator>( double aValue ) const;
		bool operator>( const CKTable & aTable ) const;
		bool operator>( const CKTimeSeries & aSeries ) const;
		bool operator>( const CKPrice & aPrice ) const;

		bool operator>=( const char *aCString ) const;
		bool operator>=( const std::string & anSTLString ) const;
		bool operator>=( const CKString & aString ) const;
		bool operator>=( int aValue ) const;
		bool operator>=( long aDateValue ) const;
		bool operator>=( double aValue ) const;
		bool operator>=( const CKTable & aTable ) const;
		bool operator>=( const CKTimeSeries & aSeries ) const;
		bool operator>=( const CKPrice & aPrice ) const;

		/*
		 * These operators are the convenience assignment operators for
		 * the variant and are meant to make it easy to use these guys in
		 * code. If the operation doesn't make sense for the data an
		 * exception will be thrown - such as adding a string to a price.
		 */
		CKVariant & operator+=( const char *aCString );
		CKVariant & operator+=( const std::string & anSTLString );
		CKVariant & operator+=( const CKString & aString );
		CKVariant & operator+=( int aValue );
		CKVariant & operator+=( long aDateValue );
		CKVariant & operator+=( double aValue );
		CKVariant & operator+=( const CKTable & aTable );
		CKVariant & operator+=( const CKTimeSeries & aSeries );
		CKVariant & operator+=( const CKPrice & aPrice );
		CKVariant & operator+=( const CKVariant & aVar );

		CKVariant & operator-=( int aValue );
		CKVariant & operator-=( long aDateValue );
		CKVariant & operator-=( double aValue );
		CKVariant & operator-=( const CKTable & aTable );
		CKVariant & operator-=( const CKTimeSeries & aSeries );
		CKVariant & operator-=( const CKPrice & aPrice );
		CKVariant & operator-=( const CKVariant & aVar );

		CKVariant & operator*=( int aValue );
		CKVariant & operator*=( long aDateValue );
		CKVariant & operator*=( double aValue );
		CKVariant & operator*=( const CKTable & aTable );
		CKVariant & operator*=( const CKTimeSeries & aSeries );
		CKVariant & operator*=( const CKPrice & aPrice );
		CKVariant & operator*=( const CKVariant & aVar );

		CKVariant & operator/=( int aValue );
		CKVariant & operator/=( long aDateValue );
		CKVariant & operator/=( double aValue );
		CKVariant & operator/=( const CKTable & aTable );
		CKVariant & operator/=( const CKTimeSeries & aSeries );
		CKVariant & operator/=( const CKPrice & aPrice );
		CKVariant & operator/=( const CKVariant & aVar );

		/*
		 * There are times that variants will be used in mathematical
		 * expressions by themselves, these operator functions will make
		 * it very easy for the user to do simple 'a + b' coding on
		 * even the most complex structures.
		 */
		friend CKVariant operator+( CKVariant & aVar, CKVariant & anOtherVar );
		friend CKVariant operator-( CKVariant & aVar, CKVariant & anOtherVar );
		friend CKVariant operator*( CKVariant & aVar, CKVariant & anOtherVar );
		friend CKVariant operator/( CKVariant & aVar, CKVariant & anOtherVar );

		/*
		 * These operator functions will allow the mixed-mode math with
		 * variants casting the result up to a variant in each case. First,
		 * start with simple addition.
		 */
		friend CKVariant operator+( CKVariant & aVar, const char *aCString );
		friend CKVariant operator+( const char *aCString, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, const std::string & anSTLString );
		friend CKVariant operator+( const std::string & anSTLString, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, const CKString & aString );
		friend CKVariant operator+( const CKString & aString, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, int aValue );
		friend CKVariant operator+( int aValue, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, long aDateValue );
		friend CKVariant operator+( long aDateValue, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, double aValue );
		friend CKVariant operator+( double aValue, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, const CKTable & aTable );
		friend CKVariant operator+( const CKTable & aTable, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, const CKTimeSeries & aSeries );
		friend CKVariant operator+( const CKTimeSeries & aSeries, CKVariant & aVar );

		friend CKVariant operator+( CKVariant & aVar, const CKPrice & aPrice );
		friend CKVariant operator+( const CKPrice & aPrice, CKVariant & aVar );

		/*
		 * Next, let's do all the different forms of the subtraction
		 * operator between the variant and the other scalar types.
		 */
		friend CKVariant operator-( CKVariant & aVar, int aValue );
		friend CKVariant operator-( int aValue, CKVariant & aVar );

		friend CKVariant operator-( CKVariant & aVar, long aDateValue );
		friend CKVariant operator-( long aDateValue, CKVariant & aVar );

		friend CKVariant operator-( CKVariant & aVar, double aValue );
		friend CKVariant operator-( double aValue, CKVariant & aVar );

		friend CKVariant operator-( CKVariant & aVar, const CKTable & aTable );
		friend CKVariant operator-( const CKTable & aTable, CKVariant & aVar );

		friend CKVariant operator-( CKVariant & aVar, const CKTimeSeries & aSeries );
		friend CKVariant operator-( const CKTimeSeries & aSeries, CKVariant & aVar );

		friend CKVariant operator-( CKVariant & aVar, const CKPrice & aPrice );
		friend CKVariant operator-( const CKPrice & aPrice, CKVariant & aVar );

		/*
		 * Next, let's do all the different forms of the multiplication
		 * operator between the variant and the other scalar types.
		 */
		friend CKVariant operator*( CKVariant & aVar, int aValue );
		friend CKVariant operator*( int aValue, CKVariant & aVar );

		friend CKVariant operator*( CKVariant & aVar, long aDateValue );
		friend CKVariant operator*( long aDateValue, CKVariant & aVar );

		friend CKVariant operator*( CKVariant & aVar, double aValue );
		friend CKVariant operator*( double aValue, CKVariant & aVar );

		friend CKVariant operator*( CKVariant & aVar, const CKTable & aTable );
		friend CKVariant operator*( const CKTable & aTable, CKVariant & aVar );

		friend CKVariant operator*( CKVariant & aVar, const CKTimeSeries & aSeries );
		friend CKVariant operator*( const CKTimeSeries & aSeries, CKVariant & aVar );

		friend CKVariant operator*( CKVariant & aVar, const CKPrice & aPrice );
		friend CKVariant operator*( const CKPrice & aPrice, CKVariant & aVar );

		/*
		 * Next, let's do all the different forms of the division
		 * operator between the variant and the other scalar types.
		 */
		friend CKVariant operator/( CKVariant & aVar, int aValue );
		friend CKVariant operator/( int aValue, CKVariant & aVar );

		friend CKVariant operator/( CKVariant & aVar, long aDateValue );
		friend CKVariant operator/( long aDateValue, CKVariant & aVar );

		friend CKVariant operator/( CKVariant & aVar, double aValue );
		friend CKVariant operator/( double aValue, CKVariant & aVar );

		friend CKVariant operator/( CKVariant & aVar, const CKTable & aTable );
		friend CKVariant operator/( const CKTable & aTable, CKVariant & aVar );

		friend CKVariant operator/( CKVariant & aVar, const CKTimeSeries & aSeries );
		friend CKVariant operator/( const CKTimeSeries & aSeries, CKVariant & aVar );

		friend CKVariant operator/( CKVariant & aVar, const CKPrice & aPrice );
		friend CKVariant operator/( const CKPrice & aPrice, CKVariant & aVar );

	protected:
		/*
		 * This method sets the 'type' of the data that's being stored in this
		 * instance. This is really an implementation method as the setters
		 * of specific values really control the type, but in the interests
		 * of encapsulation, I wanted to add this setter to the class.
		 */
		void setType( CKVariantType aType );

	private:
		/*
		 * This is the enumerated code for the type of data that's being held
		 * in this instance under one of the values in the union.
		 */
		CKVariantType	mType;
		/*
		 * These are the different possible data elements that this object
		 * can hold, and will be set differently based on how the user
		 * sets this data.
		 */
		union {
			CKString		*mStringValue;
			long			mDateValue;
			double			mDoubleValue;
			CKTable			*mTableValue;
			CKTimeSeries	*mTimeSeriesValue;
			CKPrice			*mPriceValue;
		};
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKVariant & anItem );

/*
 * There are times that variants will be used in mathematical
 * expressions by themselves, these operator functions will make
 * it very easy for the user to do simple 'a + b' coding on
 * even the most complex structures.
 */
CKVariant operator+( CKVariant & aVar, CKVariant & anOtherVar );
CKVariant operator-( CKVariant & aVar, CKVariant & anOtherVar );
CKVariant operator*( CKVariant & aVar, CKVariant & anOtherVar );
CKVariant operator/( CKVariant & aVar, CKVariant & anOtherVar );

/*
 * These operator functions will allow the mixed-mode math with
 * variants casting the result up to a variant in each case. First,
 * start with simple addition.
 */
CKVariant operator+( CKVariant & aVar, const char *aCString );
CKVariant operator+( const char *aCString, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, const std::string & anSTLString );
CKVariant operator+( const std::string & anSTLString, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, const CKString & aString );
CKVariant operator+( const CKString & aString, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, int aValue );
CKVariant operator+( int aValue, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, long aDateValue );
CKVariant operator+( long aDateValue, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, double aValue );
CKVariant operator+( double aValue, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, const CKTable & aTable );
CKVariant operator+( const CKTable & aTable, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, const CKTimeSeries & aSeries );
CKVariant operator+( const CKTimeSeries & aSeries, CKVariant & aVar );

CKVariant operator+( CKVariant & aVar, const CKPrice & aPrice );
CKVariant operator+( const CKPrice & aPrice, CKVariant & aVar );

/*
 * Next, let's do all the different forms of the subtraction
 * operator between the variant and the other scalar types.
 */
CKVariant operator-( CKVariant & aVar, int aValue );
CKVariant operator-( int aValue, CKVariant & aVar );

CKVariant operator-( CKVariant & aVar, long aDateValue );
CKVariant operator-( long aDateValue, CKVariant & aVar );

CKVariant operator-( CKVariant & aVar, double aValue );
CKVariant operator-( double aValue, CKVariant & aVar );

CKVariant operator-( CKVariant & aVar, const CKTable & aTable );
CKVariant operator-( const CKTable & aTable, CKVariant & aVar );

CKVariant operator-( CKVariant & aVar, const CKTimeSeries & aSeries );
CKVariant operator-( const CKTimeSeries & aSeries, CKVariant & aVar );

CKVariant operator-( CKVariant & aVar, const CKPrice & aPrice );
CKVariant operator-( const CKPrice & aPrice, CKVariant & aVar );

/*
 * Next, let's do all the different forms of the multiplication
 * operator between the variant and the other scalar types.
 */
CKVariant operator*( CKVariant & aVar, int aValue );
CKVariant operator*( int aValue, CKVariant & aVar );

CKVariant operator*( CKVariant & aVar, long aDateValue );
CKVariant operator*( long aDateValue, CKVariant & aVar );

CKVariant operator*( CKVariant & aVar, double aValue );
CKVariant operator*( double aValue, CKVariant & aVar );

CKVariant operator*( CKVariant & aVar, const CKTable & aTable );
CKVariant operator*( const CKTable & aTable, CKVariant & aVar );

CKVariant operator*( CKVariant & aVar, const CKTimeSeries & aSeries );
CKVariant operator*( const CKTimeSeries & aSeries, CKVariant & aVar );

CKVariant operator*( CKVariant & aVar, const CKPrice & aPrice );
CKVariant operator*( const CKPrice & aPrice, CKVariant & aVar );

/*
 * Next, let's do all the different forms of the division
 * operator between the variant and the other scalar types.
 */
CKVariant operator/( CKVariant & aVar, int aValue );
CKVariant operator/( int aValue, CKVariant & aVar );

CKVariant operator/( CKVariant & aVar, long aDateValue );
CKVariant operator/( long aDateValue, CKVariant & aVar );

CKVariant operator/( CKVariant & aVar, double aValue );
CKVariant operator/( double aValue, CKVariant & aVar );

CKVariant operator/( CKVariant & aVar, const CKTable & aTable );
CKVariant operator/( const CKTable & aTable, CKVariant & aVar );

CKVariant operator/( CKVariant & aVar, const CKTimeSeries & aSeries );
CKVariant operator/( const CKTimeSeries & aSeries, CKVariant & aVar );

CKVariant operator/( CKVariant & aVar, const CKPrice & aPrice );
CKVariant operator/( const CKPrice & aPrice, CKVariant & aVar );

#endif	// __CKVARIANT_H
