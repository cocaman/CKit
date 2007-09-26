/*
 * CKFWTimedSemaphore.cpp - this file implements a semaphore that will wait for
 *                          a certain period of time before giving up and
 *                          returning to the caller.
 *
 * $Id: CKFWTimedSemaphore.cpp,v 1.8 2007/09/26 19:33:45 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWTimedSemaphore.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


/**
 * Performs the test for the lock and test.  Basically, we want to wait
 * until the counter is greater than 0. And remember the test is a negative
 * test, meaning that returning *true* causes the thread to sleep, i.e.
 * the condition is NOT met.
 * That's counter-intuitive, but that's the way sludge built it ( damn him! )
 */
class FWTSTest :
	public ICKFWConditionalSpuriousTest
{
public:
	FWTSTest( int *aCountPtr ) :
		mCountPtr( aCountPtr )
	{
	}

	~FWTSTest()
	{
		mCountPtr = NULL;
	}

	virtual int test()
	{
		return *mCountPtr <= 0;
	}

private:
	int *mCountPtr;
};

CKFWTimedSemaphore::CKFWTimedSemaphore( int anInitialValue ) :
	mCount( anInitialValue ),
	mMutex(),
	mConditional( mMutex )
{
}

CKFWTimedSemaphore::~CKFWTimedSemaphore()
{
}

int CKFWTimedSemaphore::tryWait()
{
	int lResult = FWTS_WAIT_SUCCESS;
	if ( !mMutex.tryLock() ) {
		lResult = FWTS_WAIT_ERROR;
	} else {
		-- mCount;
		mMutex.unlock();
	}

	return lResult;
}

int CKFWTimedSemaphore::wait( int aTimeoutInMillis )
{
	FWTSTest	tst(&mCount);
	int lCondError = mConditional.lockAndTest( tst, aTimeoutInMillis );
	if ( lCondError == FWCOND_LOCK_SUCCESS ) {
		-- mCount;
		mConditional.unlock();
	}

	return ( lCondError == FWCOND_LOCK_ERROR ? FWTS_WAIT_ERROR : FWTS_WAIT_SUCCESS );
}

void CKFWTimedSemaphore::post()
{
	mMutex.lock();
	++ mCount;
	mConditional.wakeWaiter();
	mMutex.unlock();
}
