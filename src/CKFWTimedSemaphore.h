/*
 * CKFWTimedSemaphore.h - this file defines a semaphore that will wait for
 *                        a certain period of time before giving up and
 *                        returning to the caller.
 *
 * $Id: CKFWTimedSemaphore.h,v 1.1 2003/11/24 19:01:02 drbob Exp $
 */
#ifndef __CKFW_TIMEDSEMAPHORE_H
#define __CKFW_TIMEDSEMAPHORE_H

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKFWConditional.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants
/*
 * These are the return codes that the wait methods will return. Since
 * there's a possibility that the timout will occur, we need to allow
 * the user to be able to see if such a condition occurred.
 */
#define FWTS_WAIT_SUCCESS		1
#define FWTS_WAIT_ERROR			0

/**
 * A timed semaphore is pretty much the same as the regular semaphore,
 * except the user can specify a timeout when waiting.  Semaphores
 * are essentially atomic integers.  Wait decrements the integer
 * and post increments it.  If the wait would decrement the count
 * below zero then the call blocks the calling thread until another
 * thread increments ( post ) the counter.
 */
class CKFWTimedSemaphore
{
public:
	/**
	 * Constructs a new timed semaphore with the specified initial value
	 * ( defaults to zero ).
	 */	
	CKFWTimedSemaphore( int anInitialValue = 0 );
	
	/**
	 * Performs the necessary cleanup
	 */
	virtual ~CKFWTimedSemaphore();

	/**
	 * Attempts a wait on the semaphore.  If the wait would block,
	 * this returns FWTS_WAIT_ERROR immediately.  Otherwise, it decrements the value
	 * and returns FWTS_WAIT_SUCCESS. ALWAYS returns immediately.
	 */
	int tryWait();

	/**
	 * Attempts to wait on the semaphore for only the specified timeout period
	 * ( timeout is in milliseconds ). Returns FWTS_WAIT_SUCCESS if it successfully
	 * performed the decrement, otherwise, returns FWTS_WAIT_ERROR.
	 */
	int wait( int aTimeoutInMillis = -1 );

	/**
	 * Increments the counter.  This call never blocks.
	 */
	void post();

private:
	int mCount;
	CKFWMutex mMutex;
	CKFWConditional mConditional;
};

#endif // __CKFW_TIMEDSEMAPHORE_H
