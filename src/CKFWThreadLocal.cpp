/*
 * CKFWThread.cpp
 *
 * $Id: CKFWThreadLocal.cpp,v 1.5 2004/09/11 21:07:45 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWThreadLocal.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants

CKFWThreadLocal::CKFWThreadLocal( ) :
  mKey( )
{
  pthread_key_create( &mKey, NULL );
}

CKFWThreadLocal::CKFWThreadLocal( DataDestructor aDestructor ) :
  mKey( )
{
  pthread_key_create( &mKey, aDestructor );
}

CKFWThreadLocal::~CKFWThreadLocal( )
{
  pthread_key_delete( mKey );
}

void CKFWThreadLocal::set( void *aData )
{
  pthread_setspecific( mKey, aData );
}

void *CKFWThreadLocal::get( )
{
  return pthread_getspecific( mKey );
}
