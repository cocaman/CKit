/*
 * CKIRCResponder.cpp - this file implements the class that must be subclassed
 *                      for any class wishing to respond to IRC messages.
 *                      There's really only one method that matters, and that's
 *                      the respondToIRCMessage(CKIRCIncomingMessage &) and the
 *                      user really only needs to subclass off this class and
 *                      then write that one method.
 *
 * $Id: CKIRCResponder.cpp,v 1.7 2004/09/20 16:19:35 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKIRCResponder.h"

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
 * The base the constructor does almost nothing other than to
 * exist. There's nothing to it, really.
 */
CKIRCResponder::CKIRCResponder()
{
	// this guy is really the signature and the one method
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKIRCResponder::CKIRCResponder( const CKIRCResponder & anOther )
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKIRCResponder::~CKIRCResponder()
{
	// we hold nothing, so there's nothing to do
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKIRCResponder & CKIRCResponder::operator=( const CKIRCResponder & anOther )
{
	return *this;
}


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
void CKIRCResponder::respondToIRCMessage( CKIRCIncomingMessage & aMsg )
{
	/*
	 * For this simplistic version of the class, let's have a simplistic
	 * response. This guy simply returns the string "Got the message, Nick"
	 * where 'Nick' is the nickname of the person sending the message. It
	 * is just intended to show that it's working and that it knows who
	 * sent the message.
	 */
	aMsg.response = "Got the message, ";
	aMsg.response += aMsg.userNickname;
}


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
bool CKIRCResponder::operator==( const CKIRCResponder & anOther ) const
{
	// they are always equal
	return true;
}


/*
 * This method checks to see if the two CKIRCResponders are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKIRCResponder::operator!=( const CKIRCResponder & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKIRCResponder::toString() const
{
	return CKString("<there's nothing for this class to present>");
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCResponder & aResponder )
{
	aStream << aResponder.toString();
	
	return aStream;
}
