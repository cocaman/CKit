/*
 * CKSocket.cpp - the implementation of the class that can be used as a
 *                general wrapper on the unix-level sockets for the user. The
 *                idea is that low-level socket functionality is nice, but in
 *                order to be more generally useful, we need more advanced
 *                features and more object-oriented behaviors.
 *
 * $Id: CKSocket.cpp,v 1.13 2004/09/11 02:15:22 drbob Exp $
 */

//	System Headers
#include <iostream>
#include <sstream>
#include <unistd.h>
#ifdef WIN32
#include <sys/ioctl.h>
#else
#include <fcntl.h>
#endif
#ifdef __sun__
#include <stropts.h>
#include <sys/filio.h>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKSocket.h"
#include "CKException.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants
/*
 * Solaris doesn't have the same INADDR_NONE definition as do Linux and
 * Darwin. This means the easiest thing to do is to add the definition
 * here and then use it in the code.
 */
#ifdef __sun__
#define	INADDR_NONE			(unsigned long)(-1)
#endif

//	Private Datatypes

//	Private Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that doesn't establish any
 * connection to any host on any port. You can do all that with
 * one of the connect() methods, and that's OK for a lot of
 * uses.
 */
CKSocket::CKSocket() :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif
}


/*
 * This form of the constructor takes the port number and the
 * user still needs to supply the hostname with which to connect.
 * this has somewhat limited use, but in the CKit version of this
 * class I had this constructor, and I'm guessing there was a
 * reason for it.
 */
CKSocket::CKSocket( int aPort ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// now set those parameters the user has supplied
	setPort(aPort);
}


/*
 * This form of the constructor is nice in that it takes all you
 * need to establish a connection to the remote system - hostname
 * and port number. As a nice benefit, this constructor attempts
 * to establish that connection so that it's ready to send or
 * receive data after a successful return.
 */
CKSocket::CKSocket( const std::string & aHost, int aPort ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// let's try to make the connection based on this information
	if (!connect(aHost, aPort)) {
		std::ostringstream	msg;
		msg << "CKSocket::CKSocket(const std::string &, int) - the connection "
			"to the socket " << aHost << ":" << aPort << " could not be "
			"established. This is a serious problem. Please make sure that the "
			"remote service is ready to accept the connection.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This form of the constructor is the most detailed in that it
 * allows the user to specify not only the hostname and port
 * number, but also the service and protocol that this socket will
 * be using. This is important in certain specific applications,
 * but for the most part, isn't required for general communications.
 */
CKSocket::CKSocket( const std::string & aHost, int aPort, int aService, int aProtocol ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// let's try to make the connection based on this information
	if (!connect(aHost, aPort, aService, aProtocol)) {
		std::ostringstream	msg;
		msg << "CKSocket::CKSocket(const std::string &, int, int, int) - the "
			"connection to the socket " << aHost << ":" << aPort << " could "
			"not be established. This is a serious problem. Please make sure "
			"that the remote service is ready to accept the connection.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This form of the constructor sets up the CKSocket into a state
 * that it is ready to receive connections from other hosts. In
 * this case, a socket is created, bound to the filesystem,
 * interrogatted as to its parameters, and then set to
 * listen mode for incoming connections.
 */
CKSocket::CKSocket( int aService, int aProtocol ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// let's set ourselves up as a listener
	if (!createAndBindListener(aService, aProtocol)) {
		std::ostringstream	msg;
		msg << "CKSocket::CKSocket(int, int) - a socket could not be created "
			"in the proper mode for listening. This is a serious problem and "
			"needs to be looked into as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This form of the constructor sets up the CKSocket into a state
 * that it is ready to receive connections from other hosts. In
 * this case, a socket is created for listening on the provided port,
 * sith the service and protocol provided, and bound to the filesystem,
 */
CKSocket::CKSocket( int aPort, int aService, int aProtocol ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// let's set ourselves up as a listener
	if (!createAndBindListener(aPort, aService, aProtocol)) {
		std::ostringstream	msg;
		msg << "CKSocket::CKSocket(int, int, int) - a socket could not be created "
			"in the proper mode for listening. This is a serious problem and "
			"needs to be looked into as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKSocket::CKSocket( const CKSocket & anOther ) :
	mHostname(),
	mPort(-1),
	mSocketHandle(INVALID_SOCKET),
	mReadBufferSize(DEFAULT_READ_BUFFER_SIZE),
	mWaitForIncomingConnectionTimeout(DEFAULT_WAIT_FOR_INCOMING_TIMEOUT),
	mActivelyListening(false),
	mConnectionEstablished(false),
	mTraceOutgoingData(false),
	mTraceIncomingData(false),
	mIsBlockingForTransferredData(false)
{
#ifdef WIN32
	/*
	 * The default behavior for sockets on NT is that
	 * they do non-blocking reads and buffered writes
	 * so they are "intelligent".
	 */
	setIsBlockingForTransferredData(false);
#else
	/*
	 * The default behavior for sockets on Unix is that
	 * they are blocking read and write. This is not
	 * as intelligent as NT, but workable.
	 */
	setIsBlockingForTransferredData(true);
#endif

	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKSocket::~CKSocket()
{
	shutdownSocket();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKSocket & CKSocket::operator=( const CKSocket & anOther )
{
	// we need to copy over all the variables
	setHostname(anOther.getHostname());
	setPort(anOther.getPort());
	setSocketHandle(anOther.getSocketHandle());
	setReadBufferSize(anOther.getReadBufferSize());
	setWaitForIncomingConnectionTimeout(anOther.getWaitForIncomingConnectionTimeout());
	setActivelyListening(anOther.isActivelyListening());
	setConnectionEstablished(anOther.isConnectionEstablished());
	setTraceOutgoingData(anOther.traceOutgoingData());
	setTraceIncomingData(anOther.traceIncomingData());
	setIsBlockingForTransferredData(anOther.isBlockingForTransferredData());

	// finally, return this guy
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the hostname that this socket will try to
 * connect to the next time the connect() method is called. Of
 * course, the other forms of the connect() method contain the
 * hostname, and those will call this method to set the host
 * after the connection is made.
 */
void CKSocket::setHostname( const std::string & aHostname )
{
	mHostname = aHostname;
}


/*
 * This method sets the port number that this socket will try to
 * connect to the next time the connect() method is called. Of
 * course, the other forms of the connect() method contain the
 * port number, and those will call this method to set the port
 * after the connection is made.
 */
void CKSocket::setPort( int aPort )
{
	mPort = aPort;
}


/*
 * This method sets the read buffer size for subsequent reads
 * from the socket. This is important becase a buffer that's too
 * small might mean buffer overrun errors, but one too large
 * will be a waste of space. In general, the default is a good
 * compromise.
 */
void CKSocket::setReadBufferSize( int aSize )
{
	mReadBufferSize = aSize;
}


/*
 * This method sets the timeout that will be used when remote
 * hosts are initiating connections back to this box. This is
 * not nearly as long as the data transfer timeouts, but then
 * again, we're only talking about initiating a connection and
 * not pushing bits around.
 */
void CKSocket::setWaitForIncomingConnectionTimeout( int aTimeInSec )
{
	mWaitForIncomingConnectionTimeout = aTimeInSec;
}


/*
 * If this method is called with a 'true' argument, then the
 * outgoing data to the socket will be logged to the std::cout
 * for debugging purposes. This is nice in come cases to see what's
 * really going on between the processes doing the communications.
 */
void CKSocket::setTraceOutgoingData( bool aFlag )
{
	mTraceOutgoingData = aFlag;
}


/*
 * If this method is called with a 'true' argument, then the
 * incoming data to the socket will be logged to the std::cout
 * for debugging purposes. This is nice in come cases to see what's
 * really going on between the processes doing the communications.
 */
void CKSocket::setTraceIncomingData( bool aFlag )
{
	mTraceIncomingData = aFlag;
}


/*
 * This method returns the hostname that the next connection will
 * be made to, or the currently established connection. The
 * difference lies soley with the nature of the usage of this
 * class - call this after a connection and it's the active
 * hostname.
 */
const std::string CKSocket::getHostname() const
{
	return mHostname;
}


/*
 * This method returns the port number that the next connection will
 * be made on, or the currently established connection. The difference
 * is again, only in the usage.
 */
int CKSocket::getPort() const
{
	return mPort;
}


/*
 * This method returns the current read buffer size for the socket
 * level reads.
 */
int CKSocket::getReadBufferSize() const
{
	return mReadBufferSize;
}


/*
 * This method returns the timeout (in seconds) for an incoming
 * socket connection to be established from a remote host.
 */
int CKSocket::getWaitForIncomingConnectionTimeout() const
{
	return mWaitForIncomingConnectionTimeout;
}


/*
 * This method will return 'true' if the socket is currently
 * actively listening on the port. This is useful in determining
 * if the socket is busy waiting for something from the remote
 * host.
 */
bool CKSocket::isActivelyListening() const
{
	return mActivelyListening;
}


/*
 * This method returns true if the socket connection is established
 * and ready to use. It's used a lot in the methods for this class
 * to make sure that the socket is in a reasonable state before
 * doing something like sending or reading data.
 */
bool CKSocket::isConnectionEstablished() const
{
	return mConnectionEstablished;
}


/*
 * This method will return true when the socket is echoing all
 * outgoing data to std::cout as a debugging tool. This is useful
 * as it's sometimes nice to toggle the trace and negating this
 * give you ust that toggle.
 */
bool CKSocket::traceOutgoingData() const
{
	return mTraceOutgoingData;
}


/*
 * This method will return true when the socket is echoing all
 * incoming data to std::cout as a debugging tool. This is useful
 * as it's sometimes nice to toggle the trace and negating this
 * give you ust that toggle.
 */
bool CKSocket::traceIncomingData() const
{
	return mTraceIncomingData;
}


/*
 * This method will return true when the socket is blocking for
 * transferred data and therefore busy. This might happen when
 * sending a large chunk to a busy system, or reading data in
 * a blocking mode.
 */
bool CKSocket::isBlockingForTransferredData() const
{
	return mIsBlockingForTransferredData;
}


/********************************************************
 *
 *                Connection Methods
 *
 ********************************************************/
/*
 * These methods are responsible for establishing the connection
 * to the correct host, port, service and protocol assuming certain
 * defaults. With no arguments, the method takes the already set
 * hostname, port, service and protocol and attempts to make the
 * connection. This is useful when resetablishing a connection, or
 * when the initialization of this class is done in several stages.
 * The version that takes hostname and port assumes default service
 * and protocol, and the full version of the method assumes nothing
 * but sets everything after the establishment of the connection.
 */
bool CKSocket::connect()
{
	return connect( getHostname(), getPort(), DEFAULT_SERVICE, DEFAULT_PROTOCOL );
}


bool CKSocket::connect( const std::string & aHost, int aPort )
{
	return connect( aHost, aPort, DEFAULT_SERVICE, DEFAULT_PROTOCOL );
}


bool CKSocket::connect( const std::string & aHost, int aPort, int aService, int aProtocol )
{
	bool				error = false;
	SOCKET				newSocket;
	struct hostent*		serverInfo;
	struct in_addr		serverAddr;
	struct sockaddr_in	socketINetAddr;

	// Try to create a new socket
	if (!error) {
		newSocket = socket (DEFAULT_DOMAIN, aService, aProtocol);
		if (newSocket == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::connect(const std::string &, int, int, int) - a "
				"socket handle could not be created and this likely points to "
				"a problem at the operating system level.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Resolve the server's IP address by name or number
	if (!error) {
		serverInfo = gethostbyname(aHost.c_str());
	    if (serverInfo == NULL) {
			/*
			 * We weren't successful, most likely because the name was a
			 * numerical IP address of the form "a.b.c.d". Take this IP
			 * address and try to convert it directly.
			 */
			unsigned long tmpIP = inet_addr(aHost.c_str());
			if (tmpIP == INADDR_NONE) {
				error = true;
				serverAddr.s_addr = 0x0;
				std::ostringstream	msg;
				msg << "CKSocket::connect(const std::string &, int, int, int) - an "
					"IP address for the host '" << aHost << "' could not be "
					"located. Please check the DNS entries to make sure the "
					"host is resolved properly.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				serverAddr.s_addr = tmpIP;
			}
		} else {
			/*
			 * We were successful at getting the host information
			 * structure and now just need to extract the numerical
			 * IP address from the structure. This is a very poor
			 * extraction technique, but at least it is cross-
			 * platform.
			 */
			memcpy(&serverAddr, serverInfo->h_addr, serverInfo->h_length);
		}
	}

	// set up the socket info to connect to the server
	if (!error) {
		// set up the socket connection parameters first
		socketINetAddr.sin_family = AF_INET;
		socketINetAddr.sin_port = htons(aPort);
		socketINetAddr.sin_addr.s_addr = serverAddr.s_addr;

		// Now try to connect this socket to the socket on the server
		if (::connect(newSocket,
						(struct sockaddr*)&socketINetAddr,
						sizeof(socketINetAddr)) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::connect(const std::string &, int, int, int) - the "
				"socket could not be connected to port " << aPort << " on the "
				"host '" << aHost << "'. Please make sure that the remote host "
				"is ready and capable of receiving connections on this port.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now if we are here with no error, then everything worked
	 * and we have to set the port, socket and hostname ivars
	 */
	if (!error) {
		setPort(aPort);
		setSocketHandle(newSocket);
		setHostname(aHost);
		doNotBlockForTransferredData();
	}

	/*
	 * Now we can say that we certainly aren't listening, and if
	 * there was an error, we aren't connected either.
	 */
	setActivelyListening(false);
	setConnectionEstablished(!error);

	return !error;
}


/*
 * This is a convenience method that allows the user to easily
 * determine whether or not the socket is set up as a connector
 * to a remote host, and if so, is it connected to the host.
 */
bool CKSocket::outgoingConnectionActive() const
{
	bool	outActive = false;

	if (isConnectionEstablished() && (getHostname().size() > 0)) {
		outActive = true;
	}

	return outActive;
}


/*
 * This is a convenience method that will easily allow the user
 * to determine if the socket is set up as a listener for remote
 * host connections, and if so, is it onnected to any host at
 * this time.
 */
bool CKSocket::incomingConnectionActive() const
{
	bool	inActive = false;

	if (isActivelyListening()) {
		inActive = true;
	}

	return inActive;
}


/*
 * This method cleanly shuts down the socket that this CKSocket
 * represents. This includes (possibly) unbinding the socket from
 * the file system as well as closing down all socket
 * communications and releaseing all communication-related
 * resources.
 */
void CKSocket::shutdownSocket()
{
	// if we have a valid socket handle, clean it up first
	if (getSocketHandle() != INVALID_SOCKET) {
		// first shut down the socket against any communication
		shutdown(getSocketHandle(), 2);
		// ...now close out the file handle for this guy
		::close(getSocketHandle());
		// finally, invalidate the ivar
		setSocketHandle(INVALID_SOCKET);
	}

	// ...and reset the class variables
	setConnectionEstablished(false);
	setActivelyListening(false);
}


/*
 * When the socket connection is no longer needed, this call
 * closes the connection and releases those system resources
 * back to the base operating system. It closes down the
 * connection in such a way that no further reads or writes
 * to that socket will be allowed.
 */
void CKSocket::closeConnection()
{
	shutdownSocket();
}


/********************************************************
 *
 *                Publishing/Listening Methods
 *
 ********************************************************/
/*
 * This method creates a listener on the specified port with the
 * appropriate service and protocol, and sets everything up in
 * the CKSocket to be ready to be connected to from other hosts.\n
 *
 * If port is 0 then the system is allowed to pick a socket
 * that is available and is used. In this case, the accessor method
 * getPort() is used to access what this newly created port
 * number is.
 */
bool CKSocket::createAndBindListener( int aPort, int aService, int aProtocol )
{
	bool				error = false;
	SOCKET				newSocket;
	struct sockaddr_in	socketINetAddr;
	int					verifiedPort = 0;

	// Start by trying to get a new socket
	if (!error) {
		newSocket = socket(DEFAULT_DOMAIN, aService, aProtocol);
		if (newSocket == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::createAndBindListener(int, int, int) - a "
				"socket handle could not be created. This typically points to "
				"a problem at the operating system level. Please look into it.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * We need to make sure that if this socket dies, that it doesn't
	 * keep a lock on the socket forever. To do that would mean that
	 * a restart on the same port would be impossible.
	 */
	if (!error) {
		int		flag = 1;
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR,
							(char*)&flag, sizeof(flag)) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::createAndBindListener(int, int, int) - the new "
				"socket handle could not be configured to release the port at "
				"death. This typically points to a problem at the operating "
				"system level. Please look into it.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to set up the IP address info for this socket
	 * and bind the new socket handle to this address. This
	 * includes the port number that may be chosen by the
	 * system, if it's 0.
	 */
	if (!error) {
		// set the address for this listener
		socketINetAddr.sin_family = DEFAULT_DOMAIN;
		socketINetAddr.sin_port = htons(aPort);
		// a '0' for the address below means "me"
		socketINetAddr.sin_addr.s_addr = 0;

		// now let's bind the socket to the filesystem
		if (bind(newSocket,
				(struct sockaddr*)&socketINetAddr,
				sizeof(socketINetAddr)) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::createAndBindListener(int, int, int) - the "
				"socket handle could not be bound to the address provided. This "
				"could mean that all socket addresses are taken. Please check "
				"on this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now get the parameters that were assigned by the system
	if (!error) {
		socklen_t	addrSize = sizeof(socketINetAddr);

		if (getsockname(newSocket,
						(struct sockaddr*)&socketINetAddr,
						&addrSize) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::createAndBindListener(int, int, int) - the "
				"socket parameters could not be read back after binding which "
				"is very important. This could indicate a filesystem problem "
				"and needs to be checked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// save the socket port that we were assigned or asked for
			verifiedPort = ntohs(socketINetAddr.sin_port);
		}
	}

	// Now set the socket up as a listener
	if (!error) {
		if (listen(newSocket, 1) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::createAndBindListener(int, int, int) - the "
				"socket could not be set into listen mode and this could "
				"indicate a possible operating system problem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// At this point, if we're error free, we have done it all
	if (!error) {
		setPort(verifiedPort);
		setSocketHandle(newSocket);
		setActivelyListening(true);
		setConnectionEstablished(false);
	}

	return !error;
}


/*
 * This method calls createAndBindListener() with a port number of 0,
 * which will allow the system to select an unused port. When
 * necessary, the port number can be obtained by a call to
 * getPort().
 */
bool CKSocket::createAndBindListener( int aService, int aProtocol )
{
	return createAndBindListener(0, aService, aProtocol);
}


/*
 * This method returns a CKSocket that is created by
 * accepting a connection request from a remote host on the socket's
 * listening port. This is only valid on a listening socket, and if
 * no connection request is available, the method will return NULL.
 * The owvership of this returned socket belongs to the caller and
 * it is responsible for calling delete on it.
 */
CKSocket *CKSocket::socketByAcceptingConnectionFromListener()
{
	bool			error = false;
	SOCKET			newSocketHandle = INVALID_SOCKET;
	struct sockaddr	newSocketAddr;
	CKSocket*		newSocket = NULL;

	// First, see if it makes sense to even look
	if (!error) {
		if (!isActivelyListening()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::socketByAcceptingConnectionFromListener() - the "
				"socket is not currently set up as a listener, and you have to "
				"do that before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Next, let's try to get the new socket handle for the incoming
	 * connection
	 */
	if (!error) {
		int	p = poll(getSocketHandle(), 1000 * getWaitForIncomingConnectionTimeout());
		if (p == POLL_OK) {
			// get the size of the empty socket address struct
			socklen_t	newSocketAddrLength = sizeof(newSocketAddr);
			// try to get a waiting connection
			newSocketHandle = accept(getSocketHandle(), &newSocketAddr,
										&newSocketAddrLength);
			if (newSocketHandle == INVALID_SOCKET) {
				// see if it's an error we can handle
				if (errno == EWOULDBLOCK) {
					/*
					 * OK, while this is an error what's likely to have
					 * happened is that a client connected and then dropped
					 * the connection before we could get to it. In this case
					 * there is no one there and so the best thing to do is
					 * to return a NULL. The easiest way to do that is to
					 * simply flag an error now.
					 */
					error = true;
				} else {
					// OK, this is a bad error that we can't handle easily
					error = true;
					std::ostringstream	msg;
					msg << "CKSocket::socketByAcceptingConnectionFromListener() - the "
						"socket indicated to us that there was something interesting "
						"happening on the socket, but when we went to establish the "
						"connection nothing was there. This is a serious socket level "
						"problem that needs to be looked into. The errno=" << errno <<
						":" << strerror(errno);
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			}
		} else if (p == POLL_TIMEOUT) {
			error = true;
			/*
			 * This should return a NULL but there's nothing exceptional about
			 * a timeout - it could be just the way things are supposed to
			 * happen a lot of the time.
			 */
		} else {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::socketByAcceptingConnectionFromListener() - a socket "
				"connection could not be established to a waiting host connection "
				"before being interrupted, or an error occurring. This may mean "
				"that no remote host was requesting a connection, or worse. "
				"(poll()=" << p << " & errno=" << errno << ")";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now let's try to make a CKSocket for return to the caller
	if (!error) {
		newSocket = new CKSocket();
		if (newSocket == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::socketByAcceptingConnectionFromListener() - a new "
				"CKSocket could not be created for returning to the caller. This "
				"is a serious memory allocation error.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// now stuff what we have into the new CKSocket
			newSocket->setPort(getPort());
			newSocket->setSocketHandle(newSocketHandle);
			newSocket->setReadBufferSize(getReadBufferSize());
			newSocket->setConnectionEstablished(true);
			newSocket->setTraceOutgoingData(traceOutgoingData());
			newSocket->setTraceIncomingData(traceIncomingData());
			newSocket->doNotBlockForTransferredData();
		}
	}

	return error ? NULL : newSocket;
}


/********************************************************
 *
 *                Data I/O Methods
 *
 ********************************************************/
/*
 * This is a method that, given that a socket connection
 * has been initiated by some means, will set the socket's
 * reception of out-of-band data to come in as in-band data
 * for reception as normal data.
 */
bool CKSocket::allowOutOfBandDataInBand()
{
	bool		error = false;
	int			onFlag = 1;

	if (!error) {
		if (getSocketHandle() == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::allowOutOfBandDataInBand() - no socket connection "
				"has been established, and that must come first.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	if (!error) {
		if (setsockopt(getSocketHandle(), SOL_SOCKET, SO_OOBINLINE,
						(char*)&onFlag, sizeof(onFlag)) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::allowOutOfBandDataInBand() - the socket "
				"connection's options could not be altered. This is a serious "
				"problem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This is a method that, given that a socket connection
 * has been initiated by some means, will set the socket's
 * reception of out-of-band data to not come in as
 * in-band data, but rather be kept as out-of-band data.
 */
bool CKSocket::disallowOutOfBandDataInBand()
{
	bool		error = false;
	int			offFlag = 0;

	if (!error) {
		if (getSocketHandle() == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::disallowOutOfBandDataInBand() - no socket "
				"connection has been established, and that must come first.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	if (!error) {
		if (setsockopt(getSocketHandle(), SOL_SOCKET, SO_OOBINLINE,
						(char*)&offFlag, sizeof(offFlag)) == SOCKET_ERROR) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::disallowOutOfBandDataInBand() - the socket "
				"connection's options could not be altered. This is a serious "
				"problem.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method sets the existing socket connection to block
 * (i.e. wait) for transferred data no matter how long it
 * takes for the data to "move". This is not usually a good
 * idea for reads, but an excellent idea for writes.
 *
 * The default for CKSocket is to be non-blocking reads
 * with the ability for users to configure timeouts, and
 * blocking writes.
 *
 * For NT, this routine is not necessary. The default
 * behavior for sockets und NT is what you'd expect: not
 * block on read, and buffer on send so that they have no
 * real need for any of this.
 */
bool CKSocket::blockForTransferredData()
{
	return setBlockingForTransferredData(true);
}


/*
 * This method allows the user to set the existing socket
 * connection to not block (i.e. wait) for transferred data.
 * This is the default behavior for CKSocket on reads
 * so that users can create configurable timeout schemes,
 * and not worry that their application will hang waiting
 * on the receipt of some particular data.
 *
 * On writes, CKSocket goes blocking so that the sent data
 * gets through.
 *
 * For NT, this routine is not necessary. The default
 * behavior for sockets und NT is what you'd expect: not
 * block on read, and buffer on send so that they have no
 * real need for any of this.
 */
bool CKSocket::doNotBlockForTransferredData()
{
	return setBlockingForTransferredData(false);
}


/*
 * This method takes an argument that indicates if the socket
 * should block (or not) for the transferred data. Primarily,
 * this is for reads, but it's possible taht very large writes
 * could block, if necessary. In any case, this method is
 * here in case you need it.
 */
bool CKSocket::setBlockingForTransferredData( bool aShouldBlock )
{
	bool		error = false;

	if (!error) {
		if (getSocketHandle() == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::setBlockingForTransferredData(bool) - no socket "
				"connection has been established, and that must come first.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	if (!error) {
#ifdef WIN32
		unsigned long	flag = (aShouldBlock ? 0 : 1);
		int		ret = ioctlsocket(getSocketHandle(), FIONBIO, &flag);
#else
		int		flags = fcntl(getSocketHandle(), F_GETFL);
		if (flags < 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::setBlockingForTransferredData(bool) - the flags "
				"for the socket could not be obtained and this is a serious "
				"problem that needs to be looked into. Errno=" << errno <<
				"(" << strerror(errno) << ")";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
		if (aShouldBlock) {
			flags &= ~O_NONBLOCK;
		} else {
			flags |= O_NONBLOCK;
		}
		int		ret = fcntl(getSocketHandle(), F_SETFL, flags);
#endif
		if (ret < 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::setBlockingForTransferredData(bool) - the socket "
				"connection's options could not be altered. This is a "
				"serious problem. Errno=" << errno << "(" << strerror(errno) <<
				")";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			setIsBlockingForTransferredData(aShouldBlock);
		}
	}

	return !error;
}


/*
 * When data needs to be sent out the socket port, these are the
 * methods to call. Each takes a different kind of input, but all
 * have the same effect of sending a byte stream out the socket
 * to the receiver.
 */
bool CKSocket::send( const char *aBuffer )
{
	return send(aBuffer, strlen(aBuffer));
}


bool CKSocket::send( const char *aBuffer, int aLength )
{
	bool		error = false;
	int			bytesRemaining = aLength;
	int 		bytesSent = 0;
	int 		indexOffset = 0;

	// first, make sure we have a socket to write to
	if (!error) {
		if (getSocketHandle() == INVALID_SOCKET) {
			// we certainly aren't on an established connection...
			setConnectionEstablished(false);
			// ...now log the error
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::send(const char*, int) - no socket "
				"connection has been established, and that must come first.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * See if we are supposed to be "verbose" at this time and trace all
	 * the outgoing data. If so, it's a simple write to std::cout.
	 */
	if (!error) {
		if (traceOutgoingData()) {
			std::cout << "Sending " << aLength << " bytes: " << aBuffer <<
				 std::endl;
		}
	}

	/*
	 * Now I need to try and send the entire message out to the
	 * socket. However, it may not all go, and the returned value
	 * will be the number of bytes actually sent.
	 */
	while ((bytesRemaining > 0) && (!error)) {
		bytesSent = ::send(getSocketHandle(), aBuffer + indexOffset, bytesRemaining, 0);

		/*
		 * If we had an error, see if it was just a possible blocking
		 * error that really isn't an error at all, only an indicator
		 */
		if ((bytesSent == SOCKET_ERROR) && (errno == EWOULDBLOCK)) {
			/*
			 * Try to send it blocking, but leave the socket
			 * as it was originally - non-blocking.
			 */
			blockForTransferredData();
			bytesSent = ::send(getSocketHandle(), aBuffer + indexOffset, bytesRemaining, 0);
			doNotBlockForTransferredData();
		}

		// ...then check for other general errors
		if (bytesSent == SOCKET_ERROR) {
			// create the message about what's happened and include the errno
			std::ostringstream	msg;
			msg << "CKSocket::send(const char*, int) - we had a socket error while "
				"trying to send the data. This is a serious problem. Errno=" <<
				errno << "(" << strerror(errno) << ")";
			// we certainly aren't on an established connection...
			setConnectionEstablished(false);
			// ...now flag the error and throw the exception
			error = true;
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// We sent something out, so update the remaining work
			bytesRemaining -= bytesSent;
			indexOffset += bytesSent;
		}
	}

	return !error;
}


bool CKSocket::send( const std::string & aString )
{
	return send(aString.c_str(), aString.size());
}


/*
 * When data needs to be read in from the socket, this is the
 * method to call. It does not wait for data at the socket, but
 * instead returns an empty string, if no data is available. This
 * allows the user to determine how to handle the error without
 * creating a blocking condition.
 */
std::string CKSocket::readAvailableData()
{
	bool			error = false;
	int				incomingSize = getReadBufferSize();
	char			incomingPtr[incomingSize];
	int				bytesRead = 0;

	// First, make sure we have a socket to read from
	if (!error) {
		if (getSocketHandle() == INVALID_SOCKET) {
			error = true;
			std::ostringstream	msg;
			msg << "CKSocket::readAvailableData() - no socket "
				"connection has been established, and that must come first.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// Now read what we can from the socket
	if (!error) {
		bytesRead = recv(getSocketHandle(), incomingPtr, (incomingSize - 1), 0);
		/*
		 * If we had an error, see if it was just a possible blocking
		 * error that really isn't an error at all, only an indicator
		 */
		if ((bytesRead == SOCKET_ERROR) && (errno == EWOULDBLOCK)) {
			// treat it as nothing was read
			bytesRead = 0;
		} else if (bytesRead == SOCKET_ERROR)	{
			// ...then check for other general errors
			error = true;
			// this is an internal error on the reading from the socket
			std::ostringstream	msg;
			msg << "CKSocket::readAvailableData() - there was an error reading "
				"from the socket. Return code: " << bytesRead << " errno:" <<
				errno << ".";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now that I have the character data from the socket, I need to
	 * NULL terminate it and then create a std::string out of it.
	 */
	if (!error) {
		// we thankfully left room at the very end
		incomingPtr[bytesRead] = '\0';
		// now log this out if we're supposed to be doing this
		if (traceIncomingData() && (bytesRead > 0)) {
			std::cout << "Received " << bytesRead << " bytes: " << incomingPtr <<
				 std::endl;
		}
	}

    return std::string(incomingPtr);
}


/*
 * This method waits for any data to be present at the socket
 * before returning. If a timeout is specified, then this method
 * will wait for a maximum of 'aTimeout' seconds before
 * returning false, indicating a timeout. If no argument is
 * specified then the default is DEFAULT_READ_TIMEOUT for
 * this class. If this method returns true then it means that
 * data appeared in the timeout interval.
 */
bool CKSocket::waitForData( float aTimeoutInSec )
{
	bool	retval = false;
	
	if (poll(getSocketHandle(), (int)(1000 * aTimeoutInSec)) == POLL_OK) {
		// something is there *before* the timeout
		retval = true;
	}
	
	return retval;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKSockets are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKSocket::operator==( const CKSocket & anOther ) const
{
	bool		equal = true;

	if ((getHostname() != anOther.getHostname()) ||
		(getPort() != anOther.getPort()) ||
		(getSocketHandle() != anOther.getSocketHandle()) ||
		(getReadBufferSize() != anOther.getReadBufferSize()) ||
		(getWaitForIncomingConnectionTimeout() !=
					anOther.getWaitForIncomingConnectionTimeout()) ||
		(isActivelyListening() != anOther.isActivelyListening()) ||
		(isConnectionEstablished() != anOther.isConnectionEstablished()) ||
		(traceOutgoingData() != anOther.traceOutgoingData()) ||
		(traceIncomingData() != anOther.traceIncomingData()) ||
		(isBlockingForTransferredData() !=
					anOther.isBlockingForTransferredData())) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKSockets are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKSocket::operator!=( const CKSocket & anOther ) const
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
std::string CKSocket::toString() const
{
	std::ostringstream	buff;

	buff << "< Host=" << getHostname() << ", " <<
		" Port=" << getPort() << ", " <<
		" SocketHandle=" << getSocketHandle() << ", " <<
		" ReadBufferSize=" << getReadBufferSize() << ", " <<
		" WaitForIncomingConnectionTimeout=" <<
					getWaitForIncomingConnectionTimeout() << " sec., " <<
		" ActivelyListening=" << (isActivelyListening() ? "Yes" : "No") << ", " <<
		" ConnectionEstablished=" << (isConnectionEstablished() ? "Yes" : "No") <<
					", " <<
		" traceOutgoingData=" << (traceOutgoingData() ? "Yes" : "No") << ", " <<
		" traceIncomingData=" << (traceIncomingData() ? "Yes" : "No") << ", " <<
		" isBlockingForTransferredData=" <<
					(isBlockingForTransferredData() ? "Yes" : "No") <<
		">" << std::endl;

	return buff.str();
}


/*
 * This method sets the operating system-level socket descriptor,
 * or handle, for this socket class. This is really only needed
 * by the methods in this class, but on the off chance that a sub
 * class will need it, we've kept it protected.
 */
void CKSocket::setSocketHandle( SOCKET aHandle )
{
	mSocketHandle = aHandle;
}


/*
 * This method is used by the methods in this class to set the status
 * of the flag that indicates if this socket is actively listening
 * for data coming from the remote host. There's not a lot of use for
 * this method by general clients, but a subclass might need it so
 * we've kept it protected.
 */
void CKSocket::setActivelyListening( bool aFlag )
{
	mActivelyListening = aFlag;
}


/*
 * This method is used by the methods in this class to set the status
 * of the 'connected' flag so that callers can know the stus of the
 * connection. This has very little value outside this class, and
 * can cause a lot of probelms if it's not used properly, so be
 * careful.
 */
void CKSocket::setConnectionEstablished( bool aFlag )
{
	mConnectionEstablished = aFlag;
}


/*
 * This method sets the flag indicating if the socket is blocking for
 * transferred data as it's important for the clients to be able to
 * find out what's happening. This doesn't have a lot of use outside
 * of the methods in this class, and misuse can be a very bad thing,
 * so please be careful with this.
 */
void CKSocket::setIsBlockingForTransferredData( bool aFlag )
{
	mIsBlockingForTransferredData = aFlag;
}


/*
 * This method returns the socket descriptor, or handle, at the
 * operating system-level of the socket that's currently established.
 * If there's no socket available, then this method will return
 * INVALID_SOCKET.
 */
const SOCKET CKSocket::getSocketHandle() const
{
	return mSocketHandle;
}


/*
 * This method is a wrapper for the system-level poll() method
 * that makes it a lot easier for the class' methods to wait for
 * something to happen on the socket. If the timeout is reached
 * with nothing happening, then this method returns POLL_TIMEOUT.
 * If the waiting was interrupted, then this method returns
 * POLL_INTERRUPT. And if something happened that we need to do
 * something about in the interval, POLL_OK is returned.
 */
int CKSocket::poll( int aFD, int aTimeoutInMillis, bool anEmptyIsError,
					int anEvents )
{
	int				retval = POLL_OK;
	static char		buff[128];

	// build up the structure we'll need to do the polling
	struct pollfd	fds;
	fds.fd		= aFD;
	fds.events	= anEvents;

	// now issue the system-level poll() call itself
	int	results = ::poll( &fds, 1, aTimeoutInMillis );
	if (((fds.revents & POLLERR) != 0) ||
		((fds.revents & POLLHUP) != 0) ||
		((fds.revents & POLLNVAL) != 0)) {
		retval = POLL_ERROR;
	} else if (((fds.revents & POLLIN) != 0) && anEmptyIsError) {
		/*
		 * An interesting thing happens with sockets... let's say that we have
		 * a connection from a client to this socket, and that client dies for
		 * whatever reason without closing things down properly. What will
		 * happen is that ::poll() will say data is ready to read when it
		 * really isn't. So how do we check? We peek at the data so as to
		 * preserve the data on the socket, but we need to see if there is
		 * really anything there. If not, then it's an error and flag it as
		 * such.
		 */
		int bytesRead = recv(aFD, buff, 127, MSG_PEEK);
		if (bytesRead <= 0) {
			retval = POLL_ERROR;
		}
	} else if (results == 0) {
		retval = POLL_TIMEOUT;
	} else if ((results < 0) && ((errno == EAGAIN) || (errno == EINTR))) {
		retval = POLL_INTERRUPT;
	} else if (results < 0) {
		retval = POLL_ERROR;
	}

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKSocket & aSocket )
{
	aStream << aSocket.toString();

	return aStream;
}
