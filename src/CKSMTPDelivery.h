/*
 * CKSMTPDelivery.h - this file defines the class that can be used as a
 *                    general SMTP connection for the user. The idea is that
 *                    it builds on the CKTCPConnection class but adds all
 *                    the functionality that is needed for general SMTP
 *                    communications. It's used in the Mail Delivery system
 *                    as one of the ways in which a message can be sent.
 *
 * $Id: CKSMTPDelivery.h,v 1.6 2004/09/16 09:34:18 drbob Exp $
 */
#ifndef __CKSMTPDELIVERY_H
#define __CKSMTPDELIVERY_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKMailDelivery.h"
#include "CKSMTPConnection.h"
#include "CKString.h"

//	Forward Declarations

//	Public Constants
/*
 * This is the default SMTP host that this class will use. Sure, it's not
 * as flexible as picking on off the constructor - oh... wait... I have that
 * too! :) Really, this is just a convenience so that when this kit is built
 * for a location, the default can be put here and no one has to deal with
 * it.
 */
#define DEFAULT_SMTP_HOST		"smtp.comcast.net"
/*
 * Just in case the user doesn't want to specify the name of the person
 * logged in, we can have this as the default sender of the message just
 * to fill it in.
 */
#define DEFAULT_SENDER			"CKit App"

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKSMTPDelivery :
	public CKMailDelivery
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that assumes the default SMTP
		 * host and the default user login as the originator of the
		 * email.
		 */
		CKSMTPDelivery();
		/*
		 * This form of the constructor is nice in that it takes the
		 * host name and tries to establish a successful connection to the
		 * SMTP service on that host before returning to the caller.
		 */
		CKSMTPDelivery( const CKString & aHost );
		/*
		 * This form of the constructor is nice in that it takes the
		 * host name as well as the name to be the originator of the email
		 * and tries to establish a successful connection to the
		 * SMTP service on that host before returning to the caller.
		 */
		CKSMTPDelivery( const CKString & aHost, const CKString & aSender );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKSMTPDelivery( const CKSMTPDelivery & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKSMTPDelivery();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKSMTPDelivery & operator=( const CKSMTPDelivery & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/

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
		virtual bool readyToDeliverMessages();
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
		virtual bool deliver( const CKMailMessage & aMsg,
							  bool aReadReceipt,
							  const std::vector<CKString> & aPvtRecipients );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method is used to timestamp the messages sent to the SMTP
		 * server. The format is: EEE, d MMM yyyy HH:mm:ss z and it needs
		 * to be this in order to have the messages properly interpreted
		 * by the email clients.
		 */
		CKString getDateFormat() const;

		/*
		 * This method checks to see if the two CKSMTPDeliverys are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator==( const CKSMTPDelivery & anOther ) const;
		/*
		 * This method checks to see if the two CKSMTPDeliverys are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKSMTPDelivery & anOther ) const;
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
		/*
		 * This is the name of the host that we're assuming has the SMTP
		 * service running on it. If not, then we're in for a shock, but
		 * typically, this is going to be the default for a location and
		 * we won't have to worry too much about it.
		 */
		CKString			mHostname;
		/*
		 * This is the 'sender' of the messages through this SMTP channel.
		 * Typically, this is the logged in user, but it can be defaulted
		 * about anything, and that's a very interesting issue in open
		 * SMTP servers... spoofing the sender's address.
		 */
		CKString			mFromEMailAddress;
		/*
		 * This is the actual connection to the SMTP server that we'll be
		 * using to send the messages out through.
		 */
		CKSMTPConnection	mHostConnection;
};

#endif	// __CKSMTPDELIVERY_H
