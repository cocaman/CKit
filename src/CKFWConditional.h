/*
 * CKFWConditional.h - this file defines the conditional
 *
 * $Id: CKFWConditional.h,v 1.1 2003/11/21 18:08:06 drbob Exp $
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

  void lockAndTest( ICKFWConditionalSpuriousTest & aTest );
  void lockAndWait( );
  void wakeWaiter( );
  void wakeWaiters( );

private :
  CKFWMutex & mMutex;
  pthread_cond_t mConditional;

  friend int CKFWConditionalTest( char * argv[] = 0, int argc = 0 );

};

#endif	// __CKFW_CONDITIONAL_H
// vim: set ts=2:
