/*
 * CKStopwatch.cpp - this file implements a class that can be used to time things
 *                   on a unix box in terms of real time, user and system time.
 *                   It's nice to have it all in a single class so you don't have
 *                   to mess with the calls in the body of your code.
 *
 *                   An interesting wrinkle to this class is it's ability to make
 *                   split times and keep things going. When you have split times
 *                   you need to pop off the top split time to get to the next one.
 *                   This means that the list of split times it a "scan once"
 *                   scheme, but that's not a real limitation for this guy.
 * 
 * $Id: CKStopwatch.cpp,v 1.8 2004/09/20 16:19:42 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <errno.h>
#ifdef __linux__
#include <stdio.h>
#endif

//	Third-Party Headers
#include <CKException.h>

//	Other Headers
#include "CKStopwatch.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * The timing routines are defined in a way that returns the number of
 * clock ticks that have elapsed. I need this factor to scale the data
 * to make sense to humans reading the output. :)
 */
#define CLOCK_SCALE			10000.0/(double)CLOCKS_PER_SEC

/*
 * The sleep() and usleep() methods on Solaris are iffy and can be a real
 * problem. So we have created these routines to take their place. The
 * first sleeps for a given number of seconds and the second for a given
 * number of milliseconds.
 */
void msleep( long secs )
{
	mmsleep( secs * 1000 );
}


void mmsleep( unsigned int millisecs )
{
	unsigned int	secs = millisecs / 1000;
	unsigned int	nanosecs = (millisecs - (secs * 1000))*1000000;
	struct timespec rqtp = { secs, nanosecs };
	struct timespec rmtp = { 0, 0 };
	while ((nanosleep(&rqtp, &rmtp) == -1) && (errno == EINTR)) {
		rqtp = rmtp;
	}
}


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that sets the timer up so
 * it's ready to use.
 */
CKStopwatch::CKStopwatch() :
	mStartTime(),
	mRealStartTime(),
	mTimes(),
	mRealTimes()
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKStopwatch::CKStopwatch( const CKStopwatch & anOther ) :
	mStartTime(),
	mRealStartTime(),
	mTimes(),
	mRealTimes()
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKStopwatch::~CKStopwatch()
{
	// the STL containers are all going to clear out their data themselves
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKStopwatch & CKStopwatch::operator=( const CKStopwatch & anOther )
{
	// copy over everything
	mStartTime = anOther.mStartTime;
	mRealStartTime = anOther.mRealStartTime;
	mTimes = anOther.mTimes;
	mRealTimes = anOther.mRealTimes;

	return *this;
}


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
double CKStopwatch::getRealTimeInSec() const
{
	double		retval = 0.0;

	if (!mRealTimes.empty()) {
		retval = ((double)(mRealTimes.front() - mRealStartTime))*CLOCK_SCALE;
	}

	return retval;
}


/*
 * This method gets the elapsed User time in seconds from the
 * start of the timer - or the last split time. This is nice
 * in that it's always relative to the last 'event', and it's
 * easy to use.
 */
double CKStopwatch::getUserTimeInSec() const
{
	double		retval = 0.0;

	if (!mTimes.empty()) {
		retval = ((double)(mTimes.front().tms_utime - mStartTime.tms_utime))*CLOCK_SCALE;
	}

	return retval;
}


/*
 * This method gets the elapsed System time in seconds from the
 * start of the timer - or the last split time. This is nice
 * in that it's always relative to the last 'event', and it's
 * easy to use.
 */
double CKStopwatch::getSysTimeInSec() const
{
	double		retval = 0.0;

	if (!mTimes.empty()) {
		retval = ((double)(mTimes.front().tms_stime - mStartTime.tms_stime))*CLOCK_SCALE;
	}

	return retval;
}


/*
 * This method gets the elapsed real time in milliseconds from the
 * start of the timer - or the last split time. This is nice
 * in that it's always relative to the last 'event', and it's
 * easy to use.
 */
double CKStopwatch::getRealTimeInMillisec() const
{
	return getRealTimeInSec() * 1000.0;
}


/*
 * This method gets the elapsed User time in milliseconds from the
 * start of the timer - or the last split time. This is nice
 * in that it's always relative to the last 'event', and it's
 * easy to use.
 */
double CKStopwatch::getUserTimeInMillisec() const
{
	return getUserTimeInSec() * 1000.0;
}


/*
 * This method gets the elapsed System time in milliseconds from the
 * start of the timer - or the last split time. This is nice
 * in that it's always relative to the last 'event', and it's
 * easy to use.
 */
double CKStopwatch::getSysTimeInMillisec() const
{
	return getSysTimeInSec() * 1000.0;
}


/*
 * This get the complete time formatted very much like the standard
 * unix 'time' command: "0.00u 0.00s 0:00.00" where the first is the
 * user time, the second is the system time (both in seconds) and
 * the last is the real time in minutes and seconds.
 */
CKString CKStopwatch::getFormattedTime() const
{
	// make a buffer in the C-style as it's easiest to control
	char buff[128];
	// ...now format the output
	double	real = getRealTimeInSec();
	int		mins = (int)(real/60.0);
	real -= mins * 60.0;
	snprintf(buff, 128, "%0.02fu %0.02fs %0d:%05.2f",
			getUserTimeInSec(),
			getSysTimeInSec(),
			mins, real);
	// finally, return the string as a C++ string
	return CKString(buff);
}


/*
 * This method is used to remove the top time on the timer - if,
 * for instance, there are multiple splits and you want to get to
 * the next split, then you call this method to drop the top split
 * and get to the next one.
 */
void CKStopwatch::popOffTime()
{
	// see if the sizes are different and that's trouble
	if (mTimes.size() != mRealTimes.size()) {
		std::ostringstream	msg;
		msg << "Timer::popOffTime() - the sizes of the internal data structures "
			"are different and that means that there's a serious internal "
			"consistency problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// as long as they aren't empty, remove the top one of each
	if (!mTimes.empty()) {
		// save the front times as the new 'reference' times
		mStartTime = mTimes.front();
		mRealStartTime = mRealTimes.front();
		// ...now go ahead and pop them off
		mTimes.pop_front();
		mRealTimes.pop_front();
	}
}


/*
 * This clears out the timer and resets everything ready for another
 * run.
 */
void CKStopwatch::clear()
{
	mTimes.clear();
	mRealTimes.clear();
}


/********************************************************
 *
 *                Timing Methods
 *
 ********************************************************/
/*
 * This clears out the timer and starts the clock, as it were.
 */
void CKStopwatch::start()
{
	// first, clear out the watch
	clear();

	// now, get the reference time right now
	mRealStartTime = times(&mStartTime);
}


/*
 * This puts a final time on the timer and yuo're ready to read
 * them off.
 */
void CKStopwatch::stop()
{
	// interestingly, this is no different than a split time :)
	split();
}


/*
 * This puts a time on the timer 'stack' and allows you to keep
 * timing events.
 */
void CKStopwatch::split()
{
	// get the real time and time structure right now
	clock_t			realTime;
	struct tms		timeBlock;
	realTime = times(&timeBlock);

	// then put them on their lists as they should
	mTimes.push_back(timeBlock);
	mRealTimes.push_back(realTime);

	// make sure the sizes are the same
	if (mTimes.size() != mRealTimes.size()) {
		std::ostringstream	msg;
		msg << "Timer::split() - the sizes of the internal data structures "
			"are different and that means that there's a serious internal "
			"consistency problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKStopwatches are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKStopwatch::operator==( const CKStopwatch & anOther ) const
{
	bool		equal = true;

	// first, check the easy scalar values to check
	if (equal) {
		if (!areEqual(mStartTime, anOther.mStartTime) ||
			(mTimes.size() != anOther.mTimes.size()) ||
			(mRealStartTime != anOther.mRealStartTime) ||
			(mRealTimes != anOther.mRealTimes)) {
			equal = false;
		}
		equal = false;
	}

	// go one by one on the times that each has checking them
	if (equal) {
		std::list<struct tms>::const_iterator		i;
		std::list<struct tms>::const_iterator		j;
		for ( i = mTimes.begin(), j = anOther.mTimes.begin();
			  !equal && (i != mTimes.end()) && (j != anOther.mTimes.end());
			  ++i, ++j ) {
			equal = areEqual((*i), (*j));
		}
	}

	return equal;
}


/*
 * This method checks to see if the two CKStopwatches are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKStopwatch::operator!=( const CKStopwatch & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice 
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKStopwatch::toString() const
{
	CKString	retval = "<this is not yet implemented>";

	return retval;
}


/*
 * This method is useful because C++ can;t compare structs and we
 * need to be able to compare lots of them for this guy's 
 * operator==() method.
 */
bool CKStopwatch::areEqual( const struct tms & aTime, const struct tms & anOther )
{
	bool	equal = true;

	if ((aTime.tms_utime != anOther.tms_utime) ||
		(aTime.tms_stime != anOther.tms_stime) ||
		(aTime.tms_cutime != anOther.tms_cutime) ||
		(aTime.tms_cstime != anOther.tms_cstime)) {
		equal = false;
	}

	return equal;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this timer. It basically is just the value of toString() which
 * will indicate the times on the watch.
 */
std::ostream & operator<<( std::ostream & aStream, const CKStopwatch & aTimer )
{
	aStream << aTimer.toString();

	return aStream;
}

