/*
 * CKFWThreadLocal.h
 *
 * $Id: CKFWThreadLocal.h,v 1.3 2003/12/16 18:08:56 drbob Exp $
 */
#ifndef __CKFW_THREAD_LOCAL_H
#define __CKFW_THREAD_LOCAL_H

//	System Headers
#include <pthread.h>

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/**
 * CKFWThreadLocal (c) 2002 OConnor LLC
 * Author: mayesje
 *
 * Pretty simple wrapper around PThreads thread local storage.
 * Use the get method to get a generic handle to the thread specific
 * value, use the set method to set the thread specific value.
 * You can specify a *destructor* in the Constructor so that when a
 * thread goes away which has a current valid( read non-NULL ) thread specific
 * value the *destructor* will get call for that value so that you don't
 * leak.
 *
 **/
typedef void (*DataDestructor)( void * );
class CKFWThreadLocal
{
public:
  CKFWThreadLocal( );
  CKFWThreadLocal( DataDestructor aDestructor );
  virtual ~CKFWThreadLocal( );

  void set( void *aData );
  void *get( );

protected:
private:

  pthread_key_t mKey;
};

#endif	// __CKFW_THREAD_LOCAL_H
