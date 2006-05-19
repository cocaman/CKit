/*
 * CKFWConditional.h - this file defines the conditional waiter.
 *
 * $Id: CKFWConditional.cpp,v 1.10 2006/05/19 19:34:07 drbob Exp $
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

int CKFWConditionalDefaultTest::test()
{
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//
//

CKFWConditional::CKFWConditional( CKFWMutex & aMutex ) : 
	mMutex(aMutex),
	mConditional()
{  
	int lResults = 0;   
	if ((lResults = pthread_cond_init(&mConditional, 0)) != 0) {
		throw CKErrNoException(__FILE__, __LINE__, lResults);
	}
}

CKFWConditional::~CKFWConditional()
{
  pthread_cond_destroy(&mConditional);
}


int CKFWConditional::lockAndTest( ICKFWConditionalSpuriousTest & aTest,
								  int aTimeoutInMillis )
{
	int lResult = FWCOND_LOCK_SUCCESS;
	mMutex.lock();

	while(aTest.test()) {
		if (aTimeoutInMillis >= 0) {
			// get the current time of day in sec and nsec.
			timeval		lCurrentTimeval;
			gettimeofday(&lCurrentTimeval, NULL);
			// now populate when the timeout will occur based on the duration
			timespec	lTimeSpec; 
			lTimeSpec.tv_sec = lCurrentTimeval.tv_sec + aTimeoutInMillis/1000; 
			lTimeSpec.tv_nsec = ((aTimeoutInMillis % 1000)*1000
								 + lCurrentTimeval.tv_usec) * 1000;
			// if we crossed the second boundary correctly update the values
			if (lTimeSpec.tv_nsec > 1000000000) {
				lTimeSpec.tv_sec++;
				lTimeSpec.tv_nsec -= 1000000000;
			}

			// now wait just that long and no longer			
			int rc = pthread_cond_timedwait(&mConditional, &mMutex.mMutex, &lTimeSpec);
			if (rc == ETIMEDOUT) { 
				mMutex.unlock();
				lResult = FWCOND_LOCK_ERROR;
				break;
			}
		} else {
			pthread_cond_wait(&mConditional, &mMutex.mMutex);
		}
	}

	return lResult;
}

void CKFWConditional::lockAndWait()
{
	mMutex.lock();
	pthread_cond_wait(&mConditional, &mMutex.mMutex);
}

void CKFWConditional::wakeWaiter()
{
	int lError;
	if ((lError = pthread_cond_signal(&mConditional))) {
		throw CKErrNoException(__FILE__, __LINE__, lError);
	}  
}

void CKFWConditional::wakeWaiters()
{
	int lError;
	if ((lError = pthread_cond_broadcast(&mConditional))) {
		throw CKErrNoException(__FILE__, __LINE__, lError);
	}
}

void CKFWConditional::unlock()
{
	mMutex.unlock();
}
// vim: set ts=2 sw=2:
