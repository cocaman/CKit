/*
 * CKFWConditional.h - this file defines the conditional waiter.
 *
 * $Id: CKFWConditional.cpp,v 1.5 2003/12/16 18:08:55 drbob Exp $
 */

//	System Headers
#ifdef __linux__
#include <sys/time.h>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKFWConditional.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


////////////////////////////////////////////////////////////////////////////////
//
//
//

ICKFWConditionalSpuriousTest::ICKFWConditionalSpuriousTest()
{
}

ICKFWConditionalSpuriousTest::~ICKFWConditionalSpuriousTest()
{
}


////////////////////////////////////////////////////////////////////////////////
//
//
//

CKFWConditionalDefaultTest::CKFWConditionalDefaultTest() :
	ICKFWConditionalSpuriousTest()
{
}

CKFWConditionalDefaultTest::~CKFWConditionalDefaultTest()
{
}

int CKFWConditionalDefaultTest::test( )
{
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//
//

CKFWConditional::CKFWConditional( CKFWMutex & aMutex ) : 
  mMutex( aMutex ),
  mConditional( )
{  
  int lResults = 0;   
  if ( ( lResults = pthread_cond_init( &mConditional, 0 ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lResults );
  }
    
  return ;
}

CKFWConditional::~CKFWConditional( )
{
  pthread_cond_destroy( &mConditional );
}


int CKFWConditional::lockAndTest( ICKFWConditionalSpuriousTest & aTest,
				int aTimeoutInMillis )
{
	int lResult = FWCOND_LOCK_SUCCESS;
  mMutex.lock( );

  while( aTest.test() ) {
		if ( aTimeoutInMillis >= 0 ) {
			timespec lTimeSpec; 
			timeval lCurrentTimeval;
			gettimeofday(&lCurrentTimeval, NULL);
			lTimeSpec.tv_sec = lCurrentTimeval.tv_sec + aTimeoutInMillis / 1000; 
			lTimeSpec.tv_nsec = lCurrentTimeval.tv_usec * 1000
				+ (aTimeoutInMillis % 1000) * 1000000;

			int rc = pthread_cond_timedwait(&mConditional, &mMutex.mMutex, &lTimeSpec);
			if (rc == ETIMEDOUT) { 
				mMutex.unlock();
				lResult = FWCOND_LOCK_ERROR;
				break;
			}
		} else {
			pthread_cond_wait( &mConditional, &mMutex.mMutex );
		}
	}

	return lResult;
}

void CKFWConditional::lockAndWait( )
{
  mMutex.lock( );
  pthread_cond_wait( &mConditional, &mMutex.mMutex );
}

void CKFWConditional::wakeWaiter( )
{
  int lError;
  if ( (lError = pthread_cond_signal( &mConditional )) ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }  
}

void CKFWConditional::wakeWaiters( )
{
  int lError;
  if ( (lError = pthread_cond_broadcast( &mConditional )) ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }
}

void CKFWConditional::unlock()
{
	mMutex.unlock();
}

// vim: set ts=2:
