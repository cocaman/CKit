/*
 * CKFWConditional.h - this file defines the conditional waiter.
 *
 * $Id: CKFWConditional.cpp,v 1.1 2003/11/21 18:08:06 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWConditional.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


////////////////////////////////////////////////////////////////////////////////
//
//
//

ICKFWConditionalSpuriousTest::ICKFWConditionalSpuriousTest()
{
}

ICKFWConditionalSpuriousTest::~ICKFWConditionalSpuriousTest()
{
}


////////////////////////////////////////////////////////////////////////////////
//
//
//

CKFWConditionalDefaultTest::CKFWConditionalDefaultTest() :
	ICKFWConditionalSpuriousTest()
{
}

CKFWConditionalDefaultTest::~CKFWConditionalDefaultTest()
{
}

int CKFWConditionalDefaultTest::test( )
{
  return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
//
//

CKFWConditional::CKFWConditional( CKFWMutex & aMutex ) : 
  mMutex( aMutex ),
  mConditional( )
{  
  int lResults = 0;   
  if ( ( lResults = pthread_cond_init( &mConditional, 0 ) ) != 0 ) {
    throw CKErrNoException( __FILE__, __LINE__, lResults );
  }
    
  return ;
}

CKFWConditional::~CKFWConditional( )
{
  pthread_cond_destroy( &mConditional );
}


void CKFWConditional::lockAndTest( ICKFWConditionalSpuriousTest & aTest )
{
  mMutex.lock( );

  while( aTest.test() ) {
    pthread_cond_wait( &mConditional, &mMutex.mMutex );
 }
}
void CKFWConditional::lockAndWait( )
{
  mMutex.lock( );
  pthread_cond_wait( &mConditional, &mMutex.mMutex );
}

void CKFWConditional::wakeWaiter( )
{
  int lError;
  if ( (lError = pthread_cond_signal( &mConditional )) ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }  
}

void CKFWConditional::wakeWaiters( )
{
  int lError;
  if ( (lError = pthread_cond_broadcast( &mConditional )) ) {
    throw CKErrNoException( __FILE__, __LINE__, lError );
  }
}
// vim: set ts=2:
