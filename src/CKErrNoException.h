/*
 * CKErrNoException.h - this file defines the CKit exception class that throws
 *                      exceptions based on the system's errno variable
 *                      and to include that information into the exception and
 *                      pass it back up the calling stack to the catch block.
 *                      This is basically the same class as it's namesake in
 *                      the MarketMash Server.
 * 
 * $Id: CKErrNoException.h,v 1.5 2004/09/11 21:07:43 drbob Exp $
 */
#ifndef __CKERRNO_EXCEPTION_H
#define __CKERRNO_EXCEPTION_H

//	System Headers
#include <errno.h>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKErrNoException :
	public CKException
{
	public :
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
		CKErrNoException( const char *aFileName, int aLineNumber, int anError = errno );
		/*
		 * This constructor takes another exception as it's source of
		 * data and simply duplicates it.
		 */
		CKErrNoException( const CKErrNoException & anOther );

		/*
		 * The standard destructor needs to be virtual because if any
		 * subclasses are using memory we need to make sure that they
		 * all call the right version of the destructor.
		 */
		virtual ~CKErrNoException( void );

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		const CKErrNoException & operator=( const CKErrNoException & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method returns the error number for those users that would
		 * like to get at it after the creation of the exception's message.
		 */
		inline int getErrNo( ) { return mErrNo; }


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
		virtual std::ostream & printOut( std::ostream & anOStream );

	private :
		/*
		 * This is the error number as it was defined when the standard
		 * constructor was called and pulled in that value.
		 */
		int			mErrNo;
};

#endif	//__CKERRNO_EXCEPTION_H
