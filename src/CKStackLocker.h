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
 * $Id: CKStackLocker.h,v 1.2 2004/09/11 02:15:22 drbob Exp $
 */
#ifndef __CKSTACKLOCKER_H
#define __CKSTACKLOCKER_H

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"

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
		 * This is the only public form of the constructor and it takes
		 * the pointer to a CKFWMutex that needs to be non-NULL. It will
		 * then proceed to lock this mutex and return. That's it.
		 */
		CKStackLocker( CKFWMutex *aMutex );
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
};

#endif	// __CKSTACKLOCKER_H
