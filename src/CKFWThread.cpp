/*
 * CKFWMutex.cpp - this file implements the simple mutex that can
 *                 be used in a large number of applications.
 *
 * $Id: CKFWThread.cpp,v 1.1 2003/11/21 18:08:06 drbob Exp $
 */

//	System Headers
#include <pthread.h>
#include <math.h>
#include <iostream>

//	Third-Party Headers

//	Other Headers
#include "CKFWThread.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
const int CKFWThread::cDefaultPolicy = SCHED_OTHER;
const double CKFWThread::cDefaultPriority =  0.5;
const int CKFWThread::cDefaultScope = PTHREAD_SCOPE_SYSTEM;

const int CKFWThread::cSuccess = 0;
const int CKFWThread::cDone = -1;
const int CKFWThread::cExceptionCaught = 1;
const int CKFWThread::cUnexpectedException = 2;


CKFWThread::CKFWThread( int aPolicy,
                        double aPriority,
                        int aScope,
                        int aIsDetachable )
    : mPolicy( aPolicy ),
      mPriority( aPriority ),
      mScope( aScope ),
      mIsDetachable( aIsDetachable )
{
  return;
}

CKFWThread::~CKFWThread( )
{
  return;
}

void CKFWThread::run( )
{
  try {
    if ( initialize( ) == cSuccess ) {
      while( process( ) == cSuccess );
    }
  } catch ( CKException & lException ) {
    std::cerr << "CKFWThread::run() - while running the thread a CKException "
    	"was thrown: " << lException.getMessage() << std::endl;
  } catch( ... ) {
    std::cerr << "CKFWThread::run() - while running the thread an unknown "
    	"exception was thrown." << std::endl;
  }

  try {
    terminate( );
  } catch ( CKException & lException ) {
    std::cerr << "CKFWThread::run() - while terminating the thread a "
    	"CKException was thrown: " << lException.getMessage() << std::endl;
  } catch( ... ) {
    std::cerr << "CKFWThread::run() - while terminating the thread an unknown "
    	"exception was thrown." << std::endl;
  }
}

int CKFWThread::process( )
{
  return 0;
}
int CKFWThread::initialize( )
{
  return 0;
}

void CKFWThread::join( )
{
  int lResultCode = 0;
  if ( ( lResultCode = pthread_join( mThread, NULL ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lResultCode );
  }
}

int CKFWThread::start( )
{
  int lReturnCode = 0;
  pthread_attr_t lThreadAttribute;
  int lThreadPolicy;
  struct sched_param lThreadScheduleParameters;
  int lError = 0;
  if ( (lError = pthread_attr_init( &lThreadAttribute ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError = pthread_attr_getschedpolicy( &lThreadAttribute, &lThreadPolicy )) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError =
        pthread_attr_getschedparam( &lThreadAttribute, &lThreadScheduleParameters)) != 0) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  pthread_attr_setschedpolicy( &lThreadAttribute, mPolicy );
  pthread_attr_getschedpolicy( &lThreadAttribute, &mPolicy );

  if ( mPriority < 0.0 )
    mPriority = 0.0;
  if ( mPriority > 1.0 )
    mPriority = 1.0;

  int lPriorMax = sched_get_priority_max( mPolicy );
  int lPriorMin = sched_get_priority_min( mPolicy );

  lThreadScheduleParameters.sched_priority = lPriorMin +
    (int)floor( ( lPriorMax - lPriorMin ) *  mPriority );

  pthread_attr_setschedparam( &lThreadAttribute, &lThreadScheduleParameters );

  if ( ( lError = pthread_attr_setscope(&lThreadAttribute, mScope) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  if ( (lError = pthread_create( &mThread,
                                 &lThreadAttribute,
                                 CKFWThread::threadFunction,
                                 this) ) == 0 ) {
    if ( mIsDetachable ) {
      pthread_detach( mThread );
    }

    pthread_attr_destroy( &lThreadAttribute );
  }
  else {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }

  return lReturnCode;
}

int CKFWThread::terminate( void )
{
  return 0;
}

void *CKFWThread::threadFunction( void * aThread )
{
  CKFWThread * lThread = (CKFWThread *)aThread;
  lThread->run( );

  return lThread;
}
// vim: set ts=2:
