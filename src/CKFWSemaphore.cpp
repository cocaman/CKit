/*
 * CKFWMutex.h - this file defines the simple semaphore that can
 *               be used in a large number of applications.
 *
 * $Id: CKFWSemaphore.cpp,v 1.9 2005/10/27 19:25:33 drbob Exp $
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


CKFWSemaphore::CKFWSemaphore() {
	int lError = 0;
	if ( (lError = sem_init( &mSemaphore, 0, 0 ) ) == -1 ) {
		throw CKErrNoException( __FILE__, __LINE__, lError );
	}
}

CKFWSemaphore::CKFWSemaphore( unsigned int aMaxCnt ) {
	int lError = 0;
	if ( (lError = sem_init( &mSemaphore, 0, aMaxCnt ) ) == -1 ) {
		throw CKErrNoException( __FILE__, __LINE__, lError );
	}
}

CKFWSemaphore::~CKFWSemaphore() {
	sem_destroy( &mSemaphore );
}

int CKFWSemaphore::tryWait() {
	int lError;
	if ( (lError =  sem_trywait( &mSemaphore ) ) == -1 ) {
		if ( lError == EAGAIN ) {
			return 0;
		}
		throw CKErrNoException( __FILE__, __LINE__, lError );
	}
	return 1;
}

void CKFWSemaphore::wait() {
	if ( sem_wait( &mSemaphore ) == -1 ) {
		throw CKErrNoException( __FILE__, __LINE__ );
	}
}

void CKFWSemaphore::post() {
	if ( sem_post( &mSemaphore ) ) {
		throw CKErrNoException( __FILE__, __LINE__ );
	}
}

int CKFWSemaphore::count() {
	int		cnt;
	if ( sem_getvalue( &mSemaphore, &cnt ) ) {
		throw CKErrNoException( __FILE__, __LINE__ );
	}
	return cnt;
}
