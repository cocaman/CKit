/*
 * CKFWConditional.h - this file defines the conditional
 *
 * $Id: CKFWConditional.h,v 1.6 2004/09/11 21:07:44 drbob Exp $
 */
#ifndef __CKFW_CONDITIONAL_H
#define __CKFW_CONDITIONAL_H

//	System Headers
#include <pthread.h>

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"

//	Forward Declarations

//	Public Constants
/*
 * The conditional's lockAndTest() can either succeed or fail based on the
 * timeout that's supplied to it. Of course, if that timeout is -1, then it
 * will wait until it's available, but with a timeout it's possible that
 * the lock will not be available in the time allowed. These therefore, are
 * the return codes for that method as the user needs to be able to determine
 * what happened.
 */
#define FWCOND_LOCK_ERROR				0
#define FWCOND_LOCK_SUCCESS			1

//	Public Datatypes

//	Public Data Constants


////////////////////////////////////////////////////////////////////////////////
//
//
//

class ICKFWConditionalSpuriousTest
{
public :
  ICKFWConditionalSpuriousTest();
  virtual ~ICKFWConditionalSpuriousTest();
  virtual int test( ) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
//
//

class CKFWConditionalDefaultTest :
  public ICKFWConditionalSpuriousTest
{
public :
  CKFWConditionalDefaultTest();
  virtual ~CKFWConditionalDefaultTest();
  virtual int test( );
};

////////////////////////////////////////////////////////////////////////////////
//
// CKFWConditional
//
//  This class throw CKErrNoException.  Even though this may not be exactly
//  correct it works since CKErrNoException can be instantiated with a value and
//  that strerror work with the pthread error codes.
//

class CKFWConditional
{
public :

  CKFWConditional( CKFWMutex & aMutex );
  virtual ~CKFWConditional( );

  inline void waitForLock()
  {
  	CKFWConditionalDefaultTest arg;
    lockAndTest( arg );
  }

  inline void waitForLock( ICKFWConditionalSpuriousTest & aTest )
  {
    lockAndTest( aTest );
  }

  inline void lockAndTest()
  {
  	CKFWConditionalDefaultTest arg;
    lockAndTest( arg );
  }

  int lockAndTest( ICKFWConditionalSpuriousTest & aTest,
	                 int aTimeoutInMillis = -1 );
  void lockAndWait( );
  void wakeWaiter( );
  void wakeWaiters( );

	/**
	 * just unlocks the underlying mutex.  This is so that you only
	 * have to use the conditional to do the locking and unlocking.
	 * It may be confusing to use the conditional's lockAndTest
	 * but then use the mutex to unlock.
	 */
	void unlock();

private :
  CKFWMutex & mMutex;
  pthread_cond_t mConditional;

  friend int CKFWConditionalTest( char * argv[] = 0, int argc = 0 );

};

#endif	// __CKFW_CONDITIONAL_H
// vim: set ts=2:
