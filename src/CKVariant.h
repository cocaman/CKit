/*
 * CKVariant.h - this file defines a class that can be used to represent a
 *               general data value. The power of this class is that all the
 *               different kinds of values that this object can rperesent can
 *               then be treated as a single data type and thus really 
 *               simplify dealing with tables of different types of data.
 * 
 * $Id: CKVariant.h,v 1.5 2004/09/02 18:13:32 drbob Exp $
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

//	Forward Declarations
class CKTable;

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
	eTimeSeriesVariant = 4
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
		CKVariant & operator=( const CKVariant & anOther );

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
		const char *getStringValue() const;
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
		 * a string and it is the responsibility of the caller to call
		 * 'delete []' on the results. It's also possible that this method
		 * will return NULL, so you have better check the return value
		 * before assuming anything.
		 */
		char *getValueAsString() const;
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
		 *
		 * This method returns a character array that the caller is
		 * responsible for calling 'delete []' on. This is useful as these
		 * codes are used outside the scope of this class and so a copy
		 * is far more useful.
		 */
		virtual char *generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this value. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const char *aCode );
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
		 * Because there are times when it's useful to have a nice 
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString() const;

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
			char			*mStringValue;
			long			mDateValue;
			double			mDoubleValue;
			CKTable			*mTableValue;
			CKTimeSeries	*mTimeSeriesValue;
		};
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKVariant & anItem );

#endif	// __CKVARIANT_H
