/*
 * CKFWRWMutex.h - this file defines the simple read/write mutex that can be
 *                 used in a large number of applications.
 *
 * $Id: CKFWRWMutex.h,v 1.1 2004/12/01 18:28:20 drbob Exp $
 */
#ifndef __CKFWRW_MUTEX_H
#define __CKFWRW_MUTEX_H

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
class CKFWRWMutex
{
	public:
		/*
		 * This is the default constructor that does all the work necessary
		 * to get this guy up to the point that he's ready to be used.
		 */
		CKFWRWMutex();

		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKFWRWMutex();

		/**
		 * Attempts to lock the mutex with a read lock. If the mutex is already
		 * locked with a write lock, this thread will block until the mutex is
		 * available.  Throws a CKErrNoException if there is some problem.
		 */
		void readLock();
		/**
		 * Attempts to lock the mutex with a read lock. Returns true if successful,
		 * false if a write lock is already there. Throws a CKErrNoException if
		 * there is some other problem. Does Not Block.
		 */
		bool tryReadLock();

		/**
		 * Attempts to lock the mutex with a write lock. If the mutex is already
		 * locked with a read or write lock, this thread will block until the mutex
		 * is available.  Throws a CKErrNoException if there is some problem.
		 */
		void writeLock();
		/**
		 * Attempts to lock the mutex with a write lock. Returns true if successful,
		 * false if a read or write lock is already there. Throws a CKErrNoException
		 * if there is some other problem. Does Not Block.
		 */
		bool tryWriteLock();
  
		/**
		 * Attempts to unlock this mutex.  Throws a CKErrNoException if there is
		 * some problem. See man pthread_mutex_unlock
		 */
		void unlock();

	private:
		/*
		 * These are the OS-level lock and thread so that we can do this thin
		 * wrapper pretty easily.
		 */
		pthread_rwlock_t				mMutex;
		pthread_t								mLockingThread;
};
#endif //__CKFWRWMUTEX_H
// vim: set ts=2 sw=2:
