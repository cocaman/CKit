/*
 * CKSMTPConnection.cpp - this file implements the class that can be used as a
 *                        general SMTP connection for the user. The idea is that
 *                        it builds on the CKTCPConnection class but adds all
 *                        the functionality that is needed for general SMTP
 *                        communications. It's used in the Mail Delivery system
 *                        as one of the ways in which a message can be sent.
 *
 *                        While understanding of the SMTP protocol isn't really
 *                        a necessity for understanding this class, it certainly
 *                        helps in the terminology and flow of messages that
 *                        takes place during a connection. So, if you can, scan
 *                        the SMTP spec on the web.
 *
 * $Id: CKSMTPConnection.cpp,v 1.5 2004/09/11 21:07:47 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <unistd.h>
#include <netdb.h>

//	Third-Party Headers

//	Other Headers
#include "CKSMTPConnection.h"
#include "CKException.h"

//	Forward Declarations

//	Private Constants
/*
 * There are several formatted messages to send to the SMTP server. For our
 * conveneince, I'm going to define them here and then simply use them in
 * the code.
 */
#define	SMTP_HELLO			"HELO "
#define SMTP_MAIL_FROM		"MAIL FROM:"
#define SMTP_RCPT_TO		"RCPT TO:"
#define SMTP_DATA			"DATA"
#define SMTP_DATA_END		"\r\n."
#define SMTP_QUIT			"QUIT"

//	Private Datatypes

//	Private Data Constants
/*
 * The SMTP protocol defines the error codes that a server will send to a
 * client, and these error codes can be borken into two groups - OK and Bad.
 * The highest numbered return code that is still considered "OK" is 399.
 * Anything above that and it's considered an error of some kind. So... let's
 * have that be a definition here and we'll use it in the implementation.
 */
#define SMTP_MAX_OK_RETURN_CODE		399


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that gets things started, but
 * wse still need to set the host before we can make a connection.
 */
CKSMTPConnection::CKSMTPConnection() :
	CKTCPConnection(),
	mState(0)
{
	setPort(DEFAULT_SMTP_PORT);
}


/*
 * This form of the constructor is nice in that it takes the
 * host name and tries to establish a successful connection to the
 * SMTP service on that host before returning to the caller.
 */
CKSMTPConnection::CKSMTPConnection( const std::string & aHost ) :
	CKTCPConnection(),
	mState(0)
{
	// let's try to make the connection based on this information
	if (!connectToHost(aHost)) {
		std::ostringstream	msg;
		msg << "CKSMTPConnection::CKSMTPConnection(const std::string &) - the "
			"SMTP connection to the host " << aHost << " could not be "
			"established. This is a serious problem. Please make sure that the "
			"remote service is ready to accept the connection.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKSMTPConnection::CKSMTPConnection( const CKSMTPConnection & anOther ) :
	CKTCPConnection(),
	mState(0)
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKSMTPConnection::~CKSMTPConnection()
{
	shutdownSocket();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKSMTPConnection & CKSMTPConnection::operator=( const CKSMTPConnection & anOther )
{
	// the super's '=' operator is good start for me
	CKTCPConnection::operator=(anOther);
	// ...and then add what I have
	setState(anOther.getState());

	return *this;
}


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
bool CKSMTPConnection::connectToHost( const std::string & aHost )
{
	bool		error = false;

	// first, see if we're connected - if so, don't do a thing
	if (!error) {
		if (isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::connectToHost(const std::string &) - "
				"there is already an established connection to the SMTP service "
				"on the host: " << getHostname() << ". Please disconnect from "
				"this host before making the new connection.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// let's try to make the connection to the SMTP service on the host
	if (!error) {
		if (!connect(aHost, DEFAULT_SMTP_PORT)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::connectToHost(const std::string &) - "
				"the connection to the SMPT service on the host " << aHost <<
				" could not be established. This is a serious problem. Please "
				"make sure that the remote service is ready to accept the "
				"connection.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// we need to get the 'hello' message from the server
	if (!error) {
		if (getReply() > SMTP_MAX_OK_RETURN_CODE) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::connectToHost(const std::string &) - "
				"the 'hello' reply from the SMTP service on " << aHost <<
				" returned the error: " << stringForLastSMTPReturnCode() <<
				". Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now we send our 'hello' to the server
	if (!error) {
		if (hello() > SMTP_MAX_OK_RETURN_CODE) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::connectToHost(const std::string &) - "
				"the reply from my 'hello' to the SMTP service on " << aHost <<
				" returned the error: " << stringForLastSMTPReturnCode() << ". "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * In SMTP language, this is the "bye, and send" message that
 * needs to be sent to the SMTP server in order to quit the
 * communication and have the SMTP server send the message.
 */
bool CKSMTPConnection::quit()
{
	bool		error = false;

	// first, send the SMTP Quit command
	if (!error) {
		if (sendCommand(SMTP_QUIT) > SMTP_MAX_OK_RETURN_CODE) {
			error = true;
			/*
			 * There's no need to throw an exception as we're
			 * going to be killing the communication anyway.
			 */
		}
	}

	// let's close the connection to the socket
	disconnect();

	// reset the state to 'indeterminate' for later requests.
	setState(-1);

	return !error;
}


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
int CKSMTPConnection::startData()
{
	int		retval = -1;

	retval = sendCommand(SMTP_DATA);
	if (retval > SMTP_MAX_OK_RETURN_CODE) {
		std::ostringstream	msg;
		msg << "CKSMTPConnection::startData() - the reply from my "
			"'start data' message to the SMTP service on " << getHostname() <<
			" returned the error: " << stringForLastSMTPReturnCode() <<
			". Please check into this as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


/*
 * This method is called when all the data for the
 * message body has been sent and the message body
 * can be closed out.
 */
int CKSMTPConnection::endData()
{
	int		retval = -1;

	retval = sendCommand(SMTP_DATA_END);
	if (retval > SMTP_MAX_OK_RETURN_CODE) {
		std::ostringstream	msg;
		msg << "CKSMTPConnection::endData() - the reply from my "
			"'end data' message to the SMTP service on " << getHostname() <<
			" returned the error: " << stringForLastSMTPReturnCode() <<
			". Please check into this as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


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
bool CKSMTPConnection::startMessageBody()
{
	return (startData() > SMTP_MAX_OK_RETURN_CODE ? false : true);
}


/*
 * This method is a more desriptive method for appending a
 * string to the nody of the message under construction.
 * It simply calls the other SMTP methods, but makes
 * programming to the SMTP host "look" nicer.
 */
bool CKSMTPConnection::addToMessageBody( const std::string & aString )
{
	return (send(aString) ? true : false);
}


/*
 * This more descriptive method simply calls endData(), but
 * makes programming to the SMTP interface "look" nicer.
 */
bool CKSMTPConnection::closeMessageBody()
{
	return (endData() > SMTP_MAX_OK_RETURN_CODE ? false : true);
}


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
int CKSMTPConnection::mailFrom( const std::string & aFromAddress )
{
	int		retval = -1;

	// build up the command that needs to be sent
	std::string		cmd = SMTP_MAIL_FROM;
	cmd += "<";
	cmd += aFromAddress;
	cmd += ">";
	// then send it and see if it was a success
	retval = sendCommand(cmd);
	if (retval > SMTP_MAX_OK_RETURN_CODE) {
		std::ostringstream	msg;
		msg << "CKSMTPConnection::mailFrom(const std::string &) - the reply "
			"from my 'mail from' message to the SMTP service on " << getHostname() <<
			" returned the error: " << stringForLastSMTPReturnCode() <<
			". Please check into this as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


/*
 * This adds the given address to the list of recipients to
 * receive the upcoming SMTP mail message. This can be called
 * any number of times, but must be done consecutively. This
 * is a limitation of the SMTP protocol.
 */
int CKSMTPConnection::rcptTo( const std::string & aToAddress )
{
	int		retval = -1;

	// build up the command that needs to be sent
	std::string		cmd = SMTP_RCPT_TO;
	cmd += "<";
	cmd += aToAddress;
	cmd += ">";
	// then send it and see if it was a success
	retval = sendCommand(cmd);
	if (retval > SMTP_MAX_OK_RETURN_CODE) {
		std::ostringstream	msg;
		msg << "CKSMTPConnection::rcptTo(const std::string &) - the reply "
			"from my 'rcpt to' message to the SMTP service on " << getHostname() <<
			" returned the error: " << stringForLastSMTPReturnCode() <<
			". Please check into this as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


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
bool CKSMTPConnection::senderAddress( const std::string & anAddress )
{
	return (mailFrom(anAddress) > SMTP_MAX_OK_RETURN_CODE ? false : true);
}


/*
 * This more descriptive method adds the given address to the
 * list of recipients to receive the upcoming SMTP mail message.
 * This can be called any number of times, but must be done
 * consecutively. This is a limitation of the SMTP protocol.
 */
bool CKSMTPConnection::recipientAddress( const std::string & anAddress )
{
	return (rcptTo(anAddress) > SMTP_MAX_OK_RETURN_CODE ? false : true);
}


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
bool CKSMTPConnection::operator==( const CKSMTPConnection & anOther ) const
{
	bool		equal = true;

	if (!CKTCPConnection::operator==(anOther) ||
		(getState() != anOther.getState())) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKSMTPConnections are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKSMTPConnection::operator!=( const CKSMTPConnection & anOther ) const
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
std::string CKSMTPConnection::toString() const
{
	std::ostringstream	buff;

	buff << "< Socket=" << CKTCPConnection::toString() << ", " <<
		" State=" << getState() << ">" << std::endl;

	return buff.str();
}


/*
 * This method is used in the processing of the returned messages
 * to hold the state of the connection as returned from the SMTP
 * server.
 */
void CKSMTPConnection::setState( int aState )
{
	mState = aState;
}


/*
 * This method returns the saved state of the SMTP connection as
 * parsed from the latest data coming from the SMTP server.
 */
int CKSMTPConnection::getState() const
{
	return mState;
}


/*
 * As part of the SMTP protocol, the client and the server
 * say "hello" to one another and exchange such things as who
 * they are, etc. This method does this "hello" interchange.
 */
int CKSMTPConnection::hello()
{
	bool		error = false;
	int			retval = -1;

	// first, get the name of this host as we'll have to have it
	char	host[MAXHOSTNAMELEN + 1];
	if (!error) {
		if (gethostname(host, MAXHOSTNAMELEN) != 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::hello() - the hostname of this machine "
				"could not be determined. This is a serious problem as we can't "
				"communicate with the SMTP server if we don't tell it who we are. "
				"Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, send it in the 'hello' message to the SMTP server
	if (!error) {
		// build up the message that needs to be sent
		std::string		cmd = SMTP_HELLO;
		cmd += host;
		// ...and then send it to the SMTP server
		retval = sendCommand(cmd);
		if (retval > SMTP_MAX_OK_RETURN_CODE) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::hello() - the reply from my "
				"'hello' message to the SMTP service on " << getHostname() <<
				" returned the error: " << stringForLastSMTPReturnCode() <<
				". Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


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
int CKSMTPConnection::sendCommand( const std::string & aCommand )
{
	bool		error = false;
	int			retval = -1;

	// build up the command to send to the SMTP server
	std::string cmd = aCommand;
	cmd += "\r\n";
	if (!error) {
		error = !send(cmd);
		if (error) {
			std::ostringstream	msg;
			msg << "CKSMTPConnection::sendCommand(const std::string &) - the "
				"command: '" << aCommand << "' could not be sent to the SMTP "
				"service on " << getHostname() << ". Please check into this as "
				"soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// try to receive the acknowledge back
	if (!error) {
		retval = getReply();
		if (retval > SMTP_MAX_OK_RETURN_CODE) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::sendCommand(const std::string &) - the "
				"command: '" << aCommand << "' when sent to the SMTP service "
				"on " << getHostname() << " returned the error: " <<
				stringForLastSMTPReturnCode() << ". Please check into this as "
				"soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return error ? -1 : retval;
}


/*
 * This method gets the reply from the SMTP server through
 * the socket and then updates the state of the connection
 * with the return value code.
 */
int CKSMTPConnection::getReply()
{
	bool		error = false;
	int			retval = -1;

	// first, get the data up to the CRLF as it's the terminator
	std::string		data;
	if (!error) {
		data = readUpToCRLF();
		if (data.size() == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::getReply() - there was no reply from the "
				"SMTP service on " << getHostname() << ". This is a serious "
				"problem as there should have been something.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now grab the return code on the data returned
	if (!error) {
		retval = grabSMTPReturnCodeOnData(data);
		if (retval > 0) {
			setState(retval);
		}
	}

	return error ? -1 : retval;
}


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
int CKSMTPConnection::grabSMTPReturnCodeOnData( const std::string & aData )
{
	bool		error = false;
	int			retval = -1;

	// first, make sure there's enough there to look at
	if (!error) {
		if (aData.size() < 5) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::grabSMTPReturnCodeOnData(const std::string &) "
				"- the size of the SMTP reply data is too small to include a "
				"return code: '" << aData << "'. This is a serious problem that "
				"needs to be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now get the value itself
	if (!error) {
		char	h = aData[0];
		char	t = aData[1];
		char	o = aData[2];
		if ((('0' <= h) && (h <= '9')) &&
			(('0' <= t) && (t <= '9')) &&
			(('0' <= o) && (o <= '9'))) {
			retval = (h - '0')*100 + (t - '0')*10 + (o - '0');
			if ((retval < 211) || (retval > 554)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKSMTPConnection::grabSMTPReturnCodeOnData(const std::string &) "
					"- the reply data: '" << aData << "' did not have a valid "
					"SMTP reply code (between 211 and 554) as the first thing in "
					"the data. This is a serious data corruption problem that "
					"needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} else {
			error = true;
			std::ostringstream	msg;
			msg << "CKSMTPConnection::grabSMTPReturnCodeOnData(const std::string &) "
					"- the reply data: '" << aData << "' did not have a three "
					"digit SMTP reply code as the first thing in the data. This "
					"is a serious data corruption problem that needs to be "
					"looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return error ? -1 : retval;
}


/*
 * There will be times that we need to "decode" the SMTP
 * return code that is an integer into a human-readable
 * string. This method does just that. It returns a String
 * that corresponds to the SMTP error code passed in. If the
 * argument does not correcepond to any SMTP error code, a
 * descriptive error string is returned and a CKException
 * is thrown.
 */
std::string CKSMTPConnection::stringForSMTPReturnCode( int aCode )
{
	std::string		retval;

	switch (aCode) {
		case 211 :
			retval = "System status, or system help reply";
			break;
		case 214 :
			retval = "Help message [Information on how to use the receiver or "
					"the meaning of a particular non-standard command; this reply "
					"is useful only to the human user]";
			break;
		case 220 :
			retval = "Service ready";
			break;
		case 221 :
			retval = "Service closing transmission channel";
			break;
		case 250 :
			retval = "Requested mail action okay, completed";
			break;
		case 251 :
			retval = "User not local; will forward to";
			break;

		case 354 :
			retval = "Start mail input; end with .";
			break;

		case 421 :
			retval = "Service not available, closing transmission channel [This "
					"may be a reply to any command if the service knows it must "
					"shut down]";
			break;
		case 450 :
			retval = "Requested mail action not taken: mailbox unavailable "
					"[E.g., mailbox busy]";
			break;
		case 451 :
			retval = "Requested action aborted: local error in processing";
			break;
		case 452 :
			retval = "Requested action not taken: insufficient system storage";
			break;

		case 500 :
			retval = "Syntax error, command unrecognized [This may include "
					"errors such as command line too long]";
			break;
		case 501 :
			retval = "Syntax error in parameters or arguments";
			break;
		case 502 :
			retval = "Command not implemented";
			break;
		case 503 :
			retval = "Bad sequence of commands";
			break;
		case 504 :
			retval = "Command parameter not implemented";
			break;
		case 550 :
			retval = "Requested action not taken: mailbox unavailable [E.g., "
					"mailbox not found, no access]";
			break;
		case 551 :
			retval = "User not local; please try";
			break;
		case 552 :
			retval = "Requested mail action aborted: exceeded storage "
					"allocation";
			break;
		case 553 :
			retval = "Requested action not taken: mailbox name not allowed "
					"[E.g., mailbox syntax incorrect]";
			break;
		case 554 :
			retval = "Transaction failed";
			break;

		default :
			std::ostringstream	msg;
			msg << "The code:" << aCode << " is NOT a valid SMTP server return "
					"code, as defined in the document RFC-821.";
			retval = msg.str();
			break;
	}

	return retval;
}


/*
 * This method simply calls getStatus() to get the
 * last SMTP return value and then passes it to
 * stringForSMTPReturnCode() to convert that to a string.
 */
std::string CKSMTPConnection::stringForLastSMTPReturnCode()
{
	std::string		retval = "The SMTP server is in an indeterminate state, and "
							"does not have a valid return code.";
	if (getState() > 0) {
		retval = stringForSMTPReturnCode(getState());
	}

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKSMTPConnection & aConnection )
{
	aStream << aConnection.toString();

	return aStream;
}
