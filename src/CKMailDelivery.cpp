/*
 * CKMailDelivery.cpp - this file defines the general class that is used as a
 *                      reference mail delivery channel class for the developers.
 *                      The idea is that it defines the methods that need to be
 *                      implemented and therefore sets the contract that needs
 *                      to be adhered to for all delivery channels.
 *
 * $Id: CKMailDelivery.cpp,v 1.2 2003/12/03 16:45:26 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKMailDelivery.h"
#include "CKException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that does absolutely nothing.
 * This class is really just abstract in the Java sense and needs
 * to have an implementation behind it.
 */
CKMailDelivery::CKMailDelivery()
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKMailDelivery::CKMailDelivery( const CKMailDelivery & anOther )
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKMailDelivery::~CKMailDelivery()
{
	// we own nothing, so there's nothing to release
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKMailDelivery & CKMailDelivery::operator=( const CKMailDelivery & anOther )
{
	return *this;
}

/********************************************************
 *
 *                Delivery Methods
 *
 ********************************************************/
/*
 * This method is used by the CKMailDeliverySystem to ensure that
 * no messages are sent to this delivery channel class when it isn't
 * ready to receive them. In the case of SMTP delivery, this method
 * is going to initialize an CKSMTPConnection and get all it's
 * internal resources lined up and checked out -- including the
 * connection to the host, by connecting to it to receive it's
 * greeting message. It will then 'idle' the CKSMTPConnection and
 * return success. If anything fails, it will clean up nicely and
 * return false.
 */
bool CKMailDelivery::readyToDeliverMessages()
{
	return false;
}


/*
 * This method is used by the CKMailDeliverySystem to actually send
 * the passed message out the delivery channel. In order to include
 * all possibilities that might take place, we include the flag for
 * asking for a read receipt, and including private recipients. In
 * the case of SMTP delivery, we use the CKSMTPConnection to connect
 * to the server (again), exchange greetings, send the 'boilerplate'
 * information (from, to, header), and then send the message body.
 * After successful completion, we send the message and close the
 * connection, and return success. If anything fails we throw an
 * exception through the AKExceptionHandler and return false.
 */
bool CKMailDelivery::deliver( const CKMailMessage & aMsg,
							  bool aReadReceipt,
							  const std::vector<std::string> & aPvtRecipients )
{
	return false;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKMailDeliverys are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailDelivery::operator==( const CKMailDelivery & anOther ) const
{
	return true;
}


/*
 * This method checks to see if the two CKMailDeliverys are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailDelivery::operator!=( const CKMailDelivery & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a std::string.
 */
std::string CKMailDelivery::toString() const
{
	return "<this class has nothing to display>";
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMailDelivery & aDelivery )
{
	aStream << aDelivery.toString();

	return aStream;
}
