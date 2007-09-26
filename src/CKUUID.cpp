/*
 * CKUUID.cpp - this file implements a DCE-based Universally Unique Identifier
 *              class that can be used anywhere you need to have an integer
 *              that is guaranteed to be unique no matter what the circumstances.
 *              This is guaranteed by placing the IP address of the generating
 *              machine as well as the date and time into the number so that
 *              there is virtually no possibility that networked machines will
 *              be generating matching numbers. This class also includes the
 *              possibility of including a hashed class name so that the UUID
 *              can be 'tagged' for a particular class.
 *
 * $Id: CKUUID.cpp,v 1.9 2007/09/26 19:33:46 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <netdb.h>
#include <strings.h>

//	Third-Party Headers

//	Other Headers
#include "CKUUID.h"
#include "CKException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants

// Static variables
/*
 * We need to have the IP address of this machine for generating each of
 * the UUID values. Sincel this isn't going to change, let's get this once
 * and then save it in a class (static) variable and then use it after
 * that. OK... good plan, here it is.
 */
static struct in_addr		cIPAddress;

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
CKUUID::CKUUID() :
	mUUID(),
	mHashedClassName(0)
{
	// call the shared initializer
	if (!init()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::CKUUID() - the general "
			"initializer for the UUID failed. Please check the logs for a "
			"possible cause.");
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKUUID::CKUUID( const CKUUID & anOther ) :
	mUUID(),
	mHashedClassName(0)
{
	// call the shared initializer
	if (!init()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::CKUUID(const CKUUID &) - "
			"the general initializer for the UUID failed. Please check the "
			"logs for a possible cause.");
	}

	// let the '=' operator do the rest of the work
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKUUID::~CKUUID()
{
	// we don't own anything, so we don't have to free anything
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKUUID & CKUUID::operator=( const CKUUID & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		// copy over all the instance variables
		for (int i = 0; i < 4; i++) {
			mUUID.words[i] = anOther.mUUID.words[i];
		}
		mHashedClassName = anOther.mHashedClassName;
	}
	return *this;
}


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
CKUUID CKUUID::newUUID()
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateNewRawUUIDStruct());

	return retval;
}


/*
 * This generator method returns a new CKUUID with the provided
 * class name hashed into the UUID as well. This is the nice way
 * to generate UUIDs that might go into persistence schemes so
 * that the ID includes the type of object that it's referring
 * to.
 */
CKUUID CKUUID::newUUIDForClass( const CKString & aClassName )
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateNewRawUUIDStruct());
	retval.setHashedClassName(CKUUID::hashString(aClassName));

	return retval;
}


/*
 * This generator is the opposite of the getStringValue()
 * instance method - it takes that method's output and
 * "reconstitutes" it into a complete CKUUID. This is done
 * most times when the CKUUID has been saved to some persistence
 * scheme and then needs to be returned to it's object state
 * after being pulled back from persistence.
 */
CKUUID CKUUID::uuidWithString( const CKString & aString )
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateRawUUIDStructFromString(aString));
	retval.setHashedClassName(CKUUID::generateHashedClassNameFromString(aString));

	return retval;
}


/*
 * This generator builds up a CKUUID based on the DCE string
 * provided which is the UUID except for the hashed class name.
 * Again, this generator is the opposite of
 * getStringValueInDCEFormat() and can be used when you aren't
 * taking advantage of the class name hashcode.
 */
CKUUID CKUUID::uuidWithDCEString( const CKString & aString )
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateRawUUIDStructFromDCEString(aString));

	return retval;
}


/*
 * These generators take the DCE formatted string with the
 * included hashcode for class name and create the correct
 * CKUUID based on that data.
 */
CKUUID CKUUID::uuidWithDCEStringIncludingHashedClassName( const CKString & aString )
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateRawUUIDStructFromDCEString(aString));
	retval.setHashedClassName(CKUUID::generateHashedClassNameFromString(aString));

	return retval;
}


CKUUID CKUUID::uuidWithDCEStringIncludingHashedClassName( const CKString & aString, unsigned int aHashcode )
{
	CKUUID		retval;
	retval.setUUID(CKUUID::generateRawUUIDStructFromDCEString(aString));
	retval.setHashedClassName(aHashcode);

	return retval;
}


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
unsigned int CKUUID::getHashedClassName() const
{
	return mHashedClassName;
}


/*
 * This method returns the encoded string value of the CKUUID
 * in a format that's easily transported and then used to
 * re-create the CKUUID with the generator methods.
 */
CKString CKUUID::getStringValue() const
{
	CKString		retval;

	// see if we have anything to do
	if (!isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getStringValue() - this "
			"UUID has not yet been generated, and therefore there is nothing "
			"to return. Please make sure to properly generate the UUIDs with "
			"one of the class (static) generators for best success.");
	} else {
		// generate the string representation appropriately
		char buff[64];
		bzero(buff, 64);
		snprintf(buff, 63, "%08x%08x%08x%08x%08x", mUUID.words[0],
					mUUID.words[1], mUUID.words[2], mUUID.words[3],
					getHashedClassName());
		retval = buff;
	}

	return retval;
}


/*
 * This method returns the DCE formatted encoded string value
 * of the CKUUID in a format that's easily transported and then
 * used to re-create the CKUUID with the generator methods.
 */
CKString CKUUID::getStringValueInDCEFormat() const
{
	CKString		retval;

	// see if we have anything to do
	if (!isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getStringValueInDCEFormat()"
			" - this UUID has not yet been generated, and therefore there is "
			"nothing to return. Please make sure to properly generate the UUIDs "
			"with one of the class (static) generators for best success.");
	} else {
		// generate the string representation appropriately
		char buff[128];
		bzero(buff, 128);
		snprintf(buff, 127, "%08x %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
					getHash(),
					mUUID.dce.time_low,
					mUUID.dce.time_mid,
					mUUID.dce.time_hi_and_version,
					mUUID.dce.clock_seq_hi_and_reserved,
					mUUID.dce.clock_seq_low,
					mUUID.dce.node[0],
					mUUID.dce.node[1],
					mUUID.dce.node[2],
					mUUID.dce.node[3],
					mUUID.dce.node[4],
					mUUID.dce.node[5]);
		retval = buff;
	}

	return retval;
}


/*
 * This method returns the DCE formatted encoded string value
 * along with the hashed class name of the CKUUID in a format
 * that's easily transported and then used to re-create the CKUUID
 * with the generator methods.
 */
CKString CKUUID::getStringValueInDCEFormatWithClassHash() const
{
	CKString		retval;

	// see if we have anything to do
	if (!isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getStringValueInDCEFormat()"
			" - this UUID has not yet been generated, and therefore there is "
			"nothing to return. Please make sure to properly generate the UUIDs "
			"with one of the class (static) generators for best success.");
	} else {
		// generate the string representation appropriately
		char buff[128];
		bzero(buff, 128);
		snprintf(buff, 127, "%08x %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x %08x",
					getHash(),
					mUUID.dce.time_low,
					mUUID.dce.time_mid,
					mUUID.dce.time_hi_and_version,
					mUUID.dce.clock_seq_hi_and_reserved,
					mUUID.dce.clock_seq_low,
					mUUID.dce.node[0],
					mUUID.dce.node[1],
					mUUID.dce.node[2],
					mUUID.dce.node[3],
					mUUID.dce.node[4],
					mUUID.dce.node[5],
					getHashedClassName());
		retval = buff;
	}

	return retval;
}


/*
 * This method returns the date of the CKUUID's creation in
 * the format YYYYMMDD.HHMMSS so that it's easily
 * understood and though not easy to use, at least usable.
 */
double CKUUID::getDateCreated() const
{
	double		retval = 0.0;

	// see if we have anything to do
	if (!isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getDateCreated()"
			" - this UUID has not yet been generated, and therefore there is "
			"nothing to return. Please make sure to properly generate the UUIDs "
			"with one of the class (static) generators for best success.");
	} else {
		struct tm	*d = localtime(&mUUID.ns.timestamp);
		retval = (d->tm_year + 1900)*10000.0
				+ (d->tm_mon + 1)*100.0
				+ d->tm_mday*1.0
				+ d->tm_hour/100.0
				+ d->tm_min/10000.0
				+ d->tm_sec/1000000.0;
	}

	return retval;
}


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
unsigned int CKUUID::getHash() const
{
    const unsigned char	*next_uuid;
    union {
        struct {
            unsigned short	c0;
            unsigned short	c1;
        } u16;
        unsigned	u32;
    } h;

    h.u32 = 0x0;
    next_uuid = &mUUID.bytes[0];
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;

    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;

    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;

    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;
    h.u16.c0 = h.u16.c0 + *next_uuid++;
    h.u16.c1 = h.u16.c1 + h.u16.c0;

    return h.u32;
}


/*
 * This method checks to see if the two CKUUIDs are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKUUID::operator==( const CKUUID & anOther ) const
{
	bool		equal = true;

	if ((mUUID.words[0] != anOther.mUUID.words[0]) ||
		(mUUID.words[1] != anOther.mUUID.words[1]) ||
		(mUUID.words[2] != anOther.mUUID.words[2]) ||
		(mUUID.words[3] != anOther.mUUID.words[3]) ||
		(getHashedClassName() != anOther.getHashedClassName())) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKUUIDs are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKUUID::operator!=( const CKUUID & anOther ) const
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
CKString CKUUID::toString() const
{
	return getStringValue();
}


/*
 * We need to initialize the class with the IP address of this
 * machine. This IP address will be used each time a new UUID is
 * created, and so only needs to be called once, and then statically
 * stored in the class. To do this, we must first get the name of
 * the host, and then look that hostname up.
 */
bool CKUUID::getHostIPAddress()
{
	bool		error = false;

	/*
	 * Only do this if the value has not already been generated
	 */
	if (cIPAddress.s_addr != 0x0) {
		// first, try to get the name of this machine
		char 	host[MAXHOSTNAMELEN + 1];
		if (!error) {
			if (gethostname(host, MAXHOSTNAMELEN) == -1) {
				error = true;
				std::ostringstream	msg;
				msg << "CKUUID::getHostIPAddress() - the attempt to get the local "
					"hostname failed with an error number: " << errno << ": " <<
					strerror(errno) << ". This is a serious problem that needs to "
					"be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}

		// now see if we can get the details about this hostname
		if (!error) {
#ifdef __linux__
			struct hostent hostInfo;
			struct hostent *info = NULL;
			char buff[4096];
			int h_error;
			if (gethostbyname_r(host, &hostInfo, buff, 4096, &info, &h_error) != 0) {
				/*
				 * We weren't successful, most likely because the name was a
				 * numerical IP address of the form "a.b.c.d". Take this IP
				 * address and try to convert it directly. If it errors out,
				 * simply set the IP address to 0x0. We'll check on it in the
				 * initialization method.
				 */
				unsigned long tmpIP = inet_addr(host);
				if (tmpIP == (unsigned long)(-1)) {
					cIPAddress.s_addr = 0x0;
				} else {
					cIPAddress.s_addr = tmpIP;
				}
			} else {
				/*
				 * We were successful at getting the host information structure
				 * and now just need to extract the numerical IP address from the
				 * structure. This is a very poor extraction technique, but at
				 * least it is cross-platform.
				 */
				memcpy(&cIPAddress, hostInfo.h_addr, hostInfo.h_length);
			}
#endif
#ifdef __sun__
			struct hostent hostInfo;
			char buff[4096];
			int h_error;
			struct hostent *info = gethostbyname_r(host, &hostInfo, buff, 4096, &h_error);
			if (info == NULL) {
				/*
				 * We weren't successful, most likely because the name was a
				 * numerical IP address of the form "a.b.c.d". Take this IP
				 * address and try to convert it directly. If it errors out,
				 * simply set the IP address to 0x0. We'll check on it in the
				 * initialization method.
				 */
				unsigned long tmpIP = inet_addr(host);
				if (tmpIP == (unsigned long)(-1)) {
					cIPAddress.s_addr = 0x0;
				} else {
					cIPAddress.s_addr = tmpIP;
				}
			} else {
				/*
				 * We were successful at getting the host information structure
				 * and now just need to extract the numerical IP address from the
				 * structure. This is a very poor extraction technique, but at
				 * least it is cross-platform.
				 */
				memcpy(&cIPAddress, hostInfo.h_addr, hostInfo.h_length);
			}
#endif
#ifdef __MACH__
			struct hostent *hostInfo = gethostbyname(host);
			if (hostInfo == NULL) {
				/*
				 * We weren't successful, most likely because the name was a
				 * numerical IP address of the form "a.b.c.d". Take this IP
				 * address and try to convert it directly. If it errors out,
				 * simply set the IP address to 0x0. We'll check on it in the
				 * initialization method.
				 */
				unsigned long tmpIP = inet_addr(host);
				if (tmpIP == (unsigned long)(-1)) {
					cIPAddress.s_addr = 0x0;
				} else {
					cIPAddress.s_addr = tmpIP;
				}
			} else {
				/*
				 * We were successful at getting the host information structure
				 * and now just need to extract the numerical IP address from the
				 * structure. This is a very poor extraction technique, but at
				 * least it is cross-platform.
				 */
				memcpy(&cIPAddress, hostInfo->h_addr, hostInfo->h_length);
			}
#endif
		}
	}

	return !error;
}


/*
 * This is our shared initializer.  It checks to see that our
 * instance hasn't  been initialized before, and also that we have
 * the necessary information (the IP address of the host) to generate
 * a new UUID. If everything is in order, it returns true if
 * successful, otherwise false to signify an error.
 */
bool CKUUID::init()
{
	bool		error = false;

	if (!error) {
		/*
		 * Check to see that this CKUUID hasn't been generated/initialized
		 * before.
		 */
		if (isGenerated()) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKUUID::init() - the UUID for "
				"this instance has already been generated. This means that it's "
				"not 'safe' to set another value on top of this one. Please make "
				"sure to generate it only once and then not set it again.");
		}

		// Check to see if we have an IP address, if not, flag it as an error.
		if (!getHostIPAddress()) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKUUID::init() - the IP "
				"Address for this machine cannot be obtained and that means "
				"that we can't generate the UUID as it's based, in part, on the "
				"IP address. Please look into this as soon as possible.");
		}
	}

	// now clear out the instance variables
	if (!error) {
		for (int i = 0; i < 4; i++) {
			mUUID.words[i] = 0;
		}
		mHashedClassName = 0;
	}

	return !error;
}


/*
 * This method simply tell the caller if this CKUUID has been
 * generated already, and therefore, does not, and indeed should not
 * be generated again. It directly accesses the ivar because I need
 * to know the state without going through the accessor method that
 * calls this function - thus setting up an infinite loop. This
 * breach in encapsulation is kept as small as possible for the
 * obvious danger it represents to the overall design of the object.
 */
bool CKUUID::isGenerated() const
{
	return (mUUID.ns._bitset & 1);
}


/*
 * This function should not be used by any out-of-class method. It
 * is the generator of the DCE UUID structure that this class uses
 * for it's ivar data.
 */
CKUUID_struct CKUUID::generateNewRawUUIDStruct()
{
    /*
     * The UUID needs to have a timestamp in it, so this will be the class-
     * level timestamp to make sure they are really unique in time.
     */
    static time_t sTimestamp = 0;

    static struct {
        unsigned _unused : 3;
        unsigned _sequence : 13;
    } sLocal = {0x0, 0x0};

	// I need a struct to fill in and pass back to the caller
	CKUUID_struct me;

	// Check to see if we have an IP address, if not, flag it as an error.
	if (!getHostIPAddress()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::generateNewRawUUIDStruct() "
			"- the IP Address for this machine cannot be obtained and that means "
			"that we can't generate the UUID as it's based, in part, on the "
			"IP address. Please look into this as soon as possible.");
	} else {
		// Now start filling in the components of the UUID
		me.ns.ip_address = cIPAddress;
		me.ns.timestamp = time(NULL);
		// Make sure that multiple calls within the resolution of the
		// timestamp are serialized as well.
		if (sTimestamp == me.ns.timestamp) {
			sLocal._sequence++;
		} else {
			sLocal._sequence = 0x0;
		}
		sTimestamp = me.ns.timestamp;
		// This code is the DCE-compatible code for "unspecified variant".
		me.ns.variant = 0x7;
		// This is the sequence for consecutive equal timestamps
		me.ns.sequence = sLocal._sequence;
		// Bit 1 signifies that the UUID has been "initialized" (DCE design)
		me.ns._bitset = 0x1;
		me.ns._reserved = 0x0;
	}

	// Send the entire struct back on the stack to the caller.
	return me;
}


/*
 * This method will generate the CKUUID_struct that the first 32 hex
 * characters of this string represents. There may be more data in
 * the string but this method ignores all the rest.
 */
CKUUID_struct CKUUID::generateRawUUIDStructFromString( const CKString & aString )
{
	CKUUID_struct	retval;

	// I have to take only the first 32 as that defines the struct
	int args = sscanf(aString.substr(0, 32).c_str(), "%08x%08x%08x%08x",
						&retval.words[0], &retval.words[1],
						&retval.words[2], &retval.words[3]);
	if (args != 4) {
		// clear out what might be there
		for (int i = 0; i < 4; i++) {
			retval.words[i] = 0x0;
		}
		// ...and toss an exception
		std::ostringstream	msg;
		msg << "CKUUID::generateRawUUIDStructFromString(const CKString &) - "
			"the string '" << aString << "' did not contain the requisite "
			"number of hex characters to fully regenerate a UUID. This is a "
			"serious problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


/*
 * This method returns the numerical value of the last 8 hex
 * characters in this string. It is assumed that the last 8
 * characters are in fact hex charactrs, and simply turns these
 * into a numerical value. In the context of this class, this
 * number represents the hashed class name of this CKUUID.
 */
unsigned int CKUUID::generateHashedClassNameFromString( const CKString & aString )
{
	unsigned int	retval;

	/*
	 * I have to take only the last 8 as that's the place for the hashed
	 * class name.
	 */
	int args = sscanf(aString.substr(aString.size() - 8, 8).c_str(),
						"%08x", &retval);
	if (args != 1) {
		retval = 0x0;
		// ...and toss an exception
		std::ostringstream	msg;
		msg << "CKUUID::generateHashedClassNameFromString(const CKString &) - "
			"the string '" << aString << "' did not contain the requisite "
			"number of hex characters to fully regenerate a hashed class name. "
			"This is a serious problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return retval;
}


/*
 * This function should not be used by any non-CKUUID method. It is
 * the parser of the DCE UUID string that this class can use for
 * it's ivar data.
 */
CKUUID_struct CKUUID::generateRawUUIDStructFromDCEString( const CKString & aString )
{
	CKUUID_struct		retval;

	/*
	 * I have to take only the first 45 characters as that's the total length
	 * of the DCE formatted struct
	 */
	unsigned int	hashToss;
	unsigned int	time_low;
	unsigned int	time_mid;
	unsigned int	time_hi;
	unsigned int	clock_seq_hi;
	unsigned int	clock_seq_low;
	unsigned int	node[6];
	int args = sscanf(aString.substr(0, 45).c_str(),
						"%08x %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
						&hashToss, &time_low, &time_mid, &time_hi, &clock_seq_hi,
						&clock_seq_low, &node[0], &node[1], &node[2], &node[3],
						&node[4], &node[5]);
	if (args != 12) {
		for (int i = 0; i < 4; i++) {
			retval.words[i] = 0x0;
		}
		// ...and toss an exception
		std::ostringstream	msg;
		msg << "CKUUID::generateRawUUIDStructFromDCEString(const CKString &) - "
			"the string '" << aString << "' did not contain the requisite "
			"number of hex characters to fully regenerate a DCE-formatted "
			"UUID. This is a serious problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval.dce.time_low 				 = time_low;
		retval.dce.time_mid 				 = (unsigned short)time_mid;
		retval.dce.time_hi_and_version 		 = (unsigned short)time_hi;
		retval.dce.clock_seq_hi_and_reserved = (unsigned char)clock_seq_hi;
		retval.dce.clock_seq_low 			 = (unsigned char)clock_seq_low;
		for (int i = 0; i < 6; i++) {
			retval.dce.node[i] = (unsigned char)node[i];
		}
	}

	return retval;
}


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
unsigned int CKUUID::hashString( const CKString & aString )
{
	register unsigned int hashedValue = 0;
	const char* cStr = aString.c_str();

	if (cStr) {
		while (*cStr) {
			hashedValue ^= (*cStr << 11);
			hashedValue = (hashedValue * 117) + *cStr;
			cStr++;
		}
	}
	return hashedValue;
}


/*
 * For those rare times when a user actually needs access to the
 * structure of the UUID, this accessor method is available.
 * Typically, this should not be used as it represents a data
 * structure, and not a simple numerical, or string, ID number.
 */
CKUUID_struct CKUUID::getUUID() const
{
	if (!isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getUUID() - the UUID for "
			"this instance has not yet been generated. This means that it's "
			"not 'safe' to view. Please make sure to generate it before calling "
			"this method.");
	}
	return mUUID;
}


/*
 * To make sure that there are no accesses to the UUID instance
 * variable outside of our controlled environment, we have this
 * 'setter' method. If someone tries to set a UUID that is already
 * set, send an exception so they know it isn't possible.
 */
void CKUUID::setUUID( const CKUUID_struct & aUUID )
{
	if (isGenerated()) {
		throw CKException(__FILE__, __LINE__, "CKUUID::getUUID() - the UUID for "
			"this instance has already been generated. This means that it's "
			"not 'safe' to set another value on top of this one. Please make "
			"sure to generate it only once and then not set it again.");
	}
	// set the components of the UUID from the source
	for (int i = 0; i < 4; i++) {
		mUUID.words[i] = aUUID.words[i];
	}
}


/*
 * During initialization we'll possibly need to set the hashed name
 * of the class. This is the method that sets the name. It is
 * private because at no time after initialization should a user be
 * able to set this.
 */
void CKUUID::setHashedClassName( unsigned int aHashcode )
{
	mHashedClassName = aHashcode;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKUUID & anID )
{
	aStream << anID.toString();

	return aStream;
}
