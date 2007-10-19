/*
 * CKFloat.cpp - this file implements an 'infinite' precision floating point
 *               number that can be used when you need to manipulate some
 *               numbers and can't afford the loss of precision like you'll
 *               get with double.
 *
 * $Id: CKFloat.cpp,v 1.1 2007/10/19 19:02:04 drbob Exp $
 */

//	System Headers
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

//	Third-Party Headers
#include <CKException.h>
#include <CKStackLocker.h>

//	Other Headers
#include "CKFloat.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * I would have throught these were defined in math.h, but I didn't
 * see them, so here they are. Simple, but very effective.
 */
#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif


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
CKFloat::CKFloat() :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided int.
 */
CKFloat::CKFloat( int anInt ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	setIntValue(anInt);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided long.
 */
CKFloat::CKFloat( long aLong ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	setLongValue(aLong);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided float.
 */
CKFloat::CKFloat( float aFloat ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	setFloatValue(aFloat);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided double.
 */
CKFloat::CKFloat( double aDouble ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	setDoubleValue(aDouble);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided (char *).
 */
CKFloat::CKFloat( char *aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	if (aString != NULL) {
		parse(aString);
	}
}


CKFloat::CKFloat( const char *aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	if (aString != NULL) {
		parse(aString);
	}
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided CKString.
 */
CKFloat::CKFloat( CKString & aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	parse(aString);
}


CKFloat::CKFloat( const CKString & aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	parse(aString);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided STL std::string.
 */
CKFloat::CKFloat( std::string & aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	parse(aString);
}


CKFloat::CKFloat( const std::string & aString ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	parse(aString);
}


/*
 * This version of the constructor creates a new number with
 * the value contained in the provided CKVariant. It has to
 * be a string that can be cast into a number - or a number,
 * or else an exception will be thrown.
 */
CKFloat::CKFloat( CKVariant & aVariant ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
}


CKFloat::CKFloat( const CKVariant & aVariant ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKFloat::CKFloat( CKFloat & anOther ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	// let the operator=() do all the work for me
	*this = anOther;
}


CKFloat::CKFloat( const CKFloat & anOther ) :
	mNegative(false),
	mWhole(),
	mMSD(-1),
	mFraction(),
	mLSD(-1),
	mMutex()
{
	// let the operator=() do all the work for me
	*this = anOther;
}


/*
 * This is the standard destructor for this guy and makes sure
 * that everything is cleaned up before leaving.
 */
CKFloat::~CKFloat()
{
	// everything here cleans itself up nicely
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKFloat & CKFloat::operator=( CKFloat & anOther )
{
	/*
	 * We have to watch out for someone setting this guy equal
	 * to itself. If they did, we'd wipe out what is in us for what's
	 * in the other, but since the other *is* us, we'd have nothing
	 * to put back in. So we have to make sure we're not doing this
	 * to ourselves.
	 */
	if (this != & anOther) {
		mNegative = anOther.mNegative;
		mWhole = anOther.mWhole;
		mMSD = anOther.mMSD;
		mFraction = anOther.mFraction;
		mLSD = anOther.mLSD;
	}

	return *this;
}


CKFloat & CKFloat::operator=( const CKFloat & anOther )
{
	return this->operator=((CKFloat &)anOther);
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method will replace any value that's currently being
 * held and replace it with the char value provided.
 */
void CKFloat::setCharValue( char aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[64];
	bzero(buff, 64);
	snprintf(buff, 63, "%d", aVal);
	setStringValue(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the double value provided. Because
 * this takes a fractional number, there may be rounding. It
 * can't be helped.
 */
void CKFloat::setDoubleValue( double aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[128];
	bzero(buff, 128);
	snprintf(buff, 127, "%50.25lf", aVal);
	parse(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the float value provided. Because
 * this takes a fractional number, there may be rounding. It
 * can't be helped.
 */
void CKFloat::setFloatValue( float aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[128];
	bzero(buff, 128);
	snprintf(buff, 127, "%50.25g", aVal);
	parse(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the int value provided.
 */
void CKFloat::setIntValue( int aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[128];
	bzero(buff, 128);
	snprintf(buff, 127, "%d", aVal);
	setStringValue(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the long value provided.
 */
void CKFloat::setLongValue( long aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[128];
	bzero(buff, 128);
	snprintf(buff, 127, "%ld", aVal);
	setStringValue(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the short value provided.
 */
void CKFloat::setShortValue( short aVal )
{
	// we're simply going to convert the number to a (char *) and parse it
	char	buff[64];
	bzero(buff, 64);
	snprintf(buff, 63, "%d", aVal);
	setStringValue(buff);
}


/*
 * This method will replace any value that's currently being
 * held and replace it with the value in the CKVariant provided.
 * the variant will have to be holding a string that can be
 * parsed into a number or a number for this to work. Otherwise,
 * an exception will be thrown.
 */
void CKFloat::setVariantValue( const CKVariant & aVariant )
{
	switch (aVariant.getType()) {
		case eStringVariant:
			if (aVariant.getStringValue() != NULL) {
				parse(*aVariant.getStringValue());
			}
			break;
		case eNumberVariant:
			setDoubleValue(aVariant.getDoubleValue());
			break;
		default:
			std::ostringstream	msg;
			msg << "CKFloat::setVariantValue(CKVariant &) - the passed in variant "
				"is not a string or a number and those are the only two types of "
				"values that can really be added to a float. Please check the "
				"type of the variant before trying to add it to a float.";
			throw CKException(__FILE__, __LINE__, msg.str());
			break;
	}
}


/*
 * This method is really one of the focal points of this class in
 * that it's the primary reason the class exists. We want to be able
 * to take a value that was given as a string and turn it into a
 * number without loss of precision, etc.
 */
void CKFloat::parse( char *aStringValue )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (aStringValue == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::parse(char *) - the passed in string is null and "
				"that means that I can't parse a number from it. Please make "
				"sure this is not null before calling.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else if (strlen(aStringValue) == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::parse(char *) - the passed in string is empty and "
				"that means that I can't parse a number from it. Please make "
				"sure this is not null before calling.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Because it's possible that 'aStringValue' is really an immutable
	 * (char *), we need to make a copy so that if we *need* to edit
	 * the string, we can without getting an address violation.
	 */
	char	*raw = NULL;
	if (!error) {
		raw = strdup(aStringValue);
		if (raw == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::parse(char *) - while trying to copy the passed "
				"in string for processing, an allocation error hammered us. "
				"Please check on this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the number is in scientific notation
	if (!error) {
		// try to find 'E', 'e', 'G', 'g' - all are valid exponent indicators
		const char	*eTags = "EeGg";
		char	*expon = NULL;
		for (int i = 0; i < 4; ++i) {
			expon = strchr(raw, eTags[i]);
			if (expon != NULL) {
				break;
			}
		}
		// if we have an exponent, then we need to handle it
		if (expon != NULL) {
			// lock up this guy for this operation
			CKStackLocker		lockem(&mMutex);

			// change the 'E' to a '\0' to terminate the mantissa
			expon[0] = '\0';
			// set the value to be the mantissa
			setStringValue(raw, false);
			// now let's get the exponent after the 'E'
			if (expon[1] != '\0') {
				multiplyBy10(strtol(&(expon[1]), NULL, 10), false);
			}
		} else {
			// no scientific notation, so it's easy
			setStringValue(aStringValue);
		}
	}

	// clean up the mess we made
	if (raw != NULL) {
		free(raw);
	}
}


void CKFloat::parse( const char *aStringValue )
{
	parse((char *)aStringValue);
}


void CKFloat::parse( CKString & aStringValue )
{
	parse((char *)aStringValue.c_str());
}


void CKFloat::parse( const CKString & aStringValue )
{
	parse((char *)aStringValue.c_str());
}


void CKFloat::parse( std::string & aStringValue )
{
	parse((char *)aStringValue.c_str());
}


void CKFloat::parse( const std::string & aStringValue )
{
	parse((char *)aStringValue.c_str());
}


/********************************************************
 *
 *                Conversion Methods
 *
 ********************************************************/
/*
 * Returns the value of the specified number as a <TT>char</TT>. This
 * may involve rounding or truncation.
 */
char CKFloat::charValue()
{
	return (char) intValue();
}


/*
 * Returns the value of the specified number as a <TT>double</TT>. This
 * may involve rounding.
 */
double CKFloat::doubleValue()
{
	double	retval = 0;

	// lock up this guy for this operation
	CKStackLocker		lockem(&mMutex);

	// simply build up the value from the digits
	double		whole = 0.0;
	if (!mWhole.empty()) {
		for (int i = mMSD; i >= 0; --i) {
			whole = (whole * 10.0) + mWhole[i];
		}
	}
	// simply build up the value from the digits
	double		fraction = 0.0;
	if (!mFraction.empty()) {
		for (int i = mLSD; i >= 0; --i) {
			fraction = (fraction / 10.0) + mFraction[i];
		}
		fraction = fraction / 10.0;
	}
	// add them up for the final result
	retval = whole + fraction;
	// don't forget to put the sign on that bad boy
	if (mNegative) {
		retval = -retval;
	}

	return retval;
}


/*
 * Returns the value of the specified number as a <TT>float</TT>. This
 * may involve rounding or truncation.
 */
float CKFloat::floatValue()
{
	return (float) doubleValue();
}


/*
 * Returns the value of the specified number as an <TT>int</TT>. This
 * may involve rounding or truncation.
 */
int CKFloat::intValue()
{
	return (int) longValue();
}


/*
 * Returns the value of the specified number as a <TT>long</TT>. This
 * may involve rounding or truncation.
 */
long CKFloat::longValue()
{
	long	retval = 0;

	if (!mWhole.empty()) {
		// lock up this guy for this operation
		CKStackLocker		lockem(&mMutex);

		// simply build up the value from the digits
		for (int i = mMSD; i >= 0; --i) {
			retval = (retval * 10) + mWhole[i];
		}
		// don't forget to put the sign on that bad boy
		if (mNegative) {
			retval = -retval;
		}
	}

	return retval;
}


/**
 * Returns the value of the specified number as a <TT>short</TT>. This
 * may involve rounding or truncation.
 */
short CKFloat::shortValue()
{
	return (short) intValue();
}


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
void CKFloat::multiplyBy10( int anExponent, bool assureThreadSafety )
{
	// see if it's a left-shift or a right-shift
	if (anExponent < 0) {
		if (assureThreadSafety) {
			CKStackLocker		lockem(&mMutex);
			rightShift(-anExponent);
		} else {
			rightShift(-anExponent);
		}
	} else if (anExponent > 0) {
		if (assureThreadSafety) {
			CKStackLocker		lockem(&mMutex);
			leftShift(anExponent);
		} else {
			leftShift(anExponent);
		}
	}
}


/*
 * This mehod adds the provided number to 'this' modifying the
 * value. If you didn't want to modify this value, then make a
 * copy first.
 */
void CKFloat::add( const CKFloat & aNumber )
{
	bool		error = false;

	// lock this guy up for the addition
	CKStackLocker		lockem(&mMutex);

	/*
	 * We need to make sure that *my* whole number part extends at
	 * least as far as the argument's because I'm going to be
	 * retaining the result, and if there's something there for
	 * him, then there's every reason to believe there will be
	 * something there for me.
	 */
	if (!error) {
		// see if I need to reset the MSD location
		if (mWhole.empty()) {
			mMSD = -1;
		}
		// now make all the space I need in this guy's whole number storage
		if (mMSD < aNumber.mMSD) {
			int		cnt = aNumber.mMSD - mMSD;
			for (int i = 0; i < cnt; ++i) {
				mWhole.addToEnd(0);
			}
			mMSD = aNumber.mMSD;
		}
	}

	/*
	 * We need to make sure that *my* fractional part extends at
	 * least as far as the argument's because I'm going to be
	 * retaining the result, and if there's something there for
	 * him, then there's every reason to believe there will be
	 * something there for me.
	 */
	if (!error) {
		// see if I need to reset the LSD location
		if (mFraction.empty()) {
			mLSD = -1;
		}
		// now make all the space I need in this guy's fractional storage
		if (mLSD < aNumber.mLSD) {
			int		cnt = aNumber.mLSD - mLSD;
			for (int i = 0; i < cnt; ++i) {
				mFraction.addToEnd(0);
			}
			mLSD = aNumber.mLSD;
		}
	}

	/**
	 * Now I can start the process of adding the digit values in the
	 * argument to my digits, carrying between digits - just like they
	 * do in third grade.
	 */
	if (!error) {
		char	mySign = (char)(mNegative ? -1 : 1);
		char	hisSign = (char)(aNumber.mNegative ? -1 : 1);
		bool	sameSign = (mNegative == aNumber.mNegative);
		char	carry = 0;
		char	digit = 0;

		// first, run through the fractional part starting at the LSD
		for (int i = mLSD; i >= 0; --i) {
			// get the next digit in the addition
			if (sameSign) {
				digit = carry + mySign * (mFraction[i] + aNumber.mFraction[i]);
			} else {
				digit = 10 + carry + mySign * mFraction[i]
						+ hisSign * aNumber.mFraction[i];
			}
			// see if we have generated a carry
			carry = (sameSign ? 0 : -1);
			if ((digit >= 10) || (digit <= -10)) {
				carry += (digit / 10);
				digit = (digit % 10);
			}
			// put the result back in place with the right sign
			mFraction[i] = (mySign * digit);
		}

		// next, run through the whole number part starting at the LSD
		for (int i = 0; (carry != 0) || (i <= aNumber.mMSD + 1); ++i) {
			// see if I need to reset the MSD location
			if (mWhole.empty()) {
				mMSD = -1;
			}
			// now make all the space I need in this guy's whole number storage
			while (mWhole.size() - mMSD - 1 <= 0) {
				for (int i = 0; i < 10; ++i) {
					mWhole.addToEnd(0);
				}
			}

			// get the next digit in the addition
			if (!aNumber.mWhole.empty() && (aNumber.mMSD >= i)) {
				if (sameSign) {
					digit = carry + mySign * (mWhole[i] + aNumber.mWhole[i]);
				} else {
					digit = 10 + carry + mySign * mWhole[i]
							+ hisSign * aNumber.mWhole[i];
				}
			} else {
				if (sameSign) {
					digit = carry + mySign * mWhole[i];
				} else {
					digit = 10 + carry + mySign * mWhole[i];
				}
			}
			// see if we have generated a carry
			carry = (sameSign ? 0 : -1);
			if ((digit >= 10) || (digit <= -10)) {
				carry += (digit / 10);
				digit = (digit % 10);
			}
			// put the result back in place with the right sign
			mWhole[i] = mySign * digit;
			mMSD = MAX(mMSD, i);
		}
	}


	/*
	 * As a final step, we're going to trim off the leading and
	 * trailing '0's that might have been added in the course of
	 * processing. The point being that if we added a bunch of
	 * insignificant zeros, we should clean up our mess.
	 */
	if (!error) {
		// trim off the extra zeros from the fractional part
		if (!mFraction.empty()) {
			while ((mLSD >= 0) && (mFraction[mLSD] == 0)) {
				--mLSD;
			}
		}
		// trim off the extra zeros from the whole number part
		if (!mWhole.empty()) {
			while ((mMSD >= 0) && (mWhole[mMSD] == 0)) {
				--mMSD;
			}
		}
	}
}


/*
 * This method subtracts the provided number from 'this' modifying
 * the value. If you didn't want to modify this value, then make a
 * copy first.
 */
void CKFloat::subtract( const CKFloat & aNumber )
{
	/*
	 * The easiest way to do the subtraction is to change the sign
	 * on a copy of the argument and then add it in. Super easy.
	 */
	CKFloat		op(aNumber);
	op.mNegative = !op.mNegative;
	// add it in... that's all that's left
	add(op);
}


/*
 * This method multiplies 'this' by the provided number modifying
 * the current value. If you didn't want to modify this value, then
 * make a copy first.
 */
void CKFloat::multiply( const CKFloat & aNumber )
{
	bool		error = false;

	// lock this guy up for the addition
	CKStackLocker		lockem(&mMutex);

	// get the digits for each component in the multiplication
	CKVector<char>		me;
	CKVector<char>		him;
	if (!error) {
		me = getDigits();
		him = aNumber.getDigits();
		/*
		 * Not that it matters, but make he smaller of the two numbers
		 * 'him' so that I can optimize the looping to run through the
		 * numbers as quickly as possible.
		 */
		if (me.size() < him.size()) {
			CKVector<char> 	temp = me;
			me = him;
			him = temp;
		}
	}

	// make something large enough to hold the result
	CKVector<char>		result;
	if (!error) {
		// figure out how big I want it to eventually be
		int		cnt = me.size() + him.size() + 2;
		// ...now make it capable of holding all that
		result.resize(cnt);
		// ...and follow up with a zeroing out of all the digits
		for (int i = 0; i < cnt; i++) {
			result[i] = 0;
		}
	}

	// get the number of decimals in this multiplication
	int			decimals = 0;
	if (!error) {
		if (mLSD >= 0) {
			decimals += (mLSD + 1);
		}
		if (aNumber.mLSD >= 0) {
			decimals += (aNumber.mLSD + 1);
		}
	}

	/*
	 * Now we come to the third-grade math. We're going to run through
	 * the multiplication of the argument and this value by the same
	 * old scheme that you used in third-grade. Just this time, it's
	 * going to be a little faster than by hand.
	 */
	if (!error) {
		char		carry = 0;
		char		digit = 0;
		for (int h = 0; h < him.size(); ++h) {
			// run through the multiplication pass
			carry = 0;
			for (int m = 0; m < me.size(); ++m) {
				digit = him[h] * me[m] + carry;
				carry = 0;
				if (digit >= 10) {
					carry = (digit / 10);
					digit = (digit % 10);
				}
				// add the result back in place
				result[h + m] += digit;
			}
			// add the remaining carry to the next digit in the result
			result[h + me.size()] += carry;

			// now run through the result updating the addition's carry
			carry = 0;
			for (int m = 0; (carry != 0) || (m < me.size()); ++m) {
				digit = result[h + m] + carry;
				carry = 0;
				if (digit >= 10) {
					carry = (digit / 10);
					digit = (digit % 10);
				}
				// put the result back in place
				result[h + m] = digit;
			}
		}
	}

	/*
	 * At this point, we can set this <B>new</B> value as our value
	 * that exists <B>without</B> any decimal component. Then, we'll
	 * simply shift it right by the number of decimal places in the
	 * final result and we'll be all done.
	 */
	if (!error) {
		// save what we have as one large integer
		mNegative = (mNegative != aNumber.mNegative);
		mWhole = result;
		mMSD = result.size() - 1;
		mFraction.clear();
		mLSD = -1;
		// trim off the extra zeros from the whole number part
		if (!mWhole.empty()) {
			while ((mMSD >= 0) && (mWhole[mMSD] == 0)) {
				--mMSD;
			}
		}
		// now shift it right by the correct amount
		rightShift(decimals);
	}
}


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
void CKFloat::divide( const CKFloat & aNumber )
{
	bool		error = false;

	// create the answer - initialized to zero
	CKFloat			answer;

	// next, make sure we have something to do (use answer in test)
	if (!error) {
		if (aNumber == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::parse(char *) - the passed in number is zero and "
				"I'm not capable of dividing by zero and coming up with a 'NaN' "
				"value. Please use doubleValue() to cast into a double if "
				"that's what you want.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	// check that I'm not zero either as that's an easy answer
	if (!error) {
		if (operator==(answer)) {
			// no need to exception, just don't do anything else
			error = true;
		}
	}

	// we're going to be messing with our value, so make a copy
	CKFloat		dividend(*this);
	// we're going to be messing with the divisor, so make a copy
	CKFloat		divisor(aNumber);

	/*
	 * There's an interesting issue here... when to <B>stop</B> the
	 * division? How many decimal places to carry out the division
	 * to before we stop and call it 'quits'? The code will stop
	 * well short of this if the numbers cooperate, but if they don't
	 * as in the case of a repeating decimal, we need to know when
	 * to stop what we're doing and call it quits. The idea I'm
	 * going with now is to look at the 'size' of each component
	 * and say that I have to be at least as large as the largest.
	 */
	int			maxFractionalDigits = 25;
	if (!error) {
		// first get the size of the dividend
		maxFractionalDigits = MAX(maxFractionalDigits, (dividend.mMSD + dividend.mLSD + 2));
		// next get the size of the divisor
		maxFractionalDigits = MAX(maxFractionalDigits, (divisor.mMSD + divisor.mLSD + 2));
	}

	/*
	 * In order to make this easier, we're going to do the division
	 * on two <B>positive</B> numbers and then work out the sign of
	 * the result after it's all over. In order to do this, we need
	 * to save the sign flags on both us and the incoming number
	 */
	bool		answerNegative = false;
	if (!error) {
		// get the sign of the final result
		answerNegative = (dividend.mNegative != divisor.mNegative);
		// ...and then force the two numbers to be positive
		dividend.mNegative = false;
		divisor.mNegative = false;
	}

	/*
	 * Now we need to subtract the divisor from the dividend until
	 * there are no more to take from us without going negative
	 * (this is why it helps to have everything positive at this time).
	 * When we get there we have the whole number of the quotient
	 * and then we can work on the remainder.
	 */
	// make a zero value for testing
	CKFloat		zero("0");
	// make a unit value for incrementing
	CKFloat		tick("1");
	// now do the whole number division
	if (!error) {
		while (!error && (dividend >= divisor)) {
			dividend -= divisor;
			answer += tick;
		}
	}

	/*
	 * At this point we need to deal with the remainder in the whole
	 * number division. This really becomes the 'dividend' of the
	 * division for the fractional part of the answer. The trick is
	 * that if there's not enough remainder (divisor > dividend) then
	 * we need to multiply the dividend by 10 and divide the addition
	 * to the result by 10.
	 */
	if (!error) {
		while (!error && (dividend > zero) && (tick.mLSD < maxFractionalDigits)) {
			// see if I need to bump up the dividend
			if (dividend < divisor) {
				dividend.leftShift(1);
				tick.rightShift(1);
			}
			// divide the divisor into the new dividend
			while (!error && (dividend >= divisor)) {
				dividend -= divisor;
				answer += tick;
			}
		}
	}

	/*
	 * At this point we have the (positive) answer in 'answer'. We
	 * need to copy in it's <B>value</B> but pick off the negative
	 * flag from the original test.
	 */
	if (!error) {
		// lock this guy up for the addition
		CKStackLocker		lockem(&mMutex);

		// get the right sign
		mNegative = answerNegative;
		// ...and the right number
		mWhole = answer.mWhole;
		mMSD = answer.mMSD;
		mFraction = answer.mFraction;
		mLSD = answer.mLSD;
	}
}


/********************************************************
 *
 *           Mathematical Convenience Methods
 *
 ********************************************************/
/*
 * This is the convenience method that adds the provided
 * <TT>char</TT> to this value.
 */
void CKFloat::add( char aChar )
{
	add(CKFloat(aChar));
}


/*
 * This is the convenience method that adds the provided
 * <TT>int</TT> to this value.
 */
void CKFloat::add( int anInt )
{
	add(CKFloat(anInt));
}


/*
 * This is the convenience method that adds the provided
 * <TT>long</TT> to this value.
 */
void CKFloat::add( long aLong )
{
	add(CKFloat(aLong));
}


/*
 * This is the convenience method that adds the provided
 * <TT>float</TT> to this value.
 */
void CKFloat::add( float aFloat )
{
	add(CKFloat(aFloat));
}


/*
 * This is the convenience method that adds the provided
 * <TT>double</TT> to this value.
 */
void CKFloat::add( double aDouble )
{
	add(CKFloat(aDouble));
}


/*
 * This is the convenience method that adds the value of the provided
 * (char *) to this value.
 */
void CKFloat::add( char *aString )
{
	if (aString != NULL) {
		add(CKFloat(aString));
	}
}


void CKFloat::add( const char *aString )
{
	if (aString != NULL) {
		add(CKFloat((char *)aString));
	}
}


/*
 * This is the convenience method that adds the value of the provided
 * CKString to this value.
 */
void CKFloat::add( CKString & aString )
{
	add(CKFloat(aString.c_str()));
}


void CKFloat::add( const CKString & aString )
{
	add(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that adds the value of the provided
 * STL std::string to this value.
 */
void CKFloat::add( std::string & aString )
{
	add(CKFloat(aString.c_str()));
}


void CKFloat::add( const std::string & aString )
{
	add(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that adds the value of the
 * provided CKVariant to this value. This only works if the
 * variant has a numerical value or a string that can be cast
 * into a value.
 */
void CKFloat::add( CKVariant & aVariant )
{
	switch (aVariant.getType()) {
		case eStringVariant:
			if (aVariant.getStringValue() != NULL) {
				add(*aVariant.getStringValue());
			}
			break;
		case eNumberVariant:
			add(aVariant.getDoubleValue());
			break;
		default:
			std::ostringstream	msg;
			msg << "CKFloat::add(CKVariant &) - the passed in variant is not "
				"a string or a number and those are the only two types of values "
				"that can really be added to a float. Please check the type "
				"of the variant before trying to add it to a float.";
			throw CKException(__FILE__, __LINE__, msg.str());
			break;
	}
}


void CKFloat::add( const CKVariant & aVariant )
{
	add((CKVariant &)aVariant);
}


/*
 * This is the convenience method that subtracts the provided
 * <TT>char</TT> from this value.
 */
void CKFloat::subtract( char aChar )
{
	subtract(CKFloat(aChar));
}


/*
 * This is the convenience method that subtracts the provided
 * <TT>int</TT> from this value.
 */
void CKFloat::subtract( int anInt )
{
	subtract(CKFloat(anInt));
}


/*
 * This is the convenience method that subtracts the provided
 * <TT>long</TT> from this value.
 */
void CKFloat::subtract( long aLong )
{
	subtract(CKFloat(aLong));
}


/*
 * This is the convenience method that subtracts the provided
 * <TT>float</TT> from this value.
 */
void CKFloat::subtract( float aFloat )
{
	subtract(CKFloat(aFloat));
}


/*
 * This is the convenience method that subtracts the provided
 * <TT>double</TT> from this value.
 */
void CKFloat::subtract( double aDouble )
{
	subtract(CKFloat(aDouble));
}


/*
 * This is the convenience method that subtracts the value of
 * the provided (char *) from this value.
 */
void CKFloat::subtract( char *aString )
{
	if (aString != NULL) {
		subtract(CKFloat(aString));
	}
}


void CKFloat::subtract( const char *aString )
{
	if (aString != NULL) {
		subtract(CKFloat((char *)aString));
	}
}


/*
 * This is the convenience method that subtracts the value of
 * the provided CKString from this value.
 */
void CKFloat::subtract( CKString & aString )
{
	subtract(CKFloat(aString.c_str()));
}


void CKFloat::subtract( const CKString & aString )
{
	subtract(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that subtracts the value of
 * the provided STL std::string from this value.
 */
void CKFloat::subtract( std::string & aString )
{
	subtract(CKFloat(aString.c_str()));
}


void CKFloat::subtract( const std::string & aString )
{
	subtract(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that subtracts the value of the
 * provided CKVariant from this value. This only works if the
 * variant has a numerical value or a string that can be cast
 * into a value.
 */
void CKFloat::subtract( CKVariant & aVariant )
{
	switch (aVariant.getType()) {
		case eStringVariant:
			if (aVariant.getStringValue() != NULL) {
				subtract(*aVariant.getStringValue());
			}
			break;
		case eNumberVariant:
			subtract(aVariant.getDoubleValue());
			break;
		default:
			std::ostringstream	msg;
			msg << "CKFloat::subtract(CKVariant &) - the passed in variant is not "
				"a string or a number and those are the only two types of values "
				"that can really be added to a float. Please check the type "
				"of the variant before trying to add it to a float.";
			throw CKException(__FILE__, __LINE__, msg.str());
			break;
	}
}


void CKFloat::subtract( const CKVariant & aVariant )
{
	subtract((CKVariant &)aVariant);
}


/*
 * This is the convenience method that multiplies this value
 * by the provided <TT>char</TT> value.
 */
void CKFloat::multiply( char aChar )
{
	multiply(CKFloat(aChar));
}


/*
 * This is the convenience method that multiplies this value
 * by the provided <TT>int</TT> value.
 */
void CKFloat::multiply( int anInt )
{
	multiply(CKFloat(anInt));
}


/*
 * This is the convenience method that multiplies this value
 * by the provided <TT>long</TT> value.
 */
void CKFloat::multiply( long aLong )
{
	multiply(CKFloat(aLong));
}


/*
 * This is the convenience method that multiplies this value
 * by the provided <TT>float</TT> value.
 */
void CKFloat::multiply( float aFloat )
{
	multiply(CKFloat(aFloat));
}


/*
 * This is the convenience method that multiplies this value
 * by the provided <TT>double</TT> value.
 */
void CKFloat::multiply( double aDouble )
{
	multiply(CKFloat(aDouble));
}


/*
 * This is the convenience method that multiplies this value
 * by the value of the provided (char *) value.
 */
void CKFloat::multiply( char *aString )
{
	if (aString != NULL) {
		multiply(CKFloat(aString));
	}
}


void CKFloat::multiply( const char *aString )
{
	if (aString != NULL) {
		multiply(CKFloat((char *)aString));
	}
}


/*
 * This is the convenience method that multiplies this value
 * by the value of the provided CKString value.
 */
void CKFloat::multiply( CKString & aString )
{
	multiply(CKFloat(aString.c_str()));
}


void CKFloat::multiply( const CKString & aString )
{
	multiply(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that multiplies this value
 * by the value of the provided STL std::string value.
 */
void CKFloat::multiply( std::string & aString )
{
	multiply(CKFloat(aString.c_str()));
}


void CKFloat::multiply( const std::string & aString )
{
	multiply(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that multiplies this value
 * by the value of the provided CKVariant from this value. This
 * only works if the variant has a numerical value or a string
 * that can be cast into a value.
 */
void CKFloat::multiply( CKVariant & aVariant )
{
	switch (aVariant.getType()) {
		case eStringVariant:
			if (aVariant.getStringValue() != NULL) {
				multiply(*aVariant.getStringValue());
			}
			break;
		case eNumberVariant:
			multiply(aVariant.getDoubleValue());
			break;
		default:
			std::ostringstream	msg;
			msg << "CKFloat::multiply(CKVariant &) - the passed in variant is not "
				"a string or a number and those are the only two types of values "
				"that can really be added to a float. Please check the type "
				"of the variant before trying to add it to a float.";
			throw CKException(__FILE__, __LINE__, msg.str());
			break;
	}
}


void CKFloat::multiply( const CKVariant & aVariant )
{
	multiply((CKVariant &)aVariant);
}


/*
 * This is the convenience method that divides the provided
 * <TT>char</TT> into this value. (think: 'this = this / argument')
 */
void CKFloat::divide( char aChar )
{
	divide(CKFloat(aChar));
}


/*
 * This is the convenience method that divides the provided
 * <TT>int</TT> into this value. (think: 'this = this / argument')
 */
void CKFloat::divide( int anInt )
{
	divide(CKFloat(anInt));
}


/*
 * This is the convenience method that divides the provided
 * <TT>long</TT> into this value. (think: 'this = this / argument')
 */
void CKFloat::divide( long aLong )
{
	divide(CKFloat(aLong));
}


/*
 * This is the convenience method that divides the provided
 * <TT>float</TT> into this value.
 * (think: 'this = this / argument')
 */
void CKFloat::divide( float aFloat )
{
	divide(CKFloat(aFloat));
}


/*
 * This is the convenience method that divides the provided
 * <TT>double</TT> into this value.
 * (think: 'this = this / argument')
 */
void CKFloat::divide( double aDouble )
{
	divide(CKFloat(aDouble));
}


/*
 * This is the convenience method that divides the value of
 * the provided (char *) into this value.
 * (think: 'this = this / argument')
 */
void CKFloat::divide( char *aString )
{
	if (aString != NULL) {
		divide(CKFloat(aString));
	}
}


void CKFloat::divide( const char *aString )
{
	if (aString != NULL) {
		divide(CKFloat(aString));
	}
}


/*
 * This is the convenience method that divides the value of
 * the provided CKString into this value.
 * (think: 'this = this / argument')
 */
void CKFloat::divide( CKString & aString )
{
	divide(CKFloat(aString.c_str()));
}


void CKFloat::divide( const CKString & aString )
{
	divide(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that divides the value of
 * the provided STL std::string into this value.
 * (think: 'this = this / argument')
 */
void CKFloat::divide( std::string & aString )
{
	divide(CKFloat(aString.c_str()));
}


void CKFloat::divide( const std::string & aString )
{
	divide(CKFloat(aString.c_str()));
}


/*
 * This is the convenience method that divides the value of the
 * provided CKVariant into this value. This only works if the
 * variant has a numerical value or a string that can be cast
 * into a value. (think: 'this = this / argument')
 */
void CKFloat::divide( CKVariant & aVariant )
{
	switch (aVariant.getType()) {
		case eStringVariant:
			if (aVariant.getStringValue() != NULL) {
				divide(*aVariant.getStringValue());
			}
			break;
		case eNumberVariant:
			divide(aVariant.getDoubleValue());
			break;
		default:
			std::ostringstream	msg;
			msg << "CKFloat::divide(CKVariant &) - the passed in variant is not "
				"a string or a number and those are the only two types of values "
				"that can really be added to a float. Please check the type "
				"of the variant before trying to add it to a float.";
			throw CKException(__FILE__, __LINE__, msg.str());
			break;
	}
}


void CKFloat::divide( const CKVariant & aVariant )
{
	divide((CKVariant &)aVariant);
}


/********************************************************
 *
 *                Operator Methods
 *
 ********************************************************/
/*
 * For each of the add() methods we need to have a '+='
 * operator so that the code can look clean.
 */
CKFloat & CKFloat::operator+=( char aChar )
{
	add(aChar);
	return *this;
}


CKFloat & CKFloat::operator+=( int anInt )
{
	add(anInt);
	return *this;
}


CKFloat & CKFloat::operator+=( long aLong )
{
	add(aLong);
	return *this;
}


CKFloat & CKFloat::operator+=( float aFloat )
{
	add(aFloat);
	return *this;
}


CKFloat & CKFloat::operator+=( double aDouble )
{
	add(aDouble);
	return *this;
}


CKFloat & CKFloat::operator+=( char *aString )
{
	if (aString != NULL) {
		add(aString);
	}
	return *this;
}


CKFloat & CKFloat::operator+=( const char *aString )
{
	if (aString != NULL) {
		add(aString);
	}
	return *this;
}


CKFloat & CKFloat::operator+=( CKString & aString )
{
	add(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator+=( const CKString & aString )
{
	add(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator+=( std::string & aString )
{
	add(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator+=( const std::string & aString )
{
	add(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator+=( CKVariant & aVariant )
{
	add(aVariant);
	return *this;
}


CKFloat & CKFloat::operator+=( const CKVariant & aVariant )
{
	add((CKVariant &)aVariant);
	return *this;
}


CKFloat & CKFloat::operator+=( CKFloat & aFloat )
{
	add(aFloat);
	return *this;
}


CKFloat & CKFloat::operator+=( const CKFloat & aFloat )
{
	add((CKFloat &)aFloat);
	return *this;
}


/*
 * For each of the subtract() methods we need to have a '-='
 * operator so that the code can look clean.
 */
CKFloat & CKFloat::operator-=( char aChar )
{
	subtract(aChar);
	return *this;
}


CKFloat & CKFloat::operator-=( int anInt )
{
	subtract(anInt);
	return *this;
}


CKFloat & CKFloat::operator-=( long aLong )
{
	subtract(aLong);
	return *this;
}


CKFloat & CKFloat::operator-=( float aFloat )
{
	subtract(aFloat);
	return *this;
}


CKFloat & CKFloat::operator-=( double aDouble )
{
	subtract(aDouble);
	return *this;
}


CKFloat & CKFloat::operator-=( char *aString )
{
	if (aString != NULL) {
		subtract(aString);
	}
	return *this;
}


CKFloat & CKFloat::operator-=( const char *aString )
{
	if (aString != NULL) {
		subtract((char *)aString);
	}
	return *this;
}


CKFloat & CKFloat::operator-=( CKString & aString )
{
	subtract(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator-=( const CKString & aString )
{
	subtract(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator-=( std::string & aString )
{
	subtract(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator-=( const std::string & aString )
{
	subtract(aString.c_str());
	return *this;
}


CKFloat & CKFloat::operator-=( CKVariant & aVariant )
{
	subtract(aVariant);
	return *this;
}


CKFloat & CKFloat::operator-=( const CKVariant & aVariant )
{
	subtract((CKVariant &)aVariant);
	return *this;
}


CKFloat & CKFloat::operator-=( CKFloat & aFloat )
{
	subtract(aFloat);
	return *this;
}


CKFloat & CKFloat::operator-=( const CKFloat & aFloat )
{
	subtract((CKFloat &)aFloat);
	return *this;
}


/*
 * For each of the multiply() methods we need to have a '*='
 * operator so that the code can look clean.
 */
CKFloat & CKFloat::operator*=( char aChar )
{
	multiply(CKFloat(aChar));
	return *this;
}


CKFloat & CKFloat::operator*=( int anInt )
{
	multiply(CKFloat(anInt));
	return *this;
}


CKFloat & CKFloat::operator*=( long aLong )
{
	multiply(CKFloat(aLong));
	return *this;
}


CKFloat & CKFloat::operator*=( float aFloat )
{
	multiply(CKFloat(aFloat));
	return *this;
}


CKFloat & CKFloat::operator*=( double aDouble )
{
	multiply(CKFloat(aDouble));
	return *this;
}


CKFloat & CKFloat::operator*=( char *aString )
{
	if (aString != NULL) {
		multiply(CKFloat(aString));
	}
	return *this;
}


CKFloat & CKFloat::operator*=( const char *aString )
{
	if (aString != NULL) {
		multiply(CKFloat((char *)aString));
	}
	return *this;
}


CKFloat & CKFloat::operator*=( CKString & aString )
{
	multiply(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator*=( const CKString & aString )
{
	multiply(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator*=( std::string & aString )
{
	multiply(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator*=( const std::string & aString )
{
	multiply(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator*=( CKVariant & aVariant )
{
	multiply(CKFloat(aVariant));
	return *this;
}


CKFloat & CKFloat::operator*=( const CKVariant & aVariant )
{
	multiply(CKFloat((CKVariant &)aVariant));
	return *this;
}


CKFloat & CKFloat::operator*=( CKFloat & aFloat )
{
	multiply(aFloat);
	return *this;
}


CKFloat & CKFloat::operator*=( const CKFloat & aFloat )
{
	multiply((CKFloat &)aFloat);
	return *this;
}


/*
 * For each of the divide() methods we need to have a '/='
 * operator so that the code can look clean.
 */
CKFloat & CKFloat::operator/=( char aChar )
{
	divide(CKFloat(aChar));
	return *this;
}


CKFloat & CKFloat::operator/=( int anInt )
{
	divide(CKFloat(anInt));
	return *this;
}


CKFloat & CKFloat::operator/=( long aLong )
{
	divide(CKFloat(aLong));
	return *this;
}


CKFloat & CKFloat::operator/=( float aFloat )
{
	divide(CKFloat(aFloat));
	return *this;
}


CKFloat & CKFloat::operator/=( double aDouble )
{
	divide(CKFloat(aDouble));
	return *this;
}


CKFloat & CKFloat::operator/=( char *aString )
{
	if (aString != NULL) {
		divide(CKFloat(aString));
	}
	return *this;
}


CKFloat & CKFloat::operator/=( const char *aString )
{
	if (aString != NULL) {
		divide(CKFloat((char *)aString));
	}
	return *this;
}


CKFloat & CKFloat::operator/=( CKString & aString )
{
	divide(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator/=( const CKString & aString )
{
	divide(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator/=( std::string & aString )
{
	divide(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator/=( const std::string & aString )
{
	divide(CKFloat(aString.c_str()));
	return *this;
}


CKFloat & CKFloat::operator/=( CKVariant & aVariant )
{
	divide(CKFloat(aVariant));
	return *this;
}


CKFloat & CKFloat::operator/=( const CKVariant & aVariant )
{
	divide(CKFloat((CKVariant &)aVariant));
	return *this;
}


CKFloat & CKFloat::operator/=( CKFloat & aFloat )
{
	multiply(aFloat);
	return *this;
}


CKFloat & CKFloat::operator/=( const CKFloat & aFloat )
{
	multiply((CKFloat &)aFloat);
	return *this;
}


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
CKFloat operator+( CKFloat & aFloat, CKFloat & anOtherFloat )
{
	CKFloat		retval(aFloat);
	retval.add(anOtherFloat);
	return retval;
}


CKFloat operator-( CKFloat & aFloat, CKFloat & anOtherFloat )
{
	CKFloat		retval(aFloat);
	retval.subtract(anOtherFloat);
	return retval;
}


CKFloat operator*( CKFloat & aFloat, CKFloat & anOtherFloat )
{
	CKFloat		retval(aFloat);
	retval.multiply(anOtherFloat);
	return retval;
}


CKFloat operator/( CKFloat & aFloat, CKFloat & anOtherFloat )
{
	CKFloat		retval(aFloat);
	retval.divide(anOtherFloat);
	return retval;
}


/*
 * These operator functions will allow the mixed-mode math with
 * CKFloats casting the result up to a CKFloat in each case. First,
 * start with simple addition.
 */
CKFloat operator+( CKFloat & aFloat, int aValue )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( int aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( CKFloat & aFloat, char aValue )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( char aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( CKFloat & aFloat, long aValue )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( long aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( CKFloat & aFloat, double aValue )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( double aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( CKFloat & aFloat, float aValue )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


CKFloat operator+( float aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.add(aFloat);
	return retval;
}


/*
 * Next, let's do all the different forms of the subtraction
 * operator between the CKFloat and the other scalar types.
 */
CKFloat operator-( CKFloat & aFloat, int aValue )
{
	// it's going to be quicker to make the CKFloat from the primitive
	CKFloat		retval(aValue);
	retval.mNegative = !retval.mNegative;
	retval.add(aFloat);
	return retval;
}


CKFloat operator-( int aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.subtract(aFloat);
	return retval;
}


CKFloat operator-( CKFloat & aFloat, char aValue )
{
	// it's going to be quicker to make the CKFloat from the primitive
	CKFloat		retval(aValue);
	retval.mNegative = !retval.mNegative;
	retval.add(aFloat);
	return retval;
}


CKFloat operator-( char aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.subtract(aFloat);
	return retval;
}


CKFloat operator-( CKFloat & aFloat, long aValue )
{
	// it's going to be quicker to make the CKFloat from the primitive
	CKFloat		retval(aValue);
	retval.mNegative = !retval.mNegative;
	retval.add(aFloat);
	return retval;
}


CKFloat operator-( long aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.subtract(aFloat);
	return retval;
}


CKFloat operator-( CKFloat & aFloat, double aValue )
{
	// it's going to be quicker to make the CKFloat from the primitive
	CKFloat		retval(aValue);
	retval.mNegative = !retval.mNegative;
	retval.add(aFloat);
	return retval;
}


CKFloat operator-( double aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.subtract(aFloat);
	return retval;
}


CKFloat operator-( CKFloat & aFloat, float aValue )
{
	// it's going to be quicker to make the CKFloat from the primitive
	CKFloat		retval(aValue);
	retval.mNegative = !retval.mNegative;
	retval.add(aFloat);
	return retval;
}


CKFloat operator-( float aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.subtract(aFloat);
	return retval;
}


/*
 * Next, let's do all the different forms of the multiplication
 * operator between the variant and the other scalar types.
 */
CKFloat operator*( CKFloat & aFloat, int aValue )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( int aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( CKFloat & aFloat, char aValue )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( char aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( CKFloat & aFloat, long aValue )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( long aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( CKFloat & aFloat, double aValue )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( double aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( CKFloat & aFloat, float aValue )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


CKFloat operator*( float aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.multiply(aFloat);
	return retval;
}


/*
 * Next, let's do all the different forms of the division
 * operator between the variant and the other scalar types.
 */
CKFloat operator/( CKFloat & aFloat, int aValue )
{
	CKFloat		retval(aFloat);
	retval.divide(CKFloat(aValue));
	return retval;
}


CKFloat operator/( int aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.divide(aFloat);
	return retval;
}


CKFloat operator/( CKFloat & aFloat, char aValue )
{
	CKFloat		retval(aFloat);
	retval.divide(CKFloat(aValue));
	return retval;
}


CKFloat operator/( char aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.divide(aFloat);
	return retval;
}


CKFloat operator/( CKFloat & aFloat, long aValue )
{
	CKFloat		retval(aFloat);
	retval.divide(CKFloat(aValue));
	return retval;
}


CKFloat operator/( long aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.divide(aFloat);
	return retval;
}


CKFloat operator/( CKFloat & aFloat, double aValue )
{
	CKFloat		retval(aFloat);
	retval.divide(CKFloat(aValue));
	return retval;
}


CKFloat operator/( double aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.divide(aFloat);
	return retval;
}


CKFloat operator/( CKFloat & aFloat, float aValue )
{
	CKFloat		retval(aFloat);
	retval.divide(CKFloat(aValue));
	return retval;
}


CKFloat operator/( float aValue, CKFloat & aFloat )
{
	CKFloat		retval(aValue);
	retval.divide(aFloat);
	return retval;
}


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
bool CKFloat::operator==( CKFloat & anOther )
{
	bool	keepChecking = true;
	bool	isEqual = true;

	// lock up this guy for this operation
	CKStackLocker		lockem(&mMutex);

	// next see if the object is me
	if (keepChecking) {
		if (this == & anOther) {
			isEqual = true;
			keepChecking = false;
		}
	}

	/*
	 * Next, we need to check the components for
	 * equality.
	 */
	if (keepChecking) {
		if ((mMSD != anOther.mMSD) || (mLSD != anOther.mLSD) ||
			(mNegative != anOther.mNegative)) {
			isEqual = false;
			keepChecking = false;
		}
	}

	// check each of the digits of the whole number part
	if (keepChecking) {
		if (mMSD >= 0) {
			for (int i = mMSD; i >= 0; --i) {
				if (mWhole[i] != anOther.mWhole[i]) {
					isEqual = false;
					keepChecking = false;
					break;
				}
			}
		}
	}

	// check each of the digits of the fractional part
	if (keepChecking) {
		if (mLSD >= 0) {
			for (int i = 0; i < mLSD; ++i) {
				if (mFraction[i] != anOther.mFraction[i]) {
					isEqual = false;
					keepChecking = false;
					break;
				}
			}
		}
	}

	return isEqual;
}


bool CKFloat::operator==( const CKFloat & anOther )
{
	return operator==((CKFloat &)anOther);
}


bool CKFloat::operator==( CKFloat & anOther ) const
{
	return ((CKFloat *)this)->operator==(anOther);
}


bool CKFloat::operator==( const CKFloat & anOther ) const
{
	return ((CKFloat *)this)->operator==((CKFloat &)anOther);
}


/*
 * This method checks to see if the two CKFloats are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKFloat::operator!=( CKFloat & anOther )
{
	return !operator==(anOther);
}


bool CKFloat::operator!=( const CKFloat & anOther )
{
	return operator!=((CKFloat &)anOther);
}


bool CKFloat::operator!=( CKFloat & anOther ) const
{
	return ((CKFloat *)this)->operator!=(anOther);
}


bool CKFloat::operator!=( const CKFloat & anOther ) const
{
	return ((CKFloat *)this)->operator!=((CKFloat &)anOther);
}


/*
 * These methods complete the inequality tests for the CKFloat and
 * we need these as we might be doing a lot of testing and this makes
 * it a lot easier than converting to the right type and then doing
 * a low-level test.
 */
bool CKFloat::operator<( const CKFloat & anOther ) const
{
	return (((CKFloat *)this)->compareTo(anOther) < 0);
}


bool CKFloat::operator<=( const CKFloat & anOther ) const
{
	return (((CKFloat *)this)->compareTo(anOther) <= 0);
}


bool CKFloat::operator>( const CKFloat & anOther ) const
{
	return (((CKFloat *)this)->compareTo(anOther) > 0);
}


bool CKFloat::operator>=( const CKFloat & anOther ) const
{
	return (((CKFloat *)this)->compareTo(anOther) >= 0);
}


/*
 * These operators allow us to use the CKFloat as a "regular"
 * variable in conditionals which is really important because
 * we want this to fit into development like a regular scalar
 * variable.
 */
bool CKFloat::operator==( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator==( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator==(him);
}


bool CKFloat::operator!=( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator!=( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator!=(him);
}


bool CKFloat::operator<( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator<(him);
}


bool CKFloat::operator<=( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator<=( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator<=(him);
}


bool CKFloat::operator>( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator>(him);
}


bool CKFloat::operator>=( const char *aCString ) const
{
	CKFloat		him((char *)(aCString == NULL ? "0" : aCString));
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( const std::string & anSTLString ) const
{
	CKFloat		him((char *)anSTLString.c_str());
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( const CKString & aString ) const
{
	CKFloat		him((char *)aString.c_str());
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( char aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( int aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( long aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( float aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( double aValue ) const
{
	CKFloat		him(aValue);
	return ((CKFloat *)this)->operator>=(him);
}


bool CKFloat::operator>=( const CKVariant & aVariant ) const
{
	CKFloat		him(aVariant);
	return ((CKFloat *)this)->operator>=(him);
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKFloat::toString() const
{
	bool		error = false;
	CKString	buff;

	/*
	 * Build up the string representation of the value without
	 * <B>any</B> loss of significance.
	 */
	if (!error) {
		// see if it's negative - that's gotta be first
		if (mNegative) {
			buff.append("-");
		}
		// add in all the whole number digits
		if (mWhole.empty() || (mMSD < 0)) {
			buff.append("0");
		} else {
			for (int i = mMSD; i >= 0; --i) {
				buff.append((char)(mWhole[i] + '0'));
			}
		}
		// add in all the fractional digits
		if (!mFraction.empty() && (mLSD >= 0)) {
			buff.append(".");
			for (int i = 0; i <= mLSD; ++i) {
				buff.append((char)(mFraction[i] + '0'));
			}
		}
	}

	return buff;
}


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
void CKFloat::setStringValue( char *aNumber, bool assureThreadSafety )
{
	bool		error = false;

	// first, make sure we have something to do
	if (!error) {
		if (aNumber == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::setStringValue(char *, bool) - the passed in string "
				"is null and that means that there's nothing I can do. Please "
				"make sure the argument is not null before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else if (strlen(aNumber) == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::setStringValue(char *, bool) - the passed in string "
				"is empty and that means that there's nothing I can do. Please "
				"make sure the argument is not null before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Next, let's make a copy of this guy so that I can mess with him
	 * during the processing. Eventhough the argument is not 'const', I
	 * don't want to assume he hasn't been cast that way and is in actuality
	 * a const.
	 */
	char		*raw = NULL;
	char		*start = NULL;
	if (!error) {
		// first, make the copy
		raw = strdup(aNumber);
		if (raw == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFloat::setStringValue(char *, bool) - while trying to make "
				"a clean copy of the string '" << aNumber << "' for our use, an "
				"allocation error seems to have hit. Please check on this as "
				"soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// set the start at the... well... start
			start = raw;
		}
	}
	// now let's move past any whitespace at the beginning
	if (!error) {
		while ((start[0] != '\0') && isspace(start[0])) {
			++start;
		}
	}
	// now let's trim off any whitespace at the end
	int			len = 0;
	if (!error) {
		len = strlen(start);
		while ((len > 0) && isspace(start[len-1])) {
			start[len-1] = '\0';
			--len;
		}
	}

	/*
	 * As lame as it sounds, we have to check every character in this
	 * array to make sure it's an optional leading '-', a digit, or a
	 * single '.'. If we have anything other than this, we need to throw
	 * a NumberFormatException with a reasonable message.
	 */
	if (!error) {
		bool		gotDot = false;
		for (int i = 0; i < len; ++i) {
			char	c = start[i];
			if (c == '-') {
				if (i != 0) {
					error = true;
					std::ostringstream	msg;
					msg << "CKFloat::setStringValue(char *, bool) - the "
						"numerical representation '" << start << "' has a '-' "
						"somewhere other than as an optional leading first "
						"character. This is simply not allowed.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			} else if (c == '.') {
				if (gotDot) {
					error = true;
					std::ostringstream	msg;
					msg << "CKFloat::setStringValue(char *, bool) - the "
						"numerical representation '" << start << "' has more "
						"than one decimal point. This is not allowed.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					gotDot = true;
				}
			} else if (!isdigit(c)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFloat::setStringValue(char *, bool) - the "
					"numerical representation '" << start << "' has something "
					"other than a digit in it: '" << c << "'. This is not "
					"allowed.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	/*
	 * We need to start the process of parsing the input string into
	 * the three things we need: the sign, the whole number part and
	 * the fractional part. We're going to get all three and make
	 * sure they are consistent <B>before</B> we set them so that the
	 * number we're holding doesn't end up in an indeterminate state.
	 */
	// see if the number is negative (leading '-' in the string)
	bool		negative = false;
	if (!error) {
		if (start[0] == '-') {
			negative = true;
			// chop it off as it's no longer needed
			++start;
			--len;
		}
	}
	// now get the whole number part
	CKVector<char>	whole;
	int				msd = -1;
	CKVector<char>	fraction;
	int				lsd = -1;
	if (!error) {
		char	*dot = strchr(start, '.');
		if (dot == start) {
			// there is only a fractional part
			for (int i = 0; i < len; ++i) {
				fraction.addToEnd(start[i] - '0');
			}
			lsd = fraction.size() - 1;
		} else if (dot != NULL) {
			// get the whole number part
			dot[0] = '\0';
			for (int i = 0; i < (dot - start); ++i) {
				whole.addToFront(start[i] - '0');
			}
			msd = whole.size() - 1;
			// now get the fractional part
			if (dot[1] != '\0') {
				// move to the first fractional character (it's not NULL)
				++dot;
				for (unsigned int i = 0; i < strlen(dot); ++i) {
					fraction.addToEnd(dot[i] - '0');
				}
				lsd = fraction.size() - 1;
			}
		} else {
			// there is only the whole number part
			for (int i = 0; i < len; ++i) {
				whole.addToFront(start[i] - '0');
			}
			msd = whole.size() - 1;
		}
	}

	// if all is OK, then save the values
	if (!error) {
		// see if we need to lock up this guy for this operation
		if (assureThreadSafety) {
			mMutex.lock();
		}

		try {
			mNegative = negative;
			mWhole = whole;
			mMSD = msd;
			mFraction = fraction;
			mLSD = lsd;
			// trim off the extra zeros from the fractional part
			if (!mFraction.empty()) {
				while ((mLSD >= 0) && (mFraction[mLSD] == 0)) {
					--mLSD;
				}
			}
			// trim off the extra zeros from the whole number part
			if (!mWhole.empty()) {
				while ((mMSD >= 0) && (mWhole[mMSD] == 0)) {
					--mMSD;
				}
			}
		} catch (CKException & cke) {
			// now unlock this guy as we're done
			if (assureThreadSafety) {
				mMutex.unlock();
			}
			// now we can log the error and throw the exception
			std::ostringstream	msg;
			msg << "CKFloat::setStringValue(char *, bool) - while trying to "
				"set this guy to the parsed values of the string, a CKException "
				"was thrown: " << cke.getMessage();
			throw CKException(__FILE__, __LINE__, msg.str());
		} catch (...) {
			// now unlock this guy as we're done
			if (assureThreadSafety) {
				mMutex.unlock();
			}
			// now we can log the error and throw the exception
			std::ostringstream	msg;
			msg << "CKFloat::setStringValue(char *, bool) - while trying to "
				"set this guy to the parsed values of the string, an unknown "
				"exception was thrown";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
		
		// now unlock this guy as we're done
		if (assureThreadSafety) {
			mMutex.unlock();
		}
	}
}


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
void CKFloat::trim()
{
	trimLeft();
	trimRight();
}


/*
 * This method trims off the insignificant zeros from the right
 * of the fractional part of the number.
 */
void CKFloat::trimRight()
{
	// lock up this guy for this operation
	CKStackLocker		lockem(&mMutex);

	// trim off the extra zeros from the fractional part
	if (!mFraction.empty()) {
		while ((mLSD >= 0) && (mFraction[mLSD] == 0)) {
			--mLSD;
		}
	}
}


/*
 * This method trims off the insignificant zeros from the left
 * of the whole number part of the number.
 */
void CKFloat::trimLeft()
{
	// lock up this guy for this operation
	CKStackLocker		lockem(&mMutex);

	// trim off the extra zeros from the whole number part
	if (!mWhole.empty()) {
		while ((mMSD >= 0) && (mWhole[mMSD] == 0)) {
			--mMSD;
		}
	}
}


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
CKVector<char> CKFloat::getDigits() const
{
	bool			error = false;
	CKVector<char>	retval;

	// now put the digits into the right locations
	if (!error) {
		// do the fractional part first
		for (int i = mLSD; i >= 0; --i) {
			retval.addToEnd(mFraction[i]);
		}
		// now do the whole number part
		for (int i = 0; i <= mMSD; ++i) {
			retval.addToEnd(mWhole[i]);
		}
	}

	return retval;
}


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
void CKFloat::rightShift( int aCount )
{
	bool		error = false;

	/*
	 * Just to be on the safe side, we're going to make sure we
	 * have enough space in the fractional part (creating it if
	 * necessary) to hold the shift. If it turns out that we don't
	 * need the fractional part, well... then we don't have to use
	 * it, but it's better to plan for the need and not need it.
	 */
	if (!error) {
		// see if I need to reset the LSD location
		if (mFraction.empty()) {
			mLSD = -1;
		}
		// now make all the space I need in this guy's fractional storage
		while (mFraction.size() < aCount) {
			mFraction.addToEnd(0);
		}
	}

	/*
	 * At this point we need to shift everything in the fraction
	 * to the right 'aCount' spaces. The spaces that are vacated
	 * need to be set to '0' just in case there's nothing from the
	 * whole number part to back-fill.
	 */
	if (!error) {
		// move everything to the right the correct amount
		for (int i = mLSD; i >= 0; --i) {
			mFraction[i + aCount] = mFraction[i];
		}
		// now the most significant 'aCount' digits have to be '0'
		for (int i = 0; i < aCount; ++i) {
			mFraction[i] = 0;
		}
		// we also now have a new 'LSD' - update it
		mLSD += aCount;
	}

	/*
	 * OK, if we have anything in the whole number part, we need
	 * to move in the least significant 'aCount' digits from it
	 * to be the most significant digits in the fractional part.
	 */
	if (!error) {
		if (!mWhole.empty()) {
			// only move what we have available to move
			int		cnt = MIN(aCount, mMSD + 1);
			for (int i = 0; i < cnt; ++i) {
				mFraction[aCount - i - 1] = mWhole[i];
			}
			// now move over all of the whole digits the right amount
			for (int i = 0; i <= mMSD - cnt; ++i) {
				mWhole[i] = mWhole[i + cnt];
			}
			// we also now have a new 'MSD' - update it
			mMSD -= cnt;
		}
	}

	/*
	 * As a final step, we're going to trim off the leading and
	 * trailing '0's that might have been added in the course of
	 * processing. The point being that if we added a bunch of
	 * insignificant zeros, we should clean up our mess.
	 */
	if (!error) {
		if (!mFraction.empty()) {
			while ((mLSD >= 0) && (mFraction[mLSD] == 0)) {
				--mLSD;
			}
		}
	}
}


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
void CKFloat::leftShift( int aCount )
{
	bool		error = false;

	/*
	 * Just to be on the safe side, we're going to make sure we
	 * have enough space in the whole number part (creating it if
	 * necessary) to hold the shift. If it turns out that we don't
	 * need the whole number part, well... then we don't have to use
	 * it, but it's better to plan for the need and not need it.
	 */
	if (!error) {
		// see if I need to reset the MSD location
		if (mWhole.empty()) {
			mMSD = -1;
		}
		// now make all the space I need in this guy's whole number storage
		while (mWhole.size() < aCount) {
			mWhole.addToEnd(0);
		}
	}

	/*
	 * At this point we need to shift everything in the whole number
	 * to the left 'aCount' spaces. The spaces that are vacated
	 * need to be set to '0' just in case there's nothing from the
	 * fractional part to back-fill.
	 */
	if (!error) {
		// move everything to the left the correct amount
		for (int i = mMSD; i >= 0; --i) {
			mWhole[i + aCount] = mWhole[i];
		}
		// now the least significant 'aCount' digits have to be '0'
		for (int i = 0; i < aCount; ++i) {
			mWhole[i] = 0;
		}
		// we also now have a new 'MSD' - update it
		mMSD += aCount;
	}

	/*
	 * OK, if we have anything in the fractional part, we need
	 * to move in the most significant 'aCount' digits from it
	 * to be the least significant digits in the whole number
	 * part.
	 */
	if (!error) {
		if (!mFraction.empty()) {
			// only move what we have available to move
			int		cnt = MIN(aCount, mLSD + 1);
			for (int i = 0; i < cnt; ++i) {
				mWhole[aCount - i - 1] = mFraction[i];
			}
			// now move over all of the fractional digits the right amount
			for (int i = 0; i <= mLSD - cnt; ++i) {
				mFraction[i] = mFraction[i + cnt];
			}
			// we also now have a new 'LSD' - update it
			mLSD -= cnt;
		}
	}

	/*
	 * As a final step, we're going to trim off the leading and
	 * trailing '0's that might have been added in the course of
	 * processing. The point being that if we added a bunch of
	 * insignificant zeros, we should clean up our mess.
	 */
	if (!error) {
		if (!mWhole.empty()) {
			while ((mMSD >= 0) && (mWhole[mMSD] == 0)) {
				--mMSD;
			}
		}
	}
}


/*
 * Compares two BKFloat objects numerically. The return value
 * is zero if the argument and this floa are numerically equal,
 * less than zero if this float is numerically less than
 * anotherFloat, and greater than 0 if this float is numerically
 * greater than anotherFloat.
 */
int CKFloat::compareTo( const CKFloat & anotherFloat )
{
	bool		error = false;
	bool		keepChecking = true;
	int			retval = 0;

	// lock up this guy for this operation
	CKStackLocker		lockem(&mMutex);

	// see if this is the identity condition
	if (!error && keepChecking) {
		if (this == & anotherFloat) {
			keepChecking = false;
		}
	}

	// check the signs as that will tell us how we need to check
	if (!error && keepChecking) {
		// are the signs different?
		if (mNegative != anotherFloat.mNegative) {
			// I'm negative and he's not - we're done.
			keepChecking = false;
			retval = (mNegative ? -1 : 1);
		}
	}

	// check the length of the whole part, it's an easy test
	if (!error && keepChecking) {
		if (mMSD > anotherFloat.mMSD) {
			// I have a larger whole number than he does
			keepChecking = false;
			retval = (mNegative ? -1 : 1);
		} else if (mMSD < anotherFloat.mMSD) {
			// He has a larger whole number than I do
			keepChecking = false;
			retval = (mNegative ? 1 : -1);
		}
	}

	/*
	 * At this point we know that the whole numbers are of the
	 * same size and so now we're down to looking at each number
	 * from the MSD to the LSD and at the first sign that there
	 * is one larger than the other
	 */
	if (!error && keepChecking) {
		if (mMSD >= 0) {
			for (int i = mMSD; i >= 0; --i) {
				if (mWhole[i] > anotherFloat.mWhole[i]) {
					keepChecking = false;
					retval = (mNegative ? -1 : 1);
					break;
				} else if (mWhole[i] < anotherFloat.mWhole[i]) {
					keepChecking = false;
					retval = (mNegative ? 1 : -1);
					break;
				}
			}
		}
	}

	/*
	 * At this point we need to check the fractional part of the
	 * floating point numbers. We start at the MSD and move to the
	 * LSD and as soon as we find a difference we stop. The trick
	 * here is that if we run to the end of one of the numbers and
	 * all has been equal up to that point, then the longer one is
	 * the one with the larger magnitude. So stop.
	 */
	if (!error && keepChecking) {
		int		lsd = MIN(mLSD, anotherFloat.mLSD);
		for (int i = 0; i <= lsd; ++i) {
			if (mFraction[i] > anotherFloat.mFraction[i]) {
				keepChecking = false;
				retval = (mNegative ? -1 : 1);
				break;
			} else if (mFraction[i] < anotherFloat.mFraction[i]) {
				keepChecking = false;
				retval = (mNegative ? 1 : -1);
				break;
			}
		}
	}

	/*
	 * At this point we need one final check - the length of the
	 * fractional components. If they are equal up to the minimum
	 * of the two lengths, then if one is longer, it is larger.
	 * If they are the same, then these guys are equal.
	 */
	if (!error && keepChecking) {
		if (mLSD > anotherFloat.mLSD) {
			// I have a larger fractional number than he does
			keepChecking = false;
			retval = (mNegative ? -1 : 1);
		} else if (mLSD < anotherFloat.mLSD) {
			// He has a larger fractional number than I do
			keepChecking = false;
			retval = (mNegative ? 1 : -1);
		}
	}

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKFloat & aFloat )
{
	aStream << aFloat.toString();

	return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CKFloat & aFloat )
{
	aStream << aFloat.toString();

	return aStream;
}
