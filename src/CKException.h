/*
 * CKException.h - this file defines the class that is used by CKit and those
 *                 projects that use it for general exceptions. This is
 *                 important because virtually every project needs some kind
 *                 of exception handling and this forms a nice basis for that,
 *                 and is also very close to the base exception class
 *                 for the MarketMash Server.
 * 
 * $Id: CKException.h,v 1.7 2004/09/20 16:19:24 drbob Exp $
 */
#ifndef __CKEXCEPTION_H
#define __CKEXCEPTION_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <string>

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
class CKException
{
	public:
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
		CKException();
		/*
		 * This is a simple constructor for the exception. The message
		 * *MUST* be a constant string definition as *no* copy will be
		 * made of this message and it will be assumed to be around for
		 * as long as this exception is around. For most uses, a programmer-
		 * defined constant it what's used here.
		 */
		CKException( const CKString & aMessage );
		/*
		 * This form of the constructor takes the constant filename and
		 * line number where the exception is assumed to have been thrown.
		 * In practice, this form is really useful for the pre-processor
		 * directives __FILE__ and __LINE__ where the compiler will put in
		 * the proper values and they will be of the right scope and type.
		 */
		CKException( const char *aFileName, int aLineNumber );
		/*
		 * This is a commonly used form of the constructor - it
		 * takes the pre-processor direcetives __FILE__ and __LINE__ and
		 * a constant string message that will all have the proper scope
		 * and lifetime for this exception.
		 */
		CKException( const char *aFileName, int aLineNumber, const CKString & aMessage );
		/*
		 * This constructor takes another exception as it's source of
		 * data and simply duplicates it.
		 */
		CKException( const CKException & anException );
		/*
		 * The standard destructor needs to be virtual because if any
		 * subclasses are using memory we need to make sure that they
		 * all call the right version of the destructor.
		 */
		virtual ~CKException();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		virtual const CKException & operator=( const CKException & anException );
  
		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * In order to get the message out of this exception, let's
		 * make a method very similar to the Java exceptions.
		 */
		CKString getMessage() const;

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

	protected:
		/*
		 * This is the basic initialization method that for this class
		 * writes out the exception information to the standard error.
		 * This is useful as it means *all* exceptions are logged so that
		 * the user doesn't ever have to worry about doing it.
		 */
		void init();
		/*
		 * This protected constructor is necessary because our subclasses
		 * need to be able to set all our instance variables without having
		 * to dig into them.
		 */
		CKException( const char *aExceptionName, const char *aFileName, int aLineNumber, const CKString & aMessage );
 
	private:  
		/*
		 * This is the name of the exception and defaults to the class name
		 * and for all subclasses the value needs to be set in the 
		 * constructors.
		 */
		char			*mExceptionName;
		/*
		 * This is the name of the file that the exception was believed to
		 * have been thrown - or NULL in the case that the user didn't 
		 * provide it to this class.
		 */
		char			*mFileName;
		/*
		 * This is the line number in the file that the exception was believed
		 * to have been thrown - or -1 in the case that the user didn't 
		 * provide it to this class.
		 */
		int				mLineNumber;
		/*
		 * This is the message as provided by the user during the constructor
		 * that can be used to provide a detailed reason/report on the reason
		 * for the exception. It's a good idea to provide one of these to each
		 * exception.
		 */
		CKString		mMessage;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of printOut().
 */
inline std::ostream & operator<<( std::ostream & anOStream, CKException & anException  )
{
	return anException.printOut( anOStream );
}

#endif	// __CKEXCEPTION_H
