/*
 * CKFWTime.h - this file defines the simple time class that can
 *              be used in a large number of applications.
 *
 * $Id: CKFWTime.h,v 1.5 2004/09/11 21:07:45 drbob Exp $
 */
#ifndef __CKFW_TIME_H
#define __CKFW_TIME_H

//	System Headers
#include <time.h>

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFWTime {
public :
  CKFWTime( int aSetToCurrentTimeFlag = 1 );
  CKFWTime( const CKFWTime & anOther );
  CKFWTime( int aMonth,
                     int aDay,
                     int aYear,
                     int aHour,
                     int aMinute,
                     int aSecond );

  virtual ~CKFWTime( void );

  const CKFWTime &
  operator=( const CKFWTime & anOther );

  int operator==( const CKFWTime & anOther );

  int operator<( const CKFWTime & anOther );

  int operator-( const CKFWTime & anOther );

private :
  time_t mTime;

  friend int CKFWTimeTest( char * argv[] = 0, int argc = 0 );

};

#endif
// vim: set ts=2:
