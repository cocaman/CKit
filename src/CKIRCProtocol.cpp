/*
 * CKIRCProtocol.cpp - this file implements the class that can be used as a
 *                     simple conduit to a simple IRC server. The basics of
 *                     the IRC communication is handled by this class and you
 *                     can register for all incoming messages to be processed
 *                     and return a std::string as a reply. This is the core
 *                     of the chat servers.
 *
 * $Id: CKIRCProtocol.cpp,v 1.7 2004/08/31 21:23:35 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <unistd.h>
#ifdef GPP2
#include <algorithm>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKIRCProtocol.h"
#include "CKIRCProtocolListener.h"
#include "CKIRCResponder.h"
#include "CKException.h"

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
CKIRCProtocol::CKIRCProtocol() :
	mHostname(),
	mPort(DEFAULT_IRC_PORT),
	mCommPort(),
	mIsLoggedIn(false),
	mPassword(),
	mNickname(DEFAULT_NICKNAME),
	mUserHost(DEFAULT_USER_HOST),
	mUserServer(DEFAULT_USER_SERVER),
	mRealName(DEFAULT_REALNAME),
	mChannelList(),
	mChannelListMutex(),
	mListener(NULL),
	mResponders(),
	mRespondersMutex()
{
	// create a listener for this guy
	CKIRCProtocolListener	*buddy = new CKIRCProtocolListener(this);
	if (buddy == NULL) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::CKIRCProtocol() - the Listener for this instance "
			"could not be created. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		setListener(buddy);
	}
}


/*
 * This method of the constructor takes a name of a server and the
 * port to communicate on for IRC messages. This is a very common
 * form of the constructor because it creates the protocol object
 * and connects to a specific IRC server.
 */
CKIRCProtocol::CKIRCProtocol( const std::string & aHost, int aPort ) :
	mHostname(),
	mPort(DEFAULT_IRC_PORT),
	mCommPort(),
	mIsLoggedIn(false),
	mPassword(),
	mNickname(DEFAULT_NICKNAME),
	mUserHost(DEFAULT_USER_HOST),
	mUserServer(DEFAULT_USER_SERVER),
	mRealName(DEFAULT_REALNAME),
	mChannelList(),
	mChannelListMutex(),
	mListener(NULL),
	mResponders(),
	mRespondersMutex()
{
	// create a listener for this guy
	CKIRCProtocolListener	*buddy = new CKIRCProtocolListener(this);
	if (buddy == NULL) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::CKIRCProtocol(const std::string &, int) - the "
			"Listener for this instance could not be created. This is a serious "
			"allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		setListener(buddy);
	}

	// now try to make the connection to the IRC server
	if (!connect(aHost, aPort)) {
		// free up what we have created so far
		setListener(NULL);
		// ...and then log the message and toss an exception
		std::ostringstream	msg;
		msg << "CKIRCProtocol::CKIRCProtocol(const std::string &, int) - the "
			"connection to the IRC server at " << aHost << ":" << aPort <<
			" could not be established. Please check to see that there is an IRC "
			"server on that machine at that port.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This form of the constructor is nice in that it not only connects
 * to the IRC server, it sends the necessary commands to establish a
 * solid connection to the server, and in so doing, allows the user
 * to start sending messages right away.
 */
CKIRCProtocol::CKIRCProtocol( const std::string & aHost, int aPort,
							  const std::string & aNick ) :
	mHostname(),
	mPort(DEFAULT_IRC_PORT),
	mCommPort(),
	mIsLoggedIn(false),
	mPassword(),
	mNickname(DEFAULT_NICKNAME),
	mUserHost(DEFAULT_USER_HOST),
	mUserServer(DEFAULT_USER_SERVER),
	mRealName(DEFAULT_REALNAME),
	mChannelList(),
	mChannelListMutex(),
	mListener(NULL),
	mResponders(),
	mRespondersMutex()
{
	// create a listener for this guy
	CKIRCProtocolListener	*buddy = new CKIRCProtocolListener(this);
	if (buddy == NULL) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::CKIRCProtocol(const std::string &, int, "
			"const std::string &) - the Listener for this instance could not be "
			"created. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		setListener(buddy);
	}

	// now try to make the connection to the IRC server
	if (!connect(aHost, aPort)) {
		// free up what we have created so far
		setListener(NULL);
		// ...and then log the message and toss an exception
		std::ostringstream	msg;
		msg << "CKIRCProtocol::CKIRCProtocol(const std::string &, int, "
			"const std::string &) - the connection to the IRC server at " <<
			aHost << ":" << aPort << " could not be established. Please check "
			"to see that there is an IRC server on that machine at that port.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// finally, send the NICK and USER commands to get things going
	doNICK(aNick);
	doUSER(aNick, getUserHost(), getUserServer(), aNick);
	// ...and set them accordingly
	setNickname(aNick);
	setRealName(aNick);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKIRCProtocol::CKIRCProtocol( const CKIRCProtocol & anOther ) :
	mHostname(),
	mPort(DEFAULT_IRC_PORT),
	mCommPort(),
	mIsLoggedIn(false),
	mPassword(),
	mNickname(DEFAULT_NICKNAME),
	mUserHost(DEFAULT_USER_HOST),
	mUserServer(DEFAULT_USER_SERVER),
	mRealName(DEFAULT_REALNAME),
	mChannelList(),
	mChannelListMutex(),
	mListener(NULL),
	mResponders(),
	mRespondersMutex()
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKIRCProtocol::~CKIRCProtocol()
{
	// first, kill the listener and free it
	setListener(NULL);
	// now handle the connection to the IRC server itself.
	if (isConnected()) {
		if (isLoggedIn()) {
			doQUIT("bye");
			setIsLoggedIn(false);
		}
		mCommPort.disconnect();
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKIRCProtocol & CKIRCProtocol::operator=( const CKIRCProtocol & anOther )
{
	// create a listener for this guy
	CKIRCProtocolListener	*buddy = new CKIRCProtocolListener(this);
	if (buddy == NULL) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::operator=(const CKIRCProtocol &) - the Listener "
			"for this instance could not be created. This is a serious allocation "
			"problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		setListener(buddy);
	}

	// set everything that the other one has
	mHostname = anOther.mHostname;
	mPort = anOther.mPort;
	mCommPort = anOther.mCommPort;
	mIsLoggedIn = anOther.mIsLoggedIn;
	mPassword = anOther.mPassword;
	mNickname = anOther.mNickname;
	mUserHost = anOther.mUserHost;
	mUserServer = anOther.mUserServer;
	mRealName = anOther.mRealName;
	/*
	 * I'm going to break encapsulation on these lists as they are better
	 * done as the ivars themselves.
	 */
	mChannelListMutex.lock();
	mChannelList = anOther.mChannelList;
	mChannelListMutex.unlock();

	mRespondersMutex.lock();
	mResponders = anOther.mResponders;
	mRespondersMutex.unlock();

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method is the standard setter accessor method for
 * the host name that will be used in all subsequent
 * connections. Because it makes no sense to change the
 * host name while connected to a host, this method will
 * throw a CKException if a connection is already
 * established to a server.
 */
void CKIRCProtocol::setHostname( const std::string & aHost )
{
	// first, see if we're currently connected to some host
	if (isConnected() && (getHostname() != aHost)) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::setHostname(const std::string &) - there's an "
			"established connection to the server on " << getHostname() << ":" <<
			getPort() << " and that connection needs to be closed before we can "
			"change the host to connect to. Please call disconnect().";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// OK... we're clear to save the value
	mHostname = aHost;
}


/*
 * This method is the setter for the port number that will
 * be used to establish a communication port with
 * the server. Typically, this should be left as the default,
 * but on certain rare occaisions, some server may choose
 * to listen for IRC communications on a non-standard port.
 */
void CKIRCProtocol::setPort( int aPort )
{
	// first, see if we're currently connected to some host
	if (isConnected() && (getPort() != aPort)) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::setPort(int) - there's an "
			"established connection to the server on " << getHostname() << ":" <<
			getPort() << " and that connection needs to be closed before we can "
			"change the port to connect to. Please call disconnect().";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// OK... we're clear to save the value
	mPort = aPort;
}


/*
 * This method sets the password we'll be using in all communications
 * with the IRC server.
 */
void CKIRCProtocol::setPassword( const std::string & aPassword )
{
	mPassword = aPassword;
}


/*
 * This method sets the nickname we'll be using in all
 * communications with the IRC server.
 */
void CKIRCProtocol::setNickname( const std::string & aNick )
{
	mNickname = aNick;
}


/*
 * This method sets the USER host we'll be using in all
 * communications with the IRC server.
 */
void CKIRCProtocol::setUserHost( const std::string & aHost )
{
	mUserHost = aHost;
}


/*
 * This method sets the USER server we'll be using in all
 * communications with the IRC server.
 */
void CKIRCProtocol::setUserServer( const std::string & aServer )
{
	mUserServer = aServer;
}


/*
 * This method sets the real name we'll be using in all
 * communications with the IRC server.
 */
void CKIRCProtocol::setRealName( const std::string & aName )
{
	mRealName = aName;
}


/*
 * Because the IRC Protocol is based on the CKTelnetConnection,
 * we need to have a sense of the host with which we are
 * communicating - and which host it is. This method is the
 * standard getter accessor method for the host name that
 * will be used in all subsequent connections.
 */
const std::string CKIRCProtocol::getHostname() const
{
	return mHostname;
}


/*
 * This method is the getter for the port number that will
 * be used to establish a communication port with
 * the server. Typically, this is the default, but on certain
 * rare occaisions, some server may choose to listen for IRC
 * communications on a non-standard port.
 */
int CKIRCProtocol::getPort() const
{
	return mPort;
}


/*
 * The IRC Protocol maintains a CKTelnetConnection to the IRC
 * server, and because it's nice to maintain encapsulation, we're
 * going to return a pointer to it as opposed to a copy of it.
 * The big reason is that I'd like to use the getter in the methods
 * and not break encapsulation and use the iver itself. However,
 * please note that this pointer is to the existing CKTelnetConnection
 * and you should not free it or mess with it, but feel free to
 * make a copy, if you so choose.
 */
const CKTelnetConnection *CKIRCProtocol::getCommPort() const
{
	return & mCommPort;
}


/*
 * If a valid connection is made to the server, this
 * method will return true. It is very useful to see if the
 * instance is ready to take commands or messages.
 */
bool CKIRCProtocol::isLoggedIn() const
{
	return mIsLoggedIn;
}


/*
 * This method returns the password we'll be using in all
 * communications with the IRC server.
 */
const std::string CKIRCProtocol::getPassword() const
{
	return mPassword;
}


/*
 * This method returns the nickname we'll be using in all
 * communications with the IRC server.
 */
const std::string CKIRCProtocol::getNickname() const
{
	return mNickname;
}


/*
 * This method returns the USER host we'll be using in all
 * communications with the IRC server.
 */
const std::string CKIRCProtocol::getUserHost() const
{
	return mUserHost;
}


/*
 * This method returns the USER server we'll be using in all
 * communications with the IRC server.
 */
const std::string CKIRCProtocol::getUserServer() const
{
	return mUserServer;
}


/*
 * This method returns the real name we'll be using in all
 * communications with the IRC server.
 */
const std::string CKIRCProtocol::getRealName() const
{
	return mRealName;
}


/*
 * This method returns a pointer to a std::list of std::strings that
 * is the list of Channels that this IRC Connection has JOINed. Note
 * that this method will not return a NULL as it's a pointer to the
 * instance variable and therefore should also not be released, etc.
 * If you want to make a copy, do so, but otherwise, leave this guy
 * alone.
 */
const std::list<std::string>	*CKIRCProtocol::getChannelList() const
{
	return & mChannelList;
}


/*
 * This method returns a pointer to the listener thread that is
 * going to be listening to the incoming data from the IRC server.
 * This is a pointer to the actual instance variable, so please be
 * very careful with it, and if you want to keep it around, please
 * make a copy.
 */
CKIRCProtocolListener *CKIRCProtocol::getListener() const
{
	return mListener;
}


/*
 * This method will return true if the supplied channel name is
 * among the list of channels that this instance has already
 * JOINed. This is important, as you only need to JOIN a channel
 * once per connection to the IRC server, and to do mroe than
 * once is wasting bandwidth.
 */
bool CKIRCProtocol::isChannelInChannelList( const std::string & aChannel )
{
	bool	retval = false;

	// first, lock the list against any changes
	mChannelListMutex.lock();

	// try to find the channel in the list
	std::list<std::string>::iterator	i;
	i = find(mChannelList.begin(), mChannelList.end(), aChannel);
	if (i != mChannelList.end()) {
		// yippee! we have it
		retval = true;
	}

	// finally, unlock the list to allow changes
	mChannelListMutex.unlock();

	return retval;
}


/********************************************************
 *
 *                Connection Methods
 *
 ********************************************************/
/*
 * This method uses the hostname and port number contained
 * within the instance to make a connection. If successful, the
 * method returns true, othersize it returns false. If there is
 * an unusual condition, a CKException is thrown.
 */
bool CKIRCProtocol::connect()
{
	return connect(getHostname(), getPort());
}


/*
 * This method uses the instance's port number, and the host name
 * provided to establish a connection for this IRC session.
 * This is very common because the default port is the standard
 * IRC protocol port and so the user really only needs to
 * specify the host and most connections will be made.
 */
bool CKIRCProtocol::connect( const std::string & aHost )
{
	return connect(aHost, getPort());
}


/*
 * This method allows both the host name and port number to
 * be specified for making the connection. This is used by
 * the other connection mathods as it is the most general
 * form of the function.
 */
bool CKIRCProtocol::connect( const std::string & aHost, int aPort )
{
	bool		error = false;

	// first, see if we are already connected to some host
	if (!error) {
		if (isConnected() && ((getHostname() != aHost) || (getPort() != aPort))) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::connect(const std::string & , int) - there's an "
				"established connection to the server on " << getHostname() << ":" <<
				getPort() << " and that connection needs to be closed before we can "
				"connect to another host and/or port. Please call disconnect().";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, tell the connection object to connect to the right host and port
	if (!error) {
		// save the host and port for later
		setHostname(aHost);
		setPort(aPort);

		// try to make the connection
		if (!mCommPort.connect(aHost, aPort)) {
			// undo the settings we did just before this
			setHostname("");
			setPort(DEFAULT_IRC_PORT);
			// ...now flag the error and throw the exception
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::connect(const std::string & , int) - the "
				"connection to the server on " << getHostname() << ":" <<
				getPort() << " could not be created and that's a serious problem. "
				"Please make sure that there's an IRC server on that box.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			/*
			 * The connection was good, so let's set the read timeout to
			 * the default value which is understandably short given the
			 * quasi-polling nature of the IRC Listener
			 */
			mCommPort.setReadTimeout(DEFAULT_IRC_READ_TIMEOUT);
		}
	}

	// if we're good, then save all the parts and start the listener
	if (!error) {
		// start the listener so it can monitor incoming traffic
		if (getListener() == NULL) {
			// create a listener for this guy
			CKIRCProtocolListener	*buddy = new CKIRCProtocolListener(this);
			if (buddy == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKIRCProtocol::connect(const std::string & , int) - the "
					"Listener for this instance could not be created. This is a "
					"serious allocation problem.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				setListener(buddy);
			}
		}
		// ...now start it - if it needs to be started
		if (!getListener()->isRunning()) {
			getListener()->start();
		}
	}

	return !error;
}


/*
 * This method simply returns the state of the IRC
 * communications port - in so far as it's establishment.
 */
bool CKIRCProtocol::isConnected()
{
	return mCommPort.isConnected();
}


/*
 * This method will log out any logged in user and break the
 * established connection to the IRC host. This is useful
 * when "shutting down" as it takes care of all the
 * possibilities in one fell swoop.
 */
void CKIRCProtocol::disconnect()
{
	// first, stop the listener
	stopListener();

	// now handle the connection to the IRC server itself.
	if (isConnected()) {
		if (isLoggedIn()) {
			doQUIT("bye");
			setIsLoggedIn(false);
		}
		mCommPort.disconnect();
		// clear out all the channels we joined
		clearChannelList();
	}
}


/********************************************************
 *
 *            IRC Commands - Object Model
 *
 ********************************************************/
/*
 * This is a simple cover method for the sending of a message to the
 * IRC server. The 'aDest' can be a channel or a user.
 */
void CKIRCProtocol::sendMessage( const std::string & aDest, const std::string & aMsg )
{
	bool		error = false;

	// first, check to see if we have anything to do
	if (!error) {
		if (aDest.size() == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::sendMessage(const std::string &, const std::string &)"
				" - the supplied chat destination is empty, and that means that "
				"there's nothing I can do. Please make sure that there is a valid "
				"destination before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if (aMsg.size() == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::sendMessage(const std::string &, const std::string &)"
				" - the supplied chat message is empty, and that means that "
				"there's nothing I can do. Please make sure that there is a valid "
				"destination before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// if the destination is a channel, make sure we're joined
	if (!error) {
		if (aDest[0] == '#') {
			doJOIN(aDest);
		}
	}

	// now let's chunk up this message into a series of lines
	std::vector<std::string>	lines;
	if (!error) {
		lines = parseIntoChunks(aMsg, std::string("\n"));
		if (lines.size() == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::sendMessage(const std::string &, const std::string &)"
				" - the supplied chat message could not be broken up into a series "
				"of NEWLINE-delimited strings. This is a problem because there "
				"should at least be the one, original message line.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to process each line, but be careful, if the line's
	 * length exceeds the MAX_MESSAGE_LEN, it needs to be broken on the
	 * word boundaries to make it fit.
	 */
	if (!error) {
		for (unsigned int i = 0; i < lines.size(); i++) {
			// get the next line to send to the server
			std::string		line = lines[i];
			// see if it's too long to fit as one message
			while (line.length() > MAX_MESSAGE_LEN) {
				// try to cut it right at the limit
				int		pos = MAX_MESSAGE_LEN;
				// but make a break at a space in the message
				while ((line[pos] != ' ') && (pos > 0)) {
					pos--;
				}
				// if there was no break, then be harsh, but fair
				if (pos == 0) {
					pos = MAX_MESSAGE_LEN;
				}
				// send out the first part of the line
				doPRIVMSG(aDest, line.substr(0, pos));
				// update what's left of the line to send
				line.erase(0, (pos + 1));
			}
			// whatever's left is OK to send out
			if (line.length() > 0) {
				doPRIVMSG(aDest, line);
			}
		}
	}
}


/*
 * This method is interesting - there will be times that chat
 * commands come into this client and will need to be responded
 * to by this client, but there's no need to involve the "higher
 * brain" of the developer in these messages. This method will
 * be called on each message and a determination will be made if
 * each message is really necessary to process, or if it's
 * sufficiently handled in this method and then forgotten.
 *
 * In subclasses, this is probably a very useful method to
 * implement as it will allow for all the 'automatic' handling
 * of housekeeping messages without having to put that into
 * your responder's code.
 *
 * If this message is handled in this method, then this method
 * will return true, otherwise, it hasn't been handled and
 * needs to be passed to all the responders for their input.
 */
bool CKIRCProtocol::isReflexChat( std::string & aLine )
{
	bool		error = false;
	bool		handled = false;

	// first, see if we have anything to do
	if (!error && !handled) {
		if (aLine.length() == 0) {
			// this is empty, then say we did it to stop processing
			handled = true;
		}
	}

	// check to see if it's a PING message
	if (!error && !handled) {
		if (aLine.substr(0,4) == "PING") {
			doPONG();
			handled = true;
		}
	}

	return handled;
}


/********************************************************
 *
 *                Registration Methods
 *
 ********************************************************/
/*
 * This method will add the supplied target method to the list
 * of those that will receive the CKIRCIncomingMessage when the
 * protocol listener instance working on behalf of this instance
 * sees that there's something sent from the chat server that's
 * addressed to this registered chat user. When such a thing
 * happens, these targets will all get a chance to update/append
 * to the reponse that's in the CKIRCIncomingMessage which will be
 * sent back to the source as a private message.
 *
 * This method DOES NOT assume control of the memory for this
 * responder, rather, it's a registration process and the objects
 * themselves need to de-register themselves *before* they go out
 * of scope or we are going to have a serious problem.
 */
void CKIRCProtocol::addToResponders( CKIRCResponder *anObj )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (anObj == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::addToResponders(CKIRCResponder*) - the "
				"supplied chat responder is NULL and that means that there's "
				"nothing I can do. Please make sure that the argument is not "
				"NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now see if it already exists in the list
	if (!error) {
		// lock it down so it doesn't change
		mRespondersMutex.lock();

		// now try to find it
		std::list<CKIRCResponder*>::iterator	i;
		i = find(mResponders.begin(), mResponders.end(), anObj);
		if (i == mResponders.end()) {
			// not found, so it's safe to add it
			mResponders.push_back(anObj);
		}

		// unlock it regardless of the outcome
		mRespondersMutex.unlock();
	}
}


/*
 * When you are wishing to de-register from the 'alerting' process,
 * call this method and the supplied method will be removed from the
 * list of methods that will be called when an incoming chat arrives.
 */
void CKIRCProtocol::removeFromResponders( CKIRCResponder *anObj )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (anObj == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::removeFromResponders(CKIRCResponder*) - the "
				"supplied chat responder is NULL and that means that there's "
				"nothing I can do. Please make sure that the argument is not "
				"NULL before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now see if it exists in the list
	if (!error) {
		// lock it down so it doesn't change
		mRespondersMutex.lock();

		// now try to find it
		std::list<CKIRCResponder*>::iterator	i;
		i = find(mResponders.begin(), mResponders.end(), anObj);
		if (i != mResponders.end()) {
			// found, so it's safe to erase it
			mResponders.erase(i);
		}

		// unlock it regardless of the outcome
		mRespondersMutex.unlock();
	}
}


/*
 * This method removes *ALL* responders and is very useful in the
 * clean-up phase, but also can be useful in a restart-phase.
 */
void CKIRCProtocol::removeAllResponders()
{
	mRespondersMutex.lock();
	mResponders.clear();
	mRespondersMutex.unlock();
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKIRCProtocols are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKIRCProtocol::operator==( const CKIRCProtocol & anOther ) const
{
	bool		equal = true;

	if ((getHostname() != anOther.getHostname()) ||
		(getPort() != anOther.getPort()) ||
		(mCommPort != anOther.mCommPort) ||
		(isLoggedIn() != anOther.isLoggedIn()) ||
		(getPassword() != anOther.getPassword()) ||
		(getNickname() != anOther.getNickname()) ||
		(getUserHost() != anOther.getUserHost()) ||
		(getUserServer() != anOther.getUserServer()) ||
		(getRealName() != anOther.getRealName()) ||
		(mChannelList != anOther.mChannelList) ||
		(mResponders != anOther.mResponders)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKIRCProtocols are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKIRCProtocol::operator!=( const CKIRCProtocol & anOther ) const
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
std::string CKIRCProtocol::toString() const
{
	std::ostringstream	buff;

	buff << "< IRC Host=" << getHostname() << ", " <<
		" IRC Port=" << getPort() << ", " <<
		" CommPort=" << mCommPort << ", " <<
		" isLoggedIn? " << (isLoggedIn() ? "Yes" : "No") << ", " <<
		" Password=" << getPassword() << ", " <<
		" Nickname=" << getNickname() << ", " <<
		" UserHost=" << getUserHost() << ", " <<
		" UserServer=" << getUserServer() << ", " <<
		" RealName=" << getRealName() << ", " <<
		" ChannelList: [";
	std::list<std::string>::const_iterator	i;
	for (i = mChannelList.begin(); i != mChannelList.end(); ++i) {
		if (i != mChannelList.begin()) {
			buff << ", ";
		}
		buff << (*i);
	}
	buff << "]>" << std::endl;

	return buff.str();
}


/*
 * This method is used to copy the parameters for the given connection
 * to the connection that this instance is maintaining for all
 * communications with the IRC Server. It's important to note that
 * this is a copy operation and not an assignment as you might think.
 */
void CKIRCProtocol::setCommPort( const CKTelnetConnection & aConn )
{
	// first, see if we're currently connected to some host
	if (isConnected()) {
		std::ostringstream	msg;
		msg << "CKIRCProtocol::setCommPort(const CKTelnetConnection &) - there's "
			"an established connection to the server on " << getHostname() << ":" <<
			getPort() << " through the existing connection port and that connection "
			"needs to be closed before we can change the port parameters. Please "
			"call disconnect().";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// OK... we're clear to copy over the parameters
	mCommPort = aConn;
}


/*
 * When the class methods connect to the server, they call
 * this method to update the status of the connection to
 * the host. It's important that no one outside this class
 * manipulate this variable, as it's integrity is vital to
 * the operation of this class.
 */
void CKIRCProtocol::setIsLoggedIn( bool aFlag )
{
	mIsLoggedIn = aFlag;
}


/*
 * This method sets the contents of this instance's list of JOINed
 * channels to the same contents as the supplied list. This is done
 * as a copy and not as an assumption of the memory management of
 * the elements of the list.
 */
void CKIRCProtocol::setChannelList( const std::list<std::string> & aList )
{
	// first, lock the list for changes
	mChannelListMutex.lock();
	// next, clear out the existing list
	mChannelList.clear();
	// ...and then copy in all the elements of the passed-in list
	mChannelList = aList;
	// finally, unlock it
	mChannelListMutex.unlock();
}


/*
 * This method sets the pointer to this instance's listener to the
 * passed-in value. The memory management of this listener will then
 * fall to this instance and it will be cleaned up when the time is
 * right.
 */
void CKIRCProtocol::setListener( CKIRCProtocolListener *aListener )
{
	// first, see if we have any current listener
	if (mListener != NULL) {
		/*
		 * We need to nicely kill this current listener
		 */
		stopListener();
		// now let's delete it as it's no longer needed
		delete mListener;
		mListener = NULL;
	}

	// now we can set the value if it's not NULL
	if (aListener != NULL) {
		mListener = aListener;
	}
}


/*
 * This method adds the supplied channel name to the list of
 * JOINed channels for this instance. It is only added, of course,
 * if the channel does not already exist in the list.
 */
void CKIRCProtocol::addToChannelList( const std::string & aChannel )
{
	// first, lock the list against any changes
	mChannelListMutex.lock();

	// try to find the channel in the list
	std::list<std::string>::iterator	i;
	i = find(mChannelList.begin(), mChannelList.end(), aChannel);
	if (i == mChannelList.end()) {
		// we don't have it, so add it
		mChannelList.push_back(aChannel);
	}

	// finally, unlock the list to allow changes
	mChannelListMutex.unlock();
}


/*
 * This method clears out all the channels that are currently
 * JOINed to. This is necessary at times, such as the reconnection
 * to the chat server, when you need to start fresh.
 */
void CKIRCProtocol::clearChannelList()
{
	mChannelListMutex.lock();
	mChannelList.clear();
	mChannelListMutex.unlock();
}


/*
 * This method is used by several methods to try and stop the
 * Listener thread. There are a few steps to it, so it's nice to
 * have it in one place as opposed to copying code.
 */
void CKIRCProtocol::stopListener()
{
	if (getListener() != NULL) {
		if (getListener()->isRunning()) {
			getListener()->setTimeToDie(true);
			int		cnt = 0;
			while ((cnt < 6) && getListener()->isRunning()) {
				// wait for DEFAULT_IRC_READ_TIMEOUT/4 to see if it's dead
				usleep((unsigned int)(DEFAULT_IRC_READ_TIMEOUT * 1000000 / 4));
				cnt++;
			}
			// make sure it's stopped
			if (getListener()->isRunning()) {
				std::ostringstream	msg;
				msg << "CKIRCProtocol::stopListener() - the Listener for this "
					"instance could not be stopped. This is a serious threading "
					"problem.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}
}


/*
 * This is the tokenizer/parser that wasn't in the STL string
 * class for some unknown reason. It takes a source and a
 * delimiter and breaks up the source into chunks that are
 * all separated by the delimiter string. Each chunk is put
 * into the returned vector for accessing by the caller. Since
 * the return value is created on the stack, the user needs to
 * save it if they want it to stay around.
 */
std::vector<std::string> CKIRCProtocol::parseIntoChunks(
												const std::string & aString,
												const std::string & aDelim )
{
	bool						error = false;
	std::vector<std::string>	retval;

	// first, see if we have anything to do
	if (!error) {
		if (aString.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::parseIntoChunks(const std::string &, "
				"const std::string &) - the length of the source string is 0 and "
				"that means that there's nothing for me to do. Please make sure "
				"that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	int		delimLength = 0;
	if (!error) {
		delimLength = aDelim.length();
		if (delimLength <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::parseIntoChunks(const std::string &, "
				"const std::string &) - the length of the delimiter string is 0 "
				"and that means that there's nothing for me to do. Please make "
				"sure that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, copy the source to a buffer so I can consume it in the process
	std::string		buff;
	if (!error) {
		buff = aString;
	}

	/*
	 * Now loop picking off the parts bettween the delimiters. Do this by
	 * finding the first delimiter, see if it's located at buff[0], and if
	 * so, then add an empty string to the vector, otherwise, get the
	 * substring up to that delimiter and place it at the end of the vector,
	 * removing it from the buffer as you do this. Then eat up the delimiter
	 * and do it all again. In the end, there will be one more bit and that
	 * will simply be added to the end of the vector.
	 */
	while (!error) {
		// find out wherre, if anyplace, the delimiter sits
		unsigned int	pos = buff.find(aDelim);
		if (pos == std::string::npos) {
			// nothing left to parse out, bail out
			break;
		} else if (pos == 0) {
			// add an empty string to the vector
			retval.push_back(std::string(""));
		} else {
			// pick off the substring up to the delimiter
			retval.push_back(buff.substr(0, pos));
			// ...and then delete them from the buffer
			buff.erase(0, pos);
		}

		// now strip off the delimiter from the buffer
		buff.erase(0, delimLength);
	}
	// if we didn't error out, then add the remaining buff to the end
	if (!error) {
		retval.push_back(buff);
	}

	return retval;
}


/********************************************************
 *
 *           IRC Message Manipulation Methods
 *
 ********************************************************/
/*
 * This method gets the reply from the IRC server through
 * the socket. If there's no reply within the timeout then
 * this method will throw a CKException and return. It's
 * probably a good idea to trap for that in the listener
 * and use it to determine when to recycle and check for
 * things needing to be done.
 */
std::string CKIRCProtocol::getReply()
{
	bool			error = false;
	std::string		retval("");

	/*
	 * First, see if we're connected to some IRC server, and if not, then
	 * try to connect to the one that we connected to when we created this
	 * guy.
	 */
	if (!error) {
		if (!isConnected()) {
			if (!connect()) {
				std::ostringstream	msg;
				msg << "CKIRCProtocol::getReply() - this protocol is not currently "
					"connected to any IRC server, and a connection to the default "
					"server could not be made. Therefore, it's not possible to get "
					"any reply. Please make sure that there is an established "
					"connection with isConnected() before calling this method, or "
					"make sure that the IRC server is on-line.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// now read up to the "\n" NEWLINE that the IRC server sends
	if (!error) {
		retval = mCommPort.readUpToNEWLINE();
	}

	return retval;
}


/*
 * This method checks for the reply from the IRC server through
 * the socket. If there's not a valid reply on the socket, it
 * returns false. This method waits for nothing and no one.
 */
bool CKIRCProtocol::checkForReply()
{
	bool		error = false;
	bool		retval = false;

	/*
	 * First, see if we're connected to some IRC server, and if not, then
	 * try to connect to the one that we connected to when we created this
	 * guy.
	 */
	if (!error) {
		if (!isConnected()) {
			if (!connect()) {
				std::ostringstream	msg;
				msg << "CKIRCProtocol::checkForReply() - this protocol is not currently "
					"connected to any IRC server, and a connection to the default "
					"server could not be made. Therefore, it's not possible to check "
					"for any reply. Please make sure that there is an established "
					"connection with isConnected() before calling this method, or "
					"make sure that the IRC server is on-line.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// now check for pending data up to the "\n" NEWLINE
	if (!error) {
		retval = mCommPort.checkForDataUpTo("\n");
	}

	return retval;
}


/*
 * This method is used by the protocol listener to make sure that
 * all the registered responders of this class get a chance to respond
 * to this incoming chat message. It basically cycles through all the
 * registered responders, and calls them with the message reference
 * so that they can modify it, if necessary. Once all of them are
 * done, this method returns and the listener can send the response
 * back to the source as a private message.
 */
bool CKIRCProtocol::alertAllResponders( CKIRCIncomingMessage & aMsg )
{
	bool		error = false;

	// first, we need to see if the message is empty
	if (!error) {
		if (aMsg.message.length() == 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::alertAllResponders(CKIRCIncomingMessage &) - "
				"the supplied message is empty and that means that there's nothing "
				"to do. Please make sure that the message has something in it before "
				"calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we're going to scan through all the responders giving each a
	 * chance to respond to the message provided. But because we may have
	 * a lot of messages flowing through here in different threads it's a
	 * good idea to make a local copy of the list of responders at the
	 * time we are called, and then iterate on that so as not to lock up
	 * the list of responders for too long as a pokey responder might do.
	 */
	if (!error) {
		// we need to make a thread-safe local copy of the responder list
		mRespondersMutex.lock();
		std::list<CKIRCResponder*>	copy = mResponders;
		mRespondersMutex.unlock();

		// now go through the list, calling each with the message
		std::list<CKIRCResponder*>::iterator	i;
		for (i = copy.begin(); i != copy.end(); ++i) {
			(*i)->respondToIRCMessage(aMsg);
		}
	}

	return !error;
}


/********************************************************
 *
 *            Generic IRC Commands - IRC style
 *
 ********************************************************/
/*
 * This method does all the checking and building to make each
 * of the executions of an IRC command with the remote host a
 * reliable, and verifyable event. This is called by each of
 * the other IRC commands, and returns the corresponding return
 * code returned from the remote host.
 *
 * While it is possible for a user to directly call this
 * method, it is not advised, and in fact, discouraged. The
 * IRC functions are all available and waiting without
 * circumventing the designed flow.
 */
void CKIRCProtocol::executeCommand( const std::string & aCmd )
{
	bool		error = false;

	/*
	 * First, see if we're connected to some IRC server, and if not, then
	 * try to connect to the one that we connected to when we created this
	 * guy.
	 */
	if (!error) {
		if (!isConnected()) {
			if (!connect()) {
				std::ostringstream	msg;
				msg << "CKIRCProtocol::executeCommand(const std::string &) - this "
					"protocol is not currently connected to any IRC server, and a "
					"connection to the default server could not be made. Therefore, "
					"it's not possible to execute any IRC commands to the server. "
					"Please make sure that there is an established connection with "
					"isConnected() before calling this method, or make sure that "
					"the IRC server is on-line.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// now send out the command terminated in a NEWLINE
	if (!error) {
		std::string		cmd = aCmd;
		cmd += "\n";
		if (!mCommPort.send(cmd)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIRCProtocol::executeCommand(const std::string &) - while "
				"trying to send the command '" << aCmd << "' to the IRC server an "
				"error occurred. Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
}


/*
 * This executes the standard IRC 'PASS' command on the communication
 * channel to the remote host. This is meant to supply a password
 * to the IRC server to ensure at least some level of security.
 */
void CKIRCProtocol::doPASS( const std::string & aPassword )
{
	std::string		cmd = "PASS ";
	cmd += aPassword;
	executeCommand(cmd);
}


/*
 * This executes the standard IRC 'NICK' command on the communication
 * channel to the remote host. This is meant to supply the requested
 * nickname to the IRC server so that everyone knows who this is coming
 * from.
 */
void CKIRCProtocol::doNICK( const std::string & aNick )
{
	std::string		cmd = "NICK ";
	cmd += aNick;
	executeCommand(cmd);
}


/*
 * This executes the standard IRC 'USER' command on the communication
 * channel to the remote host. This is meant to supply real
 * information about the user to the IRC server.
 */
void CKIRCProtocol::doUSER( const std::string & aNick,
							const std::string & aHost,
							const std::string & aServer,
							const std::string & aRealName )
{
	std::string		cmd = "USER ";
	cmd += aNick;
	cmd += " ";
	cmd += aHost;
	cmd += " ";
	cmd += aServer;
	cmd += " :";
	cmd += aRealName;
	executeCommand(cmd);
}


/*
 * This executes the standard IRC 'QUIT' command on the communication
 * channel to the remote host. This is meant to log off this
 * connection and leave a message on the way out.
 */
void CKIRCProtocol::doQUIT( const std::string & aMsg )
{
	std::string		cmd = "QUIT :";
	cmd += aMsg;
	executeCommand(cmd);
}


/*
 * This executes the standard IRC 'JOIN' command on the communication
 * channel to the remote host. This puts us in the channel on the
 * server so that we can send messages to it.
 */
void CKIRCProtocol::doJOIN( const std::string & aChannel )
{
	// do this only if we haven't already joined this channel
	if (!isChannelInChannelList(aChannel)) {
		// do the join with the server
		std::string		cmd = "JOIN ";
		cmd += aChannel;
		executeCommand(cmd);
		// ...and add it to our list
		addToChannelList(aChannel);
	}
}


/*
 * This executes the standard IRC 'PRIVMSG' command on the
 * communication channel to the remote host. This sends a private
 * message to the supplied user or channel and includes a return
 * code - the doNOTICE is different in that no return code is sent.
 */
void CKIRCProtocol::doPRIVMSG( const std::string & aDest, const std::string & aMsg )
{
	std::string		cmd = "PRIVMSG ";
	cmd += aDest;
	cmd += " :";
	cmd += aMsg;
	executeCommand(cmd);
}


/*
 * This executes the standard IRC 'NOTICE' command on the
 * communication channel to the remote host. This is similar to
 * doPRIVMSG() but here we do NOT get a reply from the IRC server.
 */
void CKIRCProtocol::doNOTICE( const std::string & aDest, const std::string & aMsg )
{
	std::string		cmd = "MOTICE ";
	cmd += aDest;
	cmd += " :";
	cmd += aMsg;
	executeCommand(cmd);
}


/*
 * This execute the PONG command that is used in response to the PING
 * sent from the IRC Server. This lets the server know that I'm alive
 * and listening...
 */
void CKIRCProtocol::doPONG()
{
	std::string		cmd = "PONG ";
	char	host[MAXHOSTNAMELEN];
	if (gethostname(host, MAXHOSTNAMELEN) < 0) {
		strcpy(host, "localhost");
	}
	cmd += host;
	executeCommand(cmd);
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCProtocol & aProtocol )
{
	aStream << aProtocol.toString();

	return aStream;
}
