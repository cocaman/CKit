/*
 * CKSocket.h - this file defines the class that can be used as a
 *              general wrapper on the unix-level sockets for the user. The
 *              idea is that low-level socket functionality is nice, but in
 *              order to be more generally useful, we need more advanced
 *              features and more object-oriented behaviors.
 *
 * $Id: CKSocket.h,v 1.1 2003/11/21 18:08:11 drbob Exp $
 */
#ifndef __CKSOCKET_H
#define __CKSOCKET_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#ifdef WIN32
#include <winsock.h>
#else
#ifdef __MACH__
#include <libc.h>
#endif
#include <sys/errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants
/*
 * This is the default read buffer size for a CKSocket.
 * It can be easily overwritten, but there needs to be a
 * simple default for the easiest users.
 */
#define	DEFAULT_READ_BUFFER_SIZE	4096
/*
 * As with anything related to sockets, we need to have some sense of a
 * read timeout. In this case, we need to have a total read timeout value
 * in seconds. This, of course, can be overridden, but we need to have
 * something that's going to work for the majority of the uses of this
 * class.
 */
#define	DEFAULT_READ_TIMEOUT		30.0
/*
 * This is the default timeout value for waiting for an
 * incoming connection while set up as a listener. In the
 * method socketByAcceptingConnectionFromListener() we need
 * to wait for a connection on the possibility that the
 * remote host isn't as fast as we are to create the
 * connection. These timeouts are much smaller than any
 * data reading timeouts as they are specifically focused on
 * the establishment of the connection and not reading or
 * writing any data.
 */
#define	DEFAULT_WAIT_FOR_INCOMING_TIMEOUT 			10
/*
 * Because the platforms are slightly different, we might as
 * well set up the default socket creation parameters - domain,
 * service and protocol. These can be overridden, of course,
 * but we need defaults that make sense.
 */
#ifdef __MACH__
#define	DEFAULT_DOMAIN				AF_INET
#define	DEFAULT_SERVICE				SOCK_STREAM
#define	DEFAULT_PROTOCOL			0
#else
#define	DEFAULT_DOMAIN				PF_INET
#define	DEFAULT_SERVICE				SOCK_STREAM
#define	DEFAULT_PROTOCOL			0
#endif
/*
 * These are the different return values from our poll() method that
 * nicely wraps the system-level poll() method and makes it a lot
 * easier for the class methods to wait for things to happen on the
 * sockets.
 */
#define	POLL_OK						0
#define	POLL_ERROR					-100
#define POLL_TIMEOUT				-200
#define	POLL_INTERRUPT				-300

//	Public Datatypes
/*
 * Because we'll be doing a lot with the sockets, we might as well be
 * cross-platform and allow for the possibility that we'll be building
 * this on NT/XP (...shudder...), but if not, make sure to include the
 * right things to assist in the socket workings.
 */
#ifdef WIN32
#define EWOULDBLOCK		WSAEWOULDBLOCK
#else
typedef	int SOCKET;
#define	INVALID_SOCKET	-1
#define	SOCKET_ERROR	-1
#endif

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKSocket
{
	public:
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
		CKSocket();
		/*
		 * This form of the constructor takes the port number and the
		 * user still needs to supply the hostname with which to connect.
		 * this has somewhat limited use, but in the CKit version of this
		 * class I had this constructor, and I'm guessing there was a
		 * reason for it.
		 */
		CKSocket( int aPort );
		/*
		 * This form of the constructor is nice in that it takes all you
		 * need to establish a connection to the remote system - hostname
		 * and port number. As a nice benefit, this constructor attempts
		 * to establish that connection so that it's ready to send or
		 * receive data after a successful return.
		 */
		CKSocket( const std::string & aHost, int aPort );
		/*
		 * This form of the constructor is the most detailed in that it
		 * allows the user to specify not only the hostname and port
		 * number, but also the service and protocol that this socket will
		 * be using. This is important in certain specific applications,
		 * but for the most part, isn't required for general communications.
		 */
		CKSocket( const std::string & aHost, int aPort, int aService, int aProtocol );
		/*
		 * This form of the constructor sets up the CKSocket into a state
		 * that it is ready to receive connections from other hosts. In
		 * this case, a socket is created, bound to the filesystem,
		 * interrogatted as to its parameters, and then set to
		 * listen mode for incoming connections.
		 */
		CKSocket( int aService, int aProtocol );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKSocket( const CKSocket & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKSocket();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKSocket & operator=( const CKSocket & anOther );

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
		void setHostname( const std::string & aHostname );
		/*
		 * This method sets the port number that this socket will try to
		 * connect to the next time the connect() method is called. Of
		 * course, the other forms of the connect() method contain the
		 * port number, and those will call this method to set the port
		 * after the connection is made.
		 */
		void setPort( int aPort );
		/*
		 * This method sets the read buffer size for subsequent reads
		 * from the socket. This is important becase a buffer that's too
		 * small might mean buffer overrun errors, but one too large
		 * will be a waste of space. In general, the default is a good
		 * compromise.
		 */
		void setReadBufferSize( int aSize );
		/*
		 * This method sets the timeout that will be used when remote
		 * hosts are initiating connections back to this box. This is
		 * not nearly as long as the data transfer timeouts, but then
		 * again, we're only talking about initiating a connection and
		 * not pushing bits around.
		 */
		void setWaitForIncomingConnectionTimeout( int aTimeInSec );
		/*
		 * If this method is called with a 'true' argument, then the
		 * outgoing data to the socket will be logged to the std::cout
		 * for debugging purposes. This is nice in come cases to see what's
		 * really going on between the processes doing the communications.
		 */
		void setTraceOutgoingData( bool aFlag );
		/*
		 * If this method is called with a 'true' argument, then the
		 * incoming data to the socket will be logged to the std::cout
		 * for debugging purposes. This is nice in come cases to see what's
		 * really going on between the processes doing the communications.
		 */
		void setTraceIncomingData( bool aFlag );

		/*
		 * This method returns the hostname that the next connection will
		 * be made to, or the currently established connection. The
		 * difference lies soley with the nature of the usage of this
		 * class - call this after a connection and it's the active
		 * hostname.
		 */
		const std::string getHostname() const;
		/*
		 * This method returns the port number that the next connection will
		 * be made on, or the currently established connection. The difference
		 * is again, only in the usage.
		 */
		int getPort() const;
		/*
		 * This method returns the current read buffer size for the socket
		 * level reads.
		 */
		int getReadBufferSize() const;
		/*
		 * This method returns the timeout (in seconds) for an incoming
		 * socket connection to be established from a remote host.
		 */
		int getWaitForIncomingConnectionTimeout() const;
		/*
		 * This method will return 'true' if the socket is currently
		 * actively listening on the port. This is useful in determining
		 * if the socket is busy waiting for something from the remote
		 * host.
		 */
		bool isActivelyListening() const;
		/*
		 * This method returns true if the socket connection is established
		 * and ready to use. It's used a lot in the methods for this class
		 * to make sure that the socket is in a reasonable state before
		 * doing something like sending or reading data.
		 */
		bool isConnectionEstablished() const;
		/*
		 * This method will return true when the socket is echoing all
		 * outgoing data to std::cout as a debugging tool. This is useful
		 * as it's sometimes nice to toggle the trace and negating this
		 * give you ust that toggle.
		 */
		bool traceOutgoingData() const;
		/*
		 * This method will return true when the socket is echoing all
		 * incoming data to std::cout as a debugging tool. This is useful
		 * as it's sometimes nice to toggle the trace and negating this
		 * give you ust that toggle.
		 */
		bool traceIncomingData() const;
		/*
		 * This method will return true when the socket is blocking for
		 * transferred data and therefore busy. This might happen when
		 * sending a large chunk to a busy system, or reading data in
		 * a blocking mode.
		 */
		bool isBlockingForTransferredData() const;

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
		bool connect();
		bool connect( const std::string & aHost, int aPort );
		bool connect( const std::string & aHost, int aPort, int aService, int aProtocol );
		/*
		 * This is a convenience method that allows the user to easily
		 * determine whether or not the socket is set up as a connector
		 * to a remote host, and if so, is it connected to the host.
		 */
		bool outgoingConnectionActive() const;
		/*
		 * This is a convenience method that will easily allow the user
		 * to determine if the socket is set up as a listener for remote
		 * host connections, and if so, is it onnected to any host at
		 * this time.
		 */
		bool incomingConnectionActive() const;
		/*
		 * This method cleanly shuts down the socket that this CKSocket
		 * represents. This includes (possibly) unbinding the socket from
		 * the file system as well as closing down all socket
		 * communications and releasing all communication-related
		 * resources.
		 */
		void shutdownSocket();
		/*
		 * When the socket connection is no longer needed, this call
		 * closes the connection and releases those system resources
		 * back to the base operating system. It closes down the
		 * connection in such a way that #no further reads or writes
		 * to that socket will be allowed.
		 */
		void closeConnection();

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
		bool createAndBindListener( int aPort, int aService, int aProtocol );
		/*
		 * This method calls createAndBindListener() with a port number of 0,
		 * which will allow the system to select an unused port. When
		 * necessary, the port number can be obtained by a call to
		 * getPort().
		 */
		bool createAndBindListener( int aService, int aProtocol );
		/*
		 * This method returns a CKSocket that is created by
		 * accepting a connection request from a remote host on the socket's
		 * listening port. This is only valid on a listening socket, and if
		 * no connection request is available, the method will return NULL.
		 * The owvership of this returned socket belongs to the caller and
		 * it is responsible for calling delete on it.
		 */
		CKSocket *socketByAcceptingConnectionFromListener();

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
		bool allowOutOfBandDataInBand();
		/*
		 * This is a method that, given that a socket connection
		 * has been initiated by some means, will set the socket's
		 * reception of out-of-band data to not come in as
		 * in-band data, but rather be kept as out-of-band data.
		 */
		bool disallowOutOfBandDataInBand();
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
		bool blockForTransferredData();
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
		bool doNotBlockForTransferredData();
		/*
		 * This method takes an argument that indicates if the socket
		 * should block (or not) for the transferred data. Primarily,
		 * this is for reads, but it's possible that very large writes
		 * could block, if necessary. In any case, this method is
		 * here in case you need it.
		 */
		bool setBlockingForTransferredData( bool aShouldBlock );
		/*
		 * When data needs to be sent out the socket port, these are the
		 * methods to call. Each takes a different kind of input, but all
		 * have the same effect of sending a byte stream out the socket
		 * to the receiver.
		 */
		bool send( const char *aBuffer, int aLength );
		bool send( const std::string & aString );
		/*
		 * When data needs to be read in from the socket, this is the
		 * method to call. It does not wait for data at the socket, but
		 * instead returns an empty string, if no data is available. This
		 * allows the user to determine how to handle the error without
		 * creating a blocking condition.
		 */
		std::string readAvailableData();
		/*
		 * This method waits for any data to be present at the socket
		 * before returning. If a timeout is specified, then this method
		 * will wait for a maximum of 'aTimeout' seconds before
		 * returning false, indicating a timeout. If no argument is
		 * specified then the default is DEFAULT_READ_TIMEOUT for
		 * this class. If this method returns true then it means that
		 * data appeared in the timeout interval.
		 */
		bool waitForData( float aTimeoutInSec = DEFAULT_READ_TIMEOUT );

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
		bool operator==( const CKSocket & anOther ) const;
		/*
		 * This method checks to see if the two CKSockets are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKSocket & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString() const;

	protected:
		/*
		 * This method sets the operating system-level socket descriptor,
		 * or handle, for this socket class. This is really only needed
		 * by the methods in this class, but on the off chance that a sub
		 * class will need it, we've kept it protected.
		 */
		void setSocketHandle( SOCKET aHandle );
		/*
		 * This method is used by the methods in this class to set the status
		 * of the flag that indicates if this socket is actively listening
		 * for data coming from the remote host. There's not a lot of use for
		 * this method by general clients, but a subclass might need it so
		 * we've kept it protected.
		 */
		void setActivelyListening( bool aFlag );
		/*
		 * This method is used by the methods in this class to set the status
		 * of the 'connected' flag so that callers can know the stus of the
		 * connection. This has very little value outside this class, and
		 * can cause a lot of probelms if it's not used properly, so be
		 * careful.
		 */
		void setConnectionEstablished( bool aFlag );
		/*
		 * This method sets the flag indicating if the socket is blocking for
		 * transferred data as it's important for the clients to be able to
		 * find out what's happening. This doesn't have a lot of use outside
		 * of the methods in this class, and misuse can be a very bad thing,
		 * so please be careful with this.
		 */
		void setIsBlockingForTransferredData( bool aFlag );

		/*
		 * This method returns the socket descriptor, or handle, at the
		 * operating system-level of the socket that's currently established.
		 * If there's no socket available, then this method will return
		 * INVALID_SOCKET.
		 */
		const SOCKET getSocketHandle() const;

		/*
		 * This method is a wrapper for the system-level poll() method
		 * that makes it a lot easier for the class' methods to wait for
		 * something to happen on the socket. If the timeout is reached
		 * with nothing happening, then this method returns POLL_TIMEOUT.
		 * If the waiting was interrupted, then this method returns
		 * POLL_INTERRUPT. And if something happened that we need to do
		 * something about in the interval, POLL_OK is returned.
		 */
		int poll( int aFD, int aTimeoutInMillis, int anEvents = POLLIN );

	private:
		/*
		 * This is the name of the host that this connection is going to be
		 * with - if it hasn't already been established. In the case of a
		 * reconnection, this is where it'll be made to, and so it needs to
		 * be changed in concert with the disconnection and connection
		 * methods to point to a different machine.
		 */
		std::string			mHostname;
		/*
		 * This is the port number that this connection is going to be with
		 * on the host 'mHostname'. This is also important as it's the other
		 * important component of the connection description.
		 */
		int					mPort;
		/*
		 * This is the OS-level socket handle that we'll get from the low-
		 * level routines and we'll have to use in all the references to the
		 * socket with the operating system.
		 */
		SOCKET				mSocketHandle;
		/*
		 * This is the read buffer size used in each of the reads that we'll
		 * be doing from the socket. This can be set, and will have a default
		 * but a reasonable size is good enough for most socket-level
		 * communications.
		 */
		int					mReadBufferSize;
		/*
		 * This is the time that we'll wait for a remote host to establish
		 * a connection *to* us, and since it's possible we're faster than
		 * the other machine, we'll have to have a reasonable timeout, but
		 * not too long as this is only the timeout for the connection - not
		 * the transfer of any data.
		 */
		int					mWaitForIncomingConnectionTimeout;
		/*
		 * This indicates if the socket is actively in the listening mode.
		 * This is nice to know because it indicates the "flow" of the
		 * socket at this time and thus makes it easier to know what's
		 * going on with the socket.
		 */
		bool				mActivelyListening;
		/*
		 * This will be 'true' when there is an active connection with the
		 * socket.
		 */
		bool				mConnectionEstablished;
		/*
		 * The socket can trace incoming, outgoing, or both data streams
		 * as the user sees fit. This is nice because you might only want
		 * to trace outgoing data and not incoming, as it would be far to
		 * much data to manage. These can be set at any time and take
		 * effect immediately.
		 */
		bool				mTraceOutgoingData;
		bool				mTraceIncomingData;
		/*
		 * This will be set when the socket is blocking for data from the
		 * remote host. This is not necessarily the best thing to do in all
		 * cases, but when it's necessary, this flag will indicate that
		 * we are blocking and not just waiting to poll again.
		 */
		bool				mIsBlockingForTransferredData;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKSocket & aSocket );

#endif	// __CKSOCKET_H
