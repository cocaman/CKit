/*
 * CKFWMutex.h - this file defines the simple semaphore that can
 *               be used in a large number of applications.
 *
 * $Id: CKFWSemaphore.cpp,v 1.1 2003/11/21 18:08:06 drbob Exp $
 */

//	System Headers
#include <sys/errno.h>

//	Third-Party Headers

//	Other Headers
#include "CKFWSemaphore.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


CKFWSemaphore::CKFWSemaphore( void ) {
  int lError = 0;
  if ( (lError = sem_init( &mSemaphore, 0, 0 ) ) == -1 )
    throw CKErrNoException( __FILE__, __LINE__, lError );
  return ;
}

CKFWSemaphore::~CKFWSemaphore( void ) {
  sem_destroy( &mSemaphore );

  return ;
}

int CKFWSemaphore::tryWait( void ) {
  int lError;
  if ( (lError =  sem_trywait( &mSemaphore ) ) == -1 ) {
   if ( lError == EAGAIN ) {
      return 0;
    }
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }
  return 1;
}

void CKFWSemaphore::wait( void ) {
  if ( sem_wait( &mSemaphore ) == -1 ) {
    throw CKErrNoException( __FILE__, __LINE__ );
  }
  return ;
}

void CKFWSemaphore::post( void ) {
  if ( sem_post( &mSemaphore ) ) {
    throw CKErrNoException( __FILE__, __LINE__ );
  }
  return ;
}
// vim: set ts=2:
