/*
 * CKSMTPConnection.h - this file defines the class that can be used as a
 *                      general SMTP connection for the user. The idea is that
 *                      it builds on the CKTCPConnection class but adds all
 *                      the functionality that is needed for general SMTP
 *                      communications. It's used in the Mail Delivery system
 *                      as one of the ways in which a message can be sent.
 *
 *                      While understanding of the SMTP protocol isn't really
 *                      a necessity for understanding this class, it certainly
 *                      helps in the terminology and flow of messages that
 *                      takes place during a connection. So, if you can, scan
 *                      the SMTP spec on the web.
 *
 * $Id: CKSMTPConnection.h,v 1.6 2004/09/16 09:34:18 drbob Exp $
 */
#ifndef __CKSMTPCONNECTION_H
#define __CKSMTPCONNECTION_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKTCPConnection.h"
#include "CKString.h"

//	Forward Declarations

//	Public Constants
/*
 * This is the default port that SMTP services take place on. It's not that
 * they can't be done elsewhere, but the standard is port 25, so we need to
 * have that as the default we'll be using.
 */
#define	DEFAULT_SMTP_PORT			25
#ifdef __linux__
/*
 * On Linux, there is no standard definition for the maximum length of
 * a file name. So, in order to make the code as transportable as possible
 * we'll adopt the Solaris/Darwin definition and use that.
 */
#define MAXHOSTNAMELEN			255
#endif

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKSMTPConnection :
	public CKTCPConnection
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that gets things started, but
		 * wse still need to set the host before we can make a connection.
		 */
		CKSMTPConnection();
		/*
		 * This form of the constructor is nice in that it takes the
		 * host name and tries to establish a successful connection to the
		 * SMTP service on that host before returning to the caller.
		 */
		CKSMTPConnection( const CKString & aHost );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKSMTPConnection( const CKSMTPConnection & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKSMTPConnection();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKSMTPConnection & operator=( const CKSMTPConnection & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/

		/********************************************************
		 *
		 *                Connection Methods
		 *
		 ********************************************************/
		/*
		 * This method connects to the SMTP server on the provided
		 * host and exchanges the initial messages that are necessary
		 * in order to get the communication underway properly.
		 */
		bool connectToHost( const CKString & aHost );
		/*
		 * In SMTP language, this is the "bye, and send" message that
		 * needs to be sent to the SMTP server in order to quit the
		 * communication and have the SMTP server send the message.
		 */
		bool quit();

		/********************************************************
		 *
		 *                Data (Message) Methods
		 *
		 ********************************************************/
		/*
		 * This is the SMTP command to close out the header
		 * information and start sending the message body data. It's
		 * not a perfectly clear method name, but it is rooted in
		 * the SMTP legacy, so someone familiar with SMTP will know
		 * how to use this class.
		 */
		int startData();
		/*
		 * This method is called when all the data for the
		 * message body has been sent and the message body
		 * can be closed out.
		 */
		int endData();

		/********************************************************
		 *
		 *                Easy Message Body Methods
		 *
		 ********************************************************/
		/*
		 * This method is the same as startData(), but it sounds more
		 * descriptive, and makes programming to the SMTP host "look"
		 * better.
		 */
		bool startMessageBody();
		/*
		 * This method is a more desriptive method for appending a
		 * string to the nody of the message under construction.
		 * It simply calls the other SMTP methods, but makes
		 * programming to the SMTP host "look" nicer.
		 */
		bool addToMessageBody( const CKString & aString );
		/*
		 * This more descriptive method simply calls endData(), but
		 * makes programming to the SMTP interface "look" nicer.
		 */
		bool closeMessageBody();

		/********************************************************
		 *
		 *                Message Header Control Methods
		 *
		 ********************************************************/
		/*
		 * This method sets the single origination address for the
		 * upcoming SMTP mail message. It's important to note that
		 * while rcptTo() can be called any number of times, this
		 * method can only be called once without error.
		 */
		int mailFrom( const CKString & aFromAddress );
		/*
		 * This adds the given address to the list of recipients to
		 * receive the upcoming SMTP mail message. This can be called
		 * any number of times, but must be done consecutively. This
		 * is a limitation of the SMTP protocol.
		 */
		int rcptTo( const CKString & aToAddress );

		/********************************************************
		 *
		 *           Easy Message Header Control Methods
		 *
		 ********************************************************/
		/*
		 * This more descriptive method allows the user to set the
		 * single sender address for the following SMTP message. It's
		 * important to note that while recipientAddress() can be
		 * called many times, this method can only be called once
		 * without error.
		 */
		bool senderAddress( const CKString & anAddress );
		/*
		 * This more descriptive method adds the given address to the
		 * list of recipients to receive the upcoming SMTP mail message.
		 * This can be called any number of times, but must be done
		 * consecutively. This is a limitation of the SMTP protocol.
		 */
		bool recipientAddress( const CKString & anAddress );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKSMTPConnections are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator==( const CKSMTPConnection & anOther ) const;
		/*
		 * This method checks to see if the two CKSMTPConnections are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKSMTPConnection & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	protected:
		/*
		 * This method is used in the processing of the returned messages
		 * to hold the state of the connection as returned from the SMTP
		 * server.
		 */
		void setState( int aState );

		/*
		 * This method returns the saved state of the SMTP connection as
		 * parsed from the latest data coming from the SMTP server.
		 */
		int getState() const;
		
		/*
		 * As part of the SMTP protocol, the client and the server
		 * say "hello" to one another and exchange such things as who
		 * they are, etc. This method does this "hello" interchange.
		 */
		int hello();

		/********************************************************
		 *
		 *            Direct Port Manipulation Methods
		 *
		 ********************************************************/
		/*
		 * This is a low-level method to send the specified command
		 * out the socket to the SMTP server. It calls send() to send
		 * the command to the SMTP server, and then gets the reply
		 * from the server via getReply(). This method also throws a
		 * CKException so that most reporting of errors can
		 * happen at this level.
		 */
		int sendCommand( const CKString & aCommand );
		/*
		 * This method gets the reply from the SMTP server through
		 * the socket and then updates the state of the connection
		 * with the return value code.
		 */
		int getReply();

		/********************************************************
		 *
		 *            SMTP Message Manipulation Methods
		 *
		 ********************************************************/
		/*
		 * When communicating with an SMTP host the return code is
		 * embedded in the byte string returning from the server. This
		 * three-character number needs to become an integer and
		 * stored in the state variable in this class ivar. The
		 * purpose of this routine is to extract the integer off the
		 * returned byte stream and ignore the rest.
		 *
		 * This routine throws CKExceptions in the event that a
		 * processing error occurs, and returns a -1.
		 */
		int grabSMTPReturnCodeOnData( const CKString & aData );
		/*
		 * There will be times that we need to "decode" the SMTP
		 * return code that is an integer into a human-readable
		 * string. This method does just that. It returns a String
		 * that corresponds to the SMTP error code passed in. If the
		 * argument does not correcepond to any SMTP error code, a
		 * descriptive error string is returned and a CKException
		 * is thrown.
		 */
		CKString stringForSMTPReturnCode( int aCode );
		/*
		 * This method simply calls getStatus() to get the
		 * last SMTP return value and then passes it to
		 * stringForSMTPReturnCode() to convert that to a string.
		 */
		CKString stringForLastSMTPReturnCode();

	private:
		/*
		 * This is the current state of the SMTP connection as determined
		 * by the remote SMTP server. It needs to be held as it's part of
		 * the reply string(s) from the remote server, and we may want to
		 * check it after the replys are processed.
		 */
		int		mState;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKSMTPConnection & aConnection );

#endif	// __CKSMTPCONNECTION_H
