/*
 * CKTimeSeries.h - this file defines a class that can be used to represent a
 *                  time-series data stream. Basically, a double is used to
 *                  represent the date/time as YYYYMMDD.HHMMSShh with the
 *                  time resolution down to the hundredth of a second, and
 *                  a double is used to represent the data value. This is a
 *                  nice little class that is used in the CKVariant as yet
 *                  another form of data that that class can represent.
 *
 * $Id: CKTimeSeries.h,v 1.23 2007/01/18 10:02:37 drbob Exp $
 */
#ifndef __CKTIMESERIES_H
#define __CKTIMESERIES_H

//	System Headers
#include <map>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
/*
 * Because we're using the NAN value in some places in this object,
 * we need to make sure that it's defined for all the platforms that
 * will be using this object.
 */
#ifdef __linux__
#define __USE_ISOC99 1
#endif
#include <math.h>
/*
 * Oddly enough, Sun doesn't seem to have NAN defined, so we need to
 * do that here so that things run more smoothly. This is very interesting
 * because Sun has isnan() defined, but no obvious way to set a value.
 */
#ifdef __sun__
#ifndef NAN
#define	NAN	(__extension__ ((union { unsigned __l __attribute__((__mode__(__SI__))); \
			float __d; }) { __l: 0x7fc00000UL }).__d)
#endif
#endif
/*
 * This is most odd, but it seems that at least on Darwin (Mac OS X)
 * with gcc 4.0 - they have left the C99 defines in math.h but in cmath
 * they have #undef-ed all these and replaced them with the std:: templates
 * so that isnan() becomes std::isnan(). I can see the logic, but it's
 * not obvious at all. Thank goodness for Google.
 */
#ifdef __MACH__
#include <cmath>
#ifdef isnan
#undef isnan
#endif
#define isnan(x)	std::isnan(x)
#endif

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKString.h"
#include "CKVector.h"

//	Forward Declarations

//	Public Constants
/*
 * The time series can fill in missing data but there's a point at which
 * each fill is reported as a warning to the logs. This number of consecutive
 * fills is a parameter to the method, but defaults to the value below. For
 * most work, more than this is something to be concerned about.
 */
#define	DEFAULT_MAX_FILLS		5

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKTimeSeries
{
	public:
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
		CKTimeSeries();
		/*
		 * This form of the constructor takes a vector of dates (that
		 * means no time values allowed) and another of values and puts
		 * these two into the time series matching up the indeces of
		 * the two vectors. This is useful when the historical data is
		 * something that has only one value per day and time per day
		 * is not important.
		 */
		CKTimeSeries( const CKVector<long> & aDateSeries,
					  const CKVector<double> & aValueSeries );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKTimeSeries's generateCodeFromValues()
		 * method and parses it into a time series of values directly.
		 * This is very useful for serializing the table's data from one
		 * host to another across a socket, for instance.
		 */
		CKTimeSeries( const CKString & aCode );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKTimeSeries( const CKTimeSeries & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKTimeSeries();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKTimeSeries & operator=( const CKTimeSeries & anOther );

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
		void put( double aDateTime, double aValue );
		/*
		 * This method takes the vector of values and date/times and
		 * puts all the data into the timeseries by matching up the
		 * indeces of the two vectors. Each timestamp value is a double
		 * in the format YYYYMMDD.hhmmssss where seconds are in the
		 * hundredths of a second.
		 */
		void put( const CKVector<double> & aDateSeries,
				  const CKVector<double> & aValueSeries );
		/*
		 * This method essentially merges the passed-in timeseries with
		 * the existing one to create a 'super set'. This is nice because
		 * we can then work on components of the time series and then
		 * 'stitch' them back together with this method.
		 */
		void put( CKTimeSeries & aSeries );
		/*
		 * This method gets the value of the timeseries at the given
		 * timestamp or NAN as defined in <math.h>. To test the value
		 * use isnan() in <math.h>.
		 */
		double get( double aDateTime );
		double get( double aDateTime ) const;
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
		CKVector<double> get( const CKVector<double> & aDateSeries );
		CKVector<double> get( const CKVector<double> & aDateSeries ) const;
		/*
		 * This method tries to get the value from the timeseries for
		 * today. This can be tricky as the date being used here is not
		 * the complete timestamp, so you have to be sure that the data
		 * in the series is by date.
		 */
		double getToday();
		double getToday() const;
		/*
		 * This method takes today's date and marches back in time the
		 * provided number of days to arrive at the value to return.
		 * This is nice in that 0 will get the latest value and 1 will
		 * get the value yesterday, and n will get you the value n days
		 * ago.
		 */
		double getDaysBack( int aDayCnt );
		double getDaysBack( int aDayCnt ) const;
		/*
		 * This method looks at the first point in time in this series
		 * and returns the value of that point. This is an easy way to
		 * get the "starting value" of the series.
		 */
		double getFirstValue();
		double getFirstValue() const;
		/*
		 * This method looks at the last point in time in this series
		 * and returns the value of that point. This is an easy way to
		 * get the "ending value" of the series.
		 */
		double getLastValue();
		double getLastValue() const;
		/*
		 * This method looks at the first point in time in this series
		 * and returns the date of that point in the format YYYYMMDD.hhmmss.
		 * This is an easy way to get the "starting time" of the series.
		 */
		double getFirstDate();
		double getFirstDate() const;
		/*
		 * This method looks at the last point in time in this series
		 * and returns the date of that point in the format YYYYMMDD.hhmmss.
		 * This is an easy way to get the "ending time" of the series.
		 */
		double getLastDate();
		double getLastDate() const;

		/*
		 * This method clears out all the dates and values from this time
		 * series so that it's as if the time series is empty and ready to
		 * hold brand new data.
		 */
		void clear();

		/*
		 * This method removes those points in the series that have date
		 * values between the provided start date and end date - inclusive.
		 * If an end date is no specified it's assumed to be to the end
		 * of the time series.
		 */
		void eraseDates( long aStartDate = -1, long anEndDate = -1 );
		/*
		 * This method removes those points in the series that have date
		 * time values between the provided start date and end date -
		 * inclusive. If an end date is no specified it's assumed to be
		 * to the end of the time series.
		 */
		void eraseDateTimes( double aStartDate = -1, double anEndDate = -1 );

		/*
		 * This method can be used to add in a single date/value point to
		 * the time series, and returns the *new* value of that date/time
		 * to the caller in case they are keeping track of the actual value
		 * being summed. Say you're building up a time series data set and
		 * you want to add in points as they come in from some source. This
		 * single method saves you locking the set, getting the existing
		 * value, creating the sum, and then setting that sum back into the
		 * series. It's much more efficient.
		 */
		double add( double aDateTime, double aValue );
		/*
		 * This method "swaps out" the existing value for the given date/time
		 * for the provided value. This original value is then returned to
		 * the caller to do with as they see fit. If no point exists for this
		 * date/time then NAN will be returned and the value will be set.
		 */
		double swap( double aDateTime, double aValue );

		/*
		 * This method gets the complete series of dates for the current
		 * timeseries but as just the day (YYYYMMDD) portion of the date.
		 * This is useful if the data is daily data anyway and you're
		 * interested in feeding this data into another system that
		 * functions on the integer portion of the date and therefore
		 * doesn't allow for complete timestamps.
		 *
		 * If the start and end dates are specified, then only the dates
		 * that are *inclusive* of these dates will be returned.
		 */
		CKVector<long> getDates( long aStartDate = -1, long anEndDate = -1 );
		CKVector<long> getDates( long aStartDate = -1, long anEndDate = -1 ) const;
		/*
		 * This method gets the complete series of timestamps for the
		 * current timeseries. This is useful if you're interesting in
		 * knowing the time of each data point.
		 *
		 * If the start and end dates are specified, then only the dates
		 * that are *inclusive* of these dates will be returned.
		 */
		CKVector<double> getDateTimes( double aStartDate = -1, double anEndDate = -1 );
		CKVector<double> getDateTimes( double aStartDate = -1, double anEndDate = -1 ) const;

		/*
		 * These methods do the same thing - they return the number of
		 * time/value pairs in the timeseries. This is nice when you need
		 * to know the number and not necessarily what those values are -
		 * such as sizing an array or something.
		 */
		int size();
		int size() const;
		int length();
		int length() const;

		/*
		 * This method takes a timestamp in the format YYYYMMDD.hhmmssss
		 * where the seconds are to the hundredth of a second and if
		 * no data point exists in the timeseries for the time specified
		 * an interpolation is done to get the closest value possible
		 * based on the points that are in the timeseries. Of course,
		 * as with any interpolated value, the closer the available data
		 * points are, the more accurate the interpolated value.
		 */
		double interpolate( double aDateTime );
		double interpolate( double aDateTime ) const;
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
		CKVector<double> interpolate( const CKVector<double> & aDateSeries );
		CKVector<double> interpolate( const CKVector<double> & aDateSeries ) const;
		/*
		 * This method does a simple time-based accumutation of the data in
		 * the time series modifying the data as it goes. This can be thought
		 * of as a simple integrator, but since it's really not integrating,
		 * we've used the method name 'accumulate' as that's a bit more
		 * descriptive of what it's really doing.
		 */
		bool accumulate();
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
		bool fillInValues( int anInterval, double aStartDate,
						   double anEndDate, int maxFillsWarning = DEFAULT_MAX_FILLS );
		bool fillInDailyValues();

		/*
		 * This method will sum the values of this time series from the
		 * starting date (YYYYMMDD.hhmmssss) through the ending date -
		 * inclusive of the dates and return that one value. All NAN values
		 * will be skipped in the summing so as not to throw off the sum.
		 * Additionally, the default values for the starting and ending
		 * date will span the entire series so if you want the sum of the
		 * entire series just call sum().
		 */
		double sum( double aStartDate = -1, double anEndDate = -1 );
		double sum( double aStartDate = -1, double anEndDate = -1 ) const;

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
		bool add( double anOffset );
		bool add( CKTimeSeries & aSeries );
		bool add( const CKTimeSeries & aSeries );
		/*
		 * These methods allow the user to subtract values from this
		 * timeseries, in the first case, it's a constant value but
		 * in the second it's another timeseries. This last version is useful
		 * when you have two series of the same dates and you wish to
		 * difference them point by point.
		 */
		bool subtract( double anOffset );
		bool subtract( CKTimeSeries & aSeries );
		bool subtract( const CKTimeSeries & aSeries );
		/*
		 * These method allows the user to multiply a constant value to
		 * all data points in this timeseries or to multiply each of the
		 * values of one time series by the values of another where the
		 * dates line up.
		 */
		bool multiply( double aFactor );
		bool multiply( CKTimeSeries & aSeries );
		bool multiply( const CKTimeSeries & aSeries );
		/*
		 * These method allows the user to divide each point in the
		 * timeseries by a constant value or to divide each of the
		 * values of one time series by the values of another where the
		 * dates line up.
		 */
		bool divide( double aDivisor );
		bool divide( CKTimeSeries & aSeries );
		bool divide( const CKTimeSeries & aSeries );

		/*
		 * This method takes the arithmetic mean (a.k.a. average) of the
		 * timeseries and then subtracts this value from each data point in
		 * the series to make the entire series zero-mean.
		 */
		bool subtractAverage();
		/*
		 * This method takes the value at each point in the timeseries and
		 * divides it by the previous point in the series and then takes
		 * the natural log of the result and keeps that value. This is
		 * essentially computing the daily price returns of the data series
		 * and since this is getting used in financial applications this is
		 * a pretty obvious thing to do.
		 */
		bool computeDailyReturns();
		/*
		 * This method simply takes the inverse of each value in the time
		 * series so that y -> 1/y for all points. This is marginally useful
		 * I'm thinking, but I added it here to be a little more complete.
		 */
		bool inverse();
		/*
		 * This method replaces each point in the time series with the
		 * natural logarithm of that point. If the number in the series
		 * is less than or equal to zero, the result is NAN for that
		 * point.
		 */
		bool ln();
		/*
		 * This method replaces each point in the time series with the
		 * natural exponentiation (exp) of that point.
		 */
		bool exp();
		/*
		 * This method replaces each point in the series with the geometric
		 * mean of the data from the passed-in series of time series.
		 * Basically, for each point in the group of series passed in we
		 * take the natural log of each point, compute the sum, count the
		 * number of non-NANs, divide the sum by the count, and exponentiate
		 * the result.
		 */
		bool calculateGeometricMean( const CKVector<CKTimeSeries> & aList );

		/*
		 * This method computes the arithmetic mean (a.k.a. average) for all
		 * the values in the dataset. This is useful if you're interested
		 * in knowing it and don't want to hassle with computing such a
		 * common number yourself.
		 */
		double average();
		/*
		 * These methods are simple numerical attributes of the series
		 * computed each time they are called. They are pretty easy to
		 * understand, with the rms() being the root-mean-square value
		 * of the series and the linf() being the l-infinity norm of the
		 * series.
		 */
		double max();
		double min();
		double rms();
		double linf();

		/*
		 * These are the operator equivalents of the simple mathematical
		 * operations on the time series. They are here as an aid to the
		 * developer of analytic functions based on these time series.
		 */
		CKTimeSeries & operator+=( double anOffset );
		CKTimeSeries & operator+=( CKTimeSeries & aSeries );
		CKTimeSeries & operator+=( const CKTimeSeries & aSeries );
		CKTimeSeries & operator-=( double anOffset );
		CKTimeSeries & operator-=( CKTimeSeries & aSeries );
		CKTimeSeries & operator-=( const CKTimeSeries & aSeries );
		CKTimeSeries & operator*=( double aFactor );
		CKTimeSeries & operator/=( double aDivisor );

		/*
		 * These are the operators for creating new time series data from
		 * one or two existing time series. This is nice in the same vein
		 * as the simpler operators in that it makes writing code for these
		 * data sets a lot easier.
		 */
		friend CKTimeSeries operator+( CKTimeSeries & aSeries, double aValue );
		friend CKTimeSeries operator+( double aValue, CKTimeSeries & aSeries );
		friend CKTimeSeries operator+( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries );

		friend CKTimeSeries operator-( CKTimeSeries & aSeries, double aValue );
		friend CKTimeSeries operator-( double aValue, CKTimeSeries & aSeries );
		friend CKTimeSeries operator-( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries );

		friend CKTimeSeries operator*( CKTimeSeries & aSeries, double aValue );
		friend CKTimeSeries operator*( double aValue, CKTimeSeries & aSeries );

		friend CKTimeSeries operator/( CKTimeSeries & aSeries, double aValue );
		friend CKTimeSeries operator/( double aValue, CKTimeSeries & aSeries );

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
		static double getCurrentTimestamp();
		/*
		 * This method gets the current date as a double of the format
		 * YYYYMMDD which is nice for certain values like the close price
		 * or other daily numbers. This is a conveninence method that calls
		 * the more generic method and then drops off the hours, minutes,
		 * and seconds.
		 */
		static double getCurrentDate();

		/*
		 * This method adds or subtracts the count of days from the given
		 * date and returns the resulting date to the caller. This is nice
		 * because to add days to a date, make the count positive, to
		 * subtract days from a date, make the count negative. The method
		 * respects leap years, etc.
		 */
		static double addDays( double aDate, int aCnt );
		/*
		 * These are convenience methods that call the more general addDays()
		 * method to move a number of days up or back in the calendar. These
		 * are nice in case you have a simple request.
		 */
		static double moveBackDays( double aDate, int aCnt );
		static double moveUpDays( double aDate, int aCnt );
		static double moveBackADay( double aDate );
		static double moveUpADay( double aDate );

		/*
		 * These methods allow the caller to get the very first date in the
		 * series as well as the very last date in the series. This is nice
		 * because we can then determine what date range this series covers
		 * and what to operate on, for instance.
		 */
		double getStartingDate();
		double getStartingDate() const;
		double getEndingDate();
		double getEndingDate() const;

		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the value's data into a (char *) that
		 * can be converted to a Java String and then the Java object can
		 * interpret it and "reconstitue" the object from this coding.
		 */
		virtual CKString generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this value. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const CKString & aCode );
		/*
		 * This method checks to see if the two CKTimeSeries are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKTimeSeries & anOther ) const;
		/*
		 * This method checks to see if the two CKTimeSeries are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKTimeSeries & anOther ) const;
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
		 * This method can be used by the subclasses to get a handle on the
		 * map that is the master data structure for this instance. This is
		 * typically not used in the methods in this class, but if a subclass
		 * wants to get directly at the data, then they can use this.
		 */
		std::map<double, double> *getTimeseries();
		/*
		 * This method is used by the subclasses to get access to the mutex
		 * for controlling the modification to the data. In general, there's
		 * not a great need for this, but on the off-chance that it's
		 * really needed, it's here.
		 */
		CKFWMutex *getTimeseriesMutex();

	private:
		/*
		 * This is the STL map that holds the timeseries. It's organized
		 * by timestamp and value where the timestamp is formatted as
		 * YYYYMMDD.hhmmssss where the seconds are to the hundredth of
		 * a second.
		 */
		std::map<double, double>		mTimeseries;
		/*
		 * This mutex is used to ensure that we're not corrupting the
		 * data as it's being put into the map. This is really important
		 * as we're going to be doing a lot of stuff to the data and
		 * we don't want it shifting out from under us.
		 */
		CKFWMutex						mTimeseriesMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTimeSeries & anItem );

/*
 * These are the operators for creating new time series data from
 * one or two existing time series. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKTimeSeries operator+( CKTimeSeries & aSeries, double aValue );
CKTimeSeries operator+( double aValue, CKTimeSeries & aSeries );
CKTimeSeries operator+( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries );

CKTimeSeries operator-( CKTimeSeries & aSeries, double aValue );
CKTimeSeries operator-( double aValue, CKTimeSeries & aSeries );
CKTimeSeries operator-( CKTimeSeries & aSeries, CKTimeSeries & anotherSeries );

CKTimeSeries operator*( CKTimeSeries & aSeries, double aValue );
CKTimeSeries operator*( double aValue, CKTimeSeries & aSeries );

CKTimeSeries operator/( CKTimeSeries & aSeries, double aValue );
CKTimeSeries operator/( double aValue, CKTimeSeries & aSeries );

#endif	// __CKTIMESERIES_H
