/*
 * CKTimeSeries.h - this file defines a class that can be used to represent a
 *                  time-series data stream. Basically, a double is used to
 *                  represent the date/time as YYYYMMDD.HHMMSShh with the
 *                  time resolution down to the hundredth of a second, and
 *                  a double is used to represent the data value. This is a
 *                  nice little class that is used in the CKVariant as yet
 *                  another form of data that that class can represent.
 *
 * $Id: CKTimeSeries.h,v 1.2 2004/02/27 00:32:35 drbob Exp $
 */
#ifndef __CKTIMESERIES_H
#define __CKTIMESERIES_H

//	System Headers
#include <string>
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

//	Forward Declarations

//	Public Constants

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
		 * first divides it by the first point in the series and then takes
		 * the natural log of the result and keeps that value. This is
		 * essentially computing the price returns of the data series and
		 * since this is getting used in financial applications this is a
		 * pretty obvious thing to do.
		 */
		bool computeReturns();
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
		 * a std::string.
		 */
		virtual std::string toString() const;

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
