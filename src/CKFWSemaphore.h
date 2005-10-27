/*
 * CKFWMutex.h - this file defines the simple semaphore that can
 *               be used in a large number of applications.
 *
 * $Id: CKFWSemaphore.h,v 1.9 2005/10/27 19:25:33 drbob Exp $
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
		CKFWSemaphore();
		CKFWSemaphore( unsigned int aMaxCnt );
		virtual ~CKFWSemaphore();

		int tryWait();
		void wait();
		void post();
		int count();

	private :
		sem_t mSemaphore;

		friend int CKFWSemaphoreTest( char * argv[] = 0, int argc = 0 );
};

#endif	// __CKFW_SEMAPHORE_H
