/*
 * CKFWMutex.cpp - this file implements the simple mutex that can
 *                 be used in a large number of applications.
 *
 * $Id: CKFWTimer.cpp,v 1.9 2010/08/25 12:00:26 drbob Exp $
 */

//	System Headers

//	Third-Party Headers

//	Other Headers
#include "CKFWTimer.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


CKFWTimer::CKFWTimer( int aSetToCurrentTimeFlag ) :
	mTime( 0 )
{
	if ( aSetToCurrentTimeFlag ) {
		if ( time( &mTime ) == -1 ) {
			throw CKErrNoException(  __FILE__, __LINE__ ) ;
		}
	}
}


CKFWTimer::CKFWTimer( const CKFWTimer & anOther ) :
	mTime( anOther.mTime )
{
}


CKFWTimer::CKFWTimer( int aMonth,
					  int aDay,
					  int aYear,
					  int aHour,
					  int aMinute,
					  int aSecond ) :
	mTime( 0 )
{
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

	mTime = mktime( &lTime );

	if ( mTime < 0 ) {
		throw CKErrNoException( __FILE__, __LINE__ );
	}
}


CKFWTimer::~CKFWTimer( void )
{
  mTime = 0;
}


const CKFWTimer & CKFWTimer::operator=( const CKFWTimer & anOther )
{
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		mTime = anOther.mTime;
	}
	return *this;
}


int CKFWTimer::operator==( const CKFWTimer & anOther )
{
	return (mTime == anOther.mTime);
}

int CKFWTimer::operator<( const CKFWTimer & anOther )
{
	return (mTime < anOther.mTime);
}

int CKFWTimer::operator-( const CKFWTimer & anOther )
{
	return (mTime - anOther.mTime);
}

// vim: set ts=2 sw=2:
