/*
 * CKFWThread.h - this file defines the simple thread that can
 *                be used in a large number of applications.
 *
 * $Id: CKFWThread.h,v 1.1 2003/11/21 18:08:06 drbob Exp $
 */
#ifndef __CKFW_THREAD_H
#define __CKFW_THREAD_H

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
class CKFWThread
{
	public:
		/*
		 * Defaults
		 */
		static const int		cDefaultPolicy;
		static const double		cDefaultPriority;
		static const int		cDefaultScope;		// = PTHREAD_SCOPE_SYSTEM

		/*
		 * Standard return codes for initialize, process, and terminate
		 * methods.
		 */
		static const int cSuccess;
		static const int cDone;
		static const int cExceptionCaught;
		static const int cUnexpectedException;

		/*
		 * constructor
		 */
		CKFWThread( int aPolicy = cDefaultPolicy,
                    double aPriority = cDefaultPriority,
                    int aScope = cDefaultScope,  //  PTHREAD_SCOPE_PROCESS
                    int aIsDetachable = 1 );

		virtual ~CKFWThread( );

		/*
		 *
		 */
		virtual void run( );
		/*
		 *
		 */
		int start( );
		/*
		 *
		 */
		void join( );

	protected:
		/*
		 *
		 */
		virtual int initialize( );
		/*
		 *
		 */
		virtual int process( );
		/*
		 * The semantics of this method are such that, during/after this call the owner
		 * can do whatever it wants but nothing should be deleted before this call.
		 */
		virtual int terminate( );

	private :
		static void *threadFunction( void * );

		int			mPolicy;
		double		mPriority;
		int			mScope;
		pthread_t	mThread;
		int			mIsDetachable;

  friend int CKFWThreadTest( char * argv[] = 0, int argc = 0 );
};

#endif	// __CKFW_THREAD_H
// vim: set ts=2:
