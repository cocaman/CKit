/*
 * CKTelnetConnection.cpp - this file implements the class used as a more general
 *                          telnet-like connection to a remote host. As it turns
 *                          out, the telnet connection (without VT100 enulation)
 *                          is the basis of SMTP, FTP, IRC, and many other
 *                          "conversational" TCP/IP protocols. This class builds
 *                          on the lower-level socket classes and so doesn't
 *                          have to worry about the specifics of socket reading
 *                          and writing. Rather, this class focuses on the
 *                          higher-level features.
 *
 * $Id: CKTelnetConnection.cpp,v 1.1 2003/11/21 18:08:11 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKTelnetConnection.h"
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
 * wse still need to set the host before we can make a connection.
 */
CKTelnetConnection::CKTelnetConnection() :
	CKTCPConnection()
{
}


/*
 * This form of the constructor is nice in that it takes the
 * hostname and tries to make the connection to the host on
 * the default telnet port (23) before returning to the caller.
 */
CKTelnetConnection::CKTelnetConnection( const std::string & aHost ) :
	CKTCPConnection()
{
	// let's try to make the connection based on this information
	if (!connect(aHost)) {
		std::ostringstream	msg;
		msg << "CKTelnetConnection::CKTelnetConnection(const std::string &) - "
			"the telnet connection to the host " << aHost << " could not be "
			"established. This is a serious problem. Please make sure that the "
			"telnet service is ready to accept the connection.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKTelnetConnection::CKTelnetConnection( const CKTelnetConnection & anOther ) :
	CKTCPConnection()
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKTelnetConnection::~CKTelnetConnection()
{
	shutdownSocket();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTelnetConnection & CKTelnetConnection::operator=( const CKTelnetConnection & anOther )
{
	// the sure does a fine job of this for me
	CKTCPConnection::operator=(anOther);
	
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
 * This method is the easiest way to establish a connection -
 * just specify the host and away you go. The connection assumes
 * the default telnet port (23).
 */
bool CKTelnetConnection::connect( const std::string & aHost )
{
	return CKTCPConnection::connect(aHost, (int)DEFAULT_TELNET_PORT);
}


/*
 * This method is an easy way to establish a connection to a non-
 * standard telnet, or telnet-like, port. This is used a lot in
 * the SMTP, FTP, etc. clients as they are basically telnet-like
 * connections on different ports.
 */
bool CKTelnetConnection::connect( const std::string & aHost, int aPort )
{
	return CKTCPConnection::connect(aHost, aPort);
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKTelnetConnections are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKTelnetConnection::operator==( const CKTelnetConnection & anOther ) const
{
	// the super does fine on this for me
	return CKTCPConnection::operator==(anOther);
}


/*
 * This method checks to see if the two CKTelnetConnections are not
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are not equal, then
 * this method returns true, otherwise it returns false.
 */
bool CKTelnetConnection::operator!=( const CKTelnetConnection & anOther ) const
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
std::string CKTelnetConnection::toString() const
{
	// the super does fine on this for me
	return CKTCPConnection::toString();
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTelnetConnection & aConnection )
{
	aStream << aConnection.toString();

	return aStream;
}
