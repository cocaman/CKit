/*
 * CKTimeSeries.h - this file defines a class that can be used to represent a
 *                  time-series data stream. Basically, a double is used to
 *                  represent the date/time as YYYYMMDD.HHMMSShh with the
 *                  time resolution down to the hundredth of a second, and
 *                  a double is used to represent the data value. This is a
 *                  nice little class that is used in the CKVariant as yet
 *                  another form of data that that class can represent.
 *
 * $Id: CKTimeSeries.h,v 1.10 2004/09/20 16:19:54 drbob Exp $
 */
#ifndef __CKTIMESERIES_H
#define __CKTIMESERIES_H

//	System Headers
#include <vector>
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
 * there's a problem with the definition of isnan(). So... to make it
 * easier on all parties, I'm simply going to repeat the definition
 * that's in Linux and Darwin here, and it should get picked up even
 * if the headers fail us.
 */
#ifdef __MACH__
#ifndef isnan
#define	isnan(x)	((sizeof(x) == sizeof(double)) ? __isnand(x) : \
					(sizeof(x) == sizeof(float)) ? __isnanf(x) : __isnan(x))
#endif
#endif

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKString.h"

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
		CKTimeSeries( const std::vector<long> & aDateSeries,
					  const std::vector<double> & aValueSeries );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKTimeSeries's generateCodeFromValues()
		 * method and parses it into a time series of values directly.
		 * This is very useful for serializing the table's data from one
		 * host to another across a socket, for instance.
		 */
		CKTimeSeries( const char *aCode );
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
		void put( const std::vector<double> & aDateSeries,
				  const std::vector<double> & aValueSeries );
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
		std::vector<double> get( const std::vector<double> & aDateSeries );
		/*
		 * This method takes today's date and marches back in time the
		 * provided number of days to arrive at the value to return.
		 * This is nice in that 0 will get the latest value and 1 will
		 * get the value yesterday, and n will get you the value n days
		 * ago.
		 */
		double getDaysBack( int aDayCnt );

		/*
		 * This method gets the complete series of dates for the current
		 * timeseries but as just the day (YYYYMMDD) portion of the date.
		 * This is useful if the data is daily data anyway and you're
		 * interested in feeding this data into another system that
		 * functions on the integer portion of the date and therefore
		 * doesn't allow for complete timestamps.
		 */
		std::vector<long> getDates();
		/*
		 * This method gets the complete series of timestamps for the
		 * current timeseries. This is useful if you're interesting in
		 * knowing the time of each data point.
		 */
		std::vector<double> getDateTimes();

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
		std::vector<double> interpolate( const std::vector<double> & aDateSeries );
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
		/*
		 * These methods allow the user to subtract values from this
		 * timeseries, in the first case, it's a constant value but
		 * in the second it's another timeseries. This last version is useful
		 * when you have two series of the same dates and you wish to
		 * difference them point by point.
		 */
		bool subtract( double anOffset );
		bool subtract( CKTimeSeries & aSeries );
		/*
		 * These method allows the user to multiply a constant value to
		 * all data points in this timeseries.
		 */
		bool multiply( double aFactor );
		/*
		 * These method allows the user to divide each point in the
		 * timeseries by a constant value.
		 */
		bool divide( double aDivisor );

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
		CKTimeSeries & operator-=( double anOffset );
		CKTimeSeries & operator-=( CKTimeSeries & aSeries );
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
		double getEndingDate();

		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the value's data into a (char *) that
		 * can be converted to a Java String and then the Java object can
		 * interpret it and "reconstitue" the object from this coding.
		 *
		 * This method returns a character array that the caller is
		 * responsible for calling 'delete []' on. This is useful as these
		 * codes are used outside the scope of this class and so a copy
		 * is far more useful.
		 */
		virtual char *generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this value. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const char *aCode );
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
