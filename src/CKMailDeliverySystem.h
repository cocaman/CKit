/*
 * CKMailDeliverySystem.h - this file defines the class that is used as the
 *                          main mail delivery conduit for all applications
 *                          wishing to email from within their own process
 *                          space. The user tells this class the default
 *                          delivery mechanism and then the user can send
 *                          messages through that channel, or another that
 *                          they specify on the calling method.
 *
 *                          The idea is that a user would create the mail
 *                          delivery channel that suites them best, and then
 *                          set that as the default prior to sending any
 *                          messages. In practice, this works very well.
 *
 * $Id: CKMailDeliverySystem.h,v 1.6 2004/09/16 09:34:17 drbob Exp $
 */
#ifndef __CKMAILDELIVERYSYSTEM_H
#define __CKMAILDELIVERYSYSTEM_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKMailDelivery.h"
#include "CKString.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKMailDeliverySystem
{
	public:
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
		CKMailDeliverySystem();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKMailDeliverySystem( const CKMailDeliverySystem & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKMailDeliverySystem();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKMailDeliverySystem & operator=( const CKMailDeliverySystem & anOther );

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
		static void setDefaultDelivery( CKMailDelivery *aChannel );

		/*
		 * This method gets the default delivery that has been previously
		 * set by the user. If this is not properly set, then things are
		 * going to get mightly dicey...
		 */
		static CKMailDelivery *getDefaultDelivery();

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
		static bool deliver( const CKMailMessage & aMsg, bool aReadReceipt = false );
		static bool deliver( const CKMailMessage & aMsg, bool aReadReceipt,
							const std::vector<CKString> & aPvtRecipients );
		static bool deliver( const CKMailMessage & aMsg, bool aReadReceipt,
							const std::vector<CKString> & aPvtRecipients,
							CKMailDelivery & aChannel );

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
		bool operator==( const CKMailDeliverySystem & anOther ) const;
		/*
		 * This method checks to see if the two CKMailDeliverySystems are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKMailDeliverySystem & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMailDeliverySystem & aSystem );

#endif	// __CKMAILDELIVERYSYSTEM_H
