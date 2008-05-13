/*
 * CKTable.h - this file defines a general two-dimensional table class
 *             that holds rows and columns of CKVariant objects - which, of
 *             course, can be CKTable objects themselves. So this object
 *             really allows us to have a very general table structure of
 *             objects and manipulate them very easily.
 *
 * $Id: CKTable.h,v 1.20 2008/05/13 20:11:33 drbob Exp $
 */
#ifndef __CKTABLE_H
#define __CKTABLE_H

//	System Headers
#include <map>
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
#include "CKVariant.h"
#include "CKString.h"
#include "CKVector.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKTable {
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * The default constructor does *not* define the table's data
		 * structure, but a call to resizeTable(int, int) will do that
		 * once the instance is built. This constructor is here for
		 * those times that you aren't sure what you're going to be
		 * getting into and need to leave your options open.
		 */
		CKTable();
		/*
		 * The preferred constructor for this class takes the number of
		 * rows and columns and creates a table of values that can then
		 * be individually accessed by the accessor methods of this
		 * class.
		 */
		CKTable( int aNumRows, int aNumColumns );
		/*
		 * This version of the constructor for this class takes two lists -
		 * one for the list of row labels and the other for the list of
		 * column headers. These lists not only define the structure of the
		 * table, but also the row labels and column headers.
		 */
		CKTable( const CKStringList aRowLabels,
				 const CKStringList aColumnHeaders );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKTable's generateCodeFromValues() method
		 * and parses it into a table of values directly. This is very
		 * useful for serializing the table's data from one host to
		 * another across a socket, for instance.
		 */
		CKTable( const CKString & aCode );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKTable( const CKTable & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKTable();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKTable & operator=( const CKTable & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method allows the user to set the *contents* of a given value
		 * to the location specified in the table. It's important to note that
		 * the actual object is *not* being placed into the table. Only it's
		 * contents are being examined/copied into the table's datastructures.
		 */
		void setValue( int aRow, int aCol, const CKVariant & aValue );
		void setValue( int aRow, const CKString & aColHeader, const CKVariant & aValue );
		void setValue( const CKString & aRowLabel, int aCol, const CKVariant & aValue );
		void setValue( const CKString & aRowLabel, const CKString & aColHeader, const CKVariant & aValue );
		/*
		 * This method sets the value at this location based on the type
		 * of data that's represented in the supplied string. This string
		 * will *not* be modified or retained in any way, as a copy will be
		 * made for this instance, if it's necessary to have one. This is
		 * the most general form of the setter for this instance.
		 */
		void setValueAsType( int aRow, int aCol, CKVariantType aType,
				const char *aValue );
		void setValueAsType( int aRow, const CKString & aColHeader,
				CKVariantType aType, const char *aValue );
		void setValueAsType( const CKString & aRowLabel, int aCol,
				CKVariantType aType, const char *aValue );
		void setValueAsType( const CKString & aRowLabel, const CKString & aColHeader,
				CKVariantType aType, const char *aValue );
		/*
		 * This sets the value stored in this location as a string, but a local
		 * copy will be made so that the caller doesn't have to worry about
		 * holding on to the parameter, and is free to delete it.
		 */
		void setStringValue( int aRow, int aCol, const char *aStringValue );
		void setStringValue( int aRow, const CKString & aColHeader, const char *aStringValue );
		void setStringValue( const CKString & aRowLabel, int aCol, const char *aStringValue );
		void setStringValue( const CKString & aRowLabel, const CKString & aColHeader, const char *aStringValue );

		void setStringValue( int aRow, int aCol, const CKString *aStringValue );
		void setStringValue( int aRow, const CKString & aColHeader, const CKString *aStringValue );
		void setStringValue( const CKString & aRowLabel, int aCol, const CKString *aStringValue );
		void setStringValue( const CKString & aRowLabel, const CKString & aColHeader, const CKString *aStringValue );
		/*
		 * This method sets the value stored in this location as a date of the
		 * form YYYYMMDD - stored as a long.
		 */
		void setDateValue( int aRow, int aCol, long aDateValue );
		void setDateValue( int aRow, const CKString & aColHeader, long aDateValue );
		void setDateValue( const CKString & aRowLabel, int aCol, long aDateValue );
		void setDateValue( const CKString & aRowLabel, const CKString & aColHeader, long aDateValue );
		/*
		 * This method sets the value stored in this location as a double.
		 */
		void setDoubleValue( int aRow, int aCol, double aDoubleValue );
		void setDoubleValue( int aRow, const CKString & aColHeader, double aDoubleValue );
		void setDoubleValue( const CKString & aRowLabel, int aCol, double aDoubleValue );
		void setDoubleValue( const CKString & aRowLabel, const CKString & aColHeader, double aDoubleValue );
		/*
		 * This sets the value stored in this location as a table, but a local
		 * copy will be made so that the caller doesn't have to worry about
		 * holding on to the parameter, and is free to delete it.
		 */
		void setTableValue( int aRow, int aCol, const CKTable *aTableValue );
		void setTableValue( int aRow, const CKString & aColHeader, const CKTable *aTableValue );
		void setTableValue( const CKString & aRowLabel, int aCol, const CKTable *aTableValue );
		void setTableValue( const CKString & aRowLabel, const CKString & aColHeader, const CKTable *aTableValue );
		/*
		 * This sets the value stored in this location as a time series, but
		 * a local copy will be made so that the caller doesn't have to worry
		 * about holding on to the parameter, and is free to delete it.
		 */
		void setTimeSeriesValue( int aRow, int aCol, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( int aRow, const CKString & aColHeader, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( const CKString & aRowLabel, int aCol, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( const CKString & aRowLabel, const CKString & aColHeader, const CKTimeSeries *aTimeSeriesValue );
		/*
		 * This sets the value stored in this location as a price, but
		 * a local copy will be made so that the caller doesn't have to worry
		 * about holding on to the parameter, and is free to delete it.
		 */
		void setPriceValue( int aRow, int aCol, const CKPrice *aPriceValue );
		void setPriceValue( int aRow, const CKString & aColHeader, const CKPrice *aPriceValue );
		void setPriceValue( const CKString & aRowLabel, int aCol, const CKPrice *aPriceValue );
		void setPriceValue( const CKString & aRowLabel, const CKString & aColHeader, const CKPrice *aPriceValue );
		/*
		 * This method takes the supplied column number and the header and
		 * assuming the table is big enough to include that column, sets the
		 * column header for that column to the supplied value. A copy is
		 * made of the argument so the caller retains control of the memory.
		 */
		void setColumnHeader( int aCol, const CKString & aHeader );
		/*
		 * This method takes the supplied column number and the header and
		 * assuming the table is big enough to include that column, sets the
		 * column header for that column to the supplied value. The header
		 * will be left untouched as we'll be making a copy of it for our
		 * internal use.
		 */
		void setColumnHeader( int aCol, const char *aHeader );
		/*
		 * This method takes the supplied row number and the label and
		 * assuming the table is big enough to include that row, sets the
		 * row label for that row to the supplied value. A copy is
		 * made of the argument so the caller retains control of the memory.
		 */
		void setRowLabel( int aRow, const CKString & aLabel );
		/*
		 * This method takes the supplied row number and the label and
		 * assuming the table is big enough to include that row, sets the
		 * row label for that row to the supplied value. The label
		 * will be left untouched as we'll be making a copy of it for our
		 * internal use.
		 */
		void setRowLabel( int aRow, const char *aLabel );

		/*
		 * This method returns a reference to the *actual* element in the
		 * table's data structure and so the caller should be *VERY* careful
		 * what he does with it. It's nice for scanning through the data and
		 * seeing what's there, but the caller cannot delete it as it's
		 * controlled by this table's methods.
		 */
		CKVariant & getValue( int aRow, int aCol ) const;
		CKVariant & getValue( int aRow, const CKString & aColHeader ) const;
		CKVariant & getValue( const CKString & aRowLabel, int aCol ) const;
		CKVariant & getValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method returns the enumerated type of the data that this
		 * location is currently holding.
		 */
		CKVariantType getType( int aRow, int aCol ) const;
		CKVariantType getType( int aRow, const CKString & aColHeader ) const;
		CKVariantType getType( const CKString & aRowLabel, int aCol ) const;
		CKVariantType getType( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method will return the integer value of the data stored in this
		 * location - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		int getIntValue( int aRow, int aCol ) const;
		int getIntValue( int aRow, const CKString & aColHeader ) const;
		int getIntValue( const CKString & aRowLabel, int aCol ) const;
		int getIntValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method will return the double value of the data stored in this
		 * location - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		double getDoubleValue( int aRow, int aCol ) const;
		double getDoubleValue( int aRow, const CKString & aColHeader ) const;
		double getDoubleValue( const CKString & aRowLabel, int aCol ) const;
		double getDoubleValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method will return the date value of the data stored in this
		 * location in a long of the form YYYYMMDD - if the type is date. If
		 * the data isn't date an exception will be thrown as it's assumed
		 * that the user should make sure that this instance is date *before*
		 * calling this method.
		 */
		long getDateValue( int aRow, int aCol ) const;
		long getDateValue( int aRow, const CKString & aColHeader ) const;
		long getDateValue( const CKString & aRowLabel, int aCol ) const;
		long getDateValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method returns the actual string value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy,
		 * or call the getValueAsString() method that returns a copy.
		 */
		const CKString *getStringValue( int aRow, int aCol ) const;
		const CKString *getStringValue( int aRow, const CKString & aColHeader ) const;
		const CKString *getStringValue( const CKString & aRowLabel, int aCol ) const;
		const CKString *getStringValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method returns the actual table value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTable *getTableValue( int aRow, int aCol ) const;
		const CKTable *getTableValue( int aRow, const CKString & aColHeader ) const;
		const CKTable *getTableValue( const CKString & aRowLabel, int aCol ) const;
		const CKTable *getTableValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method returns the actual time series value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTimeSeries *getTimeSeriesValue( int aRow, int aCol ) const;
		const CKTimeSeries *getTimeSeriesValue( int aRow, const CKString & aColHeader ) const;
		const CKTimeSeries *getTimeSeriesValue( const CKString & aRowLabel, int aCol ) const;
		const CKTimeSeries *getTimeSeriesValue( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * This method returns the actual price value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKPrice *getPriceValue( int aRow, int aCol ) const;
		const CKPrice *getPriceValue( int aRow, const CKString & aColHeader ) const;
		const CKPrice *getPriceValue( const CKString & aRowLabel, int aCol ) const;
		const CKPrice *getPriceValue( const CKString & aRowLabel, const CKString & aColHeader ) const;

		/*
		 * This method returns the actual CKString value that is the
		 * column header for the provided column number assuming that it
		 * actually exists in the table. As such, if the user wants this
		 * value outside the scope fo this class they need to make a copy
		 * of it.
		 */
		const CKString & getColumnHeader( int aCol ) const;
		/*
		 * This method returns the actual CKString value that is the
		 * row label for the provided row number assuming that it
		 * actually exists in the table. As such, if the user wants this
		 * value outside the scope fo this class they need to make a copy
		 * of it.
		 */
		const CKString & getRowLabel( int aRow ) const;

		/*
		 * This method will return the current number of rows in the table.
		 * If no table is defined at this time, this method will return a -1.
		 */
		int getNumRows() const;
		/*
		 * This method will return the current number of columns in the
		 * table. If no table is defined at this time, this method will
		 * return a -1.
		 */
		int getNumColumns() const;

		/********************************************************
		 *
		 *                Table Methods
		 *
		 ********************************************************/
		/*
		 * This method returns the column index for the specified header.
		 * If this header is not a valid column header for this table, then
		 * this method will return a -1, please check for it.
		 */
		int getColumnForHeader( const CKString & aHeader ) const;
		/*
		 * This method returns the row index for the specified label.
		 * If this label is not a valid row label for this table, then
		 * this method will return a -1, please check for it.
		 */
		int getRowForLabel( const CKString & aLabel ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied row in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		CKVector<CKVariant> getRow( int aRow ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied row label in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		CKVector<CKVariant> getRow( const CKString & aRowLabel ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied column in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		CKVector<CKVariant> getColumn( int aCol ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied column headerin the table. This is nice
		 * if you want to take a 'slice' in the table and just return a
		 * vector of data for processing. The management of the returned
		 * value is up to the caller.
		 */
		CKVector<CKVariant> getColumn( const CKString & aColumnHeader ) const;

		/********************************************************
		 *
		 *            Table Manipulation Methods
		 *
		 ********************************************************/
		/*
		 * This method allows the user to merge two tables into one larger
		 * table by adding the rows and columns from the argument to the
		 * main instance itself. This is very useful when you have several
		 * tables that need to be combined into one table that share
		 * either a common set of column headers and/or a common set of
		 * row labels.
		 */
		bool merge( const CKTable & aTable );

		/********************************************************
		 *
		 *                Simple Math Methods
		 *
		 ********************************************************/
		/*
		 * These methods allow the user to add values to each applicable
		 * element in this table. In the first case, it's a constant value
		 * but in the second it's another table. The values updated in the
		 * methods are only those that make sense.
		 */
		bool add( double anOffset );
		bool add( CKTable & anOther );
		bool add( const CKTable & anOther );
		/*
		 * These methods allow the user to subtract values from each applicable
		 * element in this table. In the first case, it's a constant value
		 * but in the second it's another table. The values updated in the
		 * methods are only those that make sense.
		 */
		bool subtract( double anOffset );
		bool subtract( CKTable & anOther );
		bool subtract( const CKTable & anOther );
		/*
		 * These method allows the user to multiply a constant value to
		 * all elements in the table where such an activity would produce
		 * reasonable results. The second form of the method allows for the
		 * element-by-element multiplication of the tables.
		 */
		bool multiply( double aFactor );
		bool multiply( CKTable & anOther );
		bool multiply( const CKTable & anOther );
		/*
		 * These method allows the user to divide each element in this
		 * table by a constant value where such an activity would produce
		 * reasonable results. The second form of the method allows for the
		 * element-by-element division of the tables.
		 */
		bool divide( double aDivisor );
		bool divide( CKTable & anOther );
		bool divide( const CKTable & anOther );
		/*
		 * This method simply takes the inverse of each value in the table
		 * so that x -> 1/x for all points. This is marginally useful
		 * I'm thinking, but I added it here to be a little more complete.
		 */
		bool inverse();

		/*
		 * These are the operator equivalents of the simple mathematical
		 * operations on the table. They are here as an aid to the
		 * developer of analytic functions based on these guys.
		 */
		CKTable & operator+=( double anOffset );
		CKTable & operator+=( CKTable & aTable );
		CKTable & operator+=( const CKTable & aTable );
		CKTable & operator-=( double anOffset );
		CKTable & operator-=( CKTable & aTable );
		CKTable & operator-=( const CKTable & aTable );
		CKTable & operator*=( double aFactor );
		CKTable & operator/=( double aDivisor );

		/*
		 * These are the operators for creating new table data from
		 * one or two existing tables. This is nice in the same vein
		 * as the simpler operators in that it makes writing code for these
		 * data sets a lot easier.
		 */
		friend CKTable operator+( CKTable & aTable, double aValue );
		friend CKTable operator+( double aValue, CKTable & aTable );
		friend CKTable operator+( CKTable & aTable, CKTable & anotherTable );

		friend CKTable operator-( CKTable & aTable, double aValue );
		friend CKTable operator-( double aValue, CKTable & aTable );
		friend CKTable operator-( CKTable & aTable, CKTable & anotherTable );

		friend CKTable operator*( CKTable & aTable, double aValue );
		friend CKTable operator*( double aValue, CKTable & aTable );

		friend CKTable operator/( CKTable & aTable, double aValue );
		friend CKTable operator/( double aValue, CKTable & aTable );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method returns a copy of the current value as contained in
		 * a string. This is returned as a CKString as it's easy to use.
		 */
		CKString getValueAsString( int aRow, int aCol ) const;
		CKString getValueAsString( int aRow, const CKString & aColHeader ) const;
		CKString getValueAsString( const CKString & aRowLabel, int aCol ) const;
		CKString getValueAsString( const CKString & aRowLabel, const CKString & aColHeader ) const;
		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the table's data into a (char *) that
		 * can be converted to a Java String and then the Java object can
		 * interpret it and "reconstitue" the object from this coding.
		 */
		virtual CKString generateCodeFromValues() const;
		/*
		 * This method takes a code that could have been written with the
		 * generateCodeFromValues() method on either the C++ or Java
		 * versions of this class and extracts all the values from the code
		 * that are needed to populate this table. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const CKString & aCode );
		/*
		 * When this table needs to be resized, a call to this method will
		 * do the trick. It's important to note that all the data that can
		 * survive the change will survive the change. This means that if the
		 * resize is such that the new table is bigger then all the data will
		 * be preserved, but if the new dimensions are smaller than the
		 * current ones then data will be lost.
		 */
		void resizeTable( int aNumRows, int aNumColumns );
		/*
		 * This method checks to see if the two CKTables are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKTable & anOther ) const;
		/*
		 * This method checks to see if the two CKTables are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this
		 * method returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKTable & anOther ) const;
		/*
		 * To make it a little easier on the user of this table, I have
		 * created these operators so that elements in the table can be
		 * referenced like simply indexes: tbl(0,5) - for both the RHS
		 * and LHS of the equation. This requires the CKVariant to handle
		 * a lot of the work, but that's not horrible, really.
		 */
		CKVariant & operator()( int aRow, int aCol );
		CKVariant & operator()( int aRow, const CKString & aColHeader );
		CKVariant & operator()( const CKString & aRowLabel, int aCol );
		CKVariant & operator()( const CKString & aRowLabel, const CKString & aColHeader );
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this table. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

		/********************************************************
		 *
		 *               Text/String Parsing Methods
		 *
		 ********************************************************/
		/*
		 * This method is used in the creation of the encoded strings that
		 * assist in the translation of the objects from the C++ to Java
		 * environments. Basically, the CKString buffer that's passed in
		 * contains delimiters that are '\x01' but that need to be changed
		 * to a printable ASCII character. This method scans the entire
		 * string for the presence of delimiters and then selects one
		 * character that's not used in the string and replaces all the
		 * "placeholder" delimiters with this value and returns. If it was
		 * impossible to find a delimiter, this method will return false
		 * otherwise it will return true.
		 */
		static bool chooseAndApplyDelimiter( CKString & aBuff );

	protected:
		friend class CKTimeSeries;
		friend class CKVariant;

		/*
		 * This method sets the datastructure of the array of arrays of
		 * pointers of CKVariants to the proper location in this class
		 * instance so that it's safe to use. This is not something most
		 * users need to use, so it's protected, but because we like to
		 * stick with setters and getters, this is here for this class'
		 * code. This method assumes that the passed in variable was
		 * created for this class. No copy is made, but control of the
		 * deallocation of this guy will now become the job of this
		 * instance and the caller is absolved of any responsibility there.
		 */
		void setTable( CKVariant *aTable );
		/*
		 * This method sets an array of CKString values to be the column
		 * headers for the current table. It's important to note that there
		 * needs to be getNumColumns() of them and it won't be checked, so...
		 * this class will take care of making sure they are there, and it's
		 * probably best to let this class to this.
		 */
		void setColumnHeaders( const CKStringList & aList );
		/*
		 * This method sets an array of CKString values to be the row
		 * labels for the current table. It's important to note that there
		 * needs to be getNumRows() of them and it won't be checked, so...
		 * this class will take care of making sure they are there, and it's
		 * probably best to let this class to this.
		 */
		void setRowLabels( const CKStringList & aList );
		/*
		 * This method is really here to encapsulate the number of rows
		 * that this data source represents. Since it's possible to have
		 * the data and these sizes not match, special care should be
		 * taken to ensure that the data is consistent.
		 */
		void setNumRows( int aCount );
		/*
		 * This method is really here to encapsulate the number of columns
		 * that this data source represents. Since it's possible to have
		 * the data and these sizes not match, special care should be
		 * taken to ensure that the data is consistent.
		 */
		void setNumColumns( int aCount );
		/*
		 * This method returns the array of CKVariants that comprises the
		 * table's row-major data storage so that it can be used in this
		 * class. In general, this is a dangerous thing, and it needs to
		 * be used carefully, but it's important as we're trying to stick
		 * to using just the setters and getters. This method returns a
		 * pointer to the actual data and sould therefore be used very
		 * carefully as it could change underneath the caller if they aren't
		 * careful.
		 */
		CKVariant *getTable() const;
		/*
		 * This method returns the actual array of CKString values that
		 * are the column headers for this table. The number of elements
		 * in the array is given my getNumColumns() and if the user wants
		 * to have this list outside the scope of this class they need to
		 * make a copy of it and that includes all the strings in it.
		 */
		const CKString *getColumnHeaders() const;
		/*
		 * This method returns the actual array of CKString values that
		 * are the row labels for this table. The number of elements
		 * in the array is given my getNumRows() and if the user wants
		 * to have this list outside the scope of this class they need to
		 * make a copy of it and that includes all the strings in it.
		 */
		const CKString *getRowLabels() const;

	private:
		/*
		 * This is the pointer to a row-major storage of the data in the
		 * table. The index of any element in the table (i,j) is simply
		 * [i*mNumColumns + j] where i and j start indexing at zero.
		 * All the allocation/deallocation is handled in the private
		 * methods so there's no real reason for a user to have to worry
		 * about how all these things are created. It's all encapsulated
		 * pretty nicely.
		 */
		CKVariant					*mTable;
		/*
		 * This is a array of CKString values that are the column
		 * headers. The reason for picking the CKString is that it allows
		 * us a lot of freedom in the assignment, sizing and memory
		 * management of the strings and it's just as easy to get the
		 * C-string equivalent from them when using the encoding/decoding.
		 */
		CKString					*mColumnHeaders;
		/*
		 * This is the 'index' of the column headers so that it will be
		 * very fast converting a column header into it's numerical column
		 * position. This is populated at the same time as the mColumnHeaders
		 * so it stays in sync at all times.
		 */
		std::map<CKString, int>		mColumnHeadersIndex;
		/*
		 * This is a array of CKString values that are the row
		 * labels. The reason for picking the CKString is that it allows
		 * us a lot of freedom in the assignment, sizing and memory
		 * management of the strings and it's just as easy to get the
		 * C-string equivalent from them when using the encoding/decoding.
		 */
		CKString					*mRowLabels;
		/*
		 * This is the 'index' of the row labels so that it will be
		 * very fast converting a row label into it's numerical row
		 * position. This is populated at the same time as the mRowLabels
		 * so it stays in sync at all times.
		 */
		std::map<CKString, int>		mRowLabelsIndex;
		/*
		 * This is the current number of rows expressed in the table's
		 * data structure. It has been used in the creation of the data
		 * table, above, and if the value is -1, then this indicates that
		 * no table has been defined.
		 */
		int							mNumRows;
		/*
		 * This is the current number of columns expressed in the table's
		 * data structure. It has been used in the creation of the data
		 * table, above, and if the value is -1, then this indicates that
		 * no table has been defined.
		 */
		int							mNumColumns;

		/********************************************************
		 *
		 *                Private Allocation Methods
		 *
		 ********************************************************/
		/*
		 * This private method takes care of dealing with the intelligent
		 * allocation of the table's data. It's not all that complex, but
		 * it's nice to have it in one place that's insulated from all the
		 * other methods in this class. This method will throw an exception
		 * if the number of rows and/or columns make no sense, or if there's
		 * an error in the allocation of the storage.
		 */
		void createTable( int aNumRows, int aNumColumns );
		/*
		 * This private method takes care of dealing with the intelligent
		 * allocation of the table's data. It's not all that complex, but
		 * it's nice to have it in one place that's insulated from all the
		 * other methods in this class. This method will throw an exception
		 * if the number of row labels or column headers make no sense, or
		 * if there's an error in the allocation of the storage.
		 */
		void createTable( const CKStringList & aRowLabels,
						  const CKStringList & aColHeaders );
		/*
		 * This private method takes care of dealing with the careful
		 * deallocation of the table's data structure. It's not all
		 * that complex, but it's nice to have it in one place that's
		 * insulated from all the other methods in this class.
		 */
		void dropTable();
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTable & aTable );

/*
 * These are the operators for creating new table data from
 * one or two existing tables. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKTable operator+( CKTable & aTable, double aValue );
CKTable operator+( double aValue, CKTable & aTable );
CKTable operator+( CKTable & aTable, CKTable & anotherTable );

CKTable operator-( CKTable & aTable, double aValue );
CKTable operator-( double aValue, CKTable & aTable );
CKTable operator-( CKTable & aTable, CKTable & anotherTable );

CKTable operator*( CKTable & aTable, double aValue );
CKTable operator*( double aValue, CKTable & aTable );

CKTable operator/( CKTable & aTable, double aValue );
CKTable operator/( double aValue, CKTable & aTable );

#endif	// __CKTABLE_H
