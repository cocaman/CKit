/*
 * CKTimeTable.h - this file defines a time-series map of CKTables organized
 *                 by the date (stored as a long of the form YYYYMMDD) and
 *                 operated on as a single entity. This object will fit
 *                 nicely into the CKVariant scheme and will be able to
 *                 represent a series of tabular results - one per day.
 *
 * $Id: CKTimeTable.h,v 1.3 2005/10/15 16:10:55 drbob Exp $
 */
#ifndef __CKTIMETABLE_H
#define __CKTIMETABLE_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <map>

//	Third-Party Headers

//	Other Headers
#include "CKFWMutex.h"
#include "CKTable.h"
#include "CKTimeSeries.h"
#include "CKString.h"
#include "muParser.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes
/*
 * The core data structure for this class is an STL map where the key is
 * the date and the value is a CKTable. Since STL gets a little bulky when
 * it comes to iterators, etc. let's define a few datatypes that will make
 * at least this code look nice. It'll also help the users of this class as
 * they won't have to worry about data changing if they stick to the
 * typedefs in their code.
 */
typedef std::map<long, CKTable>					CKDateTableMap;

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKTimeTable
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that really doesn't contain
		 * anything. This isn't so bad, as the setters allow you to
		 * populate this guy later with anything that you could want.
		 */
		CKTimeTable();
		/*
		 * This version of the constructor takes the size of each table
		 * that will be created to hold all the data. There needs to be
		 * a singular table size as that's just the way things go, and it
		 * makes most sense, anyway.
		 */
		CKTimeTable( int aRowCount, int aColumnCount );
		/*
		 * This version of the constructor takes the list of row labels
		 * and column headers that will be applied to each table in the
		 * time series, and creates a new object that is ready for storing
		 * this type of data for different dates.
		 */
		CKTimeTable( const CKStringList & aRowLabelList,
					 const CKStringList & aColumnHeaderList );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKTimeTable's generateCodeFromValues() method
		 * and parses it into a time table of values directly. This is very
		 * useful for serializing the time table's data from one host to
		 * another across a socket, for instance.
		 */
		CKTimeTable( const CKString & aCode );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKTimeTable( const CKTimeTable & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKTimeTable();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKTimeTable & operator=( const CKTimeTable & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method is used to set the default size of the tables in
		 * this instance to the given value. This is important because
		 * when a new date is added to the instance, a new table is
		 * created and that table's size is set with the results of this
		 * method. So... typically, this is set in the constructor but
		 * in very unusual cases, you can set this manually. Please be
		 * careful with it though. You can really make a mess of things
		 * if you aren't careful.
		 */
		void setDefaultRowCount( int aCount );
		/*
		 * This method is used to set the default size of the tables in
		 * this instance to the given value. This is important because
		 * when a new date is added to the instance, a new table is
		 * created and that table's size is set with the results of this
		 * method. So... typically, this is set in the constructor but
		 * in very unusual cases, you can set this manually. Please be
		 * careful with it though. You can really make a mess of things
		 * if you aren't careful.
		 */
		void setDefaultColumnCount( int aCount );
		/*
		 * The data in this instance is organized by date, row and column
		 * where for each date there is a CKTable that holds the values.
		 * These methods allow the user to specify the value (a CKVariant)
		 * at any index using either numeric or label-based indexing. The
		 * value argument is not touched and so remains the responsibility
		 * of the caller to dispose of.
		 */
		void setValue( long aDate, int aRow, int aCol, const CKVariant & aValue );
		void setValue( long aDate, int aRow, const CKString & aColHeader, const CKVariant & aValue );
		void setValue( long aDate, const CKString & aRowLabel, int aCol, const CKVariant & aValue );
		void setValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKVariant & aValue );
		/*
		 * This method sets the value at this date, row and column based on
		 * the type of data that's represented in the supplied string. This
		 * string will *not* be modified or retained in any way, as a copy
		 * will be made for this instance, if it's necessary to have one.
		 * This is the most general form of the setter for this instance.
		 */
		void setValueAsType( long aDate, int aRow, int aCol,
							 CKVariantType aType, const char *aValue );
		void setValueAsType( long aDate, int aRow, const CKString & aColHeader,
							 CKVariantType aType, const char *aValue );
		void setValueAsType( long aDate, const CKString & aRowLabel, int aCol,
							 CKVariantType aType, const char *aValue );
		void setValueAsType( long aDate, const CKString & aRowLabel, const CKString & aColHeader,
							 CKVariantType aType, const char *aValue );
		/*
		 * This sets the value stored in this date, row and column as a
		 * string, but a local copy will be made so that the caller doesn't
		 * have to worry about holding on to the parameter, and is free to
		 * delete it.
		 */
		void setStringValue( long aDate, int aRow, int aCol, const char *aStringValue );
		void setStringValue( long aDate, int aRow, const CKString & aColHeader, const char *aStringValue );
		void setStringValue( long aDate, const CKString & aRowLabel, int aCol, const char *aStringValue );
		void setStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const char *aStringValue );
		void setStringValue( long aDate, int aRow, int aCol, const CKString *aStringValue );
		void setStringValue( long aDate, int aRow, const CKString & aColHeader, const CKString *aStringValue );
		void setStringValue( long aDate, const CKString & aRowLabel, int aCol, const CKString *aStringValue );
		void setStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKString *aStringValue );
		/*
		 * This method sets the value stored in this date, row and column
		 * as a date of the form YYYYMMDD - stored as a long.
		 */
		void setDateValue( long aDate, int aRow, int aCol, long aDateValue );
		void setDateValue( long aDate, int aRow, const CKString & aColHeader, long aDateValue );
		void setDateValue( long aDate, const CKString & aRowLabel, int aCol, long aDateValue );
		void setDateValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, long aDateValue );
		/*
		 * This method sets the value stored in this date, row and column
		 * as a double.
		 */
		void setDoubleValue( long aDate, int aRow, int aCol, double aDoubleValue );
		void setDoubleValue( long aDate, int aRow, const CKString & aColHeader, double aDoubleValue );
		void setDoubleValue( long aDate, const CKString & aRowLabel, int aCol, double aDoubleValue );
		void setDoubleValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, double aDoubleValue );
		/*
		 * This sets the value stored in this date, row and column as a
		 * table, but a local copy will be made so that the caller doesn't
		 * have to worry about holding on to the parameter, and is free to
		 * delete it.
		 */
		void setTableValue( long aDate, int aRow, int aCol, const CKTable *aTableValue );
		void setTableValue( long aDate, int aRow, const CKString & aColHeader, const CKTable *aTableValue );
		void setTableValue( long aDate, const CKString & aRowLabel, int aCol, const CKTable *aTableValue );
		void setTableValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKTable *aTableValue );
		/*
		 * This sets the value stored in this date, row and column as a
		 * time series, but a local copy will be made so that the caller
		 * doesn't have to worry about holding on to the parameter, and is
		 * free to delete it. This makes a time-dependent series of time
		 * series data so you have to be careful to keep it straight.
		 */
		void setTimeSeriesValue( long aDate, int aRow, int aCol, const CKTimeSeries *aSeriesValue );
		void setTimeSeriesValue( long aDate, int aRow, const CKString & aColHeader, const CKTimeSeries *aSeriesValue );
		void setTimeSeriesValue( long aDate, const CKString & aRowLabel, int aCol, const CKTimeSeries *aSeriesValue );
		void setTimeSeriesValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKTimeSeries *aSeriesValue );
		/*
		 * This sets the value stored in this date, row and column as a
		 * price, but a local copy will be made so that the caller doesn't
		 * have to worry about holding on to the parameter, and is free to
		 * delete it.
		 */
		void setPriceValue( long aDate, int aRow, int aCol, const CKPrice *aPriceValue );
		void setPriceValue( long aDate, int aRow, const CKString & aColHeader, const CKPrice *aPriceValue );
		void setPriceValue( long aDate, const CKString & aRowLabel, int aCol, const CKPrice *aPriceValue );
		void setPriceValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKPrice *aPriceValue );

		/*
		 * This method returns the size, in rows, of the next table to be
		 * created in response to a new date being added to this instance.
		 * This is important because it needs to be set before the first
		 * date comes in, and that typically means in the constructor.
		 */
		int getDefaultRowCount() const;
		/*
		 * This method returns the size, in columns, of the next table to be
		 * created in response to a new date being added to this instance.
		 * This is important because it needs to be set before the first
		 * date comes in, and that typically means in the constructor.
		 */
		int getDefaultColumnCount() const;
		/*
		 * The data in this instance is organized by date, row and column
		 * where for each date there is a CKTable that holds the values.
		 * These methods allow the user to access any value in the series
		 * of tables by simply providing the date and index values using
		 * either numeric or label-based indexing. The return value is
		 * the reference of the requested value so if you want to use it
		 * later, it's best to make a copy.
		 */
		const CKVariant & getValue( long aDate, int aRow, int aCol );
		const CKVariant & getValue( long aDate, int aRow, const CKString & aColHeader );
		const CKVariant & getValue( long aDate, const CKString & aRowLabel, int aCol );
		const CKVariant & getValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method returns the enumerated type of the data that this
		 * date, row and column is currently holding.
		 */
		CKVariantType getType( long aDate, int aRow, int aCol );
		CKVariantType getType( long aDate, int aRow, const CKString & aColHeader );
		CKVariantType getType( long aDate, const CKString & aRowLabel, int aCol );
		CKVariantType getType( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method will return the integer value of the data stored in this
		 * date, row and column - if the type is numeric. If the data isn't
		 * numeric an exception will be thrown as it's assumed that the user
		 * should make sure that this instance is numeric *before* calling
		 * this method.
		 */
		int getIntValue( long aDate, int aRow, int aCol );
		int getIntValue( long aDate, int aRow, const CKString & aColHeader );
		int getIntValue( long aDate, const CKString & aRowLabel, int aCol );
		int getIntValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method will return the double value of the data stored in this
		 * date, row and column - if the type is numeric. If the data isn't
		 * numeric an exception will be thrown as it's assumed that the user
		 * should make sure that this instance is numeric *before* calling
		 * this method.
		 */
		double getDoubleValue( long aDate, int aRow, int aCol );
		double getDoubleValue( long aDate, int aRow, const CKString & aColHeader );
		double getDoubleValue( long aDate, const CKString & aRowLabel, int aCol );
		double getDoubleValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method will return the date value of the data stored in this
		 * date, row and column in a long of the form YYYYMMDD - if the type
		 * is date. If the data isn't a date an exception will be thrown as
		 * it's assumed that the user should make sure that this instance is
		 * a date *before* calling this method.
		 */
		long getDateValue( long aDate, int aRow, int aCol );
		long getDateValue( long aDate, int aRow, const CKString & aColHeader );
		long getDateValue( long aDate, const CKString & aRowLabel, int aCol );
		long getDateValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method returns the actual string value of the data that
		 * this date, row and column is holding. If the user wants to use
		 * this value outside the scope of this class, then they need to
		 * make a copy.
		 */
		const CKString *getStringValue( long aDate, int aRow, int aCol );
		const CKString *getStringValue( long aDate, int aRow, const CKString & aColHeader );
		const CKString *getStringValue( long aDate, const CKString & aRowLabel, int aCol );
		const CKString *getStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method returns the actual table value of the data that
		 * this date, row and column is holding. If the user wants to use
		 * this value outside the scope of this class, then they need to
		 * make a copy.
		 */
		const CKTable *getTableValue( long aDate, int aRow, int aCol );
		const CKTable *getTableValue( long aDate, int aRow, const CKString & aColHeader );
		const CKTable *getTableValue( long aDate, const CKString & aRowLabel, int aCol );
		const CKTable *getTableValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method returns the actual time series value of the data that
		 * this date, row and column is holding. If the user wants to use
		 * this value outside the scope of this class, then they need to
		 * make a copy.
		 */
		const CKTimeSeries *getTimeSeriesValue( long aDate, int aRow, int aCol );
		const CKTimeSeries *getTimeSeriesValue( long aDate, int aRow, const CKString & aColHeader );
		const CKTimeSeries *getTimeSeriesValue( long aDate, const CKString & aRowLabel, int aCol );
		const CKTimeSeries *getTimeSeriesValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * This method returns the actual price value of the data that
		 * this date, row and column is holding. If the user wants to use
		 * this value outside the scope of this class, then they need to
		 * make a copy.
		 */
		const CKPrice *getPriceValue( long aDate, int aRow, int aCol );
		const CKPrice *getPriceValue( long aDate, int aRow, const CKString & aColHeader );
		const CKPrice *getPriceValue( long aDate, const CKString & aRowLabel, int aCol );
		const CKPrice *getPriceValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader );

		/*
		 * This method returns a pointer to the complete CKTable that this
		 * instance holds for the provided date. This is important because
		 * this method will return NULL if no such date is already in the
		 * response, and if one is desired, try getOrCreateTableForDate().
		 * If the caller wants to do something with this return value they
		 * need to make a copy as this is our original and can't be touched.
		 */
		CKTable *getTableForDate( long aDate );
		CKTable *getTableForDate( long aDate ) const;

		/*
		 * When the user needs to know what dates are in this response, this
		 * method is a nice way to get at those values. Each of the entries
		 * is a long representing a date of the format YYYYMMDD and has a
		 * corresponding CKTable in this instance's data structures.
		 */
		const CKVector<long> getDateValues();
		const CKVector<long> getDateValues() const;

		/*
		 * Each CKTable has the methods getRow() and getColumn() to get
		 * individual rows and columns out of the table for isolated
		 * processing. These methods perform a very similar function for
		 * the date 'dimension' to this response set. Given a row and column
		 * index (either numeric or label-based) these methods will return
		 * a vector of CKVariants that have been copied from the data set
		 * so that they can be worked on by the caller independent of the
		 * rest of the data.
		 */
		CKVector<CKVariant> getDateVector( int aRow, int aCol );
		CKVector<CKVariant> getDateVector( int aRow, const CKString & aColHeader );
		CKVector<CKVariant> getDateVector( const CKString & aRowLabel, int aCol );
		CKVector<CKVariant> getDateVector( const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * In addition to the vector-of-Vatiants format above, the following
		 * methods allow the user to get *numeric* data as a CKTimeSeries.
		 * These time series objects are much easier to work with and can
		 * do advanced things like take place in expressions, etc.
		 */
		CKTimeSeries getTimeSeries( int aRow, int aCol );
		CKTimeSeries getTimeSeries( int aRow, const CKString & aColHeader );
		CKTimeSeries getTimeSeries( const CKString & aRowLabel, int aCol );
		CKTimeSeries getTimeSeries( const CKString & aRowLabel, const CKString & aColHeader );

		/*
		 * This method returns the reference to the vector of default row
		 * labels that will be used for populating all the new CKTables that
		 * are created for each new date in this instance. This is important
		 * because because each of the tables needs to have the same labels
		 * and headers so that the character indexing works as it's supposed
		 * to.
		 */
		const CKStringList & getDefaultRowLabels() const;
		/*
		 * This method returns the reference to the vector of default column
		 * headers that will be used for populating all the new CKTables that
		 * are created for each new date in this instance. This is important
		 * because because each of the tables needs to have the same labels
		 * and headers so that the character indexing works as it's supposed
		 * to.
		 */
		const CKStringList & getDefaultColumnHeaders() const;

		/*
		 * This method is a nice encapsulation method that makes it easy
		 * to add the supplied row label to the vector of default labels
		 * that this instance maintains so that it can be used to properly
		 * configure each new table that gets created for this instance.
		 */
		void addToDefaultRowLabels( const CKString & aRowLabel );
		/*
		 * This method is a nice encapsulation method that makes it easy
		 * to add the supplied column header to the vector of default headers
		 * that this instance maintains so that it can be used to properly
		 * configure each new table that gets created for this instance.
		 */
		void addToDefaultColumnHeaders( const CKString & aColumnHeader );

		/*
		 * This method clears out all the tables for all the dates
		 * so that it's like starting over from scratch with this instance.
		 */
		void clear();
		/*
		 * This method clears out all the existing default row labels
		 * so that you can start with a clean slate.
		 */
		void clearDefaultRowLabels();
		/*
		 * This method clears out all the existing default column headers
		 * so that you can start with a clean slate.
		 */
		void clearDefaultColumnHeaders();

		/********************************************************
		 *
		 *                Simple Math Methods
		 *
		 ********************************************************/
		/*
		 * These methods allow the user to add values to each applicable
		 * element in this time table. In the first case, it's a constant
		 * value but in the second it's a table. In this latter case, the
		 * table's contents are added to each of the tables in this instance
		 * regardless of the date. The third method allows the point-by-point
		 * addition of two complete time tables. The values updated in the
		 * methods are only those that make sense.
		 */
		bool add( double anOffset );
		bool add( CKTable & aTable );
		bool add( const CKTable & aTable );
		bool add( CKTimeTable & anOther );
		bool add( const CKTimeTable & anOther );
		/*
		 * These methods allow the user to subtract values from each applicable
		 * element in this time table. In the first case, it's a constant value
		 * but in the second it's a table that will be used against all the
		 * tables in the time table. The third method allows the point-by-point
		 * subtraction of two complete time tables. The values updated in the
		 * methods are only those that make sense.
		 */
		bool subtract( double anOffset );
		bool subtract( CKTable & aTable );
		bool subtract( const CKTable & aTable );
		bool subtract( CKTimeTable & anOther );
		bool subtract( const CKTimeTable & anOther );
		/*
		 * These method allows the user to multiply a constant value to
		 * all elements in the time table where such an activity would produce
		 * reasonable results. The second form of the method allows for the
		 * element-by-element multiplication of the argument by each table
		 * in the time table. The third form allows a point-by-point product
		 * of two time tables.
		 */
		bool multiply( double aFactor );
		bool multiply( CKTable & aTable );
		bool multiply( const CKTable & aTable );
		bool multiply( CKTimeTable & anOther );
		bool multiply( const CKTimeTable & anOther );
		/*
		 * These method allows the user to divide each element in this
		 * table by a constant value where such an activity would produce
		 * reasonable results. The second form of the method allows for the
		 * element-by-element division of the argument by each table
		 * in the time table. The third form allows for a point-by-point
		 * division of two time tables.
		 */
		bool divide( double aDivisor );
		bool divide( CKTable & aTable );
		bool divide( const CKTable & aTable );
		bool divide( CKTimeTable & anOther );
		bool divide( const CKTimeTable & anOther );
		/*
		 * This method simply takes the inverse of each value in the time
		 * table so that x -> 1/x for all points. This is marginally useful
		 * I'm thinking, but I added it here to be a little more complete.
		 */
		bool inverse();

		/*
		 * These are the operator equivalents of the simple mathematical
		 * operations on the time table. They are here as an aid to the
		 * developer of analytic functions based on these guys.
		 */
		CKTimeTable & operator+=( double anOffset );
		CKTimeTable & operator+=( CKTable & aTable );
		CKTimeTable & operator+=( const CKTable & aTable );
		CKTimeTable & operator+=( CKTimeTable & anOther );
		CKTimeTable & operator+=( const CKTimeTable & anOther );
		CKTimeTable & operator-=( double anOffset );
		CKTimeTable & operator-=( CKTable & aTable );
		CKTimeTable & operator-=( const CKTable & aTable );
		CKTimeTable & operator-=( CKTimeTable & anOther );
		CKTimeTable & operator-=( const CKTimeTable & anOther );
		CKTimeTable & operator*=( double aFactor );
		CKTimeTable & operator/=( double aDivisor );

		/*
		 * These are the operators for creating new table data from
		 * one or two existing time tables. This is nice in the same vein
		 * as the simpler operators in that it makes writing code for these
		 * data sets a lot easier.
		 */
		friend CKTimeTable operator+( CKTimeTable & aTimeTable, double aValue );
		friend CKTimeTable operator+( double aValue, CKTimeTable & aTimeTable );
		friend CKTimeTable operator+( CKTimeTable & aTimeTable, CKTable & aTable );
		friend CKTimeTable operator+( CKTable & aTable, CKTimeTable & aTimeTable );
		friend CKTimeTable operator+( CKTimeTable & aTimeTable, CKTimeTable & anotherTimeTable );

		friend CKTimeTable operator-( CKTimeTable & aTimeTable, double aValue );
		friend CKTimeTable operator-( double aValue, CKTimeTable & aTimeTable );
		friend CKTimeTable operator-( CKTimeTable & aTimeTable, CKTable & aTable );
		friend CKTimeTable operator-( CKTable & aTable, CKTimeTable & aTimeTable );
		friend CKTimeTable operator-( CKTimeTable & aTimeTable, CKTimeTable & anotherTimeTable );

		friend CKTimeTable operator*( CKTimeTable & aTimeTable, double aValue );
		friend CKTimeTable operator*( double aValue, CKTimeTable & aTimeTable );

		friend CKTimeTable operator/( CKTimeTable & aTimeTable, double aValue );
		friend CKTimeTable operator/( double aValue, CKTimeTable & aTimeTable );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method returns a copy of the current value as contained in
		 * a string. This is returned as a CKString as it's easy to use.
		 */
		CKString getValueAsString( long aDate, int aRow, int aCol ) const;
		CKString getValueAsString( long aDate, int aRow, const CKString & aColHeader ) const;
		CKString getValueAsString( long aDate, const CKString & aRowLabel, int aCol ) const;
		CKString getValueAsString( long aDate, const CKString & aRowLabel, const CKString & aColHeader ) const;

		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the point's data into a string that
		 * can be converted to a Java String and then the Java object can
		 * interpret it and "reconstitue" the object from this coding.
		 */
		virtual CKString generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this point. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const CKString & aCode );

		/*
		 * Because there may be times that the user wants to lock us up
		 * for change, we're going to expose this here so it's easy for them
		 * to iterate, for example.
		 */
		void lock();
		void lock() const;
		void unlock();
		void unlock() const;

		/*
		 * This method checks to see if the two CKTimeTables are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator==( const CKTimeTable & anOther ) const;
		/*
		 * This method checks to see if the two CKTimeTables are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKTimeTable & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;
		/*
		 * There are times that it will be nice to be able to write out
		 * this response to a file in a nice, orderly fashion. Also, there
		 * will need to be the same for any subclasses. So, let's make this
		 * method similar to the toString() but it takes a filename and
		 * writes out the response's data to that file in a nice way.
		 */
		virtual bool toFile( const CKString & aFile ) const;
		/*
		 * This method is nice in that it shows the data in the response
		 * as a table where the first column is the date and the next
		 * column is the row labels which are the symbols and the remaining
		 * columns are the data in the request. The column headers are nice
		 * and this makes a very nice way to 'tabularize' the data for
		 * easy viewing.
		 */
		virtual CKString toTableString() const;

	protected:
		friend class MathUtils::Parser;

		/*
		 * This method sets the list of default row labels to be equivalent
		 * to the passed in list. This first clears out the existing list and
		 * then copies in all the elements from the argument into the list
		 * this instance maintains.
		 */
		void setDefaultRowLabels( const CKStringList & aList );
		/*
		 * This method sets the list of default column headers to be equivalent
		 * to the passed in list. This first clears out the existing list and
		 * then copies in all the elements from the argument into the list
		 * this instance maintains.
		 */
		void setDefaultColumnHeaders( const CKStringList & aList );
		/*
		 * This method takes the supplied date (YYYYMMDD) and CKTable reference
		 * and places the table into this instance's data structures at the
		 * proper location for easy retrieval. This is a nice little
		 * encapsulation method that makes dealing with the data structures
		 * a little bit easier.
		 */
		void setTableForDate( long aDate, const CKTable & aTable );

		/*
		 * When you want to get a table for a date regardless of whether
		 * or not one is already there, then call this method and one will
		 * be created and a pointer to it returned to you if one is not
		 * already available. This is very useful in populating data when
		 * not having one is simply not what you want to 'stick' with. If
		 * something very bad happens, this method will return NULL.
		 */
		CKTable *getOrCreateTableForDate( long aDate );

	private:
		/*
		 * This is the map that holds all the tables and we're going to have
		 * the keys be the date and the values be the tables. Since we really
		 * don't want to mess with pointers, let's keep everything in real-
		 * space :)
		 */
		CKDateTableMap		mTables;
		// ...and this is the mutex that protects it
		CKFWMutex			mTablesMutex;
		/*
		 * These are the 'default' sizes of each of the tables in the map.
		 * In reality these are the *exact* sizes of the tables, but the
		 * tables themselves can be resized and so in fact it's possible
		 * that the tables mutate beyond their starting conditions, but
		 * for the most part, these are the size of all the tables so that
		 * when we need to create a new one we have the size right here.
		 */
		int					mDefaultRowCount;
		int					mDefaultColumnCount;
		/*
		 * These are the default row labels and column headers for the
		 * tables in this instance. You see, for each 'new' date I will
		 * need to create a new table of the proper size (see the two
		 * ivars above) and then populate it with the row labels and
		 * column headers so that character-base indexing is going to
		 * work properly. While some may say that the mDefaultRowCount
		 * and mDefaultColumnCount are redundant given these vectors,
		 * I choose to think of it as added flexibility.
		 */
		CKStringList		mDefaultRowLabels;
		CKStringList		mDefaultColumnHeaders;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTimeTable & aResponse );

#endif	// __CKTIMETABLE_H
