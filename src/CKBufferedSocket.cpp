/*
 * CKBufferedSocket.cpp - this is the implementation of the class that can be
 *                        used as a more general communication object for
 *                        sockets for the user. The idea is that it builds on
 *                        the CKSocket class but adds all the functionality
 *                        like buffering that is needed for general socket
 *                        communications work. This then, easily forms the
 *                        basis of the CKTCPConnection class which in turn is
 *                        used in other higher-level classes in CKit.
 *
 * $Id: CKBufferedSocket.cpp,v 1.7 2004/09/02 20:46:43 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKBufferedSocket.h"
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
 * This is the default constructor that gets things started, but
 * wse still need to set the host and port before we can make a
 * connection.
 */
CKBufferedSocket::CKBufferedSocket() :
	CKSocket(),
	mReadTimeout(DEFAULT_READ_TIMEOUT),
	mPendingData()
{
}


/*
 * This form of the constructor is nice in that it takes the
 * hostname and port that we wish to make a connection with and
 * attempts to make that connection before returning.
 */
CKBufferedSocket::CKBufferedSocket( const std::string & aHost, int aPort ) :
	CKSocket(aHost, aPort),
	mReadTimeout(DEFAULT_READ_TIMEOUT),
	mPendingData()
{
}


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
CKBufferedSocket::CKBufferedSocket( CKSocket & anOther ) :
	CKSocket(anOther),
	mReadTimeout(DEFAULT_READ_TIMEOUT),
	mPendingData()
{
	/*
	 * Since we've now established the buffered socket we need to
	 * totally disable this existing socket. The easiest way to do
	 * that is to set a few key variables to their 'base' state.
	 *
	 * This act requires that we be a friend of the CKSocket class.
	 */
	anOther.setHostname("");
	anOther.setSocketHandle(INVALID_SOCKET);
	anOther.setActivelyListening(false);
	anOther.setConnectionEstablished(false);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKBufferedSocket::CKBufferedSocket( const CKBufferedSocket & anOther ) :
	CKSocket(anOther),
	mReadTimeout(DEFAULT_READ_TIMEOUT),
	mPendingData()
{
	// now we can use the '=' operator to do the rest of the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKBufferedSocket::~CKBufferedSocket()
{
	shutdownSocket();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKBufferedSocket & CKBufferedSocket::operator=( const CKBufferedSocket & anOther )
{
	// first, do the stuff that the superclass can do
	CKSocket::operator=(anOther);

	// now fill in the stuff we have
	setReadTimeout(anOther.getReadTimeout());
	setPendingData(anOther.getPendingData());

	return *this;
}


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
void CKBufferedSocket::setReadTimeout( float aTimeoutInSecs )
{
	mReadTimeout = aTimeoutInSecs;
}


/*
 * This method gets the read timeout (in seconds) for this socket.
 * Since this can be changed on the fly, it's nice to be able to
 * know *exactly* what it is as each read, and that's the primary
 * use of this method.
 */
float CKBufferedSocket::getReadTimeout() const
{
	return mReadTimeout;
}


/*
 * This method returns a copy of the pending data that has been
 * read from the socket and buffered in this class for eventual
 * reading by the client. This is really a building block of the
 * other "peeking" methods in this class which will probably be
 * far more useful to the user.
 */
std::string CKBufferedSocket::getPendingData() const
{
	return mPendingData;
}


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
std::string CKBufferedSocket::read()
{
	// first, transfer all the data at the socket to the pending data buffer
	transferWaitingDataAtSocketToPendingData();
	// next, grab the buffer's contents
	std::string	retval(mPendingData);
	// ...and clear it out
	mPendingData.clear();

	return retval;
}


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
std::string CKBufferedSocket::readUpTo( const std::string & aStopData )
{
	return readUpTo(aStopData.c_str());
}


std::string CKBufferedSocket::readUpTo( const char *aStopData )
{
	bool			error = false;
	std::string		retval("");
	bool			done = false;

	// first, make sure we have something interesting to do
	if (!error && !done) {
		if ((aStopData == NULL) || (strlen(aStopData) == 0)) {
			/*
			 * In this case, read all that's there to read...
			 */
			retval = read();
			// ...but do nothing more in this routine
			done = true;
		}
	}

	/*
	 * OK, here's what we're going to do... We need to create a loop where
	 * the first thing to do is to read everything that's available on the
	 * socket into the pending data buffer. Once it's there, we then check
	 * to see if the pending data buffer contains the terminal data. If it
	 * does, then we stop and move to the next phase where we pick off the
	 * data. If it's not currently in the buffer, however, then we need to
	 * wait on the socket until something arrives. When it arrives, we
	 * repeat the loop and see what happens.
	 */
	while (!error && !done) {
		/*
		 * Step 1 - transfer all the data waiting at the socket to the
		 *          pending data buffer
		 */
		if (!checkForDataUpTo(aStopData)) {
			transferWaitingDataAtSocketToPendingData();
		}

		/*
		 * Step 2 - see if the terminal data is in the pending data
		 *          buffer (break encapsulation here because we need to
		 *          work on the buffer itself and not a copy which will
		 *          be returned by the getPendingData() method.)
		 */
		unsigned int	pos = mPendingData.find(aStopData);
		if (pos != std::string::npos) {
			// OK... it's there, so move past the terminal data itself
			pos += strlen(aStopData);
			// ...and then grab the substring up to that point
			retval = mPendingData.substr(0, pos);
			// and clear out the buffer of this stuff
			mPendingData.erase(0, pos);
			// ...and flag this as done
			done = true;
			break;
		}

		/*
		 * Step 3 - wait for anything at the socket to arrive. It there's
		 *          a timeout, then flag it as an error, set the errno to
		 *          the timeout indicator, and bail out of this guy. The
		 *          one trick is that we need to assume that if the poll()
		 *          says there's data, then we ought to be able to see some
		 *          on the socket. That's the reason for the final argument
		 *          in the poll() call.
		 */
		int status = poll(getSocketHandle(), (int)(1000 * getReadTimeout()), true);
		if (status != POLL_OK) {
			error = true;
			switch (status) {
				case POLL_ERROR:
					errno = ERR_READ_ERROR;
					break;
				case POLL_TIMEOUT:
					errno = ERR_READ_TIMEOUT;
					break;
				case POLL_INTERRUPT:
					errno = ERR_READ_INTERRUPT;
					break;
			}
			break;
		}
	}

	return retval;
}


/*
 * This convenience method reads data up to the convenient line
 * ending CRLF combo. This is a common line ending in TCP
 * communications and so it's a nice little tool to have.
 */
std::string CKBufferedSocket::readUpToCRLF()
{
	return readUpTo("\r\n");
}


/*
 * This convenience method reads data up to the convenient line
 * ending NEWLINE. This is a common line ending in Unix-based TCP
 * communications and so it's a nice little tool to have.
 */
std::string CKBufferedSocket::readUpToNEWLINE()
{
	return readUpTo("\n");
}


/*
 * This method will read all the available data from the socket,
 * append it to the end of the rest of the buffered data for this
 * socket and then check in that buffer to see if the provided
 * terminal string appears. This is very useful to see if a
 * "complete" data set is available at the socket, and if not,
 * then do nothing and try again later.
 */
bool CKBufferedSocket::checkForDataUpTo( const std::string & aStopData )
{
	bool		found = false;

	if (mPendingData.find(aStopData) != std::string::npos) {
		found = true;
	}

	return found;
}


bool CKBufferedSocket::checkForDataUpTo( const char *aStopData )
{
	bool		found = false;

	if (mPendingData.find(aStopData) != std::string::npos) {
		found = true;
	}

	return found;
}


/*
 * These are convenience methods that make it easier to get a
 * complete line from the incoming socket.
 */
bool CKBufferedSocket::checkForDataUpToCRLF()
{
	return checkForDataUpTo("\r\n");
}


bool CKBufferedSocket::checkForDataUpToNEWLINE()
{
	return checkForDataUpTo("\n");
}


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
bool CKBufferedSocket::operator==( const CKBufferedSocket & anOther ) const
{
	bool		equal = true;

	if (!CKSocket::operator==(anOther) ||
		(getReadTimeout() != anOther.getReadTimeout()) ||
		(getPendingData() != anOther.getPendingData())) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKBufferedSockets are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKBufferedSocket::operator!=( const CKBufferedSocket & anOther ) const
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
std::string CKBufferedSocket::toString() const
{
	return mPendingData;
}


/*
 * This method clears out the existing buffered data read from the
 * socket (data pending a read) and replaces it with the supplied
 * data. This is a drastic step, so please use this method with care.
 */
void CKBufferedSocket::setPendingData( const std::string & aData )
{
	mPendingData = aData;
}


/*
 * These methods append the provided data to the end of the existing
 * buffered data from the socket. They are nice little methods that
 * are used in the other methods of this class to update the buffer's
 * contents with that which is read from the socket as it's available.
 */
void CKBufferedSocket::appendToPendingData( const std::string & aData )
{
	mPendingData.append(aData);
}


void CKBufferedSocket::appendToPendingData( const char *aString )
{
	mPendingData.append(aString);
}


/*
 * This method simply clears out all the pending data from the
 * socket so that we can start fresh.
 */
void CKBufferedSocket::emptyPendingData()
{
	mPendingData.empty();
}


/*
 * These methods clear out only that part of the pending data
 * buffer up to, and including, the provided terminal data. This
 * method will block until the terminal data is read from the
 * socket, so it's probably wise to see if the data you're
 * looking for is in the buffer before calling this method.
 */
void CKBufferedSocket::emptyPendingDataUpToAndIncluding( const std::string & aData )
{
	emptyPendingDataUpToAndIncluding(aData.c_str());
}


void CKBufferedSocket::emptyPendingDataUpToAndIncluding( const char *aData )
{
	unsigned int	pos = mPendingData.find(aData);
	if (pos != std::string::npos) {
		// move past the terminal data itself
		pos += strlen(aData);
		// ...and erase all the data up to that point
		mPendingData.erase(0, pos);
	}
}


/*
 * This method simply takes all the available data at the socket
 * and puts it into the pending data buffer. This is a convenience
 * method more than anything else, as it'll be used in a lot of the
 * methods in this class to move all available data to the buffer
 * so that actions can simply focus on the buffer's contents.
 */
void CKBufferedSocket::transferWaitingDataAtSocketToPendingData()
{
	mPendingData.append(readAvailableData());
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKBufferedSocket & aSocket )
{
	aStream << aSocket.toString();

	return aStream;
}
