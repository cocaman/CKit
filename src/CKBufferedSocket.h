/*
 * CKBufferedSocket.h - this file defines the class that can be used as a
 *                      more general communication object for sockets for the
 *                      user. The idea is that it builds on the CKSocket class
 *                      but adds all the functionality like buffering that is
 *                      needed for general socket communications work. This
 *                      then, easily forms the basis of the CKTCPConnection
 *                      class which in turn is used in other higher-level
 *                      classes in CKit.
 *
 * $Id: CKBufferedSocket.h,v 1.9 2004/09/20 16:19:20 drbob Exp $
 */
#ifndef __CKBUFFEREDSOCKET_H
#define __CKBUFFEREDSOCKET_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <errno.h>

//	Third-Party Headers

//	Other Headers
#include "CKSocket.h"

//	Forward Declarations

//	Public Constants
/*
 * This is returned in errno from readUpTo() methods if a read timeout
 * occurred. In those routines, we really don't want to throw an exception
 * if a timeout occurs, as it's probably not *exceptional* if a timeout
 * occurs. It could be just normal behavior. However, we also can't use the
 * return code for error condition as it's the data... so we're left with
 * errno. If it's 0 then all went well... is it's < 0 then is a problem
 * no matter what the data said, and these are the different possible
 * codes for those 'problems'.
 */
#define	ERR_READ_TIMEOUT		-11111
#define	ERR_READ_ERROR			-11112
#define	ERR_READ_INTERRUPT		-11113

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKBufferedSocket :
	public CKSocket
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that gets things started, but
		 * wse still need to set the host and port before we can make a
		 * connection.
		 */
		CKBufferedSocket();
		/*
		 * This form of the constructor is nice in that it takes the
		 * hostname and port that we wish to make a connection with and
		 * attempts to make that connection before returning.
		 */
		CKBufferedSocket( const CKString & aHost, int aPort );
		/*
		 * There are times when you are given a simple CKSocket and you
		 * want to 'promote' it to a buffered socket. This form of the
		 * constructor is made for just this type of operation. It takes
		 * the CKSocket and creates a new CKBufferedSocket so that you
		 * can take advantage of the buffered reads, etc.
		 *
		 * The wrinkle is that if this is all you do you're in trouble
		 * when the original CKSocket is deleted as it will close out
		 * the connection, just as you'd expect. So what we do with this
		 * constructor is to "incapacitate" the incoming CKSocket so that
		 * when it's deleted it will NOT disconnect the communication and
		 * then the transfer to this newly created CKBufferedSocket will
		 * have been complete.
		 */
		CKBufferedSocket( CKSocket & anOther );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKBufferedSocket( const CKBufferedSocket & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKBufferedSocket();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKBufferedSocket & operator=( const CKBufferedSocket & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method sets the read timeout for this socket. This is
		 * important because the base CKSocket doesn't have any ability
		 * to wait for data at the socket, or to buffer the reads to
		 * make things easier on the developer. This method then, sets
		 * the timeout in seconds, that the socket will wait before
		 * returning no data.
		 */
		void setReadTimeout( float aTimeoutInSecs );

		/*
		 * This method gets the read timeout (in seconds) for this socket.
		 * Since this can be changed on the fly, it's nice to be able to
		 * know *exactly* what it is as each read, and that's the primary
		 * use of this method.
		 */
		float getReadTimeout() const;
		/*
		 * This method returns a copy of the pending data that has been
		 * read from the socket and buffered in this class for eventual
		 * reading by the client. This is really a building block of the
		 * other "peeking" methods in this class which will probably be
		 * far more useful to the user.
		 */
		CKString getPendingData() const;

		/********************************************************
		 *
		 *                Data Moving Methods
		 *
		 ********************************************************/
		/*
		 * This method reads everything that's available at the socket
		 * appends it to the buffered contents of this class, and returns
		 * the entire kit-n-caboodle to the caller. This is the most
		 * complete form of the read that can be done and it clears out
		 * everything.
		 */
		CKString read();
		/*
		 * These methods read data up to - and including, the provided
		 * 'tag', or stop, data from the socket, and will wait until
		 * the terminal data is read before returning. As each data
		 * 'chunk' is read from the socket within the read timeout
		 * interval, it's contents is checked for the terminal data.
		 * If a read timeout occurs, an exception will be thrown, but
		 * if data continues to be available at the socket and the terminal
		 * data has not arrived, reading will continue.
		 */
		CKString readUpTo( const std::string & aStopData );
		CKString readUpTo( const CKString & aStopData );
		CKString readUpTo( const char *aStopData );
		/*
		 * This convenience method reads data up to the convenient line
		 * ending CRLF combo. This is a common line ending in TCP
		 * communications and so it's a nice little tool to have.
		 */
		CKString readUpToCRLF();
		/*
		 * This convenience method reads data up to the convenient line
		 * ending NEWLINE. This is a common line ending in Unix-based TCP
		 * communications and so it's a nice little tool to have.
		 */
		CKString readUpToNEWLINE();

		/*
		 * This method will read all the available data from the socket,
		 * append it to the end of the rest of the buffered data for this
		 * socket and then check in that buffer to see if the provided
		 * terminal string appears. This is very useful to see if a
		 * "complete" data set is available at the socket, and if not,
		 * then do nothing and try again later.
		 */
		bool checkForDataUpTo( const CKString & aStopData );
		bool checkForDataUpTo( const char *aStopData );
		/*
		 * These are convenience methods that make it easier to get a
		 * complete line from the incoming socket.
		 */
		bool checkForDataUpToCRLF();
		bool checkForDataUpToNEWLINE();

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKBufferedSockets are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator==( const CKBufferedSocket & anOther ) const;
		/*
		 * This method checks to see if the two CKBufferedSockets are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKBufferedSocket & anOther ) const;
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
		 * This method clears out the existing buffered data read from the
		 * socket (data pending a read) and replaces it with the supplied
		 * data. This is a drastic step, so please use this method with care.
		 */
		void setPendingData( const CKString & aData );

		/*
		 * These methods append the provided data to the end of the existing
		 * buffered data from the socket. They are nice little methods that
		 * are used in the other methods of this class to update the buffer's
		 * contents with that which is read from the socket as it's available.
		 */
		void appendToPendingData( const CKString & aData );
		void appendToPendingData( const char *aString );
		/*
		 * This method simply clears out all the pending data from the
		 * socket so that we can start fresh.
		 */
		void emptyPendingData();
		/*
		 * These methods clear out only that part of the pending data
		 * buffer up to, and including, the provided terminal data. This
		 * method will block until the terminal data is read from the
		 * socket, so it's probably wise to see if the data you're
		 * looking for is in the buffer before calling this method.
		 */
		void emptyPendingDataUpToAndIncluding( const CKString & aData );
		void emptyPendingDataUpToAndIncluding( const char *aData );

		/*
		 * This method simply takes all the available data at the socket
		 * and puts it into the pending data buffer. This is a convenience
		 * method more than anything else, as it'll be used in a lot of the
		 * methods in this class to move all available data to the buffer
		 * so that actions can simply focus on the buffer's contents.
		 */
		void transferWaitingDataAtSocketToPendingData();

	private:
		/*
		 * This is the read timeout, in seconds, for this connection at this
		 * time. The nice thing about this is that this value can be changed
		 * and it will have an immediate effect in terms of the next read
		 * requested from this connection.
		 */
		float				mReadTimeout;
		/*
		 * As data is being read from the CKSocket, it's important to have
		 * a data buffer on this end so that things are not lost. This is
		 * the data that's been read off the socket up to this point, and
		 * the user of this class has not yet reead from us and cleared it
		 * out.
		 */
		CKString			mPendingData;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKBufferedSocket & aSocket );

#endif	// __CKBUFFEREDSOCKET_H
