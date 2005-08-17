/*
 * CKPrice.h - this file defines a class that is used in the Factory++ to
 *             represent prices from the MM Server. As it turns out, the
 *             MM Server delivers both $US and Native prices for almost all
 *             the important prices and values. This object makes it easy
 *             to get at these guys.
 *
 * $Id: CKPrice.h,v 1.3 2005/08/17 13:56:54 drbob Exp $
 */
#ifndef __CKPRICE_H
#define __CKPRICE_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
/*
 * Because we're using the NAN value in some places in this object,
 * we need to make sure that it's defined for all the platforms that
 * will be using this object.
 */
#ifdef __linux__
#define __USE_ISOC99 1
#endif
#include <math.h>
/*
 * Oddly enough, Sun doesn't seem to have NAN defined, so we need to
 * do that here so that things run more smoothly. This is very interesting
 * because Sun has isnan() defined, but no obvious way to set a value.
 */
#ifdef __sun__
#ifndef NAN
#define	NAN	(__extension__ ((union { unsigned __l __attribute__((__mode__(__SI__))); \
			float __d; }) { __l: 0x7fc00000UL }).__d)
#endif
#endif
/*
 * This is most odd, but it seems that at least on Darwin (Mac OS X)
 * there's a problem with the definition of isnan(). So... to make it
 * easier on all parties, I'm simply going to repeat the definition
 * that's in Linux and Darwin here, and it should get picked up even
 * if the headers fail us.
 */
#ifdef __MACH__
#ifndef isnan
#define	isnan(x)	((sizeof(x) == sizeof(double)) ? __isnand(x) : \
					(sizeof(x) == sizeof(float)) ? __isnanf(x) : __isnan(x))
#endif
#endif

//	Third-Party Headers

//	Other Headers
#include "CKString.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKPrice
{
	public:
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
		CKPrice();
		/*
		 * This form of the constructor is nice in that it allows us to take
		 * the values right away and then the user doesn't have to worry
		 * about setting the values right after creation. If only the $US
		 * value is known, then the Native value defaults to NAN which can
		 * be checked for.
		 */
		CKPrice( double aUSD, double aNative = NAN );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKPrice's generateCodeFromValues()
		 * method and parses it into a price directly. This is very useful
		 * for serializing the price's data from one host to another across
		 * a socket, for instance.
		 */
		CKPrice( const CKString & aCode );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKPrice( const CKPrice & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKPrice();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKPrice & operator=( const CKPrice & anOther );

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
		void setUSD( double aUSD );
		void setUSD( double aUSD ) const;
		void setNative( double aNative );
		void setNative( double aNative ) const;

		/*
		 * These are the getters for the USD and Native values of this
		 * guy. They should be set before they are used, as there's no
		 * exchange rate conversion being done by this guy.
		 */
		double getUSD() const;
		double getNative() const;

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
		bool add( double anOffset );
		bool add( CKPrice & anOther );
		bool add( const CKPrice & anOther );
		/*
		 * These methods allow the user to subtract values from this
		 * price, in the first case, it's a constant value but
		 * in the second it's another price.
		 */
		bool subtract( double anOffset );
		bool subtract( CKPrice & anOther );
		bool subtract( const CKPrice & anOther );
		/*
		 * These method allows the user to multiply a constant value to
		 * both components of the price or it multiplies each by it's
		 * respective partner in the other price.
		 */
		bool multiply( double aFactor );
		bool multiply( CKPrice & anOther );
		bool multiply( const CKPrice & anOther );
		/*
		 * These method allows the user to divide each component of
		 * the price by a constant or it divides each by it's respective
		 * partner in the other price.
		 */
		bool divide( double aDivisor );
		bool divide( CKPrice & anOther );
		bool divide( const CKPrice & anOther );
		/*
		 * This method simply takes the inverse of each value in the price
		 * so that x -> 1/x for each value. This is marginally useful
		 * I'm thinking, but I added it here to be a little more complete.
		 */
		bool inverse();

		/*
		 * These are the operator equivalents of the simple mathematical
		 * operations on the price. They are here as an aid to the
		 * developer of analytic functions based on these guys.
		 */
		CKPrice & operator+=( double anOffset );
		CKPrice & operator+=( CKPrice & aPrice );
		CKPrice & operator+=( const CKPrice & aPrice );
		CKPrice & operator-=( double anOffset );
		CKPrice & operator-=( CKPrice & aPrice );
		CKPrice & operator-=( const CKPrice & aPrice );
		CKPrice & operator*=( double aFactor );
		CKPrice & operator/=( double aDivisor );

		/*
		 * These are the operators for creating new price data from
		 * one or two existing prices. This is nice in the same vein
		 * as the simpler operators in that it makes writing code for these
		 * data sets a lot easier.
		 */
		friend CKPrice operator+( CKPrice & aPrice, double aValue );
		friend CKPrice operator+( double aValue, CKPrice & aPrice );
		friend CKPrice operator+( CKPrice & aPrice, CKPrice & anotherPrice );

		friend CKPrice operator-( CKPrice & aPrice, double aValue );
		friend CKPrice operator-( double aValue, CKPrice & aPrice );
		friend CKPrice operator-( CKPrice & aPrice, CKPrice & anotherPrice );

		friend CKPrice operator*( CKPrice & aPrice, double aValue );
		friend CKPrice operator*( double aValue, CKPrice & aPrice );

		friend CKPrice operator/( CKPrice & aPrice, double aValue );
		friend CKPrice operator/( double aValue, CKPrice & aPrice );

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
		 * This method checks to see if the two CKPrices are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKPrice & anOther ) const;
		/*
		 * This method checks to see if the two CKPrices are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKPrice & anOther ) const;
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
		/*
		 * These are the two values that I should be getting from the MM
		 * Server. I'm not doing any fancy exchange rate calculation, because
		 * that's done in the Server.
		 */
		double			mUSD;
		double			mNative;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKPrice & anItem );

/*
 * These are the operators for creating new price data from
 * one or two existing prices. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKPrice operator+( CKPrice & aPrice, double aValue );
CKPrice operator+( double aValue, CKPrice & aPrice );
CKPrice operator+( CKPrice & aPrice, CKPrice & anotherPrice );

CKPrice operator-( CKPrice & aPrice, double aValue );
CKPrice operator-( double aValue, CKPrice & aPrice );
CKPrice operator-( CKPrice & aPrice, CKPrice & anotherPrice );

CKPrice operator*( CKPrice & aPrice, double aValue );
CKPrice operator*( double aValue, CKPrice & aPrice );

CKPrice operator/( CKPrice & aPrice, double aValue );
CKPrice operator/( double aValue, CKPrice & aPrice );

#endif	// __CKPRICE_H
