/*
 * CKStopwatch.h - this file defines a class that can be used to time things
 *                 on a unix box in terms of real time, user and system time.
 *                 It's nice to have it all in a single class so you don't have
 *                 to mess with the calls in the body of your code.
 *
 *                 An interesting wrinkle to this class is it's ability to make
 *                 split times and keep things going. When you have split times
 *                 you need to pop off the top split time to get to the next one.
 *                 This means that the list of split times it a "scan once"
 *                 scheme, but that's not a real limitation for this guy.
 * 
 * $Id: CKStopwatch.h,v 1.7 2004/09/25 16:14:39 drbob Exp $
 */
#ifndef __CKSTOPWATCH_H
#define __CKSTOPWATCH_H

//	System Headers
#include <list>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sys/times.h>
#include <limits.h>

//	Third-Party Headers

//	Other Headers
#include "CKString.h"
#include "CKVector.h"

//	Forward Declarations
/*
 * The sleep() and usleep() methods on Solaris are iffy and can be a real
 * problem. So we have created these routines to take their place. The
 * first sleeps for a given number of seconds and the second for a given
 * number of milliseconds.
 */
extern void msleep( long secs );
extern void mmsleep( unsigned int millisecs );

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKStopwatch
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that sets the stopwatch up so
		 * it's ready to use.
		 */
		CKStopwatch();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKStopwatch( const CKStopwatch & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKStopwatch();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKStopwatch & operator=( const CKStopwatch & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method gets the elapsed real time in seconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getRealTimeInSec() const;
		/*
		 * This method gets the elapsed User time in seconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getUserTimeInSec() const;
		/*
		 * This method gets the elapsed System time in seconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getSysTimeInSec() const;
		/*
		 * This method gets the elapsed real time in milliseconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getRealTimeInMillisec() const;
		/*
		 * This method gets the elapsed User time in milliseconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getUserTimeInMillisec() const;
		/*
		 * This method gets the elapsed System time in milliseconds from the
		 * start of the timer - or the last split time. This is nice
		 * in that it's always relative to the last 'event', and it's
		 * easy to use.
		 */
		double getSysTimeInMillisec() const;

		/*
		 * This get the complete time formatted very much like the standard
		 * unix 'time' command: "0.00u 0.00s 0:00.00" where the first is the
		 * user time, the second is the system time (both in seconds) and
		 * the last is the real time in minutes and seconds.
		 */
		CKString getFormattedTime() const;

		/*
		 * This method is used to remove the top time on the timer - if,
		 * for instance, there are multiple splits and you want to get to
		 * the next split, then you call this method to drop the top split
		 * and get to the next one.
		 */
		void popOffTime();

		/*
		 * This clears out the timer and resets everything ready for another
		 * run.
		 */
		void clear();

		/********************************************************
		 *
		 *                Timing Methods
		 *
		 ********************************************************/
		/*
		 * This clears out the timer and starts the clock, as it were.
		 */
		void start();
		/*
		 * This puts a final time on the timer and yuo're ready to read
		 * them off.
		 */
		void stop();
		/*
		 * This puts a time on the timer 'stack' and allows you to keep
		 * timing events.
		 */
		void split();

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKStopwatchs are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKStopwatch & anOther ) const;
		/*
		 * This method checks to see if the two CKStopwatchs are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKStopwatch & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice 
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	protected:
		/*
		 * This method is useful because C++ can't compare structs and we
		 * need to be able to compare lots of them for this guy's 
		 * operator==() method.
		 */
		static bool areEqual( const struct tms & aTime, const struct tms & anOther );

	private:
		/*
		 * When the timer is started, this value is populated so that it
		 * becomes the reference for the top ending time on the list,
		 * below. As each element gets popped off the stack, it's placed
		 * here so as to become the next reference (or starting point) for
		 * the 'top' time on the stack.
		 */
		struct tms				mStartTime;
		clock_t					mRealStartTime;
		/*
		 * As the user asks for split times, this guy adds the structures
		 * to the end of itself and you can get pretty elaborate with this
		 * in terms of a lot of splits.
		 */
		CKVector<struct tms>	mTimes;
		CKVector<clock_t>		mRealTimes;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this timer. It basically is just the value of toString() which
 * will indicate the times on the watch.
 */
std::ostream & operator<<( std::ostream & aStream, const CKStopwatch & aTimer );

#endif	// __CKSTOPWATCH_H
