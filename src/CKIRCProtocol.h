/*
 * CKIRCProtocol.h - this file defines the class that can be used as a
 *                   simple conduit to a simple IRC server. The basics of
 *                   the IRC communication is handled by this class and you
 *                   can register for all incoming messages to be processed
 *                   and return a CKString as a reply. This is the core
 *                   of the chat servers.
 *
 * $Id: CKIRCProtocol.h,v 1.8 2004/09/20 16:19:33 drbob Exp $
 */
#ifndef __CKIRCPROTOCOL_H
#define __CKIRCPROTOCOL_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <list>
#include <vector>

//	Third-Party Headers

//	Other Headers
#include "CKTelnetConnection.h"
#include "CKFWMutex.h"
#include "CKString.h"

//	Forward Declarations
class CKIRCProtocolListener;
class CKIRCResponder;

//	Public Constants
/*
 * This is the standard IRC port address for the standard IRC protocol
 * implementation. While it's not as standard as, say, FTP, it's still
 * pretty common the that primary IRC server on a box is at this address.
 */
#define	DEFAULT_IRC_PORT			6667
/*
 * Because we'll have a thread listening to the data coming from the IRC
 * server, we need to have a concept of a timeout for these reads. The
 * tricky point about these reads is that we don't want to lock up the
 * thread for too long, but at the same time, we don't want to spend an
 * inordinate amount of time servicing the thread. There needs to be a
 * compromise. What we've chosen as a default is a reasonable balance
 * between how often we'll be waiting and how often we'll be needing to
 * service the thread.
 */
#define DEFAULT_IRC_READ_TIMEOUT	3.0
/*
 * Each 'user' of IRC needs to have a unique 'nickname' as well as a 'real
 * name' that they need to register with the IRC server. Because we can't
 * *NOT* have something, we have to have defaults. In reality, the user
 * needs to set these to be specific for their usage.
 */
#define	DEFAULT_NICKNAME			"CKIRCProtocol"
#define	DEFAULT_REALNAME			"CKit IRC User"
#define	DEFAULT_USER_HOST			"host"
#define	DEFAULT_USER_SERVER			"server"
/*
 * There's a limit to the IRC message length, and while it might seem that
 * 512 is a logical length, it's really 500 I guess to give them a little
 * buffer on the message. In any case, if a message is longer than this,
 * then we'll break it up on sending at word boundaries.
 */
#define MAX_MESSAGE_LEN				500

#ifdef __linux__
/*
 * On Linux, there is no standard definition for the maximum length of
 * a file name. So, in order to make the code as transportable as possible
 * we'll adopt the Solaris/Darwin definition and use that.
 */
#define MAXHOSTNAMELEN			255
#endif

//	Public Datatypes
/*
 * The way we'll be passing messages from the other chat users to the
 * clients of this class is through the following structure. The idea is
 * that the person (by nickname) along with what they said needs to be
 * kept in context with the response generated by the client(s) of this
 * protocol. By doing this, we make the processing far more dynamic as
 * it's only single message context and the data is in the structure.
 * This means the 'client' can respond to many more messages without
 * having to worry about threading problems to maintain context.
 */
typedef struct CKIRCIncomingMessageBlock {
	// this is the user's nickname that the message is coming from
	CKString			userNickname;
	// ...and this is the message they have typed
	CKString			message;
	// ...and the receiver should fill this with the message to send back
	CKString			response;
} CKIRCIncomingMessage;

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKIRCProtocol
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
		CKIRCProtocol();
		/*
		 * This method of the constructor takes a name of a server and the
		 * port to communicate on for IRC messages. This is a very common
		 * form of the constructor because it creates the protocol object
		 * and connects to a specific IRC server.
		 */
		CKIRCProtocol( const CKString & aHost, int aPort );
		/*
		 * This form of the constructor is nice in that it not only connects
		 * to the IRC server, it sends the necessary commands to establish a
		 * solid connection to the server, and in so doing, allows the user
		 * to start sending messages right away.
		 */
		CKIRCProtocol( const CKString & aHost, int aPort,
					   const CKString & aNick );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKIRCProtocol( const CKIRCProtocol & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKIRCProtocol();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKIRCProtocol & operator=( const CKIRCProtocol & anOther );

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
		void setHostname( const CKString & aHost );
		/*
		 * This method is the setter for the port number that will
		 * be used to establish a communication port with
		 * the server. Typically, this should be left as the default,
		 * but on certain rare occaisions, some server may choose
		 * to listen for IRC communications on a non-standard port.
		 */
		void setPort( int aPort );
		/*
		 * This method sets the password we'll be using in all communications
		 * with the IRC server.
		 */
		void setPassword( const CKString & aPassword );
		/*
		 * This method sets the nickname we'll be using in all
		 * communications with the IRC server.
		 */
		void setNickname( const CKString & aNick );
		/*
		 * This method sets the USER host we'll be using in all
		 * communications with the IRC server.
		 */
		void setUserHost( const CKString & aHost );
		/*
		 * This method sets the USER server we'll be using in all
		 * communications with the IRC server.
		 */
		void setUserServer( const CKString & aServer );
		/*
		 * This method sets the real name we'll be using in all
		 * communications with the IRC server.
		 */
		void setRealName( const CKString & aName );

		/*
		 * Because the IRC Protocol is based on the CKTelnetConnection,
		 * we need to have a sense of the host with which we are
		 * communicating - and which host it is. This method is the
		 * standard getter accessor method for the host name that
		 * will be used in all subsequent connections.
		 */
		const CKString getHostname() const;
		/*
		 * This method is the getter for the port number that will
		 * be used to establish a communication port with
		 * the server. Typically, this is the default, but on certain
		 * rare occaisions, some server may choose to listen for IRC
		 * communications on a non-standard port.
		 */
		int getPort() const;
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
		const CKTelnetConnection *getCommPort() const;
		/*
		 * If a valid connection is made to the server, this
		 * method will return true. It is very useful to see if the
		 * instance is ready to take commands or messages.
		 */
		bool isLoggedIn() const;
		/*
		 * This method returns the password we'll be using in all
		 * communications with the IRC server.
		 */
		const CKString getPassword() const;
		/*
		 * This method returns the nickname we'll be using in all
		 * communications with the IRC server.
		 */
		const CKString getNickname() const;
		/*
		 * This method returns the USER host we'll be using in all
		 * communications with the IRC server.
		 */
		const CKString getUserHost() const;
		/*
		 * This method returns the USER server we'll be using in all
		 * communications with the IRC server.
		 */
		const CKString getUserServer() const;
		/*
		 * This method returns the real name we'll be using in all
		 * communications with the IRC server.
		 */
		const CKString getRealName() const;
		/*
		 * This method returns a pointer to a std::list of CKStrings that
		 * is the list of Channels that this IRC Connection has JOINed. Note
		 * that this method will not return a NULL as it's a pointer to the
		 * instance variable and therefore should also not be released, etc.
		 * If you want to make a copy, do so, but otherwise, leave this guy
		 * alone.
		 */
		const std::list<CKString>	*getChannelList() const;
		/*
		 * This method returns a pointer to the listener thread that is
		 * going to be listening to the incoming data from the IRC server.
		 * This is a pointer to the actual instance variable, so please be
		 * very careful with it, and if you want to keep it around, please
		 * make a copy.
		 */
		CKIRCProtocolListener *getListener() const;

		/*
		 * This method will return true if the supplied channel name is
		 * among the list of channels that this instance has already
		 * JOINed. This is important, as you only need to JOIN a channel
		 * once per connection to the IRC server, and to do mroe than
		 * once is wasting bandwidth.
		 */
		bool isChannelInChannelList( const CKString & aChannel );

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
		bool connect();
		/*
		 * This method uses the instance's port number, and the host name
		 * provided to establish a connection for this IRC session.
		 * This is very common because the default port is the standard
		 * IRC protocol port and so the user really only needs to
		 * specify the host and most connections will be made.
		 */
		bool connect( const CKString & aHost );
		/*
		 * This method allows both the host name and port number to
		 * be specified for making the connection. This is used by
		 * the other connection mathods as it is the most general
		 * form of the function.
		 */
		bool connect( const CKString & aHost, int aPort );
		/*
		 * This method simply returns the state of the IRC
		 * communications port - in so far as it's establishment.
		 */
		bool isConnected();
		/*
		 * This method will log out any logged in user and break the
		 * established connection to the IRC host. This is useful
		 * when "shutting down" as it takes care of all the
		 * possibilities in one fell swoop.
		 */
		void disconnect();

		/********************************************************
		 *
		 *            IRC Commands - Object Model
		 *
		 ********************************************************/
		/*
		 * This is a simple cover method for the sending of a message to the
		 * IRC server. The 'aDest' can be a channel or a user.
		 */
		void sendMessage( const CKString & aDest, const CKString & aMsg );

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
		virtual bool isReflexChat( CKString & aLine );

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
		void addToResponders( CKIRCResponder *anObj );
		/*
		 * When you are wishing to de-register from the 'alerting' process,
		 * call this method and the supplied method will be removed from the
		 * list of methods that will be called when an incoming chat arrives.
		 */
		void removeFromResponders( CKIRCResponder *anObj );
		/*
		 * This method removes *ALL* responders and is very useful in the
		 * clean-up phase, but also can be useful in a restart-phase.
		 */
		void removeAllResponders();

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
		bool operator==( const CKIRCProtocol & anOther ) const;
		/*
		 * This method checks to see if the two CKIRCProtocols are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKIRCProtocol & anOther ) const;
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
		 * communications with the IRC Server. It's important to note that
		 * this is a copy operation and not an assignment as you might think.
		 */
		void setCommPort( const CKTelnetConnection & aConn );
		/*
		 * When the class methods connect to the server, they call
		 * this method to update the status of the connection to
		 * the host. It's important that no one outside this class
		 * manipulate this variable, as it's integrity is vital to
		 * the operation of this class.
		 */
		void setIsLoggedIn( bool aFlag );
		/*
		 * This method sets the contents of this instance's list of JOINed
		 * channels to the same contents as the supplied list. This is done
		 * as a copy and not as an assumption of the memory management of
		 * the elements of the list.
		 */
		void setChannelList( const std::list<CKString> & aList );
		/*
		 * This method sets the pointer to this instance's listener to the
		 * passed-in value. The memory management of this listener will then
		 * fall to this instance and it will be cleaned up when the time is
		 * right.
		 */
		void setListener( CKIRCProtocolListener *aListener );

		/*
		 * This method adds the supplied channel name to the list of
		 * JOINed channels for this instance. It is only added, of course,
		 * if the channel does not already exist in the list.
		 */
		void addToChannelList( const CKString & aChannel );
		/*
		 * This method clears out all the channels that are currently
		 * JOINed to. This is necessary at times, such as the reconnection
		 * to the chat server, when you need to start fresh.
		 */
		void clearChannelList();

		/*
		 * This method is used by several methods to try and start the
		 * Listener thread. There are a few steps to it, so it's nice to
		 * have it in one place as opposed to copying code.
		 */
		void startListener();
		/*
		 * This method is used by several methods to try and stop the
		 * Listener thread. There are a few steps to it, so it's nice to
		 * have it in one place as opposed to copying code.
		 */
		void stopListener();

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
		CKString getReply();
		/*
		 * This method checks for the reply from the IRC server through
		 * the socket. If there's not a valid reply on the socket, it
		 * returns false. This method waits for nothing and no one.
		 */
		bool checkForReply();
		/*
		 * This method is used by the protocol listener to make sure that
		 * all the registered responders of this class get a chance to respond
		 * to this incoming chat message. It basically cycles through all the
		 * registered responders, and calls them with the message reference
		 * so that they can modify it, if necessary. Once all of them are
		 * done, this method returns and the listener can send the response
		 * back to the source as a private message.
		 */
		bool alertAllResponders( CKIRCIncomingMessage & aMsg );

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
		void executeCommand( const CKString & aCmd );
		/*
		 * This executes the standard IRC 'PASS' command on the communication
		 * channel to the remote host. This is meant to supply a password
		 * to the IRC server to ensure at least some level of security.
		 */
		void doPASS( const CKString & aPassword );
		/*
		 * This executes the standard IRC 'NICK' command on the communication
		 * channel to the remote host. This is meant to supply the requested
		 * nickname to the IRC server so that everyone knows who this is coming
		 * from.
		 */
		void doNICK( const CKString & aNick );
		/*
		 * This executes the standard IRC 'USER' command on the communication
		 * channel to the remote host. This is meant to supply real
		 * information about the user to the IRC server.
		 */
		void doUSER( const CKString & aNick,
					 const CKString & aHost,
					 const CKString & aServer,
					 const CKString & aRealName );
		/*
		 * This executes the standard IRC 'QUIT' command on the communication
		 * channel to the remote host. This is meant to log off this
		 * connection and leave a message on the way out.
		 */
		void doQUIT( const CKString & aMsg );
		/*
		 * This executes the standard IRC 'JOIN' command on the communication
		 * channel to the remote host. This puts us in the channel on the
		 * server so that we can send messages to it.
		 */
		void doJOIN( const CKString & aChannel );
		/*
		 * This executes the standard IRC 'PRIVMSG' command on the
		 * communication channel to the remote host. This sends a private
		 * message to the supplied user or channel and includes a return
		 * code - the doNOTICE is different in that no return code is sent.
		 */
		void doPRIVMSG( const CKString & aDest, const CKString & aMsg );
		/*
		 * This executes the standard IRC 'NOTICE' command on the
		 * communication channel to the remote host. This is similar to
		 * doPRIVMSG() but here we do NOT get a reply from the IRC server.
		 */
		void doNOTICE( const CKString & aDest, const CKString & aMsg );
		/*
		 * This execute the PONG command that is used in response to the PING
		 * sent from the IRC Server. This lets the server know that I'm alive
		 * and listening...
		 */
		void doPONG();

	private:
		/*
		 * This is the hostname of the IRC (Chat) server that we'll be
		 * using to send and receive messages.
		 */
		CKString					mHostname;
		/*
		 * This is the port number on the host 'mHostname' that we'll be
		 * talking to to send and receive messages. This defaults to 6667,
		 * but through the setters can be set to any value that's necessary.
		 */
		int							mPort;
		/*
		 * This is the primary data conduit for this class' communications
		 * with the chat server on the remote host. This will handle all the
		 * right socket-level details so that this class can focus on the
		 * higher-level protocol.
		 */
		CKTelnetConnection			mCommPort;
		/*
		 * This is used in those circumstances where we really need to only
		 * have one thread working on the comm port at one time. Most 
		 * notably these times are when sending data so that messages aren't
		 * garbled.
		 */
		CKFWMutex					mCommPortMutex;
		/*
		 * Since you can log into a Chat server, we have this flag set up
		 * so that the methods can update this status, and the accessor
		 * methods can make this visible to the user.
		 */
		bool						mIsLoggedIn;
		/*
		 * These are the IRC (Chat) parameters for describing the user.
		 * They are, in order, the pawword, nick name (primary chat ID),
		 * user's host machine, user's server, and real-life name of the
		 * person.
		 */
		CKString					mPassword;
		CKString					mNickname;
		CKString					mUserHost;
		CKString					mUserServer;
		CKString					mRealName;
		/*
		 * This is a vector of strings that are the channels that this
		 * instance has JOINed in the course of the messages getting sent
		 * through it. This is useful because if a message comes in and it's
		 * destined for a channel, there's no need to JOIN a channel that we
		 * have already joined. Each new connection will need to reset this
		 * list as it isn't persistent on the server.
		 */
		std::list<CKString>			mChannelList;
		// ...and this is the mutex for it to control access
		CKFWMutex					mChannelListMutex;
		/*
		 * This is the listening thread that will continually monitor the
		 * chat communications for incoming messages that may, or may not,
		 * be destined for this chat user, and if they are, then we need to
		 * alert all the listeners of this guy and let them know that they
		 * have a message to process.
		 */
		CKIRCProtocolListener		*mListener;
		/*
		 * This is the list of all the registered responders of this class
		 * that are interested in looking at each received message for this
		 * class' registered nick, and even have a chance to reply to the
		 * sender with something. This is important because this is the only
		 * real way for the processes in this space to respond to messages
		 * from the other nicks on the chat server, and at the same time
		 * allow for multiple 'listeners' in case the user wants to break
		 * out their functionality.
		 */
		std::list<CKIRCResponder*>	mResponders;
		// ...and this is the mutex for it to control access
		CKFWMutex					mRespondersMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCProtocol & aProtocol );

#endif	// __CKIRCPROTOCOL_H
