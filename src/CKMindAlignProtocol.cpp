/*
 * CKMindAlignProtocol.cpp - this file implements the class that can be used as
 *                           a simple conduit to a MindAlign messaging server.
 *                           The basics of the IRC communication is handled
 *                           by the super class and you can register for all
 *                           incoming messages to be processed and return a
 *                           CKString as a reply. The specifics of the MindAlign
 *                           authentication scheme are handled in this class
 *                           and the other work is left to the super to deal
 *                           with. This is the core of the secure chat servers.
 *
 * $Id: CKMindAlignProtocol.cpp,v 1.2 2007/09/26 19:33:45 drbob Exp $
 */

//	System Headers
#include <iostream>
#include <sstream>
#include <unistd.h>
#ifdef GPP2
#include <algorithm>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKMindAlignProtocol.h"
#include "CKIRCProtocolListener.h"
#include "CKIRCResponder.h"
#include "CKException.h"
#include "CKStopwatch.h"
#include "CKStackLocker.h"

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
 * This is the default constructor that doesn't specify a
 * remote host or a port on which to communicate. While this
 * may be used in certain conditions, it is not the most
 * likely use of the class' constructor.
 */
CKMindAlignProtocol::CKMindAlignProtocol() :
	CKIRCProtocol(),
	mAuthHostname(),
	mAuthPort(DEFAULT_AUTH_PORT),
	mAuthComm(),
	mAuthCommMutex(),
	mToken()
{
}


/*
 * This method of the constructor takes the names of the
 * authentication server and messaging server as well as the
 * port numbers for each so that it can establish connections
 * to both necessary services.
 */
CKMindAlignProtocol::CKMindAlignProtocol( const CKString & anAuthHost, int anAuthPort,
										  const CKString & aMesgHost, int aMesgPort ) :
	CKIRCProtocol(),
	mAuthHostname(),
	mAuthPort(DEFAULT_AUTH_PORT),
	mAuthComm(),
	mAuthCommMutex(),
	mToken()
{
	// set the authentication server connection parameters
	setAuthHostname(anAuthHost);
	setAuthPort(anAuthPort);
	// now try to make the connection to the MindAlign *system*
	if (!connect(aMesgHost, aMesgPort)) {
		std::ostringstream	msg;
		msg << "CKMindAlignProtocol::CKMindAlignProtocol(const CKString &, int, "
			"const CKString &, int) - the connection to the MindAlign system "
			"with the authentication server at " << anAuthHost << ":"
			<< anAuthPort << " and the messaging server at " << aMesgHost << ":"
			<< aMesgPort << " could not be established. Please check to see "
			"that there is an authentication server and messaging server on "
			"these machines.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This form of the constructor is nice in that it not only connects
 * to the authentication server and obtains the authentication token
 * for the MindAlign messaging server, it sends all the necessary
 * commands to establish a solid connection to the messaging server,
 * and in so doing, allows the user to start sending messages right
 * away.
 */
CKMindAlignProtocol::CKMindAlignProtocol( const CKString & anAuthHost, int anAuthPort,
										  const CKString & aMesgHost, int aMesgPort,
										  const CKString & aNick, const CKString & aPassword )
{
	// set the nick and password for authentication
	setNickname(aNick);
	setPassword(aPassword);

	// set the authentication server connection parameters
	setAuthHostname(anAuthHost);
	setAuthPort(anAuthPort);

	// now try to make the connection to the MindAlign *system*
	if (!connect(aMesgHost, aMesgPort)) {
		std::ostringstream	msg;
		msg << "CKMindAlignProtocol::CKMindAlignProtocol(const CKString &, int, "
			"const CKString &, int, const CKString &, const CKString &) - the "
			"connection to the MindAlign system with the authentication server "
			"at " << anAuthHost << ":" << anAuthPort << " and the messaging "
			"server at " << aMesgHost << ":" << aMesgPort << " could not be "
			"established. Please check to see that there is an authentication "
			"server and messaging server on these machines. It's also possible "
			"that the user '" << aNick << "' with password '" << aPassword
			<< "' is invalid. Check on that too.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// finally, send the NICK and USER commands to get things going
	doUSER(getNickname(), getUserHost(), getUserServer(), getRealName());
	doNICK(getNickname());
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKMindAlignProtocol::CKMindAlignProtocol( const CKMindAlignProtocol & anOther )
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKMindAlignProtocol::~CKMindAlignProtocol()
{
	// disconnect from the authentication server... super does the rest
	disconnectAuthServer();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKMindAlignProtocol & CKMindAlignProtocol::operator=( const CKMindAlignProtocol & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		// let the super equate it's stuff
		CKIRCProtocol::operator=(anOther);

		// set everything that the other one has
		mAuthHostname = anOther.mAuthHostname;
		mAuthPort = anOther.mAuthPort;
		mAuthComm = anOther.mAuthComm;
		mToken = anOther.mToken;
	}
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method is the standard setter accessor method for
 * the authentication server host name that will be used in
 * all subsequent authentication connections. Because it makes
 * no sense to change the host name while connected to a host,
 * this method will throw a CKException if a connection is
 * already established to a server.
 */
void CKMindAlignProtocol::setAuthHostname( const CKString & aHost )
{
	mAuthHostname = aHost;
}


/*
 * This method is the setter for the port number that will
 * be used to establish a communication port with
 * the authentication server. Typically, this should be left
 * as the default, but on certain rare occaisions, some
 * authentication hosts may choose to listen for communications
 * on a non-standard port.
 */
void CKMindAlignProtocol::setAuthPort( int aPort )
{
	mAuthPort = aPort;
}


/*
 * This method is typically called by the other methods in this
 * class when a valid authentication token has been received from
 * the authentication server. However, if you want to set it to
 * something else, good luck.
 */
void CKMindAlignProtocol::setToken( const CKString & aToken )
{
	mToken = aToken;
}


/*
 * Because the MindAlign Protocol is based on the CKTelnetConnection,
 * we need to have a sense of the host with which we are
 * communicating - and which host it is. This method is the
 * standard getter accessor method for the authentication host
 * name that will be used in all subsequent connections.
 */
const CKString & CKMindAlignProtocol::getAuthHostname() const
{
	return mAuthHostname;
}


/*
 * This method is the getter for the port number that will
 * be used to establish a communication port with
 * the authentication server. Typically, this is the default,
 * but on certain rare occaisions, some authentication server
 * may choose to listen for communications on a non-standard port.
 */
int CKMindAlignProtocol::getAuthPort() const
{
	return mAuthPort;
}


/*
 * This method returns the token that has been obtained from the
 * authentication server for the nickname and password that have
 * been given to this class. If that exchange has not been done,
 * then the returned string will simply be empty.
 */
const CKString & CKMindAlignProtocol::getToken() const
{
	return mToken;
}


/*
 * The MindAlign Protocol maintains a CKTelnetConnection to the
 * authentication server, and because it's nice to maintain
 * encapsulation, we're going to return a pointer to it as
 * opposed to a copy of it. The big reason is that I'd like to
 * use the getter in the methods and not break encapsulation and
 * use the ivar itself. However, please note that this pointer
 * is to the existing CKTelnetConnection and you should not
 * free it or mess with it, but feel free to make a copy, if
 * you so choose.
 */
const CKTelnetConnection *CKMindAlignProtocol::getAuthComm() const
{
	return & mAuthComm;
}


/********************************************************
 *
 *                Connection Methods
 *
 ********************************************************/
/*
 * This method allows both the host name and port number to
 * be specified for making the connection. This is used by
 * the other connection mathods as it is the most general
 * form of the function.
 */
bool CKMindAlignProtocol::connect( const CKString & aHost, int aPort )
{
	bool		error = false;

	// first, see if we are already connected to some host
	if (!error) {
		if (isConnected() && ((getHostname() != aHost) || (getPort() != aPort))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connect(const CKString & , int) - there's an "
				"established connection to the server on " << getHostname() << ":" <<
				getPort() << " and that connection needs to be closed before we can "
				"connect to another host and/or port. Please call disconnect().";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now create the connection to the authentication server
	if (!error) {
		if (!connectAuthServer()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connect(const CKString & , int) - we "
				"could not connect to the authentication server at " <<
				mAuthHostname << ":" << mAuthPort << " and that means we can't "
				"connect to the messaging server. Please check on this as soon "
				"as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, let's get the authentication token for this user
	if (!error) {
		mToken = obtainToken(getNickname(), getPassword());
		if (mToken.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connect(const CKString & , int) - the "
				"authentication server at " << mAuthHostname << ":"
				<< mAuthPort << " did not return an authentication token for "
				"the nick '" << getNickname() << "' and password '"
				<< getPassword() << "'. Please check on this as this login may "
				"not be valid.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, try to get the connection to the messaging server going
	if (!error) {
		if (!CKIRCProtocol::connect(aHost, aPort)) {
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connect(const CKString & , int) - a "
				"connection to the messaging server at " << aHost << ":"
				<< aPort << " could not be established. Please check the logs "
				"for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// send the AUTH message as that's part of it
			doAUTH(mToken);
		}
	}

	return !error;
}


/*
 * This method will log out any logged in user and break the
 * established connection to the IRC host. This is useful
 * when "shutting down" as it takes care of all the
 * possibilities in one fell swoop.
 */
void CKMindAlignProtocol::disconnect()
{
	// first, disconnect from the authentication server
	disconnectAuthServer();
	// now handle the connection to the IRC server itself.
	CKIRCProtocol::disconnect();
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKMindAlignProtocols are
 * equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKMindAlignProtocol::operator==( const CKMindAlignProtocol & anOther ) const
{
	bool		equal = true;

	if (CKIRCProtocol::operator!=(anOther) ||
		(mAuthHostname != anOther.mAuthHostname) ||
		(mAuthPort != anOther.mAuthPort) ||
		(mAuthComm != anOther.mAuthComm) ||
		(mToken != anOther.mToken)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKMindAlignProtocols are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKMindAlignProtocol::operator!=( const CKMindAlignProtocol & anOther ) const
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
CKString CKMindAlignProtocol::toString() const
{
	/*
	 * I haven't decided what I want to do with this, but rather
	 * than remove it and let the IRC version always win, I'll put
	 * this as a placeholder so that in the future when I do decide
	 * what I want to do the change will be easier.
	 */
	return CKIRCProtocol::toString();
}


/*
 * This method is used to copy the parameters for the given connection
 * to the connection that this instance is maintaining for all
 * communications with the authentication server. It's important to
 * note that this is a copy operation and not an assignment as you
 * might think.
 */
void CKMindAlignProtocol::setAuthComm( const CKTelnetConnection & aConn )
{
	// first, see if we're currently connected to some host
	if (isConnectedToAuthServer()) {
		std::ostringstream	msg;
		msg << "CKMindAlignProtocol::setAuthComm(const CKTelnetConnection &) - "
			"there's an established connection to the authentication server on "
			<< getAuthHostname() << ":" << getAuthPort() << " through the "
			"existing connection port and that connection needs to be closed "
			"before we can change the port parameters. Please call disconnect().";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// OK... we're clear to copy over the parameters
	mAuthComm = aConn;
}


/********************************************************
 *
 *            Authentication Server Methods
 *
 ********************************************************/
/*
 * This method uses the hostname and port number contained
 * within the instance to make a connection to the authentication
 * server. If successful, the method returns true, othersize it
 * returns false. If there is an unusual condition, a CKException
 * is thrown.
 */
bool CKMindAlignProtocol::connectAuthServer()
{
	return connectAuthServer(mAuthHostname, mAuthPort);
}


/*
 * This method uses the instance's port number, and the host name
 * provided to establish a connection to the authentication
 * server. This is very common because the default port is the
 * standard MindAlign authentication protocol port and so the
 * user really only needs to specify the host and most
 * connections will be made.
 */
bool CKMindAlignProtocol::connectAuthServer( const CKString & aHost )
{
	return connectAuthServer(aHost, mAuthPort);
}


/*
 * This method allows both the host name and port number to
 * be specified for making the connection. This is used by
 * the other connection mathods as it is the most general
 * form of the function.
 */
bool CKMindAlignProtocol::connectAuthServer( const CKString & aHost, int aPort )
{
	bool		error = false;

	// first, see if we are already connected to some host
	if (!error) {
		if (isConnectedToAuthServer() &&
			((mAuthHostname != aHost) || (mAuthPort != aPort))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"there's an established connection to the authentication server "
				"on " << mAuthHostname << ":" << mAuthPort << " and that connection "
				"needs to be closed before we can connect to another host and/or "
				"port. Please call disconnectAuthServer().";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, tell the connection object to connect to the right host and port
	if (!error) {
		// lock up the port
		CKStackLocker	lockem(&mAuthCommMutex);
		// try to make the connection
		if (!mAuthComm.connect(aHost, aPort)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the connection to the authentication server on " << aHost << ":" <<
				aPort << " could not be created and that's a serious problem. "
				"Please make sure that there's an authentication server on that "
				"box.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			/*
			 * The connection was good, so let's set the read timeout to
			 * the default value which is understandably short
			 */
			mAuthComm.setReadTimeout(DEFAULT_IRC_READ_TIMEOUT);
		}
	}

	/*
	 * OK, let's do the initial handshaking with the authentication
	 * server to make sure they know what we're doing.
	 */
	if (!error) {
		// build up the handshake message to send to the server
		CKString		cmd = AUTH_HANDSHAKE;
		cmd.append("\n");
		// now lock the connection down and send the message
		CKStackLocker	lockem(&mAuthCommMutex);
		if (!mAuthComm.send(cmd)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server on " << aHost << ":" << aPort
				<< " could not be sent the handshaking code. Please make sure "
				"that the server is OK.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get the response and clean it up
			CKString	answer = mAuthComm.readUpToNEWLINE();
			answer.trim();
			// see if it matches the handshake
			if (answer != AUTH_HANDSHAKE) {
				error = true;
				std::ostringstream	msg;
				msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
					"the authentication server on " << aHost << ":" << aPort
					<< " sent back something (" << answer << ") that doesn't "
					"match the handshaking code I sent (" << AUTH_HANDSHAKE
					<< "). Please make sure that the server is OK.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// if we're error-free, save the connection information
	if (!error) {
		// save the host and port for later
		mAuthHostname = aHost;
		mAuthPort = aPort;
	}

	return !error;
}


/*
 * This method simply returns the state of the authentication
 * server communications port - in so far as it's establishment.
 */
bool CKMindAlignProtocol::isConnectedToAuthServer()
{
	bool	conn = false;
	CKStackLocker	lockem(&mAuthCommMutex);
	conn = mAuthComm.isConnected();
	return conn;
}


/*
 * This method will break any connection with the authentication
 * server that might have been established.
 */
void CKMindAlignProtocol::disconnectAuthServer()
{
	// now handle the connection to the IRC server itself.
	if (isConnectedToAuthServer()) {
		CKStackLocker	lockem(&mAuthCommMutex);
		mAuthComm.disconnect();
	}
}


/*
 * This method is used to ask the already connected authentication
 * server what the authentication token is for the supplied user
 * and password. The returned value is the token itself.
 */
CKString CKMindAlignProtocol::obtainToken( const CKString & aUser, const CKString & aPassword )
{
	return obtainToken(&mAuthComm, aUser, aPassword);
}


/*
 * This method is used to ask the provided authentication server
 * connection what the authentication token is for the supplied user
 * and password. The returned value is the token itself.
 */
CKString CKMindAlignProtocol::obtainToken( CKTelnetConnection *aConn, const CKString & aUser, const CKString & aPassword )
{
	bool		error = false;

	// first, check to see if we have a valid connection
	if (!error) {
		if (aConn == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::obtainToken(const CKString &, const CKString &) - "
				"the provided connection is NULL and that means that there's "
				"nothing for me to do. Please make sure that the connection is "
				"valid before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, make sure we have a decent username and password
	if (!error) {
		if (aUser.empty() || aPassword.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::obtainToken(const CKString &, const CKString &) - "
				"the provided username and/or password are empty and that's not "
				"going to be acceptable to the authentication server. Please "
				"make sure that the strings are valid before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, make sure the connection is OK
	if (!error) {
		if (!aConn->isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::obtainToken(const CKString &, const CKString &) - "
				"the provided telnet connection is not actually connected to "
				"anything at this time. Please make sure that it is before calling "
				"this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * OK, now we get into the good stuff. We need to take the password
	 * and encode it Base64 for sending to the authentication server.
	 * Then we need to send the authentication command to the server and
	 * get the response.
	 */
	CKString	info;
	if (!error) {
		// copy it because the encoding will change the value
		CKString	encoded(aPassword);
		encoded.convertToBase64();
		// make the command to send to the authentication server
		CKString	cmd = "LOGIN:";
		cmd.append(aUser).append(";SECRET:").append(encoded).append("\n");
		// now lock down the connection and send the command
		if (!aConn->send(cmd)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server could not be sent the login credentials. "
				"Please make sure that the server is OK.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// get the response and clean it up
			info = aConn->readUpToNEWLINE();
			info.trim();
		}
	}

	/*
	 * Now let's parse up this response we got back from the server
	 * and see what they have given us.
	 */
	CKString	uid;
	CKString	token;
	CKString	nick;
	CKString	firstName;
	CKString	lastName;
	if (!error) {
		CKStringList	chunks = CKStringList::parseIntoChunks(info, ";");
		if (chunks.size() < 5) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server responded to the login credentials "
				"with '" << info << "'. This does not have the 5 components that "
				"it should. Please check with them to see if the format changed.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			CKStringNode	*c = NULL;
			for (c = chunks.getHead(); c != NULL; c = c->getNext()) {
				if (c->left(4) == "UID:") {
					uid = c->substr(4);
				} else if (c->left(6) == "TOKEN:") {
					token = c->substr(6);
				} else if (c->left(5) == "NICK:") {
					nick = c->substr(5);
				} else if (c->left(3) == "FN:") {
					firstName = c->substr(3);
				} else if (c->left(3) == "LN:") {
					lastName = c->substr(3);
				}
			}
		}
	}

	/*
	 * Now let's verify that it's the same nickname we asked for.
	 * If so, the set the real nameof this guy and return the
	 * token.
	 */
	if (!error) {
		if (nick.empty()) {
			// clear out the token as the response
			token.clear();
			// flag the error and throw the exception
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server responded to the login credentials "
				"with '" << info << "' and the 'NICK' component was empty. "
				"That's bad news. Please check with the authentication folks.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else if (nick != aUser) {
			// clear out the token as the response
			token.clear();
			// flag the error and throw the exception
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server responded to the login credentials "
				"with '" << info << "' and the 'NICK' component was not what we "
				"sent (" << aUser << "). That's bad news. Please check with the "
				"authentication folks.";
		} else if (token.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKMindAlignProtocol::connectAuthServer(const CKString & , int) - "
				"the authentication server responded to the login credentials "
				"with '" << info << "' and the 'TOKEN' component was empty. "
				"That's bad news. Please check with the authentication folks.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// set the real name to what we got back
			firstName.append(" ").append(lastName);
			setRealName(firstName);
		}
	}

	return token;
}


/********************************************************
 *
 *            Generic IRC Commands - IRC style
 *
 ********************************************************/
/*
 * This executes the MindAlign 'AUTH' command on the communication
 * channel to the messaging host. This is meant to supply the requested
 * authentication token to the MindAlign server so that the server
 * knows who this is coming from.
 */
void CKMindAlignProtocol::doAUTH( const CKString & aToken )
{
	CKString		cmd = "AUTH ";
	cmd += aToken;
	executeCommand(cmd);
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMindAlignProtocol & aProtocol )
{
	aStream << aProtocol.toString();

	return aStream;
}
