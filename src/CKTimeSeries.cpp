/*
 * CKTimeSeries.cpp - this file implements a class that can be used to
 *                    represent a time-series data stream. Basically, a
 *                    double is used to represent the date/time as
 *                    YYYYMMDD.HHMMSShh with the time resolution down to the
 *                    hundredth of a second, and a double is used to represent
 *                    the data value. This is a nice little class that is used
 *                    in the CKVariant as yet another form of data that that
 *                    class can represent.
 *
 * $Id: CKTimeSeries.cpp,v 1.16 2004/12/06 14:03:20 drbob Exp $
 */

//	System Headers
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <strings.h>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"
#include "CKTable.h"
#include "CKTimeSeries.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that creates a nice, empty
 * time series object ready for the user to dump data into as
 * needed.
 */
CKTimeSeries::CKTimeSeries() :
	mTimeseries(),
	mTimeseriesMutex()
{
}


/*
 * This form of the constructor takes a vector of dates (that
 * means no time values allowed) and another of values and puts
 * these two into the time series matching up the indeces of
 * the two vectors. This is useful when the historical data is
 * something that has only one value per day and time per day
 * is not important.
 */
CKTimeSeries::CKTimeSeries( const CKVector<long> & aDateSeries,
							const CKVector<double> & aValueSeries ) :
	mTimeseries(),
	mTimeseriesMutex()
{
	// first, see if the lengths are different
	int		cnt = aDateSeries.size();
	if (cnt != aValueSeries.size()) {
		std::ostringstream	msg;
		msg << "CKTimeSeries::CKTimeSeries(const CKVector<long>, "
			"const CKVector<double>) - the size of the timestamp vector "
			"is " << cnt << " while the size of the data vector is " <<
			aValueSeries.size() << ". This is a problem as they have to be the "
			"same size to make sense. Please check the data.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// next, rip through the vectors adding the numbers to the series
	if (cnt > 0) {
		mTimeseriesMutex.lock();
		for (int i = 0; i < cnt; i++) {
			mTimeseries[aDateSeries[i]] = aValueSeries[i];
		}
		mTimeseriesMutex.unlock();
	}
}


/*
 * This constructor is interesting in that it takes the data as
 * it comes from another CKTimeSeries's generateCodeFromValues()
 * method and parses it into a time series of values directly.
 * This is very useful for serializing the table's data from one
 * host to another across a socket, for instance.
 */
CKTimeSeries::CKTimeSeries( const CKString & aCode ) :
	mTimeseries(),
	mTimeseriesMutex()
{
	// first, make sure we have something to do
	if (aCode.empty()) {
		std::ostringstream	msg;
		msg << "CKTimeSeries::CKTimeSeries(const char *) - the provided "
			"argument is empty and that means that nothing can be done. Please "
			"make sure that the argument is not empty before calling this "
			"constructor.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// load in the values from the code
		takeValuesFromCode(aCode);
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKTimeSeries::CKTimeSeries( const CKTimeSeries & anOther ) :
	mTimeseries(),
	mTimeseriesMutex()
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKTimeSeries::~CKTimeSeries()
{
	// all the STL containers delete their data in the end
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTimeSeries & CKTimeSeries::operator=( const CKTimeSeries & anOther )
{
	// copy over the map as that's all that matters
	mTimeseries = anOther.mTimeseries;

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method puts the given value at the provided timestamp
 * in the time series. The timestamp is a double in the format
 * YYYYMMDD.hhmmssss where seconds are in the hundredths of a
 * second.
 */
void CKTimeSeries::put( double aDateTime, double aValue )
{
	// put the value in the map safely
	mTimeseriesMutex.lock();
	mTimeseries[aDateTime] = aValue;
	mTimeseriesMutex.unlock();
}


/*
 * This method takes the vector of values and date/times and
 * puts all the data into the timeseries by matching up the
 * indeces of the two vectors. Each timestamp value is a double
 * in the format YYYYMMDD.hhmmssss where seconds are in the
 * hundredths of a second.
 */
void CKTimeSeries::put( const CKVector<double> & aDateSeries,
						const CKVector<double> & aValueSeries )
{
	// first, see if the lengths are different
	int		cnt = aDateSeries.size();
	if (cnt != aValueSeries.size()) {
		std::ostringstream	msg;
		msg << "CKTimeSeries::put(const CKVector<double>, "
			"const CKVector<double>) - the size of the timestamp vector "
			"is " << cnt << " while the size of the data vector is " <<
			aValueSeries.size() << ". This is a problem as they have to be the "
			"same size to make sense. Please check the data.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// next, rip through the vectors adding the numbers to the series
	if (cnt > 0) {
		mTimeseriesMutex.lock();
		for (int i = 0; i < cnt; i++) {
			mTimeseries[aDateSeries[i]] = aValueSeries[i];
		}
		mTimeseriesMutex.unlock();
	}
}


/*
 * This method essentially merges the passed-in timeseries with
 * the existing one to create a 'super set'. This is nice because
 * we can then work on components of the time series and then
 * 'stitch' them back together with this method.
 */
void CKTimeSeries::put( CKTimeSeries & aSeries )
{
	if (aSeries.mTimeseries.size() > 0) {
		// add them all in a thread-safe manner
		mTimeseriesMutex.lock();
		// ...lock the source too
		aSeries.mTimeseriesMutex.lock();
		// copy over all the elements from teh source
		std::map<double, double>::iterator	i;
		for (i = aSeries.mTimeseries.begin(); i != aSeries.mTimeseries.end(); ++i) {
			mTimeseries[(*i).first] = (*i).second;
		}
		// unlock the source
		aSeries.mTimeseriesMutex.unlock();
		// ...and us too
		mTimeseriesMutex.unlock();
	}
}


/*
 * This method gets the value of the timeseries at the given
 * timestamp or NAN as defined in <math.h>. To test the value
 * use isnan() in <math.h>.
 */
double CKTimeSeries::get( double aDateTime )
{
	double		retval = NAN;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// see if it exists in the map
	if (!mTimeseries.empty()) {
		std::map<double, double>::iterator	i = mTimeseries.find(aDateTime);
		if (i != mTimeseries.end()) {
			retval = (*i).second;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * This method gets a series of values for the timestamp
 * series that is supplied. The format of each timestamp in
 * the series is YYYYMMDD.hhmmssss where the seconds are to
 * the hundredths of a second. If there is no data in the
 * current series for the timestamp in the input, a NAN will
 * be placed in the output so that there will always be the
 * same number of elements in the output as there are in the
 * input unless an error occurs. Use isnan() in <math.h> to
 * test these values.
 */
CKVector<double> CKTimeSeries::get( const CKVector<double> & aDateSeries )
{
	CKVector<double>		retval;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	/*
	 * For each date/time see if it exists in the map, if it
	 * doesn't, then fill in a NAN for that guy.
	 */
	unsigned int	cnt = aDateSeries.size();
	if (cnt > 0) {
		std::map<double, double>::iterator	i;
		for (unsigned int d = 0; d < cnt; d++) {
			if (!mTimeseries.empty()) {
				i = mTimeseries.find(aDateSeries[d]);
				if (i != mTimeseries.end()) {
					retval.addToEnd((double)(*i).second);
				} else {
					retval.addToEnd(NAN);
				}
			} else {
				retval.addToEnd(NAN);
			}
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * This method takes today's date and marches back in time the
 * provided number of days to arrive at the value to return.
 * This is nice in that 0 will get the latest value and 1 will
 * get the value yesterday, and n will get you the value n days
 * ago.
 */
double CKTimeSeries::getDaysBack( int aDayCnt )
{
	double		retval = NAN;

	// first, get today's date offset the requested amount
	double		date = addDays(getCurrentDate(), -1 * aDayCnt);
	retval = get(date);

	return retval;
}


/*
 * This method gets the complete series of dates for the current
 * timeseries but as just the day (YYYYMMDD) portion of the date.
 * This is useful if the data is daily data anyway and you're
 * interested in feeding this data into another system that
 * functions on the integer portion of the date and therefore
 * doesn't allow for complete timestamps.
 */
CKVector<long> CKTimeSeries::getDates()
{
	CKVector<long>		retval;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// get all the keys from the map no matter what
	if (!mTimeseries.empty()) {
		std::map<double, double>::iterator	i;
		for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
			retval.addToEnd((long) (*i).first);
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * This method gets the complete series of timestamps for the
 * current timeseries. This is useful if you're interesting in
 * knowing the time of each data point.
 */
CKVector<double> CKTimeSeries::getDateTimes()
{
	CKVector<double>		retval;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// get all the keys from the map no matter what
	if (!mTimeseries.empty()) {
		std::map<double, double>::iterator	i;
		for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
			retval.addToEnd((*i).first);
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * This method takes a timestamp in the format YYYYMMDD.hhmmssss
 * where the seconds are to the hundredth of a second and if
 * no data point exists in the timeseries for the time specified
 * an interpolation is done to get the closest value possible
 * based on the points that are in the timeseries. Of course,
 * as with any interpolated value, the closer the available data
 * points are, the more accurate the interpolated value.
 */
double CKTimeSeries::interpolate( double aDateTime )
{
	bool		error = false;
	double		retval = NAN;

	// first, see if we have anything to do
	if (!error) {
		if (mTimeseries.size() <= 0) {
			error = true;
			// no need to log anything, just return 'nothing'
		}
	}

	// lock up this guy against changes
	mTimeseriesMutex.lock();

	// next thing we need are the bracketing values
	double		lowTime = NAN;
	double		lowValue = 0;
	double		highTime = NAN;
	double		highValue = 0;
	if (!error) {
		/*
		 * In checking for the limits, we need to check all but the limits
		 * for each bracketing value. Meaning, that for the low value, don't
		 * even bother looking at the highest value, and vice versa for the
		 * high bracketing value.
		 *
		 * So... get one less than the size of the series and use that as
		 * the loop test condition.
		 */
		unsigned int	cnt = mTimeseries.size() - 1;

		// go from low to high finding the best low value
		std::map<double, double>::iterator	i = mTimeseries.begin();
		for (unsigned int c = 0; c < cnt; ++i, ++c) {
			if ((((*i).first <= aDateTime) && !isnan((*i).second)) ||
				isnan(lowTime)) {
				lowTime = (*i).first;
				lowValue = (*i).second;
			}
		}

		// go from high to low finding the best high value
		std::map<double, double>::reverse_iterator	r = mTimeseries.rbegin();
		for (unsigned int c = 0; c < cnt; ++r, ++c) {
			if ((((*r).first >= aDateTime) && !isnan((*r).second)) ||
				isnan(highTime)) {
				highTime = (*r).first;
				highValue = (*r).second;
			}
		}
	}

	/*
	 * Make sre we have ppoints that we can do soemthing with. That
	 * means bascially that they are not NANs on either axis.
	 */
	if (!error) {
		if (isnan(lowTime) || isnan(lowValue) ||
			isnan(highTime) || isnan(highValue)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeSeries::interpolate(double) - the timeseries data "
				"does not contain two reasonable points that can be used in "
				"the interpolation. Please make sure there's data in the series "
				"before attempting an interpolation.";
			// unlock up this guy for changes
			mTimeseriesMutex.unlock();
			// ...and throw the exception
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Next, we'll break this down into the different types of estimation
	 * based on the location of the reference points and the value of the
	 * requested point with regards to these points.
	 */
	// try the simplest case - the data is one of the end points
	if (!error && isnan(retval)) {
		if (lowTime == aDateTime) {
			retval = lowValue;
		} else if (highTime == aDateTime) {
			retval = highValue;
		} else if (lowTime == highTime) {
			// average any discontinuities in the series
			retval = (lowValue + highValue)/2.0;
		}
	}
	// next, see where the data point loes w.r.t. to the series
	if (!error && isnan(retval)) {
		if (aDateTime < lowTime) {
			// this is to the left of the entire series
			double	x = (lowTime - aDateTime)/(highTime - aDateTime);
			retval = (lowValue - x * highValue)/(1.0 - x);
		} else if (aDateTime > highTime) {
			// this is to the right of the entire series
			double	x = (highTime - lowTime)/(aDateTime - lowTime);
			retval = (highValue + (x - 1.0) * lowValue)/x;
		} else {
			// this is within the series
			double	x = (aDateTime - lowTime)/(highTime - lowTime);
			retval = lowValue + x * (highValue - lowValue);
		}
	}

	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * This method takes a series of timestamps each in the format
 * YYYYMMDD.hhmmssss where the seconds are to the hundredth of a
 * second and if no data point exists in the timeseries for each
 * of the times specified an interpolation is done to get the
 * closest value possible based on the points that are in the
 * timeseries. Of course, as with any interpolated value, the
 * closer the available data points are, the more accurate the
 * interpolated value.
 */
CKVector<double> CKTimeSeries::interpolate( const CKVector<double> & aDateSeries )
{
	CKVector<double>		retval;

	// simply call the single value interpolator many times
	unsigned int	cnt = aDateSeries.size();
	if (cnt > 0) {
		for (unsigned int i = 0; i < cnt; i++) {
			retval.addToEnd(interpolate(aDateSeries[i]));
		}
	}

	return retval;
}


/*
 * This method does a simple time-based accumutation of the data in
 * the time series modifying the data as it goes. This can be thought
 * of as a simple integrator, but since it's really not integrating,
 * we've used the method name 'accumulate' as that's a bit more
 * descriptive of what it's really doing.
 */
bool CKTimeSeries::accumulate()
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// keep track of the running sum
	double		sum = 0.0;
	// for each value in the series, accumulate it and replace it
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		// hold on to the value as it's going to be used a few times
		double	value = (*i).second;
		// if the value is NAN, then accumulate nothing
		if (!isnan(value)) {
			sum += value;
		}
		// OK, save the sum up to this point
		(*i).second = sum;
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


/*
 * This method, and it's convenience method, fill in the values
 * in the time series by starting at the point furthest back in time
 * and "filling in" any values that don't exist on the given interval.
 * For example, the convenience method call this guy with the argument
 * of 1 - meaning that the resulting time series will be assured to
 * have values that are no more than one day apart in time - filling
 * in the 'new' values with the one from the most recent past.
 *
 * This is very useful when a group of time series values need to be
 * consistent in their interval and not miss any points in the
 * series.
 */
bool CKTimeSeries::fillInValues( int anInterval, double aStartDate,
								 double anEndDate, int maxFillsWarning )
{
	bool		error = false;

	// see if we have to figure out the limits
	if (!error) {
		if (aStartDate < 0) {
			aStartDate = getStartingDate();
		}
	}
	if (!error) {
		if (anEndDate < 0) {
			anEndDate = getEndingDate();
		}
	}

	// make sure we have something to do
	if (!error) {
		if (anEndDate < aStartDate) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeSeries::fillInValues(double, double, double, int) - the "
				"ending date: " << anEndDate << " is before the starting date: " <<
				aStartDate << " and this makes no sense at all. Please make sure "
				"that the date interval makes sense.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// start at the start - where else?
	double		date = aStartDate;
	double		lastValue = NAN;
	double		value = NAN;
	int			fills = 0;
	while (!error && (date <= anEndDate)) {
		// get the value for this day
		value = get(date);
		// see if it's empty...
		if (isnan(value)) {
			// if we have a good value, use it - else force a NAN for spacing
			if (isnan(lastValue)) {
				put(date, value);
			} else {
				// put in the last good value and update the number of fills
				put(date, lastValue);
				fills++;
			}
		} else {
			// see if we've passed the magic number of fills
			if (fills > maxFillsWarning) {
				char	buff[512];
				bzero(buff, 512);
				snprintf(buff, 511, "CKTimeSeries::fillInValues(double, double, "
					"double, int) - we have filled in %17.8lf with %lf "
					" which is consecutive fill #%d", date, lastValue, fills);
				std::cerr << buff << std::endl;
			}
			// update the last value and reset the number of consecutive fills
			lastValue = value;
			fills = 0;
		}
		// now increment the date properly
		date = addDays(date, anInterval);
	}

	return !error;
}


bool CKTimeSeries::fillInDailyValues()
{
	// dates of '-1' mean "use what's in the time series as limits"
	return fillInValues(1, -1.0, -1.0);
}


/********************************************************
 *
 *                Simple Series Math Methods
 *
 ********************************************************/
/*
 * These methods allow the user to add values to this timeseries,
 * in the first case, it's a constant value but in the
 * second it's another timeseries. This last version is useful when
 * you have two series of the same dates and you wish to sum them
 * point by point.
 */
bool CKTimeSeries::add( double anOffset )
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, add the constant
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			(*i).second += anOffset;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


bool CKTimeSeries::add( CKTimeSeries & aSeries )
{
	bool		error = false;

	// first, see if the two timeseries are made of the same timestamps
	if (!error) {
		CKVector<double>		me = getDateTimes();
		CKVector<double>		him = aSeries.getDateTimes();
		if (me != him) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeSeries::add(CKTimeSeries &) - the timestamps in the "
				"passed-in series are not the same as the ones in this guy. "
				"This implementation needs to have the dates match up.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we might as well add the series to this one point-by-point
	 */
	if (!error) {
		// first, lock up the two series against changes
		mTimeseriesMutex.lock();
		aSeries.mTimeseriesMutex.lock();

		// get going on the looping
		std::map<double, double>::iterator	i;
		std::map<double, double>::iterator	j;
		for (i = mTimeseries.begin(), j = aSeries.mTimeseries.begin();
			(i != mTimeseries.end()) && (j != aSeries.mTimeseries.end());
			++i, ++j) {
			(*i).second += (*j).second;
		}

		// lastly, unlock up the two series for changes
		aSeries.mTimeseriesMutex.unlock();
		mTimeseriesMutex.unlock();
	}

	return !error;
}


/*
 * These methods allow the user to subtract values from this
 * timeseries, in the first case, it's a constant value but
 * in the second it's another timeseries. This last version is useful
 * when you have two series of the same dates and you wish to
 * difference them point by point.
 */
bool CKTimeSeries::subtract( double anOffset )
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, subtract the constant
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			(*i).second -= anOffset;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


bool CKTimeSeries::subtract( CKTimeSeries & aSeries )
{
	bool		error = false;

	// first, see if the two timeseries are made of the same timestamps
	if (!error) {
		CKVector<double>		me = getDateTimes();
		CKVector<double>		him = aSeries.getDateTimes();
		if (me != him) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeSeries::subtract(CKTimeSeries &) - the timestamps in the "
				"passed-in series are not the same as the ones in this guy. "
				"This implementation needs to have the dates match up.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we might as well add the series to this one point-by-point
	 */
	if (!error) {
		// first, lock up the two series against changes
		mTimeseriesMutex.lock();
		aSeries.mTimeseriesMutex.lock();

		// get going on the looping
		std::map<double, double>::iterator	i;
		std::map<double, double>::iterator	j;
		for (i = mTimeseries.begin(), j = aSeries.mTimeseries.begin();
			(i != mTimeseries.end()) && (j != aSeries.mTimeseries.end());
			++i, ++j) {
			(*i).second -= (*j).second;
		}

		// lastly, unlock up the two series for changes
		aSeries.mTimeseriesMutex.unlock();
		mTimeseriesMutex.unlock();
	}

	return !error;
}


/*
 * These method allows the user to multiply a constant value to
 * all data points in this timeseries.
 */
bool CKTimeSeries::multiply( double aFactor )
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, multiply the factor
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			(*i).second *= aFactor;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


/*
 * These method allows the user to divide each point in the
 * timeseries by a constant value.
 */
bool CKTimeSeries::divide( double aDivisor )
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, divide by the divisor
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			(*i).second /= aDivisor;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


/*
 * This method takes the arithmetic mean (a.k.a. average) of the
 * timeseries and then subtracts this value from each data point in
 * the series to make the entire series zero-mean.
 */
bool CKTimeSeries::subtractAverage()
{
	return subtract(average());
}


/*
 * This method takes the value at each point in the timeseries and
 * divides it by the previous point in the series and then takes
 * the natural log of the result and keeps that value. This is
 * essentially computing the daily price returns of the data series
 * and since this is getting used in financial applications this is
 * a pretty obvious thing to do.
 */
bool CKTimeSeries::computeDailyReturns()
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// save the previous value as the divisor
	double		previous = NAN;
	// for each value in the series, divide by the previous value
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		// hold on to the value as it's going to be 'previous' soon
		double	value = (*i).second;
		// if the 'previous' is NAN, then this is too...
		if (isnan(previous)) {
			(*i).second = NAN;
		} else if (!isnan(value)) {
			// OK, got good data to divide, so do it
			(*i).second = log(value / previous);
		}
		// now update the 'previous' with the last good value
		if (!isnan(value)) {
			previous = value;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


/*
 * This method simply takes the inverse of each value in the time
 * series so that y -> 1/y for all points. This is marginally useful
 * I'm thinking, but I added it here to be a little more complete.
 */
bool CKTimeSeries::inverse()
{
	bool		error = false;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, invert it
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			(*i).second = 1.0 / (*i).second;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return !error;
}


/*
 * This method computes the arithmetic mean (a.k.a. average) for all
 * the values in the dataset. This is useful if you're interested
 * in knowing it and don't want to hassle with computing such a
 * common number yourself.
 */
double CKTimeSeries::average()
{
	double		retval = 0;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// for each value in the series, multiply the factor
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			retval += (*i).second;
		}
	}
	// now divide by the number
	retval /= (double) mTimeseries.size();
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * These methods are simple numerical attributes of the series
 * computed each time they are called. They are pretty easy to
 * understand, with the rms() being the root-mean-square value
 * of the series and the linf() being the l-infinity norm of the
 * series.
 */
double CKTimeSeries::max()
{
	double		retval = NAN;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// check each value in the series for the numerically largest
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second) && ((retval < (*i).second) || isnan(retval))) {
			retval = (*i).second;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


double CKTimeSeries::min()
{
	double		retval = NAN;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// check each value in the series for the numerically smallest
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second) && ((retval > (*i).second) || isnan(retval))) {
			retval = (*i).second;
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


double CKTimeSeries::rms()
{
	double		retval = 0;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// compute the sum of the squares of each value
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			retval += (*i).second * (*i).second;
		}
	}
	// finally take the square root of the average of the squares
	retval = sqrt(retval/((double) mTimeseries.size()));
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


double CKTimeSeries::linf()
{
	double		retval = NAN;

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// find the largest absolute value of any element
	std::map<double, double>::iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		if (!isnan((*i).second)) {
			double	a = fabs((*i).second);
			if ((retval < a) || isnan(retval)) {
				retval = a;
			}
		}
	}
	// unlock up this guy for changes
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * These are the operator equivalents of the simple mathematical
 * operations on the time series. They are here as an aid to the
 * developer of analytic functions based on these time series.
 */
CKTimeSeries & CKTimeSeries::operator+=( double anOffset )
{
	add(anOffset);
	return *this;
}


CKTimeSeries & CKTimeSeries::operator+=( CKTimeSeries & aSeries )
{
	add(aSeries);
	return *this;
}


CKTimeSeries & CKTimeSeries::operator-=( double anOffset )
{
	subtract(anOffset);
	return *this;
}


CKTimeSeries & CKTimeSeries::operator-=( CKTimeSeries & aSeries )
{
	add(aSeries);
	return *this;
}


CKTimeSeries & CKTimeSeries::operator*=( double aFactor )
{
	multiply(aFactor);
	return *this;
}


CKTimeSeries & CKTimeSeries::operator/=( double aDivisor )
{
	divide(aDivisor);
	return *this;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method is very useful when you want to get the current
 * date and time in the proper format and data type for some kind
 * of operation. There's not a lot to do but it's nice to have a
 * method that will create the current date and time as a double
 * of the format YYYYMMDD.hhmmssss with a resolution of hundredths
 * of a second.
 */
double CKTimeSeries::getCurrentTimestamp()
{
	double		retval = 0;

	time_t		now_t = time(NULL);
	struct tm	now;
	localtime_r(&now_t, &now);

	// make a double of the form YYYYMMDD.hhmmss
	retval = ((now.tm_year + 1900) * 100.0 + (now.tm_mon + 1)) * 100.0 +
			 now.tm_mday +
			 (now.tm_hour  + ((now.tm_min + (now.tm_sec/100.0)) / 100.0)) / 100.0;

	return retval;
}


/*
 * This method gets the current date as a double of the format
 * YYYYMMDD which is nice for certain values like the close price
 * or other daily numbers. This is a conveninence method that calls
 * the more generic method and then drops off the hours, minutes,
 * and seconds.
 */
double CKTimeSeries::getCurrentDate()
{
	return floor(getCurrentTimestamp());
}


/*
 * This method adds or subtracts the count of days from the given
 * date and returns the resulting date to the caller. This is nice
 * because to add days to a date, make the count positive, to
 * subtract days from a date, make the count negative. The method
 * respects leap years, etc.
 */
double CKTimeSeries::addDays( double aDate, int aCnt )
{
	bool		done = false;
	double		retval = 0;

	// first, see if we have anything to do
	if (!done) {
		if (aCnt == 0) {
			retval = aDate;
			done = true;
		}
	}

	// next, get the components as year, month, date, and time
	int		year = 0;
	int		month = 0;
	int		day = 0;
	double	time = 0.0;
	if (!done) {
		year = (int) floor(aDate / 10000.0);
		month = (int) floor((aDate - year * 10000) / 100.0);
		day = (int) floor(aDate - year * 10000 - month * 100);
		time = aDate - floor(aDate);
	}

	/*
	 * I need to break this guy up now based on whether we are moving
	 * forward or backward in time. It just means different tests and
	 * limit conditions, but the same kind of logic prevails.
	 */
	if (!done) {
		if (aCnt > 0) {
			int		lastDayInMonth = 0;
			while (aCnt > 0) {
				// get the last day in this current month
				lastDayInMonth = 31;
				if ((month == 4) || (month == 6) ||
					(month == 9) || (month == 11)) {
					lastDayInMonth = 30;
				} else if (month == 2) {
					lastDayInMonth = 28;
					if ((year % 4) == 0) {
						lastDayInMonth = 29;
						if ((year % 100) == 0) {
							lastDayInMonth = 28;
							if ((year % 400) == 0) {
								lastDayInMonth = 29;
							}
						}
					}
				}

				// see if the interval is passing the month boundary
				if (day + aCnt > lastDayInMonth) {
					// consume those days left in this month
					aCnt -= (lastDayInMonth - day + 1);
					day = 1;
					month++;
					// is the new month crossing the year boundary?
					if (month == 13) {
						month = 1;
						year++;
					}
				} else {
					// the move is within this month and we're all done
					day += aCnt;
					aCnt = 0;
				}
			}
		} else {
			int		lastDayInPrevMonth = 0;
			while (aCnt < 0) {
				// get the last day in the month before us in the calendar
				lastDayInPrevMonth = 31;
				if ((month == 5) || (month == 7) ||
					(month == 10) || (month == 12)) {
					lastDayInPrevMonth = 30;
				} else if (month == 3) {
					lastDayInPrevMonth = 28;
					if ((year % 4) == 0) {
						lastDayInPrevMonth = 29;
						if ((year % 100) == 0) {
							lastDayInPrevMonth = 28;
							if ((year % 400) == 0) {
								lastDayInPrevMonth = 29;
							}
						}
					}
				}

				// see if the interval is corring into the prev. month
				if (day + aCnt < 1) {
					// consume those days left in this month
					aCnt += day;
					day = lastDayInPrevMonth;
					month--;
					// is the new month crossing the year boundary?
					if (month == 0) {
						month = 12;
						year--;
					}
				} else {
					// the move is within this month and we're all done
					day += aCnt;
					aCnt = 0;
				}
			}
		}

		// let's put this back together for a date
		retval = (year * 100.0 + month) * 100.0 + day + time;
	}

	return retval;
}


/*
 * These are convenience methods that call the more general addDays()
 * method to move a number of days up or back in the calendar. These
 * are nice in case you have a simple request.
 */
double CKTimeSeries::moveBackDays( double aDate, int aCnt )
{
	return addDays(aDate, -1 * aCnt);
}


double CKTimeSeries::moveUpDays( double aDate, int aCnt )
{
	return addDays(aDate, aCnt);
}


double CKTimeSeries::moveBackADay( double aDate )
{
	return addDays(aDate, -1);
}


double CKTimeSeries::moveUpADay( double aDate )
{
	return addDays(aDate, 1);
}


/*
 * These methods allow the caller to get the very first date in the
 * series as well as the very last date in the series. This is nice
 * because we can then determine what date range this series covers
 * and what to operate on, for instance.
 */
double CKTimeSeries::getStartingDate()
{
	double		retval;

	// lock this guy up so it doesn't change
	mTimeseriesMutex.lock();
	// now get the first pair in the map
	std::map<double, double>::iterator	i;
	i = mTimeseries.begin();
	retval = (*i).first;
	// ...and unlock as we're done
	mTimeseriesMutex.unlock();

	return retval;
}


double CKTimeSeries::getEndingDate()
{
	double		retval;

	// lock this guy up so it doesn't change
	mTimeseriesMutex.lock();
	// now get the last pair in the map
	std::map<double, double>::reverse_iterator	i;
	i = mTimeseries.rbegin();
	retval = (*i).first;
	// ...and unlock as we're done
	mTimeseriesMutex.unlock();

	return retval;
}


/*
 * In order to simplify the move of this object from C++ to Java
 * it makes sense to encode the value's data into a (char *) that
 * can be converted to a Java String and then the Java object can
 * interpret it and "reconstitue" the object from this coding.
 */
CKString CKTimeSeries::generateCodeFromValues() const
{
	// start by getting a buffer to build up this value
	CKString buff;

	// first, send out the count of data points
	buff.append("\x01").append((int)mTimeseries.size()).append("\x01");

	// next, loop over all the data points and write each out
	std::map<double, double>::const_iterator	i;
	for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
		// see if the timestamp is just a date
		if ((*i).first == floor((*i).first)) {
			// this will be just YYYYMMDD on the output
			buff.append(((long)floor((*i).first))).append("\x01");
		} else {
			// this will be YYYYMMDD.hhmmssss on the output
			char	dateVal[20];
			bzero(dateVal, 20);
			snprintf(dateVal, 19, "%17.8lf", (*i).first);
			buff.append(dateVal).append("\x01");
		}
		// finish it off with the value in scientific notation
		buff.append((*i).second).append("\x01");
	}

	/*
	 * OK, it's now in a simple character array that we can scan to check
	 * for acceptable delimiter values. What we'll do is to check the string
	 * for the existence of a series of possible delimiters, and as soon as
	 * we find one that's not used in the string we'll use that guy.
	 */
	if (!CKTable::chooseAndApplyDelimiter(buff)) {
		throw CKException(__FILE__, __LINE__, "CKTimeSeries::generateCodeFrom"
			"Values() - while trying to find an acceptable delimiter for "
			"the data in the timeseries we ran out of possibles before finding "
			"one that wasn't being used in the text of the code. This is "
			"a serious problem that the developers need to look into.");
	}

	return buff;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this value. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKTimeSeries::takeValuesFromCode( const CKString & aCode )
{
	// first, see if we have anything to do
	if (aCode.empty()) {
		throw CKException(__FILE__, __LINE__, "CKTimeSeries::takeValuesFromCode("
			"const char *) - the passed-in code is empty which means that "
			"there's nothing I can do. Please make sure that the argument is "
			"not empty before calling this method.");
	}

	// lock up this guy against changes
	mTimeseriesMutex.lock();
	// ...and clear out all the existing data
	mTimeseries.clear();

	/*
	 * The data is character-delimited and the delimiter is
	 * the first character of the field data. All subsequent
	 * values will be delimited by this character. We need to
	 * get it.
	 */
	char	delim = aCode[0];
	// ...and parse this guy into chunks
	int		bit = 0;
	CKStringList	chunks = CKStringList::parseIntoChunks(
									aCode.substr(1, aCode.size()-2), delim);
	if (chunks.size() < 1) {
		std::ostringstream	msg;
		msg << "CKTimeSeries::takeValuesFromCode(const CKString &) - the code: '" <<
			aCode << "' does not represent a valid timeseries encoding. Please "
			"check on it's source as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * Next thing is the count of time/value pairs in the series.
	 */
	int cnt = chunks[bit++].intValue();
	// see if we have enough to fill in this table
	if (chunks.size() < (1 + 2*cnt)) {
		std::ostringstream	msg;
		msg << "CKTimeSeries::takeValuesFromCode(const CKString &) - the code: '" <<
			aCode << "' does not represent a valid timeseries encoding. Please "
			"check on it's source as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * Next, we need to read off the pairs of timestamps and values.
	 * both are doubles, the timestamp in the form YYYYMMDD.hhmmssss.
	 */
	for (int i = 0; i < cnt; i++) {
		double	timestamp = chunks[bit++].doubleValue();
		double	value = chunks[bit++].doubleValue();
		if (timestamp == NAN) {
			// unlock up this guy for changes
			mTimeseriesMutex.unlock();
			// ...and throw the exception
			throw CKException(__FILE__, __LINE__, "CKTimeSeries::takeValues"
				"FromCode(const char *) - while trying to read the timestamp "
				"value, a NAN was read. This is a serious problem in the "
				"code.");
		} else {
			// save it as any other value
			mTimeseries[timestamp] = value;
		}
	}

	// unlock up this guy for changes
	mTimeseriesMutex.unlock();
}


/*
 * This method checks to see if the two CKTimeSeries are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKTimeSeries::operator==( const CKTimeSeries & anOther ) const
{
	bool		equal = true;

	// see if the series match
	if (mTimeseries != anOther.mTimeseries) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKTimeSeries are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKTimeSeries::operator!=( const CKTimeSeries & anOther ) const
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
CKString CKTimeSeries::toString() const
{
	CKString		retval = "[";

	// make sure we have something to show...
	if (mTimeseries.size() > 0) {
		// need to write them to a buffer first
		char	buff[256];
		// now get busy on the looping
		std::map<double, double>::const_iterator	i;
		for (i = mTimeseries.begin(); i != mTimeseries.end(); ++i) {
			snprintf(buff, 255, "(%17.8lf, %lf)\n", (*i).first, (*i).second);
			retval.append(buff);
		}
	}

	// finish off the series
	retval.append("]\n");

	return retval;
}


/*
 * This method can be used by the subclasses to get a handle on the
 * map that is the master data structure for this instance. This is
 * typically not used in the methods in this class, but if a subclass
 * wants to get directly at the data, then they can use this.
 */
std::map<double, double> *CKTimeSeries::getTimeseries()
{
	return & mTimeseries;
}


/*
 * This method is used by the subclasses to get access to the mutex
 * for controlling the modification to the data. In general, there's
 * not a great need for this, but on the off-chance that it's
 * really needed, it's here.
 */
CKFWMutex *CKTimeSeries::getTimeseriesMutex()
{
	return & mTimeseriesMutex;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTimeSeries & anItem )
{
	aStream << anItem.toString();

	return aStream;
}


/*
 * These are the operators for creating new time series data from
 * one or two existing time series. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKTimeSeries operator+( CKTimeSeries & aSeries, double aValue )
{
	CKTimeSeries	retval(aSeries);
	retval += aValue;
	return retval;
}


CKTimeSeries operator+( double aValue, CKTimeSeries & aSeries )
{
	return operator+(aSeries, aValue);
}


CKTimeSeries operator+( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries )
{
	CKTimeSeries	retval(aSeries);
	retval += anotherSeries;
	return retval;
}


CKTimeSeries operator-( CKTimeSeries & aSeries, double aValue )
{
	CKTimeSeries	retval(aSeries);
	retval -= aValue;
	return retval;
}


CKTimeSeries operator-( double aValue, CKTimeSeries & aSeries )
{
	CKTimeSeries	retval(aSeries);
	retval *= -1.0;
	retval += aValue;
	return retval;
}


CKTimeSeries operator-( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries )
{
	CKTimeSeries	retval(aSeries);
	retval -= anotherSeries;
	return retval;
}


CKTimeSeries operator*( CKTimeSeries & aSeries, double aValue )
{
	CKTimeSeries	retval(aSeries);
	retval *= aValue;
	return retval;
}


CKTimeSeries operator*( double aValue, CKTimeSeries & aSeries )
{
	return operator*(aSeries, aValue);
}


CKTimeSeries operator/( CKTimeSeries & aSeries, double aValue )
{
	CKTimeSeries	retval(aSeries);
	retval /= aValue;
	return retval;
}


CKTimeSeries operator/( double aValue, CKTimeSeries & aSeries )
{
	CKTimeSeries	retval(aSeries);
	retval.inverse();
	retval *= aValue;
	return retval;
}
