/*
 * CKFWMutex.cpp - this file implements the simple mutex that can
 *                 be used in a large number of applications.
 *
 * $Id: CKFWThread.cpp,v 1.11 2004/09/22 21:09:16 drbob Exp $
 */

//	System Headers
#include <pthread.h>
#include <math.h>
#include <iostream>
#include <sstream>
#ifdef GPP2
#include <exception>
#endif

//	Third-Party Headers
#include <SQLAPI.h>

//	Other Headers
#include "CKFWThread.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
const int CKFWThread::cDefaultPolicy = SCHED_OTHER;
const double CKFWThread::cDefaultPriority =  0.5;
const int CKFWThread::cDefaultScope = PTHREAD_SCOPE_SYSTEM;

const int CKFWThread::cSuccess = 0;
const int CKFWThread::cDone = -1;
const int CKFWThread::cExceptionCaught = 1;
const int CKFWThread::cUnexpectedException = 2;


CKFWThread::CKFWThread( int aPolicy,
                        double aPriority,
                        int aScope,
                        int aIsDetachable ) :
	mPolicy( aPolicy ),
	mPriority( aPriority ),
	mScope( aScope ),
	mIsDetachable( aIsDetachable ),
	mTag( NULL )
{
	return;
}

CKFWThread::~CKFWThread( )
{
	setTag(NULL);
  return;
}

CKFWThread & CKFWThread::operator=( CKFWThread & anOther )
{
	mPolicy = anOther.mPolicy;
	mPriority = anOther.mPriority;
	mScope = anOther.mScope;
	mIsDetachable = anOther.mIsDetachable;
	setTag(anOther.mTag);

	return *this;
}

/*
 * This is used to 'tag' the thread so that the exception reports tell
 * us something more than nothing. It's about the only information we're
 * going to get in some cases.
 */
void CKFWThread::setTag( const char *aTag )
{
	if (mTag != NULL) {
			delete [] mTag;
			mTag = NULL;
	}
	if (aTag != NULL) {
			int		size = strlen(aTag) + 1;
			mTag = new char[size];
			if (mTag == NULL) {
				std::ostringstream	msg;
				msg << "CKFWThread::setTag(const char*) - space for the tag ('" <<
						aTag << "') could not be created. This is a serious allocation "
						"error.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
					strncpy(mTag, aTag, size);
			}
	}
}


void CKFWThread::run( )
{
	bool		error = false;

	try {
		if ( initialize( ) != cSuccess ) {
			error = true;
		}
	} catch ( CKException & lException ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread a CKException was thrown: " <<
			lException.getMessage() << std::endl;
	} catch ( char* charstar ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread a (char*) exception was thrown: " << charstar <<
			std::endl;
	} catch ( std::string & str ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread a std::string exception was thrown: " << str <<
			std::endl;
	} catch ( std::exception & excep ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread a std::exception exception was thrown: " <<
			excep.what() << std::endl;
	} catch ( SAException & sae ) {
		error = true;
		std::string	excep = (const SAChar *)sae.ErrText();
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread a SAException was thrown: " << excep << std::endl;
	} catch ( ... ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"initializing the thread an unknown exception was thrown." << std::endl;
	}

	try {
		if ( !error ) {
			while( process( ) == cSuccess );
		}
	} catch ( CKException & lException ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread a CKException was thrown: " <<
			lException.getMessage() << std::endl;
	} catch ( char* charstar ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread a (char*) exception was thrown: " << charstar <<
			std::endl;
	} catch ( std::string & str ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread a std::string exception was thrown: " << str <<
			std::endl;
	} catch ( std::exception & excep ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread a std::exception exception was thrown: " <<
			excep.what() << std::endl;
	} catch ( SAException & sae ) {
		error = true;
		std::string	excep = (const SAChar *)sae.ErrText();
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread a SAException was thrown: " << excep << std::endl;
	} catch ( ... ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"running the thread an unknown exception was thrown." << std::endl;
	}

	try {
		terminate( );
	} catch ( CKException & lException ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread a CKException was thrown: " <<
			lException.getMessage() << std::endl;
	} catch ( char* charstar ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread a (char*) exception was thrown: " << charstar <<
			std::endl;
	} catch ( std::string & str ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread a std::string exception was thrown: " << str <<
			std::endl;
	} catch ( std::exception & excep ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread a std::exception exception was thrown: " <<
			excep.what() << std::endl;
	} catch ( SAException & sae ) {
		error = true;
		std::string	excep = (const SAChar *)sae.ErrText();
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread a SAException was thrown: " << excep << std::endl;
	} catch( ... ) {
		error = true;
		std::cerr << "CKFWThread::run(" << (mTag == NULL ? "" : mTag) << ") - while "
			"terminating the thread an unknown exception was thrown." << std::endl;
	}
}

int CKFWThread::process( )
{
  return 0;
}
int CKFWThread::initialize( )
{
  return 0;
}

void CKFWThread::join( )
{
  int lResultCode = 0;
  if ( ( lResultCode = pthread_join( mThread, NULL ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lResultCode );
  }
}

int CKFWThread::start( )
{
  int lReturnCode = 0;
  pthread_attr_t lThreadAttribute;
  int lThreadPolicy;
  struct sched_param lThreadScheduleParameters;
  int lError = 0;
  if ( (lError = pthread_attr_init( &lThreadAttribute ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError = pthread_attr_getschedpolicy( &lThreadAttribute, &lThreadPolicy )) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError =
        pthread_attr_getschedparam( &lThreadAttribute, &lThreadScheduleParameters)) != 0) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  pthread_attr_setschedpolicy( &lThreadAttribute, mPolicy );
  pthread_attr_getschedpolicy( &lThreadAttribute, &mPolicy );

  if ( mPriority < 0.0 )
    mPriority = 0.0;
  if ( mPriority > 1.0 )
    mPriority = 1.0;

  int lPriorMax = sched_get_priority_max( mPolicy );
  int lPriorMin = sched_get_priority_min( mPolicy );

  lThreadScheduleParameters.sched_priority = lPriorMin +
    (int)floor( ( lPriorMax - lPriorMin ) *  mPriority );

  pthread_attr_setschedparam( &lThreadAttribute, &lThreadScheduleParameters );

  if ( ( lError = pthread_attr_setscope(&lThreadAttribute, mScope) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError = pthread_create( &mThread,
                                 &lThreadAttribute,
                                 CKFWThread::threadFunction,
                                 this) ) == 0 ) {
    if ( mIsDetachable ) {
      pthread_detach( mThread );
    }

    pthread_attr_destroy( &lThreadAttribute );
  }
  else {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  return lReturnCode;
}

int CKFWThread::terminate( void )
{
  return 0;
}

void *CKFWThread::threadFunction( void * aThread )
{
  CKFWThread * lThread = (CKFWThread *)aThread;
  lThread->run( );

  return lThread;
}
// vim: set ts=2:
