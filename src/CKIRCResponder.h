/*
 * CKIRCResponder.h - this file defines the class that must be subclassed for
 *                    any class wishing to respond to IRC messages. There's
 *                    really only one method that matters, and that's the
 *                    respondToIRCMessage(CKIRCIncomingMessage &) and the
 *                    user really only needs to subclass off this class and
 *                    then write that one method.
 *
 * $Id: CKIRCResponder.h,v 1.7 2004/09/20 16:19:35 drbob Exp $
 */
#ifndef __CKIRCRESPONDER_H
#define __CKIRCRESPONDER_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKIRCProtocol.h"
#include "CKString.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKIRCResponder
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * The base the constructor does almost nothing other than to
		 * exist. There's nothing to it, really.
		 */
		CKIRCResponder();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKIRCResponder( const CKIRCResponder & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKIRCResponder();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKIRCResponder & operator=( const CKIRCResponder & anOther );

		/********************************************************
		 *
		 *             IRC Processing Methods
		 *
		 ********************************************************/
		/*
		 * This method is called when the thread is started and it's
		 * within this method that all the main processing of the listener
		 * is really done.
		 */
		virtual void respondToIRCMessage( CKIRCIncomingMessage & aMsg );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKIRCResponders are
		 * equal to one another based on the values they represent and *not*
		 * on the actual pointers themselves. If they are equal, then this
		 * method returns true, otherwise it returns false.
		 */
		bool operator==( const CKIRCResponder & anOther ) const;
		/*
		 * This method checks to see if the two CKIRCResponders are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKIRCResponder & anOther ) const;
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
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCResponder & aResponder );

#endif	// __CKIRCRESPONDER_H
