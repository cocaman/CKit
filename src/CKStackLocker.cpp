/*
 * CKStackLocker.cpp - this file implements a class that is very useful for
 *                     those times when a mutex is needing to be locked, but
 *                     any exceptions that are being thrown in the scope
 *                     covered by the mutex will cause a popping of the
 *                     runtime stack and the lock not to be 'unlocked'.
 *
 *                     What you do with this class is to define a scope in the
 *                     code and create a normal CKFWMutex. Then, you can create
 *                     this object on the stack with the single argument to the
 *                     constructor being the pointer to this CKFWMutex. In the
 *                     constructor of this instance, the mutex will be locked
 *                     and in the destructor it will be unlocked. This means
 *                     no matter how the scope is exited - normally or by an
 *                     exception being thrown, the mutex will be unlocked.
 *
 * $Id: CKStackLocker.cpp,v 1.5 2004/09/20 16:19:41 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"
#include "CKStackLocker.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


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
CKStackLocker::CKStackLocker( CKFWMutex *aMutex ) :
	mMutex(NULL)
{
	if (aMutex == NULL) {
		std::ostringstream	msg;
		msg << "CKStackLocker::CKStackLocker(CKFWMutex *) - the passed-in mutex "
			"is NULL and that means that there's nothing I can do. Please make "
			"sure that the argument is not NULL before calling this constructor.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		mMutex = aMutex;
		mMutex->lock();
	}
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called. This guy will simply unlock the mutex and that's it.
 */
CKStackLocker::~CKStackLocker()
{
	if (mMutex == NULL) {
		std::ostringstream	msg;
		msg << "CKStackLocker::~CKStackLocker() - the mutex is now NULL and it "
			"had to be non-NULL in the constructor. Please check on this data "
			"corruption problem as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		mMutex->unlock();
		mMutex = NULL;
	}
}


/*
 * This is the default constructor that can't really stand-alone as
 * the purpose of the constructor is to lock this mutex, but without
 * a mutex argument, it can't do this, and that really means we need
 * to keep it private.
 */
CKStackLocker::CKStackLocker() :
	mMutex(NULL)
{
}