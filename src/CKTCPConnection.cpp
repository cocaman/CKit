/*
 * CKTCPConnection.cpp - this is the implementation of the class that can be
 *                       used as a general TCP connection for the user. The
 *                       idea is that it builds on the CKBufferedSocket class
 *                       but adds all the functionality that is needed for
 *                       general TCP on IP (TCP/IP) connections. This then,
 *                       forms the basis of the CKTelnetConnection class
 *                       which in turn is used in other higher-level classes
 *                       in CKit.
 *
 * $Id: CKTCPConnection.cpp,v 1.8 2007/09/26 19:33:46 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKTCPConnection.h"

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
CKTCPConnection::CKTCPConnection() :
	CKBufferedSocket()
{
	// start off not tracing any data
	setTraceData(false);
}


/*
 * This form of the constructor is nice in that it takes the
 * parameters and tries to make the connection to the host on
 * that port before returning to the caller.
 */
CKTCPConnection::CKTCPConnection( const CKString & aHost, int aPort ) :
	CKBufferedSocket(aHost, aPort)
{
	// start off not tracing any data
	setTraceData(false);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKTCPConnection::CKTCPConnection( const CKTCPConnection & anOther ) :
	CKBufferedSocket()
{
	// start off not tracing any data
	setTraceData(false);

	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKTCPConnection::~CKTCPConnection()
{
	shutdownSocket();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTCPConnection & CKTCPConnection::operator=( const CKTCPConnection & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		// the super's '=' operator is good enough for me
		CKBufferedSocket::operator=(anOther);
	}
	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method is used to flag the tracing of both incoming and
 * outgoing data through the connection. While there are methods
 * in the superclass for each individually, at this level, it's
 * most often the case that you want to see both.
 */
void CKTCPConnection::setTraceData( bool aFlag )
{
	setTraceOutgoingData(aFlag);
	setTraceIncomingData(aFlag);
}


/*
 * This method is used to indicate the tracing of both incoming and
 * outgoing data through the connection. While there are methods
 * in the superclass for each individually, at this level, it's
 * most often the case that you want to see both.
 */
bool CKTCPConnection::traceData() const
{
	bool		retval = false;

	if (traceOutgoingData() || traceIncomingData()) {
		retval = true;
	}

	return retval;
}


/********************************************************
 *
 *                Connection Methods
 *
 ********************************************************/
/*
 * This method indicates if the connection is established.
 */
bool CKTCPConnection::isConnected() const
{
	return isConnectionEstablished();
}


/*
 * This method disconnects the connection from the remote
 * host.
 */
void CKTCPConnection::disconnect()
{
	shutdownSocket();
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKTCPConnections are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKTCPConnection::operator==( const CKTCPConnection & anOther ) const
{
	return CKBufferedSocket::operator==(anOther);
}


/*
 * This method checks to see if the two CKTCPConnections are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKTCPConnection::operator!=( const CKTCPConnection & anOther ) const
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
CKString CKTCPConnection::toString() const
{
	return CKBufferedSocket::toString();
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTCPConnection & aConnection )
{
	aStream << aConnection.toString();

	return aStream;
}
