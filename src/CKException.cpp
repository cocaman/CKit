/*
 * CKException.cpp - this is the implementation of the base exception class
 *                   for CKit and has the basic functionality that most
 *                   projects are going to need. It's also a simplified
 *                   version of the base exception class used in the MarketMash
 *                   Server.
 * 
 * $Id: CKException.cpp,v 1.7 2004/09/20 16:19:23 drbob Exp $
 */

//	System Headers
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"

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
 * This the is default constructor for the exception. It doesn't
 * set anything up, which means it won't be too informative to the
 * receiver (catcher), but there are probably times that this is
 * important. It really should be used infrequently.
 */
CKException::CKException( ) :
	mExceptionName( (char*)"CKException" ),
	mFileName( NULL ),
	mLineNumber( -1 ),
	mMessage()
{
	init();
}


/*
 * This is a simple constructor for the exception. The message
 * *MUST* be a constant string definition as *no* copy will be
 * made of this message and it will be assumed to be around for
 * as long as this exception is around. For most uses, a programmer-
 * defined constant it what's used here.
 */
CKException::CKException( const CKString & aMessage ) :
	mExceptionName( (char*)"CKException" ),
	mFileName( NULL ),
	mLineNumber( -1 ),
	mMessage( aMessage )
{
	init();
}


/*
 * This form of the constructor takes the constant filename and
 * line number where the exception is assumed to have been thrown.
 * In practice, this form is really useful for the pre-processor
 * directives __FILE__ and __LINE__ where the compiler will put in
 * the proper values and they will be of the right scope and type.
 */
CKException::CKException( const char *aFileName, int aLineNumber ) :
	mExceptionName( (char*)"CKException" ),
	mFileName( (char*)aFileName ),
	mLineNumber( aLineNumber ),
	mMessage()
{
	init();
}


/*
 * This is the most commonly used form of the constructor - it
 * takes the pre-processor direcetives __FILE__ and __LINE__ and
 * a constant string message that will all have the proper scope
 * and lifetime for this exception.
 */
CKException::CKException( const char *aFileName, int aLineNumber, const CKString & aMessage ) :
	mExceptionName( (char*)"CKException" ),
	mFileName( (char*)aFileName ),
	mLineNumber( aLineNumber ),
	mMessage( aMessage )
{
	init();
}


/*
 * This constructor takes another exception as it's source of
 * data and simply duplicates it.
 */
CKException::CKException( const CKException & anException ) :
	mExceptionName( anException.mExceptionName ),
	mFileName( anException.mFileName ),
	mLineNumber( anException.mLineNumber ),
	mMessage( anException.mMessage )
{
	init();
}


/*
 * The standard destructor needs to be virtual because if any
 * subclasses are using memory we need to make sure that they
 * all call the right version of the destructor.
 */
CKException::~CKException( )
{
	// release nothing because we've taken ownership of nothing
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
const CKException & CKException::operator=( const CKException & anException )
{
	mExceptionName = anException.mExceptionName;
	mFileName = anException.mFileName;
	mLineNumber = anException.mLineNumber;
	mMessage = anException.mMessage;

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * In order to get the message out of this exception, let's
 * make a method very similar to the Java exceptions.
 */
CKString CKException::getMessage() const
{
	return mMessage;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
 
/*
 * This method writes out the relavent details from the exception
 * to the provided stream. This is nice in that it writes out only
 * that data it's been given and that makes it look clean.
 */
std::ostream & CKException::printOut( std::ostream & anOStream )
{
	anOStream << mExceptionName << ": " << mMessage << " ";
	if ( mLineNumber != -1 ) {
		anOStream << mFileName << "@" << mLineNumber;
	}

	return anOStream;
}


/*
 * This is the basic initialization method that for this class
 * writes out the exception information to the standard error.
 * This is useful as it means *all* exceptions are logged so that
 * the user doesn't ever have to worry about doing it.
 */
void CKException::init( )
{
	// log this guy to the standard error as the default behavior
	std::cerr << *this << std::endl;
}


/*
 * This protected constructor is necessary because our subclasses
 * need to be able to set all our instance variables without having
 * to dig into them.
 */
CKException::CKException( const char *anExceptionName, const char *aFileName, int aLineNumber, const CKString & aMessage ) :
	mExceptionName( (char*)anExceptionName ),
	mFileName( (char*)aFileName ),
	mLineNumber( aLineNumber ),
	mMessage( aMessage )
{
	init();
}
