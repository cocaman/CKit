/*
 * CKFWMutex.h - this file defines the simple mutex that can be used in a
 *               large number of applications.
 *
 * $Id: CKFWMutex.h,v 1.5 2004/09/11 21:07:44 drbob Exp $
 */
#ifndef __CKFW_MUTEX_H
#define __CKFW_MUTEX_H

//	System Headers
#include <pthread.h>

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFWMutex
{
public :
  CKFWMutex( );
  virtual ~CKFWMutex( );

  /**
   * Attempts to lock the mutex.  Returns true if successful, false if the
   * lock is already busy. Throws a CKErrNoException if there is some other
   * problem. Does Not Block.
   */
  bool tryLock( );

  /**
   * Attempts to lock the mutex.  If the mutex is already locked, this thread
   * will block until the mutex is available.  Throws a CKErrNoException if
   * there is some problem
   */
  void lock( );

  /**
   * Attempts to unlock this mutex.  Throws a CKErrNoException if there is
   * some problem. See man pthread_mutex_unlock
   */
  void unlock( ) ;

  // void clear( );

private :
  friend class CKFWConditional;

  pthread_mutex_t mMutex;

  pthread_t mLockingThread;

  friend int CKFWMutexTest( char * argv[] = 0, int argc = 0 );
};

#endif	// __CKFW_MUTEX_H
// vim: set ts=2:
