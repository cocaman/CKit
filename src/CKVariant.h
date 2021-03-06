/*
 * CKVariant.h - this file defines a class that can be used to represent a
 *               general data value. The power of this class is that all the
 *               different kinds of values that this object can rperesent can
 *               then be treated as a single data type and thus really
 *               simplify dealing with tables of different types of data.
 *
 * $Id: CKVariant.h,v 1.19 2008/12/23 17:36:23 drbob Exp $
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
class CKVariantList;
class CKTimeTable;

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
	ePriceVariant = 5,
	eListVariant = 6,
	eTimeTableVariant = 7
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
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a CKVariantList, and the value
		 * provided is what's to be stored. The value argument will not be
		 * touched in this constructor as we'll be making a copy of the
		 * contents for local use.
		 */
		CKVariant( const CKVariantList *aListValue );
		/*
		 * This form of the constructor understands that the value that's
		 * intended to be stored here is a CKTimeTable, and the value provided
		 * is what's to be stored. The value argument will not be touched
		 * in this constructor as we'll be making a copy of the contents
		 * for local use.
		 */
		CKVariant( const CKTimeTable *aTimeTableValue );
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
		CKVariant & operator=( const CKVariantList & aList );
		CKVariant & operator=( const CKTimeTable & aTimeTable );

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
		 * This sets the value stored in this instance as a list of
		 * variants, but a local copy will be made so that the caller
		 * doesn't have to worry about holding on to the parameter, and
		 * is free to delete it.
		 */
		void setListValue( const CKVariantList *aListValue );
		/*
		 * This sets the value stored in this instance to a copy of the
		 * time table that's pointed to by the argument. Because we'll
		 * be making a copy, the caller is still in control of the
		 * argument.
		 */
		void setTimeTableValue( const CKTimeTable *aTimeTableValue );

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
		 * This method returns the actual list of the variants that
		 * this instance is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKVariantList *getListValue() const;
		/*
		 * This method returns the actual time table that this instance is
		 * holding. If the user wants to use this value outside the scope
		 * of this class, then they need to make a copy.
		 */
		const CKTimeTable *getTimeTableValue() const;

		/*
		 * These operators allow me to use the variants as regular types
		 * when casting to the right type. The trick is that if the variant
		 * is not the casted type, an exception will be thrown due to an
		 * illegal cast.
		 */
		operator int() const;
		operator double() const;
		operator long() const;
		operator CKString &() const;
		operator CKString *() const;
		operator CKTable &() const;
		operator CKTable *() const;
		operator CKTimeSeries &() const;
		operator CKTimeSeries *() const;
		operator CKPrice &() const;
		operator CKPrice *() const;
		operator CKVariantList &() const;
		operator CKVariantList *() const;
		operator CKTimeTable &() const;
		operator CKTimeTable *() const;

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
		 * When parsing the incoming data, it's important to be able
		 * to tell what the data coming back is. That's the purpose of this
		 * function - if the data (string) can be represented as a list
		 * without problems then we return true, otherwise we return false.
		 */
		static bool isList( const char *aValue );

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
		bool operator==( const CKVariantList & aList ) const;
		bool operator==( const CKTimeTable & aTimeTable ) const;

		bool operator!=( const char *aCString ) const;
		bool operator!=( const std::string & anSTLString ) const;
		bool operator!=( const CKString & aString ) const;
		bool operator!=( int aValue ) const;
		bool operator!=( long aDateValue ) const;
		bool operator!=( double aValue ) const;
		bool operator!=( const CKTable & aTable ) const;
		bool operator!=( const CKTimeSeries & aSeries ) const;
		bool operator!=( const CKPrice & aPrice ) const;
		bool operator!=( const CKVariantList & aList ) const;
		bool operator!=( const CKTimeTable & aTimeTable ) const;

		bool operator<( const char *aCString ) const;
		bool operator<( const std::string & anSTLString ) const;
		bool operator<( const CKString & aString ) const;
		bool operator<( int aValue ) const;
		bool operator<( long aDateValue ) const;
		bool operator<( double aValue ) const;
		bool operator<( const CKTable & aTable ) const;
		bool operator<( const CKTimeSeries & aSeries ) const;
		bool operator<( const CKPrice & aPrice ) const;
		bool operator<( const CKVariantList & aList ) const;
		bool operator<( const CKTimeTable & aTimeTable ) const;

		bool operator<=( const char *aCString ) const;
		bool operator<=( const std::string & anSTLString ) const;
		bool operator<=( const CKString & aString ) const;
		bool operator<=( int aValue ) const;
		bool operator<=( long aDateValue ) const;
		bool operator<=( double aValue ) const;
		bool operator<=( const CKTable & aTable ) const;
		bool operator<=( const CKTimeSeries & aSeries ) const;
		bool operator<=( const CKPrice & aPrice ) const;
		bool operator<=( const CKVariantList & aList ) const;
		bool operator<=( const CKTimeTable & aTimeTable ) const;

		bool operator>( const char *aCString ) const;
		bool operator>( const std::string & anSTLString ) const;
		bool operator>( const CKString & aString ) const;
		bool operator>( int aValue ) const;
		bool operator>( long aDateValue ) const;
		bool operator>( double aValue ) const;
		bool operator>( const CKTable & aTable ) const;
		bool operator>( const CKTimeSeries & aSeries ) const;
		bool operator>( const CKPrice & aPrice ) const;
		bool operator>( const CKVariantList & aList ) const;
		bool operator>( const CKTimeTable & aTimeTable ) const;

		bool operator>=( const char *aCString ) const;
		bool operator>=( const std::string & anSTLString ) const;
		bool operator>=( const CKString & aString ) const;
		bool operator>=( int aValue ) const;
		bool operator>=( long aDateValue ) const;
		bool operator>=( double aValue ) const;
		bool operator>=( const CKTable & aTable ) const;
		bool operator>=( const CKTimeSeries & aSeries ) const;
		bool operator>=( const CKPrice & aPrice ) const;
		bool operator>=( const CKVariantList & aList ) const;
		bool operator>=( const CKTimeTable & aTimeTable ) const;

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
		CKVariant & operator+=( const CKVariantList & aList );
		CKVariant & operator+=( const CKTimeTable & aTimeTable );

		CKVariant & operator-=( int aValue );
		CKVariant & operator-=( long aDateValue );
		CKVariant & operator-=( double aValue );
		CKVariant & operator-=( const CKTable & aTable );
		CKVariant & operator-=( const CKTimeSeries & aSeries );
		CKVariant & operator-=( const CKPrice & aPrice );
		CKVariant & operator-=( const CKVariant & aVar );
		CKVariant & operator-=( const CKVariantList & aList );
		CKVariant & operator-=( const CKTimeTable & aTimeTable );

		CKVariant & operator*=( int aValue );
		CKVariant & operator*=( long aDateValue );
		CKVariant & operator*=( double aValue );
		CKVariant & operator*=( const CKTable & aTable );
		CKVariant & operator*=( const CKTimeSeries & aSeries );
		CKVariant & operator*=( const CKPrice & aPrice );
		CKVariant & operator*=( const CKVariant & aVar );
		CKVariant & operator*=( const CKVariantList & aList );
		CKVariant & operator*=( const CKTimeTable & aTimeTable );

		CKVariant & operator/=( int aValue );
		CKVariant & operator/=( long aDateValue );
		CKVariant & operator/=( double aValue );
		CKVariant & operator/=( const CKTable & aTable );
		CKVariant & operator/=( const CKTimeSeries & aSeries );
		CKVariant & operator/=( const CKPrice & aPrice );
		CKVariant & operator/=( const CKVariant & aVar );
		CKVariant & operator/=( const CKVariantList & aList );
		CKVariant & operator/=( const CKTimeTable & aTimeTable );

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
			CKVariantList	*mListValue;
			CKTimeTable		*mTimeTableValue;
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




/*
 * ----------------------------------------------------------------------------
 * This is the low-level node in the doubly-linked list that will be used
 * to organize the variants into a list that can also be a variant. This is
 * nice in that it's easy to use, easy to deal with, and the destructor takes
 * care of cleaning up the individual variants in the list itself.
 *
 * We base it off the variant so that it appears to be a normal point in
 * all regards - save the ability to exist in a doubly-linked list.
 * ----------------------------------------------------------------------------
 */
class CKVariantNode :
	public CKVariant
{
	public:
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
		CKVariantNode();
		/*
		 * This is a "promotion" constructor that takes a variant and
		 * creates a new variant node based on the data in that variant.
		 * This is important because it'll be an easy way to add variants
		 * to the list.
		 */
		CKVariantNode( const CKVariant & anOther,
					   CKVariantNode *aPrev = NULL,
					   CKVariantNode *aNext = NULL );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKVariantNode( const CKVariantNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKVariantNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKVariantNode & operator=( const CKVariantNode & anOther );
		/*
		 * At times it's also nice to be able to set a data point to this
		 * node so that there's not a ton of casting in the code.
		 */
		CKVariantNode & operator=( const CKVariant & anOther );


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
		void setPrev( CKVariantNode *aNode );
		void setNext( CKVariantNode *aNode );

		/*
		 * These are the simple getters for the links to the previous and
		 * next nodes in the list. There's nothing special here, so we're
		 * exposing them directly.
		 */
		CKVariantNode *getPrev();
		CKVariantNode *getNext();

		/*
		 * This method is used to 'unlink' the node from the list it's in.
		 * This will NOT delete the node, merely take it out the the list
		 * and now it becomes the responsibility of the caller to delete
		 * this node, or add him to another list.
		 */
		void removeFromList();

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKVariantNodes are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKVariantNode & anOther ) const;
		/*
		 * This method checks to see if the two CKVariantNodes are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKVariantNode & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	private:
		friend class CKVariantList;

		/*
		 * Since we're a doubly-linked list, I'm just going to have a
		 * prev and next pointers and that will take care of the linking.
		 */
		CKVariantNode		*mPrev;
		CKVariantNode		*mNext;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKVariantNode & aNode );




/*
 * ----------------------------------------------------------------------------
 * This is the high-level interface to a list of CKVariant objects. It
 * is organized as a doubly-linked list of CKVariantNodes and the interface
 * to the list if controlled by a nice CKFWMutex. This is a nice and clean
 * replacement to the STL std::list.
 * ----------------------------------------------------------------------------
 */
class CKVariantList
{
	public:
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
		CKVariantList();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKVariantList( const CKVariantList & anOther );
		/*
		 * This method takes an encoded CKString and creates a new CKVariant
		 * list based on the decoded contents. It's an easy way to get the
		 * instance up and running from the other side of a serialized
		 * connection.
		 */
		CKVariantList( const CKString & aCodedList );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKVariantList();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKVariantList & operator=( CKVariantList & anOther );
		CKVariantList & operator=( const CKVariantList & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * These are the easiest ways to get at the head and tail of this
		 * list. After that, the CKVariantNode's getPrev() and getNext()
		 * do a good job of moving you around the list.
		 */
		CKVariantNode *getHead() const;
		CKVariantNode *getTail() const;

		/*
		 * Because there may be times that the user wants to lock us up
		 * for change, we're going to expose this here so it's easy for them
		 * to iterate, for example.
		 */
		void lock();
		void unlock();

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
		int size();
		int size() const;

		/*
		 * This is used to tell the caller if the list is empty. It's
		 * faster than checking for a size() == 0.
		 */
		bool empty();
		bool empty() const;

		/*
		 * This method clears out the entire list and deletes all it's
		 * contents. After this, all node pointers to nodes in this list
		 * will be pointing to nothing, so watch out.
		 */
		void clear();

		/*
		 * When I want to add a point to the front or back of the list,
		 * these are the simplest ways to do that. The passed-in data point
		 * is left untouched, and a copy is made of it at the proper point
		 * in the list.
		 */
		void addToFront( const CKVariant & aPoint );
		void addToEnd( const CKVariant & aPoint );

		/*
		 * These methods take control of the passed-in arguments and place
		 * them in the proper place in the list. This is different in that
		 * the control of the node is passed to the list, but that's why
		 * we've created them... to make it easy to add in nodes by just
		 * changing the links.
		 */
		void putOnFront( CKVariantNode *aNode );
		void putOnEnd( CKVariantNode *aNode );

		/*
		 * When you have a list that you want to add to this list, these
		 * are the methods to use. It's important to note that the arguments
		 * will NOT be altered - which is why this is called the 'copy' as
		 * opposed to the 'splice'.
		 */
		void copyToFront( CKVariantList & aList );
		void copyToFront( const CKVariantList & aList );
		void copyToEnd( CKVariantList & aList );
		void copyToEnd( const CKVariantList & aList );

		/*
		 * When you have a list that you want to merge into this list, these
		 * are the methods to use. It's important to note that the argument
		 * lists will be EMPTIED - which is why this is called the 'splice'
		 * as opposed to the 'copy'.
		 */
		void spliceOnFront( CKVariantList & aList );
		void spliceOnEnd( CKVariantList & aList );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
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
		 * This method checks to see if the two CKVariantLists are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator==( CKVariantList & anOther );
		bool operator==( const CKVariantList & anOther );
		bool operator==( const CKVariantList & anOther ) const;
		/*
		 * This method checks to see if the two CKVariantLists are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKVariantList & anOther );
		bool operator!=( const CKVariantList & anOther );
		bool operator!=( const CKVariantList & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString();

	protected:
		/*
		 * Setting the head or the tail is a bit dicey and so we're not
		 * going to let just anyone change these guys.
		 */
		void setHead( CKVariantNode *aNode );
		void setTail( CKVariantNode *aNode );

	private:
		/*
		 * A Doubly-linked list is pretty easy - there's a head and a
		 * tail and that's about it.
		 */
		CKVariantNode		*mHead;
		CKVariantNode		*mTail;
		/*
		 * This is the mutex that is going to protect all the dangerous
		 * operations so that this list is thread-safe.
		 */
		CKFWMutex			mMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKVariantList & aList );
std::ostream & operator<<( std::ostream & aStream, const CKVariantList & aList );

#endif	// __CKVARIANT_H
