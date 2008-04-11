/*
 * CKURL.h - this file defines a simple URL system based on cURL that's
 *           on almost all platrofms. The idea is that while cURL is nice,
 *           it can be made nicer to the users of CKit by wrapping it up
 *           in an object and making it easy to pass in parameters and
 *           get the results back.
 *
 *           This is meant as a simple, light-weight class. It's
 *           thread-safe, but it's small and light enough that you should
 *           be creating one for each task or thread. It's smart enough that
 *           the connection to the host will be maintained if you issue
 *           multiple calls to the same host.
 *
 * $Id: CKURL.h,v 1.1 2008/04/11 18:22:04 drbob Exp $
 */
#ifndef __CKURL_H
#define __CKURL_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sstream>
#include <map>

//	Third-Party Headers
/*
 * While this is technically a third-party library, it's in so many
 * systems, it's almost something to be considered as 'system'. So,
 * rather than mask it from the user, let's plop it out here in the
 * open.
 */
#include <curl/curl.h>

//	Other Headers
#include "CKString.h"
#include "CKFWMutex.h"

//	Forward Declarations

//	Public Constants
/*
 * I need to have a global status on cURL, and these are my values so
 * that I can tell if I've tried to do the initialization (global or
 * local) and what the results might have been.
 */
#define CKURL_CURL_UNINITIALIZED		-1
#define CKURL_CURL_INITIALIZED			0
#define CKURL_CURL_FAILED_INIT			1

//	Public Datatypes
/*
 * We're going to be storing all the string options as a simple map as
 * we have to hold on to them as cURL expects us to. There are options
 * that aren't strings, but in reality they aren't going to be exposed
 * to the user as they are internal to the implementation.
 */
typedef std::map<CKString, CKString> CKURLFieldMap;

//	Public Data Constants
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
class CKURLCanary
{
	public:
		CKURLCanary();
		virtual ~CKURLCanary();
};


/*
 * This is the main class definition.
 */
class CKURL
{
	public :
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
		CKURL();
		/*
		 * This form of the constructor takes a simple string URL and
		 * saves it so that you can execute it as a GET or POST with
		 * the appropriate call. This is primarily used for simple
		 * URLs - those most often using GET.
		 */
		CKURL( const CKString & aURL );
		/*
		 * This form of the constructor takes a base URL and then a
		 * map of fields, or options, or arguments to the URL that will
		 * be set in the POST or formatted in the GET. This is a nice,
		 * general way of constructing a complex URL.
		 */
		CKURL( const CKString & aURL, const CKURLFieldMap & aMap );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKURL( CKURL & anOther );
		CKURL( const CKURL & anOther );
		/*
		 * This is the standard destructor for this guy and makes sure
		 * that everything is cleaned up before leaving.
		 */
		virtual ~CKURL();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKURL & operator=( CKURL & anOther );
		CKURL & operator=( const CKURL & anOther );

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
		void setLocation( const CKString & aLocation );
		/*
		 * This method returns the location, or 'base' URL for this instance
		 * which is what will be used on any calls to doGET or doPOST. If you
		 * make multiple calls to this location, the connection will remain
		 * open to be more efficient.
		 */
		const CKString & getLocation() const;

		/*
		 * This method copies all the key/value pairs of the supplied
		 * map into the map of field names and values that this instance
		 * will use on it's next call. It's important to note that a COPY
		 * is being made.
		 */
		void setFields( const CKURLFieldMap & aMap );
		/*
		 * This method returns the map of the field names and values that
		 * will be used on the next call to doGET or doPOST.
		 */
		const CKURLFieldMap & getFields() const;
		/*
		 * This method will add the provided key/value pair to the map of
		 * fields to use on the next call to doGET or doPOST. Please note
		 * that a COPY is made of these strings for this instance's use.
		 */
		bool addToFields( const CKString & aKey, const CKString & aValue );
		/*
		 * This method is the same as addToFields() but presents a little
		 * different signature so that people wanting to deal with the
		 * fields as a map can 'set' and 'get' based on the key value.
		 */
		bool setField( const CKString & aKey, const CKString & aValue );
		/*
		 * This method returns the reference to the value of the provided
		 * key - assuming it's in the map of all field values. If it's
		 * not, a CKException will be thrown - catch it if you're not
		 * sure the key has been defined.
		 */
		const CKString & getField( const CKString & aKey ) const;
		/*
		 * This method removes the key and it's associated value from the
		 * map of fields for this instance. If the key is not in the map
		 * at this time, nothing will be done, but no exception will be
		 * thrown.
		 */
		void removeFromFields( const CKString & aKey );
		/*
		 * This method will clear out all the field variables so that the
		 * next call to doGET or doPOST started with a clean slate.
		 */
		void clearFields();

		/*
		 * This method returns a reference to the results we've gotten
		 * from the doGET or doPOST. If nothing has been done, this will
		 * be empty, so it's a good idea to call haveResults() to see if
		 * there's really something to get.
		 */
		const CKString & getResults() const;
		/*
		 * If you're done with the results, you can call this method and
		 * it'll clear out the results. However, realize that there will
		 * be no memory reclimation until this instance is destroyed. It's
		 * only then that all resources used by this instance will be
		 * returned to the system pool.
		 */
		void clearResults();
		/*
		 * This method returns 'true' if there is something in the results
		 * as a consequence of the call to doGET or doPOST. It's a nice
		 * way to check and see if there's anything there to process.
		 */
		bool haveResults() const;

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
		bool doGET();
		/*
		 * This method will check to make sure that the base URL has been
		 * defined, and if it hasn't, then a CKException will be thrown, but
		 * if it's there, then a POST will be issued to that URL with the
		 * fields properly applied so that everything will be
		 * processed properly. The results of this will be stored in the
		 * instance for later examination and/or processing.
		 */
		bool doPOST();

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
		bool operator==( CKURL & anOther );
		bool operator==( const CKURL & anOther );
		bool operator==( CKURL & anOther ) const;
		bool operator==( const CKURL & anOther ) const;
		/*
		 * This method checks to see if the two CKURLs are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKURL & anOther );
		bool operator!=( const CKURL & anOther );
		bool operator!=( CKURL & anOther ) const;
		bool operator!=( const CKURL & anOther ) const;

		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		CKString toString() const;

	protected:
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
		bool doStartup();
		/*
		 * In the destructor we need to shut down this instance, and in
		 * cURL terms that means we need to clean it up. This guy makes
		 * sure that we do this properly.
		 */
		bool doShutdown();

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
		CKString encode( const CKString & aString );
		/*
		 * This method takes all the fields and encodes them as if they
		 * were going to be part of the GET URL. This is the way cURL
		 * parses them for POST variables as well, so it makes sense to
		 * have a general method for encoding these variables.
		 */
		CKString encodeFields();

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
		static size_t captureData( void *buffer, size_t size, size_t nmemb, void *user_ptr );

	private:
		friend class CKURLCanary;

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
		static bool doGlobalStartup();
		/*
		 * This method is called once, and only once, in the life of
		 * the application, and it's on the very end of life. What it
		 * does is to globally cleanup everything that cURL has done
		 * so that it's a 'good citizen' of the system.
		 */
		static bool doGlobalShutdown();

		/*
		 * Because cURL requires that we do some global initialization
		 * and cleanup, I need to have a variable that will tell me the
		 * state of that initialization. Did we try it yet? Was it
		 * successful? Did it fail with an error code? All these will
		 * be held in this class ivar and protected by the mutex so that
		 * there's no way two threads are going to mess things up.
		 */
		static int			mGlobalStatus;
		// ...and the mutex that will protect it
		static CKFWMutex	mGlobalMutex;

		/*
		 * The basis of all operations in cURL is the CURL handle. This is
		 * obtained on the first call to curl_easy_init() and it maintained
		 * as long as this instance is around.
		 */
		CURL				*mHandle;
		// ...and the mutex to protect against overlapping calls
		CKFWMutex			mHandleMutex;

		/*
		 * This is the 'base URL' or location as you'd type it into a
		 * browser. This has to be set as the fields (or parameters) are
		 * tacked onto the end of this for a GET, or added to the 'handle'
		 * for a POST.
		 */
		CKString			mLocation;

		/*
		 * These are the string options that I'm going to be passing into
		 * the cURL handle when I need to get it ready to GET or POST.
		 * The reason that I have a separate copy is because there's no
		 * real provision for cURL to 'give back' it's fields and I think
		 * it'll be important in debugging and resetting.
		 */
		CKURLFieldMap		mFields;
		// ...and the mutex that protects it
		CKFWMutex			mFieldsMutex;
		
		/*
		 * This is a limitation with cURL - it doesn't copy anything. So
		 * if I give it a URL, I need to keep it around for as long as
		 * cURL might need it. This means that I'm going to have to save
		 * the URL I give cURL, and this is that URL.
		 */
		CKString			mURL;
		/*
		 * Another little oddity of cURL is the fact that the POST variables
		 * need to be provided as a single string in the same manner as the
		 * GET fields. cURL then decodes them and uses them. Problem is that
		 * you need to hold onto them like all strings, so we need to have
		 * something here to hold onto them as the call is made.
		 */
		CKString			mPOSTvars;

		/*
		 * This is the data that's retured from the request. I'm going to
		 * buffer it in this instance because the user may want to do some
		 * things with it as opposed to handle it as it comes in.
		 */
		CKString			mResults;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKURL & aURL );
std::ostream & operator<<( std::ostream & aStream, const CKURL & aURL );

#endif	// __CKURL_H
