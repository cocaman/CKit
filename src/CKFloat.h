/*
 * CKFloat.h - this file defines an 'infinite' precision floating point
 *             number that can be used when you need to manipulate some
 *             numbers and can't afford the loss of precision like you'll
 *             get with double.
 *
 * $Id: CKFloat.h,v 1.1 2007/10/19 19:02:04 drbob Exp $
 */
#ifndef __CKFLOAT_H
#define __CKFLOAT_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKString.h"
#include "CKVector.h"
#include "CKVariant.h"
#include "CKFWMutex.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFloat
{
	public :
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the standard empty constructor that creates a new
		 * number with a value of zero. Nothing special here, but
		 * necessary.
		 */
		CKFloat();
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided int.
		 */
		CKFloat( int anInt );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided long.
		 */
		CKFloat( long aLong );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided float.
		 */
		CKFloat( float aFloat );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided double.
		 */
		CKFloat( double aDouble );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided (char *).
		 */
		CKFloat( char *aString );
		CKFloat( const char *aString );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided CKString.
		 */
		CKFloat( CKString & aString );
		CKFloat( const CKString & aString );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided STL std::string.
		 */
		CKFloat( std::string & aString );
		CKFloat( const std::string & aString );
		/*
		 * This version of the constructor creates a new number with
		 * the value contained in the provided CKVariant. It has to
		 * be a string that can be cast into a number - or a number,
		 * or else an exception will be thrown.
		 */
		CKFloat( CKVariant & aVariant );
		CKFloat( const CKVariant & aVariant );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKFloat( CKFloat & anOther );
		CKFloat( const CKFloat & anOther );
		/*
		 * This is the standard destructor for this guy and makes sure
		 * that everything is cleaned up before leaving.
		 */
		virtual ~CKFloat();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKFloat & operator=( CKFloat & anOther );
		CKFloat & operator=( const CKFloat & anOther );


		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the char value provided.
		 */
		void setCharValue( char aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the double value provided. Because
		 * this takes a fractional number, there may be rounding. It
		 * can't be helped.
		 */
		void setDoubleValue( double aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the float value provided. Because
		 * this takes a fractional number, there may be rounding. It
		 * can't be helped.
		 */
		void setFloatValue( float aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the int value provided.
		 */
		void setIntValue( int aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the long value provided.
		 */
		void setLongValue( long aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the short value provided.
		 */
		void setShortValue( short aVal );
		/*
		 * This method will replace any value that's currently being
		 * held and replace it with the value in the CKVariant provided.
		 * the variant will have to be holding a string that can be
		 * parsed into a number or a number for this to work. Otherwise,
		 * an exception will be thrown.
		 */
		void setVariantValue( const CKVariant & aVariant );

		/*
		 * This method is really one of the focal points of this class in
		 * that it's the primary reason the class exists. We want to be able
		 * to take a value that was given as a string and turn it into a
		 * number without loss of precision, etc.
		 */
		void parse( char *aStringValue );
		void parse( const char *aStringValue );
		void parse( CKString & aStringValue );
		void parse( const CKString & aStringValue );
		void parse( std::string & aStringValue );
		void parse( const std::string & aStringValue );

		/********************************************************
		 *
		 *                Conversion Methods
		 *
		 ********************************************************/
		/*
		 * Returns the value of the specified number as a <TT>char</TT>. This
		 * may involve rounding or truncation.
		 */
		char charValue();
		/*
		 * Returns the value of the specified number as a <TT>double</TT>. This
		 * may involve rounding.
		 */
		double doubleValue();
		/*
		 * Returns the value of the specified number as a <TT>float</TT>. This
		 * may involve rounding or truncation.
		 */
		float floatValue();
		/*
		 * Returns the value of the specified number as an <TT>int</TT>. This
		 * may involve rounding or truncation.
		 */
		int intValue();
		/*
		 * Returns the value of the specified number as a <TT>long</TT>. This
		 * may involve rounding or truncation.
		 */
		long longValue();
		/**
		 * Returns the value of the specified number as a <TT>short</TT>. This
		 * may involve rounding or truncation.
		 */
		short shortValue();

		/********************************************************
		 *
		 *            Useful Mathematical Methods
		 *
		 ********************************************************/
		/*
		 * This method multiplies the number by 10^arg so if the arg is
		 * 2, it multiplies the number by 100. If it's -2, then it divides
		 * the number by 100. This is basic decimal point shifting, but it
		 * also makes sure that there is room in the storage of the number
		 * to hold it as it gets moved to and fro.
		 */
		void multiplyBy10( int anExponent, bool assureThreadSafety = true );

		/*
		 * This mehod adds the provided number to 'this' modifying the
		 * value. If you didn't want to modify this value, then make a
		 * copy first.
		 */
		void add( const CKFloat & aNumber );
		/*
		 * This method subtracts the provided number from 'this' modifying
		 * the value. If you didn't want to modify this value, then make a
		 * copy first.
		 */
		void subtract( const CKFloat & aNumber );
		/*
		 * This method multiplies 'this' by the provided number modifying
		 * the current value. If you didn't want to modify this value, then
		 * make a copy first.
		 */
		void multiply( const CKFloat & aNumber );
		/*
		 * This method divides the provided number into 'this' modifying the
		 * value. Think of it as 'this / argument'. If you didn't want to
		 * modify this value, then make a copy first.
		 *
		 * This method by it's very nature has the possibility of loosing
		 * precision in it's execution. '1/3' is a classic example, and while
		 * every attempt is made to return as accurate a number as possible,
		 * there will most likely be round-off errors in this guy so please
		 * be aware of them.
		 */
		void divide( const CKFloat & aNumber );

		/********************************************************
		 *
		 *           Mathematical Convenience Methods
		 *
		 ********************************************************/
		/*
		 * This is the convenience method that adds the provided
		 * <TT>char</TT> to this value.
		 */
		void add( char aChar );
		/*
		 * This is the convenience method that adds the provided
		 * <TT>int</TT> to this value.
		 */
		void add( int anInt );
		/*
		 * This is the convenience method that adds the provided
		 * <TT>long</TT> to this value.
		 */
		void add( long aLong );
		/*
		 * This is the convenience method that adds the provided
		 * <TT>float</TT> to this value.
		 */
		void add( float aFloat );
		/*
		 * This is the convenience method that adds the provided
		 * <TT>double</TT> to this value.
		 */
		void add( double aDouble );
		/*
		 * This is the convenience method that adds the value of the provided
		 * (char *) to this value.
		 */
		void add( char *aString );
		void add( const char *aString );
		/*
		 * This is the convenience method that adds the value of the provided
		 * CKString to this value.
		 */
		void add( CKString & aString );
		void add( const CKString & aString );
		/*
		 * This is the convenience method that adds the value of the provided
		 * STL std::string to this value.
		 */
		void add( std::string & aString );
		void add( const std::string & aString );
		/*
		 * This is the convenience method that adds the value of the
		 * provided CKVariant to this value. This only works if the
		 * variant has a numerical value or a string that can be cast
		 * into a value.
		 */
		void add( CKVariant & aVariant );
		void add( const CKVariant & aVariant );

		/*
		 * This is the convenience method that subtracts the provided
		 * <TT>char</TT> from this value.
		 */
		void subtract( char aChar );
		/*
		 * This is the convenience method that subtracts the provided
		 * <TT>int</TT> from this value.
		 */
		void subtract( int anInt );
		/*
		 * This is the convenience method that subtracts the provided
		 * <TT>long</TT> from this value.
		 */
		void subtract( long aLong );
		/*
		 * This is the convenience method that subtracts the provided
		 * <TT>float</TT> from this value.
		 */
		void subtract( float aFloat );
		/*
		 * This is the convenience method that subtracts the provided
		 * <TT>double</TT> from this value.
		 */
		void subtract( double aDouble );
		/*
		 * This is the convenience method that subtracts the value of
		 * the provided (char *) from this value.
		 */
		void subtract( char *aString );
		void subtract( const char *aString );
		/*
		 * This is the convenience method that subtracts the value of
		 * the provided CKString from this value.
		 */
		void subtract( CKString & aString );
		void subtract( const CKString & aString );
		/*
		 * This is the convenience method that subtracts the value of
		 * the provided STL std::string from this value.
		 */
		void subtract( std::string & aString );
		void subtract( const std::string & aString );
		/*
		 * This is the convenience method that subtracts the value of the
		 * provided CKVariant from this value. This only works if the
		 * variant has a numerical value or a string that can be cast
		 * into a value.
		 */
		void subtract( CKVariant & aVariant );
		void subtract( const CKVariant & aVariant );

		/*
		 * This is the convenience method that multiplies this value
		 * by the provided <TT>char</TT> value.
		 */
		void multiply( char aChar );
		/*
		 * This is the convenience method that multiplies this value
		 * by the provided <TT>int</TT> value.
		 */
		void multiply( int anInt );
		/*
		 * This is the convenience method that multiplies this value
		 * by the provided <TT>long</TT> value.
		 */
		void multiply( long aLong );
		/*
		 * This is the convenience method that multiplies this value
		 * by the provided <TT>float</TT> value.
		 */
		void multiply( float aFloat );
		/*
		 * This is the convenience method that multiplies this value
		 * by the provided <TT>double</TT> value.
		 */
		void multiply( double aDouble );
		/*
		 * This is the convenience method that multiplies this value
		 * by the value of the provided (char *) value.
		 */
		void multiply( char *aString );
		void multiply( const char *aString );
		/*
		 * This is the convenience method that multiplies this value
		 * by the value of the provided CKString value.
		 */
		void multiply( CKString & aString );
		void multiply( const CKString & aString );
		/*
		 * This is the convenience method that multiplies this value
		 * by the value of the provided STL std::string value.
		 */
		void multiply( std::string & aString );
		void multiply( const std::string & aString );
		/*
		 * This is the convenience method that multiplies this value
		 * by the value of the provided CKVariant from this value. This
		 * only works if the variant has a numerical value or a string
		 * that can be cast into a value.
		 */
		void multiply( CKVariant & aVariant );
		void multiply( const CKVariant & aVariant );

		/*
		 * This is the convenience method that divides the provided
		 * <TT>char</TT> into this value. (think: 'this = this / argument')
		 */
		void divide( char aChar );
		/*
		 * This is the convenience method that divides the provided
		 * <TT>int</TT> into this value. (think: 'this = this / argument')
		 */
		void divide( int anInt );
		/*
		 * This is the convenience method that divides the provided
		 * <TT>long</TT> into this value. (think: 'this = this / argument')
		 */
		void divide( long aLong );
		/*
		 * This is the convenience method that divides the provided
		 * <TT>float</TT> into this value.
		 * (think: 'this = this / argument')
		 */
		void divide( float aFloat );
		/*
		 * This is the convenience method that divides the provided
		 * <TT>double</TT> into this value.
		 * (think: 'this = this / argument')
		 */
		void divide( double aDouble );
		/*
		 * This is the convenience method that divides the value of
		 * the provided (char *) into this value.
		 * (think: 'this = this / argument')
		 */
		void divide( char *aString );
		void divide( const char *aString );
		/*
		 * This is the convenience method that divides the value of
		 * the provided CKString into this value.
		 * (think: 'this = this / argument')
		 */
		void divide( CKString & aString );
		void divide( const CKString & aString );
		/*
		 * This is the convenience method that divides the value of
		 * the provided STL std::string into this value.
		 * (think: 'this = this / argument')
		 */
		void divide( std::string & aString );
		void divide( const std::string & aString );
		/*
		 * This is the convenience method that divides the value of the
		 * provided CKVariant into this value. This only works if the
		 * variant has a numerical value or a string that can be cast
		 * into a value. (think: 'this = this / argument')
		 */
		void divide( CKVariant & aVariant );
		void divide( const CKVariant & aVariant );

		/********************************************************
		 *
		 *                Operator Methods
		 *
		 ********************************************************/
		/*
		 * For each of the add() methods we need to have a '+='
		 * operator so that the code can look clean.
		 */
		CKFloat & operator+=( char aChar );
		CKFloat & operator+=( int anInt );
		CKFloat & operator+=( long aLong );
		CKFloat & operator+=( float aFloat );
		CKFloat & operator+=( double aDouble );
		CKFloat & operator+=( char *aString );
		CKFloat & operator+=( const char *aString );
		CKFloat & operator+=( CKString & aString );
		CKFloat & operator+=( const CKString & aString );
		CKFloat & operator+=( std::string & aString );
		CKFloat & operator+=( const std::string & aString );
		CKFloat & operator+=( CKVariant & aVariant );
		CKFloat & operator+=( const CKVariant & aVariant );
		CKFloat & operator+=( CKFloat & aFloat );
		CKFloat & operator+=( const CKFloat & aFloat );

		/*
		 * For each of the subtract() methods we need to have a '-='
		 * operator so that the code can look clean.
		 */
		CKFloat & operator-=( char aChar );
		CKFloat & operator-=( int anInt );
		CKFloat & operator-=( long aLong );
		CKFloat & operator-=( float aFloat );
		CKFloat & operator-=( double aDouble );
		CKFloat & operator-=( char *aString );
		CKFloat & operator-=( const char *aString );
		CKFloat & operator-=( CKString & aString );
		CKFloat & operator-=( const CKString & aString );
		CKFloat & operator-=( std::string & aString );
		CKFloat & operator-=( const std::string & aString );
		CKFloat & operator-=( CKVariant & aVariant );
		CKFloat & operator-=( const CKVariant & aVariant );
		CKFloat & operator-=( CKFloat & aFloat );
		CKFloat & operator-=( const CKFloat & aFloat );

		/*
		 * For each of the multiply() methods we need to have a '*='
		 * operator so that the code can look clean.
		 */
		CKFloat & operator*=( char aChar );
		CKFloat & operator*=( int anInt );
		CKFloat & operator*=( long aLong );
		CKFloat & operator*=( float aFloat );
		CKFloat & operator*=( double aDouble );
		CKFloat & operator*=( char *aString );
		CKFloat & operator*=( const char *aString );
		CKFloat & operator*=( CKString & aString );
		CKFloat & operator*=( const CKString & aString );
		CKFloat & operator*=( std::string & aString );
		CKFloat & operator*=( const std::string & aString );
		CKFloat & operator*=( CKVariant & aVariant );
		CKFloat & operator*=( const CKVariant & aVariant );
		CKFloat & operator*=( CKFloat & aFloat );
		CKFloat & operator*=( const CKFloat & aFloat );

		/*
		 * For each of the divide() methods we need to have a '/='
		 * operator so that the code can look clean.
		 */
		CKFloat & operator/=( char aChar );
		CKFloat & operator/=( int anInt );
		CKFloat & operator/=( long aLong );
		CKFloat & operator/=( float aFloat );
		CKFloat & operator/=( double aDouble );
		CKFloat & operator/=( char *aString );
		CKFloat & operator/=( const char *aString );
		CKFloat & operator/=( CKString & aString );
		CKFloat & operator/=( const CKString & aString );
		CKFloat & operator/=( std::string & aString );
		CKFloat & operator/=( const std::string & aString );
		CKFloat & operator/=( CKVariant & aVariant );
		CKFloat & operator/=( const CKVariant & aVariant );
		CKFloat & operator/=( CKFloat & aFloat );
		CKFloat & operator/=( const CKFloat & aFloat );

		/********************************************************
		 *
		 *                Operator Functions
		 *
		 ********************************************************/
		/*
		 * There are times that CKFloats will be used in mathematical
		 * expressions by themselves, these operator functions will make
		 * it very easy for the user to do simple 'a + b' coding on
		 * even the most complex structures.
		 */
		friend CKFloat operator+( CKFloat & aFloat, CKFloat & anOtherFloat );
		friend CKFloat operator-( CKFloat & aFloat, CKFloat & anOtherFloat );
		friend CKFloat operator*( CKFloat & aFloat, CKFloat & anOtherFloat );
		friend CKFloat operator/( CKFloat & aFloat, CKFloat & anOtherFloat );

		/*
		 * These operator functions will allow the mixed-mode math with
		 * CKFloats casting the result up to a CKFloat in each case. First,
		 * start with simple addition.
		 */
		friend CKFloat operator+( CKFloat & aFloat, int aValue );
		friend CKFloat operator+( int aValue, CKFloat & aFloat );

		friend CKFloat operator+( CKFloat & aFloat, char aValue );
		friend CKFloat operator+( char aValue, CKFloat & aFloat );

		friend CKFloat operator+( CKFloat & aFloat, long aValue );
		friend CKFloat operator+( long aValue, CKFloat & aFloat );

		friend CKFloat operator+( CKFloat & aFloat, double aValue );
		friend CKFloat operator+( double aValue, CKFloat & aFloat );

		friend CKFloat operator+( CKFloat & aFloat, float aValue );
		friend CKFloat operator+( float aValue, CKFloat & aFloat );

		/*
		 * Next, let's do all the different forms of the subtraction
		 * operator between the CKFloat and the other scalar types.
		 */
		friend CKFloat operator-( CKFloat & aFloat, int aValue );
		friend CKFloat operator-( int aValue, CKFloat & aFloat );

		friend CKFloat operator-( CKFloat & aFloat, char aValue );
		friend CKFloat operator-( char aValue, CKFloat & aFloat );

		friend CKFloat operator-( CKFloat & aFloat, long aValue );
		friend CKFloat operator-( long aValue, CKFloat & aFloat );

		friend CKFloat operator-( CKFloat & aFloat, double aValue );
		friend CKFloat operator-( double aValue, CKFloat & aFloat );

		friend CKFloat operator-( CKFloat & aFloat, float aValue );
		friend CKFloat operator-( float aValue, CKFloat & aFloat );

		/*
		 * Next, let's do all the different forms of the multiplication
		 * operator between the variant and the other scalar types.
		 */
		friend CKFloat operator*( CKFloat & aFloat, int aValue );
		friend CKFloat operator*( int aValue, CKFloat & aFloat );

		friend CKFloat operator*( CKFloat & aFloat, char aValue );
		friend CKFloat operator*( char aValue, CKFloat & aFloat );

		friend CKFloat operator*( CKFloat & aFloat, long aValue );
		friend CKFloat operator*( long aValue, CKFloat & aFloat );

		friend CKFloat operator*( CKFloat & aFloat, double aValue );
		friend CKFloat operator*( double aValue, CKFloat & aFloat );

		friend CKFloat operator*( CKFloat & aFloat, float aValue );
		friend CKFloat operator*( float aValue, CKFloat & aFloat );

		/*
		 * Next, let's do all the different forms of the division
		 * operator between the variant and the other scalar types.
		 */
		friend CKFloat operator/( CKFloat & aFloat, int aValue );
		friend CKFloat operator/( int aValue, CKFloat & aFloat );

		friend CKFloat operator/( CKFloat & aFloat, char aValue );
		friend CKFloat operator/( char aValue, CKFloat & aFloat );

		friend CKFloat operator/( CKFloat & aFloat, long aValue );
		friend CKFloat operator/( long aValue, CKFloat & aFloat );

		friend CKFloat operator/( CKFloat & aFloat, double aValue );
		friend CKFloat operator/( double aValue, CKFloat & aFloat );

		friend CKFloat operator/( CKFloat & aFloat, float aValue );
		friend CKFloat operator/( float aValue, CKFloat & aFloat );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKFloats are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKFloat & anOther );
		bool operator==( const CKFloat & anOther );
		bool operator==( CKFloat & anOther ) const;
		bool operator==( const CKFloat & anOther ) const;
		/*
		 * This method checks to see if the two CKFloats are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKFloat & anOther );
		bool operator!=( const CKFloat & anOther );
		bool operator!=( CKFloat & anOther ) const;
		bool operator!=( const CKFloat & anOther ) const;
		/*
		 * These methods complete the inequality tests for the CKFloat and
		 * we need these as we might be doing a lot of testing and this makes
		 * it a lot easier than converting to the right type and then doing
		 * a low-level test.
		 */
		bool operator<( const CKFloat & anOther ) const;
		bool operator<=( const CKFloat & anOther ) const;
		bool operator>( const CKFloat & anOther ) const;
		bool operator>=( const CKFloat & anOther ) const;

		/*
		 * These operators allow us to use the CKFloat as a "regular"
		 * variable in conditionals which is really important because
		 * we want this to fit into development like a regular scalar
		 * variable.
		 */
		bool operator==( const char *aCString ) const;
		bool operator==( const std::string & anSTLString ) const;
		bool operator==( const CKString & aString ) const;
		bool operator==( char aValue ) const;
		bool operator==( int aValue ) const;
		bool operator==( long aValue ) const;
		bool operator==( float aValue ) const;
		bool operator==( double aValue ) const;
		bool operator==( const CKVariant & aVariant ) const;

		bool operator!=( const char *aCString ) const;
		bool operator!=( const std::string & anSTLString ) const;
		bool operator!=( const CKString & aString ) const;
		bool operator!=( char aValue ) const;
		bool operator!=( int aValue ) const;
		bool operator!=( long aValue ) const;
		bool operator!=( float aValue ) const;
		bool operator!=( double aValue ) const;
		bool operator!=( const CKVariant & aVariant ) const;

		bool operator<( const char *aCString ) const;
		bool operator<( const std::string & anSTLString ) const;
		bool operator<( const CKString & aString ) const;
		bool operator<( char aValue ) const;
		bool operator<( int aValue ) const;
		bool operator<( long aValue ) const;
		bool operator<( float aValue ) const;
		bool operator<( double aValue ) const;
		bool operator<( const CKVariant & aVariant ) const;

		bool operator<=( const char *aCString ) const;
		bool operator<=( const std::string & anSTLString ) const;
		bool operator<=( const CKString & aString ) const;
		bool operator<=( char aValue ) const;
		bool operator<=( int aValue ) const;
		bool operator<=( long aValue ) const;
		bool operator<=( float aValue ) const;
		bool operator<=( double aValue ) const;
		bool operator<=( const CKVariant & aVariant ) const;

		bool operator>( const char *aCString ) const;
		bool operator>( const std::string & anSTLString ) const;
		bool operator>( const CKString & aString ) const;
		bool operator>( char aValue ) const;
		bool operator>( int aValue ) const;
		bool operator>( long aValue ) const;
		bool operator>( float aValue ) const;
		bool operator>( double aValue ) const;
		bool operator>( const CKVariant & aVariant ) const;

		bool operator>=( const char *aCString ) const;
		bool operator>=( const std::string & anSTLString ) const;
		bool operator>=( const CKString & aString ) const;
		bool operator>=( char aValue ) const;
		bool operator>=( int aValue ) const;
		bool operator>=( long aValue ) const;
		bool operator>=( float aValue ) const;
		bool operator>=( double aValue ) const;
		bool operator>=( const CKVariant & aVariant ) const;

		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		CKString toString() const;

	protected:
		/********************************************************
		 *
		 *               Protected Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method takes a floating point number that's represented as
		 * an optional '-' sign, a series of digits, an optional '.' and
		 * another series of digits and turns this into the value that we're
		 * going to hold. It's important to note that this method CANNOT
		 * deal properly with scientific notiation, so you are going to get
		 * a CKException if you give it a string with scientific notation
		 * in it.
		 */
		void setStringValue( char *aNumber, bool assureThreadSafety = true );

		/********************************************************
		 *
		 *               Housekeeping Methods
		 *
		 ********************************************************/
		/*
		 * This method trims off the insignificant zeros from the right
		 * side of the fractional part of the number as well as the left
		 * side of the whole number part.
		 */
		void trim();
		/*
		 * This method trims off the insignificant zeros from the right
		 * of the fractional part of the number.
		 */
		void trimRight();
		/*
		 * This method trims off the insignificant zeros from the left
		 * of the whole number part of the number.
		 */
		void trimLeft();

		/*
		 * This method returns a CKVector<char> that contains the digits
		 * of the number where the LSD is in location 0 and the MSD is in
		 * the last. There is no information about the location of the
		 * decimal point in this data, and for good reason - this method
		 * is used in the arithmetic methods where the number of digits
		 * to the right of the decimal in this instance is known. This
		 * really just concatenates the two halves of the floating
		 * point number for easier manipulation.
		 */
		CKVector<char> getDigits() const;

	private:
		/********************************************************
		 *
		 *              Mathematical Support Methods
		 *
		 ********************************************************/
		/*
		 * This method is the implementation of the right shift - or a
		 * division of the number by a power of 10. This has to be broken
		 * down into sections like - do we have any fraction? Will our
		 * shift create a fractional part? Lots of little things need to
		 * be taken into account which is why this is it's own method.
		 *
		 * It's important to note that this method DOES NOT lock the
		 * mutex to make sure this is a thread-safe operation. That's
		 * up to the caller, and multiplyBy10() does that - so if you
		 * choose to use this method, be warned.
		 */
		void rightShift( int aCount );
		/*
		 * This method is the implementation of the left shift - or a
		 * multiplication of the number by a power of 10. This has to be
		 * broken down into sections like - do we have any whole number?
		 * Will our shift create a whole number part? Lots of little
		 * things need to be taken into account which is why this is
		 * it's own method.
		 *
		 * It's important to note that this method DOES NOT lock the
		 * mutex to make sure this is a thread-safe operation. That's
		 * up to the caller, and multiplyBy10() does that - so if you
		 * choose to use this method, be warned.
		 */
		void leftShift( int aCount );

		/*
		 * Compares two BKFloat objects numerically. The return value
		 * is zero if the argument and this floa are numerically equal,
		 * less than zero if this float is numerically less than
		 * anotherFloat, and greater than 0 if this float is numerically
		 * greater than anotherFloat.
		 */
		int compareTo( const CKFloat & anotherFloat );

		/*
		 * The sign of the floating point number is stored in a simple bool
		 * if it's true then we're negative, if it's not, then we are zero
		 * or positive.
		 */
		bool			mNegative;
		/*
		 * The floating point number is comprised of two parts - this is the
		 * whole number part where the least-significant-digit (LSD) is at
		 * mWhole[0] and the most-significant-digit (MSD) is at mWhole[mMSD].
		 */
		CKVector<char>	mWhole;
		// and this is the location in the buffer of the MSD
		int				mMSD;
		/*
		 * The floating point number is comprised of two parts - this is the
		 * fractional part where the least-significant-digit (LSD) is at
		 * mFraction[mLSD] and the most-significant-digit (MSD) is at
		 * mFraction[0].
		 */
		CKVector<char>	mFraction;
		// and this is the location in the buffer of the LSD
		int				mLSD;
		/*
		 * Because the arithmetic operations can take some time, we are
		 * going to use a simple mutex to make sure that the operations
		 * on this guy are atomic so that they are thread-safe. It's a
		 * little overhead, but in general it'll be worth it.
		 */
		CKFWMutex		mMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKFloat & aFloat );
std::ostream & operator<<( std::ostream & aStream, const CKFloat & aFloat );

#endif	// __CKFLOAT_H
