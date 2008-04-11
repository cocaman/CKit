/*
 * CKURL.cpp - this file implements a simple URL system based on cURL that's
 *             on almost all platrofms. The idea is that while cURL is nice,
 *             it can be made nicer to the users of CKit by wrapping it up
 *             in an object and making it easy to pass in parameters and
 *             get the results back.
 *
 *             This is meant as a simple, light-weight class. It's
 *             thread-safe, but it's small and light enough that you should
 *             be creating one for each task or thread. It's smart enough that
 *             the connection to the host will be maintained if you issue
 *             multiple calls to the same host.
 *
 * $Id: CKURL.cpp,v 1.1 2008/04/11 18:22:04 drbob Exp $
 */

//	System Headers
#include <string.h>

//	Third-Party Headers

//	Other Headers
#include "CKURL.h"
#include "CKStackLocker.h"
#include "CKException.h"

//	Forward Declarations

//	Static Declarations
/*
 * Because cURL requires that we do some global initialization
 * and cleanup, I need to have a variable that will tell me the
 * state of that initialization. Did we try it yet? Was it
 * successful? Did it fail with an error code? All these will
 * be held in this class ivar and protected by the mutex so that
 * there's no way two threads are going to mess things up.
 */
int			CKURL::mGlobalStatus = CKURL_CURL_UNINITIALIZED;
// ...and the mutex that will protect it
CKFWMutex	CKURL::mGlobalMutex;

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * The issue with cURL is that you have to call a global initialization
 * method before you do anything, and when you're all done, you need to
 * call a global cleanup method. This is OK if you're going to work with
 * cURL in your app, but for CKit users, they don't need to know this.
 * While I could put in an instance counter and then do the init/cleanup
 * when the count hit 0, that's doing it too many times in the same app.
 *
 * A far better solution is the Canary.
 *
 * What I'm going to do is to have a static instance of the Canary that
 * will do nothing until it goes out of scope - at which time it'll mean
 * that the application is dead. At this time, it'll look to see if the
 * cURL library has been globally initialized, and if it has, then it'll
 * do the global cleanup. The global init will be done in the instance
 * init where it'll check to see if things have been initialized, and if
 * not, then it'll do them at that time.
 *
 * This means that we'll initialize only when we need to, and cleanup
 * only at the end of the application. Nice.
 */
static CKURLCanary		sCKURLBirdInACage;


/*
 * The initialization of cURL is not really thought out as well as it
 * might be. Maybe this is because of the libraries it uses, or maybe
 * it's because the developers didn't imagine that someone would want
 * to use their code sometimes and not all the time... but the problem
 * is this: we have to do the global initialization of cURL before
 * anything really gets going - with only a single thread in the system.
 * Likewise, we have to clean it up at the end under the same conditions -
 * only one thread operating in the system. This is really pretty bizarre
 * if you ask me, but maybe there's a reason for it that I can't see.
 * But my guess is that the developers are just lazy and think everyone
 * can put this one call into their code at the beginning and the end
 * and that's all that's needed.
 *
 * The way we're going to accomplish this is to have a static canary
 * like so many other projects I've done in the past. Make this guy
 * static so he's created when the library is loaded, and his constructor
 * will fire when there's the minimum of fuss - ideally when there's only
 * on thread active, but we can't really control that 100% of the time.
 *
 * On shutdown, the destructor will be called, and the global cleanup
 * code will be called. In general, this is a decent idea, but it fails
 * in the area of efficiency. There's no way to have 'lazy global
 * initialization' that's done when it's needed as when it's first
 * needed there may be many threads active in the system at that time.
 *
 * So we're in a pickle. If we follow the cURL docs then all CKit apps
 * will have cURL initialized - like it or not. If we do the lazy
 * global initialization then we run the risk of messing things up.
 */
/*
 * Right now, we're going to have an #ifdef on the startup. I think
 * I can get away without it, but I may be forced into it, or the
 * user may have to call it on their own. I don't like that idea one
 * bit.
 */
CKURLCanary::CKURLCanary()
{
#ifdef USE_CURL_EARLY_STARTUP
	CKURL::doGlobalStartup();
#endif
};


/*
 * This is the canary's destructor that *always* needs to be called
 * when the static canary goes out of scope.
 */
CKURLCanary::~CKURLCanary()
{
	CKURL::doGlobalShutdown();
};


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the standard empty constructor that creates a new
 * empty URL. You're going to have to fill in all the stuff
 * before it'll be any good.
 */
CKURL::CKURL() :
	mHandle(NULL),
	mHandleMutex(),
	mLocation(),
	mFields(),
	mFieldsMutex(),
	mURL(),
	mPOSTvars(),
	mResults()
{
}


/*
 * This form of the constructor takes a simple string URL and
 * saves it so that you can execute it as a GET or POST with
 * the appropriate call. This is primarily used for simple
 * URLs - those most often using GET.
 */
CKURL::CKURL( const CKString & aURL ) :
	mHandle(NULL),
	mHandleMutex(),
	mLocation(),
	mFields(),
	mFieldsMutex(),
	mURL(),
	mPOSTvars(),
	mResults()
{
	mLocation = aURL;
}


/*
 * This form of the constructor takes a base URL and then a
 * map of fields, or options, or arguments to the URL that will
 * be set in the POST or formatted in the GET. This is a nice,
 * general way of constructing a complex URL.
 */
CKURL::CKURL( const CKString & aURL, const CKURLFieldMap & aMap ) :
	mHandle(NULL),
	mHandleMutex(),
	mLocation(),
	mFields(),
	mFieldsMutex(),
	mURL(),
	mPOSTvars(),
	mResults()
{
	mLocation = aURL;
	mFields = aMap;
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKURL::CKURL( CKURL & anOther ) :
	mHandle(NULL),
	mHandleMutex(),
	mLocation(),
	mFields(),
	mFieldsMutex(),
	mURL(),
	mPOSTvars(),
	mResults()
{
	// let the '=' operator do all the heavy lifting
	*this = anOther;
}


CKURL::CKURL( const CKURL & anOther ) :
	mHandle(NULL),
	mHandleMutex(),
	mLocation(),
	mFields(),
	mFieldsMutex(),
	mURL(),
	mPOSTvars(),
	mResults()
{
	// let the '=' operator do all the heavy lifting
	*this = anOther;
}


/*
 * This is the standard destructor for this guy and makes sure
 * that everything is cleaned up before leaving.
 */
CKURL::~CKURL()
{
	// we need to shut down the cURL instance stuff
	doShutdown();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKURL & CKURL::operator=( CKURL & anOther )
{
	/*
	 * We have to watch out for someone setting this string equal
	 * to itself. If they did, we'd wipe out what is in us for what's
	 * in the other, but since the other *is* us, we'd have nothing
	 * to put back in. So we have to make sure we're not doing this
	 * to ourselves.
	 */
	if (this != & anOther) {
		/*
		 * We can't copy the cURL handle from the other guy, and we don't
		 * need to. If we have one, then we're good to go. If we don't,
		 * then we need to make one. But once we make one, that's all that
		 * matters.
		 */
		doStartup();
		// now get all the things we need from the other guy
		mLocation = anOther.mLocation;
		mFields = anOther.mFields;
		mResults = anOther.mResults;
	}

	return *this;
}


CKURL & CKURL::operator=( const CKURL & anOther )
{
	this->operator=((CKURL &) anOther);

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the location, or 'base' URL for this instance.
 * In general terms, this is the server and directory path of the
 * URL - but does not necessarily contain any optional parameters
 * or fields.
 */
void CKURL::setLocation( const CKString & aLocation )
{
	mLocation = aLocation;
}


/*
 * This method returns the location, or 'base' URL for this instance
 * which is what will be used on any calls to doGET or doPOST. If you
 * make multiple calls to this location, the connection will remain
 * open to be more efficient.
 */
const CKString & CKURL::getLocation() const
{
	return mLocation;
}


/*
 * This method copies all the key/value pairs of the supplied
 * map into the map of field names and values that this instance
 * will use on it's next call. It's important to note that a COPY
 * is being made.
 */
void CKURL::setFields( const CKURLFieldMap & aMap )
{
	CKStackLocker		lockem(&mFieldsMutex);

	mFields = aMap;
}


/*
 * This method returns the map of the field names and values that
 * will be used on the next call to doGET or doPOST.
 */
const CKURLFieldMap & CKURL::getFields() const
{
	return mFields;
}


/*
 * This method will add the provided key/value pair to the map of
 * fields to use on the next call to doGET or doPOST. Please note
 * that a COPY is made of these strings for this instance's use.
 */
bool CKURL::addToFields( const CKString & aKey, const CKString & aValue )
{
	bool		error = false;

	if (aKey.empty()) {
		error = true;
		std::ostringstream	msg;
		msg << "CKURL::addToFields(const CKString &, const CKString &) - the "
			"CKString value of the 'key' is empty and that's not allowed. "
			"Please make sure the key is not empty before calling this method.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		CKStackLocker		lockem(&mFieldsMutex);

		mFields[aKey] = aValue;
	}

	return !error;
}


/*
 * This method is the same as addToFields() but presents a little
 * different signature so that people wanting to deal with the
 * fields as a map can 'set' and 'get' based on the key value.
 */
bool CKURL::setField( const CKString & aKey, const CKString & aValue )
{
	return addToFields(aKey, aValue);
}


/*
 * This method returns the reference to the value of the provided
 * key - assuming it's in the map of all field values. If it's
 * not, a CKException will be thrown - catch it if you're not
 * sure the key has been defined.
 */
const CKString & CKURL::getField( const CKString & aKey ) const
{
	if (aKey.empty()) {
		std::ostringstream	msg;
		msg << "CKURL::getField(const CKString &) - the CKString value of the "
			"'key' is empty and that's not allowed. Please make sure the key "
			"is not empty before calling this method.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	
	// see if it's in the map - of not, then it's an exception
	CKStackLocker		lockem((CKFWMutex *)&mFieldsMutex);

	CKURLFieldMap::const_iterator	i = mFields.find(aKey);
	if (i == mFields.end()) {
		std::ostringstream	msg;
		msg << "CKURL::getField(const CKString &) - the key: '" << aKey <<
			"' is not in the list of fields at this time. "
			"You might want to make sure it's there first.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return ((CKURLFieldMap &)mFields)[(CKString &)aKey];
}


/*
 * This method removes the key and it's associated value from the
 * map of fields for this instance. If the key is not in the map
 * at this time, nothing will be done, but no exception will be
 * thrown.
 */
void CKURL::removeFromFields( const CKString & aKey )
{
	if (aKey.empty()) {
		std::ostringstream	msg;
		msg << "CKURL::getField(const CKString &) - the CKString value of the "
			"'key' is empty and that's not allowed. Please make sure the key "
			"is not empty before calling this method.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	
	// erase it from the map - if it's not there no biggie
	CKStackLocker		lockem(&mFieldsMutex);

	mFields.erase(aKey);
}


/*
 * This method will clear out all the field variables so that the
 * next call to doGET or doPOST started with a clean slate.
 */
void CKURL::clearFields()
{
	// erase everything from the map
	CKStackLocker		lockem(&mFieldsMutex);

	mFields.clear();
}


/*
 * This method returns a reference to the results we've gotten
 * from the doGET or doPOST. If nothing has been done, this will
 * be empty, so it's a good idea to call haveResults() to see if
 * there's really something to get.
 */
const CKString & CKURL::getResults() const
{
	return mResults;
}


/*
 * If you're done with the results, you can call this method and
 * it'll clear out the results. However, realize that there will
 * be no memory reclimation until this instance is destroyed. It's
 * only then that all resources used by this instance will be
 * returned to the system pool.
 */
void CKURL::clearResults()
{
	mResults.clear();
}


/*
 * This method returns 'true' if there is something in the results
 * as a consequence of the call to doGET or doPOST. It's a nice
 * way to check and see if there's anything there to process.
 */
bool CKURL::haveResults() const
{
	return !mResults.empty();
}


/********************************************************
 *
 *               Requesting Methods
 *
 ********************************************************/
/*
 * This method will check to make sure that the base URL has been
 * defined, and if it hasn't, then a CKException will be thrown, but
 * if it's there, then a GET will be issued to that URL with the
 * fields appended properly and encoded so that everything will be
 * processed properly. The results of this will be stored in the
 * instance for later examination and/or processing.
 */
bool CKURL::doGET()
{
	bool		error = false;

	// first, make sure we're all ready to go
	if (!error) {
		if (!doStartup()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doGET() - the cURL libraries could not be initialized "
				"properly. This is a serious error. Please check the logs for "
				"any possible details.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, make sure we have a location - best check on it
	if (!error) {
		if (mLocation.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doGET() - the 'base' URL (location) is empty "
				"and that means that there's no place to send the request to. "
				"Please make sure that there's a defined location before "
				"trying to request data.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, get the URL we're going to be using
	CKString	url;
	if (!error) {
		// start with the base URL
		url = mLocation;
		// ...now add in the fields
		CKString	flds = encodeFields();
		if (!flds.empty()) {
			// slap down the separator between the base and fields
			url.append("?");
			// ...and then add the fields
			url.append(flds);
		}
		// check what we have at this point in time
		if (url.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doGET() - the URL is empty and that means that "
				"there's no place to send the request to. check the logs to "
				"see what might have caused this.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's set up cURL and do the magic
	if (!error) {
		CKStackLocker		lockem(&mHandleMutex);

		// set the URL - if it's different
		if (url != mURL) {
			mURL = url;
			curl_easy_setopt(mHandle, CURLOPT_URL, mURL.c_str());
		}
		
		// no need for the progress meter
		curl_easy_setopt(mHandle, CURLOPT_NOPROGRESS, 1);
		
		// get an error message
		char	errBuf[CURL_ERROR_SIZE];
		bzero(errBuf, CURL_ERROR_SIZE);
		curl_easy_setopt(mHandle, CURLOPT_ERRORBUFFER, errBuf);
		
		// set up the callback for getting the data
		mResults.clear();
		curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, captureData);
		curl_easy_setopt(mHandle, CURLOPT_WRITEDATA, &mResults);
		
		// do the 'do'
		if (curl_easy_perform(mHandle) != 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doGET() - while processing the GET an error was "
				"returned from cURL: '" << errBuf << "'";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/*
 * This method will check to make sure that the base URL has been
 * defined, and if it hasn't, then a CKException will be thrown, but
 * if it's there, then a POST will be issued to that URL with the
 * fields properly applied so that everything will be
 * processed properly. The results of this will be stored in the
 * instance for later examination and/or processing.
 */
bool CKURL::doPOST()
{
	bool		error = false;

	// first, make sure we're all ready to go
	if (!error) {
		if (!doStartup()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doPOST() - the cURL libraries could not be initialized "
				"properly. This is a serious error. Please check the logs for "
				"any possible details.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, make sure we have a location - best check on it
	if (!error) {
		if (mLocation.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doPOST() - the 'base' URL (location) is empty "
				"and that means that there's no place to send the request to. "
				"Please make sure that there's a defined location before "
				"trying to request data.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's set up cURL and do the magic
	if (!error) {
		CKStackLocker		lockem(&mHandleMutex);

		// set the URL - if it's different
		if (mLocation != mURL) {
			mURL = mLocation;
			curl_easy_setopt(mHandle, CURLOPT_URL, mURL.c_str());
		}
		
		// get the encoded fields and set them as POST variables
		CKString	flds = encodeFields();
		if (!flds.empty() && (flds != mPOSTvars)) {
			// save these variables
			mPOSTvars = flds;
			// ...and then give them to the handle for processing
			curl_easy_setopt(mHandle, CURLOPT_POSTFIELDS, mPOSTvars.c_str());
		}
		
		// no need for the progress meter
		curl_easy_setopt(mHandle, CURLOPT_NOPROGRESS, 1);
		
		// get an error message
		char	errBuf[CURL_ERROR_SIZE];
		bzero(errBuf, CURL_ERROR_SIZE);
		curl_easy_setopt(mHandle, CURLOPT_ERRORBUFFER, errBuf);
		
		// set up the callback for getting the data
		mResults.clear();
		curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, captureData);
		curl_easy_setopt(mHandle, CURLOPT_WRITEDATA, &mResults);
		
		// do the 'do'
		if (curl_easy_perform(mHandle) != 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKURL::doGET() - while processing the POST an error was "
				"returned from cURL: '" << errBuf << "'";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return !error;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKURLs are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKURL::operator==( CKURL & anOther )
{
	bool		equal = true;

	// check the contents
	if (equal) {
		if ((mLocation != anOther.mLocation) ||
			(mFields != anOther.mFields) ||
			(mResults != anOther.mResults)) {
			equal = false;
		}
	}

	return equal;
}


bool CKURL::operator==( const CKURL & anOther )
{
	return this->operator==((CKURL & )anOther);
}


bool CKURL::operator==( CKURL & anOther ) const
{
	return ((CKURL *)this)->operator==(anOther);
}


bool CKURL::operator==( const CKURL & anOther ) const
{
	return ((CKURL *)this)->operator==((CKURL & )anOther);
}


/*
 * This method checks to see if the two CKURLs are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKURL::operator!=( CKURL & anOther )
{
	return !this->operator==(anOther);
}


bool CKURL::operator!=( const CKURL & anOther )
{
	return !this->operator==((CKURL &)anOther);
}


bool CKURL::operator!=( CKURL & anOther ) const
{
	return !((CKURL *)this)->operator==(anOther);
}


bool CKURL::operator!=( const CKURL & anOther ) const
{
	return !((CKURL *)this)->operator==((CKURL &)anOther);
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKURL::toString() const
{
	bool		error = false;
	CKString	retval = "<empty";
	
	if (!error) {
		if (!mLocation.empty()) {
			retval = "<Location='";
			retval.append(mLocation).append("'");
		}
	}

	if (!error) {
		CKStackLocker		lockem((CKFWMutex *)&mFieldsMutex);
		
		// see if we have any fields
		if (mFields.size() > 0) {
			// slap down the separator between the base and fields
			retval.append(", Fields={");
			// now let's run through all the fields
			CKURLFieldMap::const_iterator		i;
			for (i = mFields.begin(); i != mFields.end(); ++i) {
				// see if I need to add a '&' separator
				if (i != mFields.begin()) {
					retval.append(", ");
				}
				// ...now slap down the encoded key and value
				retval.append("[").append(i->first).append('=').
						append(i->second).append("]");
			}
			// terminate the field list
			retval.append("}");
		}
	}

	retval.append(">");

	return retval;
}


/********************************************************
 *
 *           cURL Initialization Methods
 *
 ********************************************************/
/*
 * For cURL, each instance will need to get a 'handle' - just
 * like an instance in C++. This method makes sure that we have
 * a handle that's ready to use.
 */
bool CKURL::doStartup()
{
	bool		error = false;

	// lock this guy up
	CKStackLocker		lockem(&mHandleMutex);

	// ...and check to see if there's something I need to do
	if (mHandle == NULL) {
		// first, make sure the global initialization is done
		if (doGlobalStartup()) {
			// now initialize a new handle for curl
			mHandle = curl_easy_init();
			if (mHandle == NULL) {
				error = true;
			}
		}
	}

	return !error;
}


/*
 * In the destructor we need to shut down this instance, and in
 * cURL terms that means we need to clean it up. This guy makes
 * sure that we do this properly.
 */
bool CKURL::doShutdown()
{
	bool		error = false;

	// lock this guy up
	CKStackLocker		lockem(&mHandleMutex);

	// ...and check to see if there's something I need to do
	if (mHandle != NULL) {
		curl_easy_cleanup(mHandle);
		mHandle = NULL;
	}

	return !error;
}


/********************************************************
 *
 *           cURL Encoding/Decoding Methods
 *
 ********************************************************/
/*
 * This method takes a string and converts it to URL encoding so
 * that none of the characters in the string are going to give
 * the web server fits. This is an essential part of the creation
 * of the GET URL.
 */
CKString CKURL::encode( const CKString & aString )
{
	CKString	retval;
	
	// get the (char *) for this guy as that's the easiest way
	const char *str = aString.c_str();
	if (str != NULL) {
		int		len = strlen(str);
		for (int i = 0; i < len; ++i) {
			if (isalnum(str[i])) {
				// all a-z, A-Z, 0-9 pass through unaltered
				retval.append((char)str[i]);
			} else {
				// all else needs to be converted to hex values
				retval.append("%");
				char	msn = (char)((str[i] & 0xf0) >> 4);
				char	lsn = (char)(str[i] & 0x0f);
				if (msn <= 9) {
					retval.append((char)(msn + '0'));
				} else {
					retval.append((char)(msn - 10 + 'A'));
				}
				if (lsn <= 9) {
					retval.append((char)(lsn + '0'));
				} else {
					retval.append((char)(lsn - 10 + 'A'));
				}
			}
		}
	}

	return retval;
}


/*
 * This method takes all the fields and encodes them as if they
 * were going to be part of the GET URL. This is the way cURL
 * parses them for POST variables as well, so it makes sense to
 * have a general method for encoding these variables.
 */
CKString CKURL::encodeFields()
{
	bool		error = false;
	CKString	retval;

	/*
	 * At this point, we need to process the fields in this guy
	 * and add them to the return value properly formatted. They
	 * look like this:
	 *     <key>=<value>&<key>=<value>&...
	 * and on the front of the first one, we need to add the '?'
	 * in order to separate the base URL from the fields. The
	 * final trick is that the keys and values have to be run
	 * through encode() to encode them properly.
	 */
	if (!error) {
		CKStackLocker		lockem(&mFieldsMutex);
		
		// see if we have any fields
		if (mFields.size() > 0) {
			// let's run through all the fields
			CKURLFieldMap::iterator		i;
			for (i = mFields.begin(); i != mFields.end(); ++i) {
				// see if I need to add a '&' separator
				if (i != mFields.begin()) {
					retval.append('&');
				}
				// ...now slap down the encoded key and value
				retval.append(encode(i->first)).append('=').
						append(encode(i->second));
			}
		}
	}

	return retval;
}


/********************************************************
 *
 *               cURL Callback Methods
 *
 ********************************************************/
/*
 * This is the static method that is the callback for cURL's
 * getting data and passing it to us. What we're going to do
 * is to have the 'user_ptr' be a pointer to a CKString - the
 * results, and then in this method we'll simply append the
 * data in the buffer to that CKString and then return the
 * number of bytes added. Simple.
 */
size_t CKURL::captureData( void *buffer, size_t size, size_t nmemb, void *user_ptr )
{
	bool		error = false;
	size_t		retval = 0;

	// first, see if we have the pointer that we need
	if (!error) {
		if (user_ptr == NULL) {
			error = true;
		}
	}
	
	// let's see what kind of data we're getting
	if (!error) {
		if (size == sizeof(char)) {
			((CKString *)user_ptr)->append((char *)buffer, nmemb);
			retval = nmemb;
		}
	}

	return error ? 0 : retval;
}


/********************************************************
 *
 *           cURL Global Initialization Methods
 *
 ********************************************************/
/*
 * This method is called to ensure that the cURL global
 * initialization is done and that the 'global status' is set
 * based on the outcome of that initialization. This has to be
 * controlled by a mutex because cURL clearly states that this
 * global initialization is NOT thread-safe.
 */
bool CKURL::doGlobalStartup()
{
	bool		error = false;

	// lock this guy up
	CKStackLocker		lockem(&mGlobalMutex);

	// ...and check to see if there's something I can do
	if (mGlobalStatus == CKURL_CURL_UNINITIALIZED) {
		if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
			error = true;
			mGlobalStatus = CKURL_CURL_FAILED_INIT;
		} else {
			mGlobalStatus = CKURL_CURL_INITIALIZED;
		}
	}
	
	return !error;
}


/*
 * This method is called once, and only once, in the life of
 * the application, and it's on the very end of life. What it
 * does is to globally cleanup everything that cURL has done
 * so that it's a 'good citizen' of the system.
 */
bool CKURL::doGlobalShutdown()
{
	bool		error = false;

	// lock this guy up
	CKStackLocker		lockem(&mGlobalMutex);

	// ...and check to see if there's something I can do
	if (mGlobalStatus == CKURL_CURL_INITIALIZED) {
		curl_global_cleanup();
		mGlobalStatus = CKURL_CURL_UNINITIALIZED;
	}
	
	return !error;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKURL & aURL )
{
	aStream << aURL.toString();

	return aStream;
}


std::ostream & operator<<( std::ostream & aStream, const CKURL & aURL )
{
	aStream << ((CKURL &)aURL).toString();

	return aStream;
}
