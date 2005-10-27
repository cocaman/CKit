/*
 * CKStackLocker.h - this file defines a class that is very useful for those
 *                   times when a mutex is needing to be locked, but any
 *                   exceptions that are being thrown in the scope covered
 *                   by the mutex will cause a popping of the runtime stack
 *                   and the lock not to be 'unlocked'.
 *
 *                   What you do with this class is to define a scope in the
 *                   code and create a normal CKFWMutex. Then, you can create
 *                   this object on the stack with the single argument to the
 *                   constructor being the pointer to this CKFWMutex. In the
 *                   constructor of this instance, the mutex will be locked
 *                   and in the destructor it will be unlocked. This means
 *                   no matter how the scope is exited - normally or by an
 *                   exception being thrown, the mutex will be unlocked.
 *
 * $Id: CKStackLocker.h,v 1.7 2005/10/27 19:25:33 drbob Exp $
 */
#ifndef __CKSTACKLOCKER_H
#define __CKSTACKLOCKER_H

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKFWRWMutex.h"
#include "CKFWSemaphore.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKStackLocker
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This form of the constructor takes a pointer to a CKFWMutex that
		 * needs to be non-NULL. It then proceeds to lock this mutex and
		 * return. That's it.
		 */
		CKStackLocker( CKFWMutex *aMutex );
		/*
		 * This form of the constructor takes a pointer to a CKFWRWMutex that
		 * needs to be non-NULL. It then proceeds to lock this mutex and
		 * return. The nature of the lock is dictated by the boolean with
		 * the default being to do a read lock. That's it.
		 */
		CKStackLocker( CKFWRWMutex *aRWMutex, bool aReadLock = true );
		/*
		 * This form of the constructor takes a semaphore and will immediately
		 * do a 'wait()' on it to make sure that we're one of the chosen few.
		 * Then, when this object goes out of scope, we'll do a 'post()' and
		 * everything will be back the way it should be.
		 */
		CKStackLocker( CKFWSemaphore *aSemaphore );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called. This guy will simply unlock the mutex and that's it.
		 */
		virtual ~CKStackLocker();

	private:
		/*
		 * This is the default constructor that can't really stand-alone as
		 * the purpose of the constructor is to lock this mutex, but without
		 * a mutex argument, it can't do this, and that really means we need
		 * to keep it private.
		 */
		CKStackLocker();

		/*
		 * This is the pointer to the actual mutex that will be passed in
		 * to the public constructor. We didn't create this guy so we're
		 * going to leave him alone.
		 */
		CKFWMutex		*mMutex;
		// ...and this is the pointer to the read/write mutex
		CKFWRWMutex		*mRWMutex;
		// ...and this is the pointer to the semaphore
		CKFWSemaphore	*mSemaphore;
};

#endif	// __CKSTACKLOCKER_H
