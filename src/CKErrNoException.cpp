/*
 * CKErrNoException.cpp - this is the implementation of the CKit system
 *                        errno-based exception class that can be quite useful
 *                        when there's a system call that updates errno, and
 *                        you want to pass that information up to the caller
 *                        for them to decode as they see fit.
 * 
 * $Id: CKErrNoException.cpp,v 1.7 2004/09/20 16:19:23 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKErrNoException.h"

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
 * This is the general form of the errno-based constructor and
 * requires the __FILE__ and __LINE__ pre-processor directives
 * while automatically picking up the errno from the system. The
 * error number is converted to the text of the exception so there's
 * no real reason to include a message at this level.
 */
CKErrNoException::CKErrNoException( const char *aFileName, int aLineNumber, int anError ) :
	CKException( "CKErrNoException", aFileName, aLineNumber, "" ),
	mErrNo( anError )
{
}


/*
 * This constructor takes another exception as it's source of
 * data and simply duplicates it.
 */
CKErrNoException::CKErrNoException( const CKErrNoException & anOther ) :
	CKException( anOther ),
	mErrNo( anOther.mErrNo )
{
}


/*
 * The standard destructor needs to be virtual because if any
 * subclasses are using memory we need to make sure that they
 * all call the right version of the destructor.
 */
CKErrNoException::~CKErrNoException()
{
}
  

/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
const CKErrNoException & CKErrNoException::operator=( const CKErrNoException & anOther )
{
	CKException::operator=( anOther );
	mErrNo = anOther.mErrNo;
  
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/


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
std::ostream & CKErrNoException::printOut( std::ostream & anOStream )
{
	CKException::printOut( anOStream );  
	anOStream << " - " << strerror( mErrNo );

	return anOStream;
}
