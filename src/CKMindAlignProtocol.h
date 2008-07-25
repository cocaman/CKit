/*
 * CKMindAlignProtocol.h - this file defines the class that can be used as
 *                         a simple conduit to a MindAlign messaging server.
 *                         The basics of the IRC communication is handled
 *                         by the super class and you can register for all
 *                         incoming messages to be processed and return a
 *                         CKString as a reply. The specifics of the MindAlign
 *                         authentication scheme are handled in this class
 *                         and the other work is left to the super to deal
 *                         with. This is the core of the secure chat servers.
 *
 * $Id: CKMindAlignProtocol.h,v 1.3 2008/07/25 19:07:02 drbob Exp $
 */
#ifndef __CKMINDALIGNPROTOCOL_H
#define __CKMINDALIGNPROTOCOL_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKIRCProtocol.h"

//	Forward Declarations

//	Public Constants
/*
 * This is the standard MindAlign authentication server port address for
 * obtaining the authentication token that's necessary for connecting to
 * the MindAlign messaging server. While it's not as standard as, say,
 * FTP, it's still pretty common.
 */
#define	DEFAULT_AUTH_PORT			2323

/*
 * This is the string that we'll send to the authentication server
 * when we get connected to let it know what it is we're going to
 * be doing. We'll get the same thing back from the authentication
 * server and will check it to make sure.
 */
#define AUTH_HANDSHAKE		"VERSION:2.0;HANDSHAKE:NONE;CIPHER:NONE;METHOD:USER_PASS"

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKMindAlignProtocol :
	public CKIRCProtocol
{
	friend class CKIRCProtocolListener;
	friend class CKIRCProtocolExec;

	public:
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
		CKMindAlignProtocol();
		/*
		 * This method of the constructor takes the names of the
		 * authentication server and messaging server as well as the
		 * port numbers for each so that it can establish connections
		 * to both necessary services.
		 */
		CKMindAlignProtocol( const CKString & anAuthHost, int anAuthPort,
							 const CKString & aMesgHost, int aMesgPort );
		/*
		 * This form of the constructor is nice in that it not only connects
		 * to the authentication server and obtains the authentication token
		 * for the MindAlign messaging server, it sends all the necessary
		 * commands to establish a solid connection to the messaging server,
		 * and in so doing, allows the user to start sending messages right
		 * away.
		 */
		CKMindAlignProtocol( const CKString & anAuthHost, int anAuthPort,
							 const CKString & aMesgHost, int aMesgPort,
							 const CKString & aNick, const CKString & aPassword );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKMindAlignProtocol( const CKMindAlignProtocol & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKMindAlignProtocol();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKMindAlignProtocol & operator=( const CKMindAlignProtocol & anOther );

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
		void setAuthHostname( const CKString & aHost );
		/*
		 * This method is the setter for the port number that will
		 * be used to establish a communication port with
		 * the authentication server. Typically, this should be left
		 * as the default, but on certain rare occaisions, some
		 * authentication hosts may choose to listen for communications
		 * on a non-standard port.
		 */
		void setAuthPort( int aPort );
		/*
		 * This method is typically called by the other methods in this
		 * class when a valid authentication token has been received from
		 * the authentication server. However, if you want to set it to
		 * something else, good luck.
		 */
		void setToken( const CKString & aToken );

		/*
		 * Because the MindAlign Protocol is based on the CKTelnetConnection,
		 * we need to have a sense of the host with which we are
		 * communicating - and which host it is. This method is the
		 * standard getter accessor method for the authentication host
		 * name that will be used in all subsequent connections.
		 */
		const CKString & getAuthHostname() const;
		/*
		 * This method is the getter for the port number that will
		 * be used to establish a communication port with
		 * the authentication server. Typically, this is the default,
		 * but on certain rare occaisions, some authentication server
		 * may choose to listen for communications on a non-standard port.
		 */
		int getAuthPort() const;
		/*
		 * This method returns the token that has been obtained from the
		 * authentication server for the nickname and password that have
		 * been given to this class. If that exchange has not been done,
		 * then the returned string will simply be empty.
		 */
		const CKString & getToken() const;
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
		const CKTelnetConnection *getAuthComm() const;

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
		virtual bool connect( const CKString & aHost, int aPort );
		/*
		 * This method will log out any logged in user and break the
		 * established connection to the IRC host. This is useful
		 * when "shutting down" as it takes care of all the
		 * possibilities in one fell swoop.
		 */
		virtual void disconnect();

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
		bool operator==( const CKMindAlignProtocol & anOther ) const;
		/*
		 * This method checks to see if the two CKMindAlignProtocols are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKMindAlignProtocol & anOther ) const;
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
		 * This method is used to copy the parameters for the given connection
		 * to the connection that this instance is maintaining for all
		 * communications with the authentication server. It's important to
		 * note that this is a copy operation and not an assignment as you
		 * might think.
		 */
		void setAuthComm( const CKTelnetConnection & aConn );

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
		bool connectAuthServer();
		/*
		 * This method uses the instance's port number, and the host name
		 * provided to establish a connection to the authentication
		 * server. This is very common because the default port is the
		 * standard MindAlign authentication protocol port and so the
		 * user really only needs to specify the host and most
		 * connections will be made.
		 */
		bool connectAuthServer( const CKString & aHost );
		/*
		 * This method allows both the host name and port number to
		 * be specified for making the connection. This is used by
		 * the other connection mathods as it is the most general
		 * form of the function.
		 */
		bool connectAuthServer( const CKString & aHost, int aPort );
		/*
		 * This method simply returns the state of the authentication
		 * server communications port - in so far as it's establishment.
		 */
		bool isConnectedToAuthServer();
		/*
		 * This method will break any connection with the authentication
		 * server that might have been established.
		 */
		void disconnectAuthServer();

		/*
		 * This method is used to ask the already connected authentication
		 * server what the authentication token is for the supplied user
		 * and password. The returned value is the token itself.
		 *
		 * If the MindAlign authentication server does not accept this set
		 * of login credentials without error (save the failed login) then
		 * this method will return the empty string. Please check for this
		 * before assuming it's a valid token.
		 */
		CKString obtainToken( const CKString & aUser, const CKString & aPassword );
		/*
		 * This method is used to ask the provided authentication server
		 * connection what the authentication token is for the supplied user
		 * and password. The returned value is the token itself.
		 *
		 * If the MindAlign authentication server does not accept this set
		 * of login credentials without error (save the failed login) then
		 * this method will return the empty string. Please check for this
		 * before assuming it's a valid token.
		 */
		CKString obtainToken( CKTelnetConnection *aConn, const CKString & aUser, const CKString & aPassword );

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
		virtual void doAUTH( const CKString & aToken );
		/*
		 * This executes the standard IRC 'JOIN' command but because MindAlign
		 * is not as fast on it's feet as IRC, I need to be a little careful
		 * on the verification that we're in the channel before returning.
		 * This puts us in the channel on the server so that we can send
		 * messages to it.
		 */
		virtual void doJOIN( const CKString & aChannel );

	private:
		/*
		 * This is the hostname of the authentication server that we'll be
		 * using to obtain authentication tokens from.
		 */
		CKString					mAuthHostname;
		/*
		 * This is the port number on the host 'mAuthHostname' that we'll be
		 * talking to to receive authentication tokens. This defaults to 2323,
		 * but through the setters can be set to any value that's necessary.
		 */
		int							mAuthPort;
		/*
		 * This is the primary data conduit for this class' communications
		 * with the authentication server on the remote host. This will
		 * handle all the right socket-level details so that this class
		 * can focus on the higher-level protocol.
		 */
		CKTelnetConnection			mAuthComm;
		/*
		 * This is used in those circumstances where we really need to only
		 * have one thread working on the comm port at one time. Most
		 * notably these times are when sending data so that messages aren't
		 * garbled.
		 */
		CKFWMutex					mAuthCommMutex;
		/*
		 * This is the authentication token that is received from the
		 * authentication server in response to our query. It is matched
		 * to the nickname and password that have been supplied by the
		 * user.
		 */
		CKString					mToken;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMindAlignProtocol & aProtocol );

#endif	// __CKMINDALIGNPROTOCOL_H
