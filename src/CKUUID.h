/*
 * CKUUID.h - this file defines a DCE-based Universally Unique Identifier
 *            class that can be used anywhere you need to have an integer
 *            that is guaranteed to be unique no matter what the circumstances.
 *            This is guaranteed by placing the IP address of the generating
 *            machine as well as the date and time into the number so that
 *            there is virtually no possibility that networked machines will
 *            be generating matching numbers. This class also includes the
 *            possibility of including a hashed class name so that the UUID
 *            can be 'tagged' for a particular class.
 *
 * $Id: CKUUID.h,v 1.7 2004/09/20 16:19:56 drbob Exp $
 */
#ifndef __CKUUID_H
#define __CKUUID_H

//	System Headers
#ifdef WIN32
#include <System/winnt-pdo.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

//	Third-Party Headers

//	Other Headers
#include "CKString.h"

//	Forward Declarations

//	Public Constants
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
 * Because we'll be dealing with IP addresses, and each operating system
 * defines them differently, we'll be wise to define a type at this point
 * that will allow us to manipulate these IP addresses in an operating
 * system-independent way. The 'in_addr' struct is the BSD "standard", so
 * we'll simply use it for NT, as it's already in the Mach includes.
 */
#ifdef WIN32
struct in_addr {
	unsigned long	s_addr;
};
#endif

/*
 * The following two structure definitions are from the DCE specification
 * and are two different ways of looking at the exact same bit-level
 * representation of the UUID.
 */
typedef struct {
    unsigned int	time_low;
    unsigned short	time_mid;
    unsigned short	time_hi_and_version;
    unsigned char	clock_seq_hi_and_reserved;
    unsigned char	clock_seq_low;
    unsigned char	node[6];
} uuid_t, *uuid_p_t;

typedef struct {
    time_t			timestamp;
    unsigned		variant : 3;
    unsigned		sequence : 13;
    unsigned char	_bitset;
    unsigned char	_reserved;
    struct in_addr	ip_address;
} ns_uuid_t, *ns_uuid_p_t;

/*
 * This structure is a representational mapping of four distinct ways of
 * looking at the UUID - as a DCE UUID, as an ns_uuid_t (?), and as either
 * a collection of four 4-byte words, or sixteen individual bytes. During
 * the lifetime of a UUID, all four of these representations will be
 * used at one time or another. This is just a convenient way to reference
 * those different parts without a lot of bit-fiddeling.
 */
typedef union {
    uuid_t			dce;
    ns_uuid_t		ns;
    unsigned int	words[4];
    unsigned char	bytes[16];
} CKUUID_struct;

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKUUID
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that doesn't generate a
		 * new UUID because it's possible that this instance is meant
		 * to assume the UUID "identity" of a previous UUID in the
		 * case of coming out of persistence, etc.
		 */
		CKUUID();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKUUID( const CKUUID & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKUUID();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKUUID & operator=( const CKUUID & anOther );

		/********************************************************
		 *
		 *                Class Construction Methods
		 *
		 ********************************************************/
		/*
		 * This generator method returns a new CKUUID ready for just
		 * about anything. There will be no hash code for a class name,
		 * but for a lot of cases, this is more than enough.
		 */
		static CKUUID newUUID();
		/*
		 * This generator method returns a new CKUUID with the provided
		 * class name hashed into the UUID as well. This is the nice way
		 * to generate UUIDs that might go into persistence schemes so
		 * that the ID includes the type of object that it's referring
		 * to.
		 */
		static CKUUID newUUIDForClass( const CKString & aClassName );
		/*
		 * This generator is the opposite of the getStringValue()
		 * instance method - it takes that method's output and
		 * "reconstitutes" it into a complete CKUUID. This is done
		 * most times when the CKUUID has been saved to some persistence
		 * scheme and then needs to be returned to it's object state
		 * after being pulled back from persistence.
		 */
		static CKUUID uuidWithString( const CKString & aString );
		/*
		 * This generator builds up a CKUUID based on the DCE string
		 * provided which is the UUID except for the hashed class name.
		 * Again, this generator is the opposite of
		 * getStringValueInDCEFormat() and can be used when you aren't
		 * taking advantage of the class name hashcode.
		 */
		static CKUUID uuidWithDCEString( const CKString & aString );
		/*
		 * These generators take the DCE formatted string with the
		 * included hashcode for class name and create the correct
		 * CKUUID based on that data.
		 */
		static CKUUID uuidWithDCEStringIncludingHashedClassName( const CKString & aString );
		static CKUUID uuidWithDCEStringIncludingHashedClassName( const CKString & aString, unsigned int aHashcode );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method gets the hash code of the class name for this
		 * UUID. This is not convertible into the class name, but the
		 * hash code is easily compared and the mapping is quite unique
		 * so it's a good comparison tool.
		 */
		unsigned int getHashedClassName() const;
		/*
		 * This method returns the encoded string value of the CKUUID
		 * in a format that's easily transported and then used to
		 * re-create the CKUUID with the generator methods.
		 */
		CKString getStringValue() const;
		/*
		 * This method returns the DCE formatted encoded string value
		 * of the CKUUID in a format that's easily transported and then
		 * used to re-create the CKUUID with the generator methods.
		 */
		CKString getStringValueInDCEFormat() const;
		/*
		 * This method returns the DCE formatted encoded string value
		 * along with the hashed class name of the CKUUID in a format
		 * that's easily transported and then used to re-create the CKUUID
		 * with the generator methods.
		 */
		CKString getStringValueInDCEFormatWithClassHash() const;
		/*
		 * This method returns the date of the CKUUID's creation in
		 * the format YYYYMMDD.HHMMSS so that it's easily
		 * understood and though not easy to use, at least usable.
		 */
		double getDateCreated() const;

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This is a simple hashing function for the entire CKUUID and
		 * can be used to place it into maps, etc. that require a
		 * reasonably unique, unsigned value representing the object
		 * itself.
		 */
		unsigned int getHash() const;
		/*
		 * This method checks to see if the two CKUUIDs are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator==( const CKUUID & anOther ) const;
		/*
		 * This method checks to see if the two CKUUIDs are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKUUID & anOther ) const;
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
		 * We need to initialize the class with the IP address of this
		 * machine. This IP address will be used each time a new UUID is
		 * created, and so only needs to be called once, and then statically
		 * stored in the class. To do this, we must first get the name of
		 * the host, and then look that hostname up.
		 */
		static bool getHostIPAddress();
		/*
		 * This is our shared initializer.  It checks to see that our
		 * instance hasn't  been initialized before, and also that we have
		 * the necessary information (the IP address of the host) to generate
		 * a new UUID. If everything is in order, it returns true if
		 * successful, otherwise false to signify an error.
		 */
		bool init();
		/*
		 * This method simply tell the caller if this CKUUID has been
		 * generated already, and therefore, does not, and indeed should not
		 * be generated again. It directly accesses the ivar because I need
		 * to know the state without going through the accessor method that
		 * calls this function - thus setting up an infinite loop. This
		 * breach in encapsulation is kept as small as possible for the
		 * obvious danger it represents to the overall design of the object.
		 */
		bool isGenerated() const;
		/*
		 * This function should not be used by any out-of-class method. It
		 * is the generator of the DCE UUID structure that this class uses
		 * for it's ivar data.
		 */
		static CKUUID_struct generateNewRawUUIDStruct();
		/*
		 * This method will generate the CKUUID_struct that the first 32 hex
		 * characters of this string represents. There may be more data in
		 * the string but this method ignores all the rest.
		 */
		static CKUUID_struct generateRawUUIDStructFromString( const CKString & aString );
		/*
		 * This method returns the numerical value of the last 8 hex
		 * characters in this string. It is assumed that the last 8
		 * characters are in fact hex charactrs, and simply turns these
		 * into a numerical value. In the context of this class, this
		 * number represents the hashed class name of this CKUUID.
		 */
		static unsigned int generateHashedClassNameFromString( const CKString & aString );
		/*
		 * This function should not be used by any non-CKUUID method. It is
		 * the parser of the DCE UUID string that this class can use for
		 * it's ivar data.
		 */
		static CKUUID_struct generateRawUUIDStructFromDCEString( const CKString & aString );
		/*
		 * As part of the CKUUID, there will be the possibility for a user
		 * to include a hashed version of the class name that this
		 * CKUUID_struct belongs to. Since this is redundant information,
		 * it is tacked on to the end of the getStringValue() string, and
		 * need not be used. However, when it is needed, we need to have a
		 * good hashing function for the class names. This function is said
		 * to be a superior hash function, and is very compact. It will
		 * be called only from those routines that need to initialize the
		 * CKUUID with a hashed class name.
		 */
		static unsigned int hashString( const CKString & aString );

	private:
		/*
		 * For those rare times when a user actually needs access to the
		 * structure of the UUID, this accessor method is available.
		 * Typically, this should not be used as it represents a data
		 * structure, and not a simple numerical, or string, ID number.
		 */
		CKUUID_struct getUUID() const;
		/*
		 * To make sure that there are no accesses to the UUID instance
		 * variable outside of our controlled environment, we have this
		 * 'setter' method. If someone tries to set a UUID that is already
		 * set, send an exception so they know it isn't possible.
		 */
		void setUUID( const CKUUID_struct & aUUID );
		/*
		 * During initialization we'll possibly need to set the hashed name
		 * of the class. This is the method that sets the name. It is
		 * private because at no time after initialization should a user be
		 * able to set this.
		 */
		void setHashedClassName( unsigned int aHashcode );

		/*
		 * This is teh actual UUID structure that this method is based on.
		 */
	    CKUUID_struct	mUUID;
		/*
		 * This is the hashed class name when it's supplied to the class.
		 */
		unsigned int	mHashedClassName;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKUUID & anID );

#endif	// __CKUUID_H
