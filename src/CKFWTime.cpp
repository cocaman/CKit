/*
 * CKFWTime.cpp - this file implements the simple time class that can
 *                be used in a large number of applications.
 *
 * $Id: CKFWTime.cpp,v 1.7 2004/09/20 16:19:31 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWTime.h"
#include "CKErrNoException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


CKFWTime::CKFWTime( int aSetToCurrentTimeFlag )
  :  mTime( 0 ) {
  if ( aSetToCurrentTimeFlag ) {
    if ( time( &mTime ) == -1 ) {
      throw CKErrNoException( __FILE__, __LINE__ );
    }
  }

  return ;
}

CKFWTime::CKFWTime( const CKFWTime & anOther )
  :  mTime( anOther.mTime ) {

  return;
}

CKFWTime::CKFWTime( int aMonth,
                                      int aDay,
                                      int aYear,
                                      int aHour,
                                      int aMinute,
                                      int aSecond )
  : mTime( 0 ) {

  struct tm lTime;

  lTime.tm_sec  = aSecond;
  lTime.tm_min  = aMinute;
  lTime.tm_hour = aHour;
  lTime.tm_mday = aDay;
  lTime.tm_mon  = aMonth;
  lTime.tm_year = aYear;
  lTime.tm_wday = -1;
  lTime.tm_yday = -1;
  lTime.tm_isdst= -1;

  if ( (mTime = mktime( &lTime )) == -1 ) {
    throw CKErrNoException( __FILE__, __LINE__ );
  }

  return ;
}



CKFWTime::~CKFWTime( void ) {
  mTime = 0;

  return ;
}

const CKFWTime &
CKFWTime::operator=( const CKFWTime & anOther ) {
  mTime = anOther.mTime;

  return *this;
}

int
CKFWTime::operator==( const CKFWTime & anOther ) {

  return mTime == anOther.mTime;
}

int
CKFWTime::operator<( const CKFWTime & anOther ) {

  return mTime < anOther.mTime;
}

int
CKFWTime::operator-( const CKFWTime & anOther ) {
  return mTime - anOther.mTime;
}

// vim: set ts=2:
