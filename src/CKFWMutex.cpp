/*
 * CKFWMutex.cpp - this file implements the simple mutex that can be used
 *                 in a large number of applications.
 *
 * $Id: CKFWMutex.cpp,v 1.5 2004/09/11 21:07:44 drbob Exp $
 */

//	System Headers
#include <sys/errno.h>

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


CKFWMutex::CKFWMutex() :
	mLockingThread((pthread_t)-1)
{
  int lError = pthread_mutex_init(&mMutex,0);
  if ( lError != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }
}

CKFWMutex::~CKFWMutex()
{
  pthread_mutex_unlock( &mMutex );
  pthread_mutex_destroy( &mMutex );
}

/**
 * Attempts to lock the mutex.  Returns true if successful, false if the
 * lock is already busy. Throws a CKErrNoException if there is some other
 * problem. Does Not Block.
 */
bool CKFWMutex::tryLock()
{
  int lError = pthread_mutex_trylock( &mMutex );
  if ( lError != 0 ) {
    if ( lError == EBUSY ) {
      return false;
    }
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  mLockingThread = pthread_self( );

  return true;
}

/**
 * Attempts to lock the mutex.  If the mutex is already locked, this thread
 * will block until the mutex is available.  Throws a CKErrNoException if
 * there is some problem
 */
void CKFWMutex::lock()
{
  int lError = pthread_mutex_lock( &mMutex );
  if ( lError != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  mLockingThread = pthread_self();
}

void CKFWMutex::unlock()
{
  int lError = pthread_mutex_unlock( &mMutex );
  if ( lError != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  mLockingThread = ( pthread_t )-1;
}
// vim: set ts=2:
