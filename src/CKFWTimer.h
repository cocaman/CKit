/*
 * CKFWTimer.h - this file defines the simple timer that can
 *               be used in a large number of applications.
 *
 * $Id: CKFWTimer.h,v 1.5 2004/09/11 21:07:45 drbob Exp $
 */
#ifndef __CKFW_TIMER_H
#define __CKFW_TIMER_H

//	System Headers
#include <time.h>

//	Third-Party Headers

//	Other Headers
#include "CKErrNoException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFWTimer {
public :
  CKFWTimer( int aSetToCurrentTimeFlag = 1 );
  CKFWTimer( const CKFWTimer & anOther );
  CKFWTimer( int aMonth,
                      int aDay,
                      int aYear,
                      int aHour,
                      int aMinute,
                      int aSecond );

  virtual ~CKFWTimer( void );

  const CKFWTimer & operator=( const CKFWTimer & anOther );

  int operator==( const CKFWTimer & anOther );

  int operator<( const CKFWTimer & anOther );

  int operator-( const CKFWTimer & anOther );

private :
  time_t mTime;

  friend int CKFWTimerTest( char * argv[] = 0, int argc = 0 );

};

#endif	// __CKFW_TIMER_H
// vim: set ts=2:
