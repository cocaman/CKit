/*
 * CKTCPConnection.h - this file defines the class that can be used as a
 *                     general TCP connection for the user. The idea is that
 *                     it builds on the CKBufferedSocket class but adds all
 *                     the functionality that is needed for general TCP on IP
 *                     (TCP/IP) connections. This then, forms the basis of
 *                     the CKTelnetConnection class which in turn is used
 *                     in other higher-level classes in CKit.
 *
 * $Id: CKTCPConnection.h,v 1.6 2004/09/16 09:34:20 drbob Exp $
 */
#ifndef __CKTCPCONNECTION_H
#define __CKTCPCONNECTION_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKBufferedSocket.h"
#include "CKString.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKTCPConnection :
	public CKBufferedSocket
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
		CKTCPConnection();
		/*
		 * This form of the constructor is nice in that it takes the
		 * parameters and tries to make the connection to the host on
		 * that port before returning to the caller.
		 */
		CKTCPConnection( const CKString & aHost, int aPort );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKTCPConnection( const CKTCPConnection & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKTCPConnection();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKTCPConnection & operator=( const CKTCPConnection & anOther );

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
		void setTraceData( bool aFlag );

		/*
		 * This method is used to indicate the tracing of both incoming and
		 * outgoing data through the connection. While there are methods
		 * in the superclass for each individually, at this level, it's
		 * most often the case that you want to see both.
		 */
		bool traceData() const;

		/********************************************************
		 *
		 *                Connection Methods
		 *
		 ********************************************************/
		/*
		 * This method indicates if the connection is established.
		 */
		bool isConnected() const;
		/*
		 * This method disconnects the connection from the remote
		 * host.
		 */
		void disconnect();

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
		bool operator==( const CKTCPConnection & anOther ) const;
		/*
		 * This method checks to see if the two CKTCPConnections are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKTCPConnection & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTCPConnection & aConnection );

#endif	// __CKTCPCONNECTION_H
