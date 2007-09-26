/*
 * CKFWRWMutex.cpp - this file implements the simple read/write mutex that can
 *                   be used in a large number of applications.
 *
 * $Id: CKFWRWMutex.cpp,v 1.3 2007/09/26 20:14:22 drbob Exp $
 */

//	System Headers
#include <sys/errno.h>
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKFWRWMutex.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the default constructor that does all the work necessary
 * to get this guy up to the point that he's ready to be used.
 */
CKFWRWMutex::CKFWRWMutex() :
	mLockingThread((pthread_t)-1)
{
	int lError = pthread_rwlock_init( &mMutex, 0 );
	if ( lError != 0 ) {
		std::ostringstream		msg;
		msg << "CKFWRWMutex::CKFWRWMutex() - while trying to initialize the read/write "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKFWRWMutex::~CKFWRWMutex()
{
	pthread_rwlock_unlock( &mMutex );
	pthread_rwlock_destroy( &mMutex );
}


/**
 * Attempts to lock the mutex with a read lock. If the mutex is already
 * locked with a write lock, this thread will block until the mutex is
 * available.  Throws a CKErrNoException if there is some problem.
 */
void CKFWRWMutex::readLock()
{
	int lError = pthread_rwlock_rdlock( &mMutex );
	if ( lError != 0 ) {
		std::ostringstream		msg;
		msg << "CKFWRWMutex::readLock() - while trying to place a read lock on the "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/**
 * Attempts to lock the mutex with a read lock. Returns true if successful,
 * false if a write lock is already there. Throws a CKErrNoException if
 * there is some other problem. Does Not Block.
 */
bool CKFWRWMutex::tryReadLock()
{
	int lError = pthread_rwlock_tryrdlock( &mMutex );
	if ( lError != 0 ) {
		if ( lError == EBUSY ) {
			return false;
		}
		std::ostringstream		msg;
		msg << "CKFWRWMutex::tryReadLock() - while trying to place a read lock on the "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return true;
}


/**
 * Attempts to lock the mutex with a write lock. If the mutex is already
 * locked with a read or write lock, this thread will block until the mutex
 * is available.  Throws a CKErrNoException if there is some problem.
 */
void CKFWRWMutex::writeLock()
{
	int lError = pthread_rwlock_wrlock( &mMutex );
	if ( lError != 0 ) {
		std::ostringstream		msg;
		msg << "CKFWRWMutex::writeLock() - while trying to place a write lock on the "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	mLockingThread = pthread_self( );
}


/**
 * Attempts to lock the mutex with a write lock. Returns true if successful,
 * false if a read or write lock is already there. Throws a CKErrNoException
 * if there is some other problem. Does Not Block.
 */
bool CKFWRWMutex::tryWriteLock()
{
	int lError = pthread_rwlock_trywrlock( &mMutex );
	if ( lError != 0 ) {
		if ( lError == EBUSY ) {
			return false;
		}
		std::ostringstream		msg;
		msg << "CKFWRWMutex::tryWriteLock() - while trying to place a write lock on the "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	mLockingThread = pthread_self( );
	return true;
}


/**
 * Attempts to unlock this mutex.  Throws a CKErrNoException if there is
 * some problem. See man pthread_mutex_unlock
 */
void CKFWRWMutex::unlock()
{
	int lError = pthread_rwlock_unlock( &mMutex );
	if ( lError != 0 ) {
		std::ostringstream		msg;
		msg << "CKFWRWMutex::unlock() - while trying to remove all locks on the "
			   "mutex an error occured: (" << lError << ") " << strerror(lError);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	if ( pthread_self( ) == mLockingThread ) {
		mLockingThread = ( pthread_t )-1;
	}
}
// vim: set ts=2 sw=2:
