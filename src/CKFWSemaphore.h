/*
 * CKFWMutex.h - this file defines the simple semaphore that can
 *               be used in a large number of applications.
 *
 * $Id: CKFWSemaphore.h,v 1.5 2004/09/11 02:15:19 drbob Exp $
 */
#ifndef __CKFW_SEMAPHORE_H
#define __CKFW_SEMAPHORE_H

//	System Headers
#include <semaphore.h>

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


///////////////////////////////////////////////////////////////////////////////
//
// CKFWSemaphore
//
//  This class throw CKErrNoException.  Even though this may not be exactly
//  correct it works since CKErrNoException can be instantiated with a value
//  and that strerror work with the pthread error codes.
//

class CKFWSemaphore {
public :
  CKFWSemaphore( void );
  CKFWSemaphore( unsigned int aMaxCnt );
  virtual ~CKFWSemaphore( void );

  int tryWait( void );
  void wait( void );
  void post( void );

private :
  sem_t mSemaphore;

  friend int
  CKFWSemaphoreTest( char * argv[] = 0, int argc = 0 );

};

#endif	// __CKFW_SEMAPHORE_H
// vim: set ts=2:
