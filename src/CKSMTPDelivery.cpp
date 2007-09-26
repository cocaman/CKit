/*
 * CKSMTPDelivery.cpp - this file implements the class that can be used as a
 *                      general SMTP connection for the user. The idea is that
 *                      it builds on the CKTCPConnection class but adds all
 *                      the functionality that is needed for general SMTP
 *                      communications. It's used in the Mail Delivery system
 *                      as one of the ways in which a message can be sent.
 *
 * $Id: CKSMTPDelivery.cpp,v 1.10 2007/09/26 19:33:46 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <stdio.h>

//	Third-Party Headers

//	Other Headers
#include "CKSMTPDelivery.h"
#include "CKException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * When I'm converting the date to a nice, readable format for the SMTP
 * server, it's going to be a lot easier if I have these arrays defined
 * and so I can use them to look up the textual values of things given
 * only the numerical values that C like to give us.
 */
char *wdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
					"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


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
CKSMTPDelivery::CKSMTPDelivery() :
	mHostname(DEFAULT_SMTP_HOST),
	mFromEMailAddress(DEFAULT_SENDER),
	mHostConnection(DEFAULT_SMTP_HOST)
{
	// nothing to do as the initializers do it all
}


/*
 * This form of the constructor is nice in that it takes the
 * host name and tries to establish a successful connection to the
 * SMTP service on that host before returning to the caller.
 */
CKSMTPDelivery::CKSMTPDelivery( const CKString & aHost ) :
	mHostname(aHost),
	mFromEMailAddress(DEFAULT_SENDER),
	mHostConnection(aHost)
{
	// nothing to do as the initializers do it all
}


/*
 * This form of the constructor is nice in that it takes the
 * host name as well as the name to be the originator of the email
 * and tries to establish a successful connection to the
 * SMTP service on that host before returning to the caller.
 */
CKSMTPDelivery::CKSMTPDelivery( const CKString & aHost, const CKString & aSender ) :
	mHostname(aHost),
	mFromEMailAddress(aSender),
	mHostConnection(aHost)
{
	// nothing to do as the initializers do it all
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKSMTPDelivery::CKSMTPDelivery( const CKSMTPDelivery & anOther ) :
	mHostname(anOther.mHostname),
	mFromEMailAddress(anOther.mFromEMailAddress),
	mHostConnection(anOther.mHostname)
{
	// nothing to do as the initializers do it all
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKSMTPDelivery::~CKSMTPDelivery()
{
	// disconnect from the server
	mHostConnection.disconnect();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKSMTPDelivery & CKSMTPDelivery::operator=( const CKSMTPDelivery & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		mHostname = anOther.mHostname;
		mFromEMailAddress = anOther.mFromEMailAddress;
		mHostConnection = anOther.mHostConnection;
	}
	return *this;
}


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
bool CKSMTPDelivery::readyToDeliverMessages()
{
	bool		error = false;

	/*
	 * See if we can connect to the SMTP host that we're pointing
	 * at
	 */
	if (!error) {
		if (!mHostConnection.isConnected()) {
			if (!mHostConnection.connectToHost(mHostname)) {
				// couldn't make the connection
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPDelivery::readyToDeliverMessages() - there was an "
					"error while trying to connect to the SMTP server on " <<
					mHostname << ". Please make sure it's there and available.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// good connection, so now disconnect
				mHostConnection.disconnect();
			}
		}
	}

	return !error;
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
bool CKSMTPDelivery::deliver( const CKMailMessage & aMsg,
							  bool aReadReceipt,
							  const CKStringList & aPvtRecipients )
{
	bool		error = false;

	// first, make sure we're ready to deliver the message
	if (!error) {
		try {
			if (!readyToDeliverMessages()) {
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
					"bool, const CKStringList &) - there was an "
					"error while trying to check the connection to the SMTP "
					"server on " << mHostname << ". Please make sure it's "
					"there and available.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} catch (CKException & e) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - there was an "
				"error while trying to verify the connection to the SMTP server "
				"on " << mHostname << ". Please make sure it's there and "
				"available.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// let's connect to the host, if we aren't already connected
	if (!error) {
		if (!mHostConnection.isConnected()) {
			if (!mHostConnection.connectToHost(mHostname)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
					"bool, const CKStringList &) - there was an "
					"error while trying to connect to the SMTP server on " <<
					mHostname << ". Please make sure it's there and available.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// now we need to send the sender's address for this message
	if (!error) {
		if (!mHostConnection.senderAddress(mFromEMailAddress)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - there was an "
				"error while trying to set the sender's email address with the "
				"SMTP server on " << mHostname << ". Please make sure it's "
				"there and available.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to send the recipient address list for
	 * this message and at the same time, build up the mail
	 * header component that is a list of all the public
	 * recipients.
	 */
	CKString		publicRecipientList = "To:";
	if (!error) {
		CKStringNode		*i = NULL;
		for (i = aMsg.getRecipients()->getHead(); i != NULL; i = i->getNext()) {
			if (!mHostConnection.recipientAddress(*i)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
					"bool, const CKStringList &) - while trying to "
					"add the public recipient '" << (*i) << "' to the list for this message "
					"an error occurred. Please check into it as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// add this guy to the list of public recipients
				publicRecipientList.append(" ").append(*i).append("\r\n");
			}
		}
	}

	/*
	 * Now we need to send the *private* recipient address
	 * list for this message. Note we don't add these to the
	 * publicRecipientList.
	 */
	if (!error) {
		CKStringNode		*i = NULL;
		for (i = aPvtRecipients.getHead(); i != NULL; i = i->getNext()) {
			if (!mHostConnection.recipientAddress(*i)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
					"bool, const CKStringList &) - while trying to "
					"add the private recipient '" << (*i) << "' to the list for this message "
					"an error occurred. Please check into it as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	/*
	 * Now we need to start the message body, create and send
	 * the message header, send the message, and close the
	 * message body. Since these are somewhat related, they
	 * are treated as a somewhat single unit.
	 */
	// Start the message body with the SMTP server
	if (!error) {
		if (!mHostConnection.startMessageBody()) {
			// Couldn't start the message header
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - while trying to "
				"start the message body for this message an error occurred at the "
				"SMTP server. Please check into it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	// Now format and send the message header to the SMTP server
	if (!error) {
		/*
		 * Format the message header (complex, but necessary)
		 * Print message header according to RFC 822:
		 * Return-path, Received, Date, From, Subject, Sender, To, cc
		 */
		CKString msgHeader;

		// First, indicate if it's a MIME message
		if (aMsg.isMIME()) {
			msgHeader.append("MIME-Version: 1.0\r\n");
		}

		// Next, put up the return path
		msgHeader.append("Return-Path: ").append(mFromEMailAddress).append("\r\n");
		msgHeader.append("X-Sender: ").append(mFromEMailAddress).append("\r\n");
		msgHeader.append("X-Mailer: CKit Mailer\r\n");

		// Indicate the client's date and time
		msgHeader.append("Date: ").append(getDateFormat()).append("\r\n");

		/*
		 * Now add the simple 'from', 'subject', and 'to'
		 * (built up already from the recipient list.)
		 */
		msgHeader.append("From: ").append(mFromEMailAddress).append("\r\n");
		msgHeader.append("Subject: ").append(aMsg.getSubject()).append("\r\n");
		msgHeader.append(publicRecipientList);

		/**
		 * Now try to send this header to the SMTP server
		 */
		// Try to send the message body
		if (!mHostConnection.addToMessageBody(msgHeader)) {
			// Couldn't send the message header
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - while trying to "
				"add the message header to this message an error occurred. "
				"Please check into it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	// Now send the message body to the SMTP server
	if (!error) {
		// Try to send the message body
		if (!mHostConnection.addToMessageBody(aMsg.getMessageBody())) {
			// Couldn't send the message body
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - while trying to "
				"add the message body to this message an error occurred. "
				"Please check into it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	// Now close out the message with the SMTP server
	if (!error) {
		if (!mHostConnection.closeMessageBody()) {
			// Couldn't end the message section
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - while trying to "
				"end the message section to this message an error occurred. "
				"Please check into it as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now we need to close the connection to the server
	if (mHostConnection.isConnected()) {
		if (!mHostConnection.quit()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPDelivery::deliverMessage(const CKMailMessage &, "
				"bool, const CKStringList &) - while trying to "
				"close the connection to the SMTP server and send the message "
				"on it's way, an error occurred. Please check into it as soon "
				"as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Return the success of this entire process...
	return !error;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method is used to timestamp the messages sent to the SMTP
 * server. The format is: EEE, d MMM yyyy HH:mm:ss and it needs
 * to be this in order to have the messages properly interpreted
 * by the email clients.
 */
CKString CKSMTPDelivery::getDateFormat() const
{
	CKString		retval;

	/*
	 * Get the time right now, and the format it into a reasonable
	 * struct for dealing with the individual components of the
	 * date.
	 */
	time_t		now_t = time(NULL);
	struct tm	now;
	localtime_r(&now_t, &now);
	/*
	 * Build up all the arguments in the date we need
	 */
	char buff[256];
	snprintf( buff, 255, "%s, %d %s %04d %02d:%02d:%02d",
				wdays[now.tm_wday],
				now.tm_mday,
				months[now.tm_mon],
				(now.tm_year + 1900),
				now.tm_hour, now.tm_min, now.tm_sec
			);
	retval = buff;

	return retval;
}


/*
 * This method checks to see if the two CKSMTPDeliverys are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKSMTPDelivery::operator==( const CKSMTPDelivery & anOther ) const
{
	bool		equal = true;

	if ((mHostname != anOther.mHostname) ||
		(mFromEMailAddress != anOther.mFromEMailAddress) ||
		(mHostConnection != anOther.mHostConnection)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKSMTPDeliverys are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKSMTPDelivery::operator!=( const CKSMTPDelivery & anOther ) const
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
CKString CKSMTPDelivery::toString() const
{
	CKString	retval = "< Host=";
	retval += mHostname;
	retval += ", ";
	retval += " Sender=";
	retval += mFromEMailAddress;
	retval += ", ";
	retval += " Connection=";
	retval +=  mHostConnection.toString();
	retval += ">\n";

	return retval;
}
