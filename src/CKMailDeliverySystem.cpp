/*
 * CKMailDeliverySystem.cpp - this file implements the class that is used as the
 *                            main mail delivery conduit for all applications
 *                            wishing to email from within their own process
 *                            space. The user tells this class the default
 *                            delivery mechanism and then the user can send
 *                            messages through that channel, or another that
 *                            they specify on the calling method.
 *
 *                            The idea is that a user would create the mail
 *                            delivery channel that suites them best, and then
 *                            set that as the default prior to sending any
 *                            messages. In practice, this works very well.
 *
 * $Id: CKMailDeliverySystem.cpp,v 1.8 2004/09/22 12:08:32 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKMailDeliverySystem.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * This is the static default delivery variable that the class methods
 * will deal with in sending the message. It assumes a simple default,
 * but that had better be replaced with something that can really deliver
 * mail as it surely can't.
 */
static CKMailDelivery		*cDefaultDelivery;


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that never needs to be called.
 * This class functions entirely on class methods that are invoked
 * directly on this class without having to create an instance.
 */
CKMailDeliverySystem::CKMailDeliverySystem()
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKMailDeliverySystem::CKMailDeliverySystem( const CKMailDeliverySystem & anOther )
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKMailDeliverySystem::~CKMailDeliverySystem()
{
	// we have nothing, so there's nothing to release
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKMailDeliverySystem & CKMailDeliverySystem::operator=( const CKMailDeliverySystem & anOther )
{
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the default delivery channel for this
 * delivery system. By setting this, all messages delivered
 * through this interface will use this channel for their
 * delivery. The memory management of the delivery channel
 * is the responsibility of the caller, as they had to have
 * created it, and they need to make sure that it's going to
 * be around as long as this class is using it.
 */
void CKMailDeliverySystem::setDefaultDelivery( CKMailDelivery *aChannel )
{
	cDefaultDelivery = aChannel;
}


/*
 * This method gets the default delivery that has been previously
 * set by the user. If this is not properly set, then things are
 * going to get mightly dicey...
 */
CKMailDelivery *CKMailDeliverySystem::getDefaultDelivery()
{
	return cDefaultDelivery;
}


/********************************************************
 *
 *                Delivery Methods
 *
 ********************************************************/
/*
 * These methods are the different ways that a message can be
 * sent through this interface. The most simplistic is using
 * the default channel and not worrying about the fancy features.
 * Then again, if you want private recipients and read receipt,
 * we can do that too.
 */
bool CKMailDeliverySystem::deliver( const CKMailMessage & aMsg, bool aReadReceipt )
{
	bool		error = false;

	CKStringList	list;
	error = !cDefaultDelivery->deliver( aMsg, aReadReceipt, list );

	return !error;
}


bool CKMailDeliverySystem::deliver( const CKMailMessage & aMsg, bool aReadReceipt,
									const CKStringList & aPvtRecipients )
{
	bool		error = false;

	error = !cDefaultDelivery->deliver( aMsg, aReadReceipt, aPvtRecipients );

	return !error;
}


bool CKMailDeliverySystem::deliver( const CKMailMessage & aMsg, bool aReadReceipt,
									const CKStringList & aPvtRecipients,
									CKMailDelivery & aChannel )
{
	bool		error = false;

	error = !aChannel.deliver( aMsg, aReadReceipt, aPvtRecipients );

	return !error;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKMailDeliverySystems are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailDeliverySystem::operator==( const CKMailDeliverySystem & anOther ) const
{
	return true;
}


/*
 * This method checks to see if the two CKMailDeliverySystems are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailDeliverySystem::operator!=( const CKMailDeliverySystem & anOther ) const
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
CKString CKMailDeliverySystem::toString() const
{
	return "<this object has no instance variables>";
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMailDeliverySystem & aSystem )
{
	aStream << aSystem.toString();

	return aStream;
}
