/*
 * CKTable.h - this file defines a general two-dimensional table class
 *             that holds rows and columns of CKVariant objects - which, of
 *             course, can be CKTable objects themselves. So this object
 *             really allows us to have a very general table structure of
 *             objects and manipulate them very easily.
 *
 * $Id: CKTable.h,v 1.8 2004/09/11 02:15:23 drbob Exp $
 */
#ifndef __CKTABLE_H
#define __CKTABLE_H

//	System Headers
#include <vector>
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
#include "CKVariant.h"

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
		CKTable( const std::vector<std::string> aRowLabels,
				 const std::vector<std::string> aColumnHeaders );
		/*
		 * This constructor is interesting in that it takes the data as
		 * it comes from another CKTable's generateCodeFromValues() method
		 * and parses it into a table of values directly. This is very
		 * useful for serializing the table's data from one host to
		 * another across a socket, for instance.
		 */
		CKTable( const char *aCode );
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
		void setValue( int aRow, const std::string & aColHeader, const CKVariant & aValue );
		void setValue( const std::string & aRowLabel, int aCol, const CKVariant & aValue );
		void setValue( const std::string & aRowLabel, const std::string & aColHeader, const CKVariant & aValue );
		/*
		 * This method sets the value at this location based on the type
		 * of data that's represented in the supplied string. This string
		 * will *not* be modified or retained in any way, as a copy will be
		 * made for this instance, if it's necessary to have one. This is
		 * the most general form of the setter for this instance.
		 */
		void setValueAsType( int aRow, int aCol, CKVariantType aType,
				const char *aValue );
		void setValueAsType( int aRow, const std::string & aColHeader,
				CKVariantType aType, const char *aValue );
		void setValueAsType( const std::string & aRowLabel, int aCol,
				CKVariantType aType, const char *aValue );
		void setValueAsType( const std::string & aRowLabel, const std::string & aColHeader,
				CKVariantType aType, const char *aValue );
		/*
		 * This sets the value stored in this location as a string, but a local
		 * copy will be made so that the caller doesn't have to worry about
		 * holding on to the parameter, and is free to delete it.
		 */
		void setStringValue( int aRow, int aCol, const char *aStringValue );
		void setStringValue( int aRow, const std::string & aColHeader, const char *aStringValue );
		void setStringValue( const std::string & aRowLabel, int aCol, const char *aStringValue );
		void setStringValue( const std::string & aRowLabel, const std::string & aColHeader, const char *aStringValue );
		/*
		 * This method sets the value stored in this location as a date of the
		 * form YYYYMMDD - stored as a long.
		 */
		void setDateValue( int aRow, int aCol, long aDateValue );
		void setDateValue( int aRow, const std::string & aColHeader, long aDateValue );
		void setDateValue( const std::string & aRowLabel, int aCol, long aDateValue );
		void setDateValue( const std::string & aRowLabel, const std::string & aColHeader, long aDateValue );
		/*
		 * This method sets the value stored in this location as a double.
		 */
		void setDoubleValue( int aRow, int aCol, double aDoubleValue );
		void setDoubleValue( int aRow, const std::string & aColHeader, double aDoubleValue );
		void setDoubleValue( const std::string & aRowLabel, int aCol, double aDoubleValue );
		void setDoubleValue( const std::string & aRowLabel, const std::string & aColHeader, double aDoubleValue );
		/*
		 * This sets the value stored in this location as a table, but a local
		 * copy will be made so that the caller doesn't have to worry about
		 * holding on to the parameter, and is free to delete it.
		 */
		void setTableValue( int aRow, int aCol, const CKTable *aTableValue );
		void setTableValue( int aRow, const std::string & aColHeader, const CKTable *aTableValue );
		void setTableValue( const std::string & aRowLabel, int aCol, const CKTable *aTableValue );
		void setTableValue( const std::string & aRowLabel, const std::string & aColHeader, const CKTable *aTableValue );
		/*
		 * This sets the value stored in this location as a time series, but
		 * a local copy will be made so that the caller doesn't have to worry
		 * about holding on to the parameter, and is free to delete it.
		 */
		void setTimeSeriesValue( int aRow, int aCol, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( int aRow, const std::string & aColHeader, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( const std::string & aRowLabel, int aCol, const CKTimeSeries *aTimeSeriesValue );
		void setTimeSeriesValue( const std::string & aRowLabel, const std::string & aColHeader, const CKTimeSeries *aTimeSeriesValue );
		/*
		 * This method takes the supplied column number and the header and
		 * assuming the table is big enough to include that column, sets the
		 * column header for that column to the supplied value. A copy is
		 * made of the argument so the caller retains control of the memory.
		 */
		void setColumnHeader( int aCol, const std::string & aHeader );
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
		void setRowLabel( int aRow, const std::string & aLabel );
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
		CKVariant & getValue( int aRow, const std::string & aColHeader ) const;
		CKVariant & getValue( const std::string & aRowLabel, int aCol ) const;
		CKVariant & getValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method returns the enumerated type of the data that this
		 * location is currently holding.
		 */
		CKVariantType getType( int aRow, int aCol ) const;
		CKVariantType getType( int aRow, const std::string & aColHeader ) const;
		CKVariantType getType( const std::string & aRowLabel, int aCol ) const;
		CKVariantType getType( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method will return the integer value of the data stored in this
		 * location - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		int getIntValue( int aRow, int aCol ) const;
		int getIntValue( int aRow, const std::string & aColHeader ) const;
		int getIntValue( const std::string & aRowLabel, int aCol ) const;
		int getIntValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method will return the double value of the data stored in this
		 * location - if the type is numeric. If the data isn't numeric an
		 * exception will be thrown as it's assumed that the user should make
		 * sure that this instance is numeric *before* calling this method.
		 */
		double getDoubleValue( int aRow, int aCol ) const;
		double getDoubleValue( int aRow, const std::string & aColHeader ) const;
		double getDoubleValue( const std::string & aRowLabel, int aCol ) const;
		double getDoubleValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method will return the date value of the data stored in this
		 * location in a long of the form YYYYMMDD - if the type is date. If
		 * the data isn't date an exception will be thrown as it's assumed
		 * that the user should make sure that this instance is date *before*
		 * calling this method.
		 */
		long getDateValue( int aRow, int aCol ) const;
		long getDateValue( int aRow, const std::string & aColHeader ) const;
		long getDateValue( const std::string & aRowLabel, int aCol ) const;
		long getDateValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method returns the actual string value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy,
		 * or call the getValueAsString() method that returns a copy.
		 */
		const char *getStringValue( int aRow, int aCol ) const;
		const char *getStringValue( int aRow, const std::string & aColHeader ) const;
		const char *getStringValue( const std::string & aRowLabel, int aCol ) const;
		const char *getStringValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method returns the actual table value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTable *getTableValue( int aRow, int aCol ) const;
		const CKTable *getTableValue( int aRow, const std::string & aColHeader ) const;
		const CKTable *getTableValue( const std::string & aRowLabel, int aCol ) const;
		const CKTable *getTableValue( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * This method returns the actual time series value of the data that
		 * this location is holding. If the user wants to use this value
		 * outside the scope of this class, then they need to make a copy.
		 */
		const CKTimeSeries *getTimeSeriesValue( int aRow, int aCol ) const;
		const CKTimeSeries *getTimeSeriesValue( int aRow, const std::string & aColHeader ) const;
		const CKTimeSeries *getTimeSeriesValue( const std::string & aRowLabel, int aCol ) const;
		const CKTimeSeries *getTimeSeriesValue( const std::string & aRowLabel, const std::string & aColHeader ) const;

		/*
		 * This method returns the actual std::string value that is the
		 * column header for the provided column number assuming that it
		 * actually exists in the table. As such, if the user wants this
		 * value outside the scope fo this class they need to make a copy
		 * of it.
		 */
		const std::string & getColumnHeader( int aCol ) const;
		/*
		 * This method returns the actual std::string value that is the
		 * row label for the provided row number assuming that it
		 * actually exists in the table. As such, if the user wants this
		 * value outside the scope fo this class they need to make a copy
		 * of it.
		 */
		const std::string & getRowLabel( int aRow ) const;

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
		int getColumnForHeader( const std::string & aHeader ) const;
		/*
		 * This method returns the row index for the specified label.
		 * If this label is not a valid row label for this table, then
		 * this method will return a -1, please check for it.
		 */
		int getRowForLabel( const std::string & aLabel ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied row in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		std::vector<CKVariant> getRow( int aRow ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied row label in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		std::vector<CKVariant> getRow( const std::string & aRowLabel ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied column in the table. This is nice if you
		 * want to take a 'slice' in the table and just return a vector
		 * of data for processing. The management of the returned value
		 * is up to the caller.
		 */
		std::vector<CKVariant> getColumn( int aCol ) const;
		/*
		 * This method returns a complete vector of the CKVariants that
		 * make up the supplied column headerin the table. This is nice
		 * if you want to take a 'slice' in the table and just return a
		 * vector of data for processing. The management of the returned
		 * value is up to the caller.
		 */
		std::vector<CKVariant> getColumn( const std::string & aColumnHeader ) const;

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
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method returns a copy of the current value as contained in
		 * a string and it is the responsibility of the caller to call
		 * 'delete []' on the results. It's also possible that this method
		 * will return NULL, so you have better check the return value
		 * before assuming anything.
		 */
		char *getValueAsString( int aRow, int aCol ) const;
		char *getValueAsString( int aRow, const std::string & aColHeader ) const;
		char *getValueAsString( const std::string & aRowLabel, int aCol ) const;
		char *getValueAsString( const std::string & aRowLabel, const std::string & aColHeader ) const;
		/*
		 * In order to simplify the move of this object from C++ to Java
		 * it makes sense to encode the table's data into a (char *) that
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
		 * that are needed to populate this table. The argument is left
		 * untouched, and is the responsible of the caller to free.
		 */
		virtual void takeValuesFromCode( const char *aCode );
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
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this table. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString() const;

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
		 * This method sets an array of std::string values to be the column
		 * headers for the current table. It's important to note that there
		 * needs to be getNumColumns() of them and it won't be checked, so...
		 * this class will take care of making sure they are there, and it's
		 * probably best to let this class to this.
		 */
		void setColumnHeaders( const std::vector<std::string> & aList );
		/*
		 * This method sets an array of std::string values to be the row
		 * labels for the current table. It's important to note that there
		 * needs to be getNumRows() of them and it won't be checked, so...
		 * this class will take care of making sure they are there, and it's
		 * probably best to let this class to this.
		 */
		void setRowLabels( const std::vector<std::string> & aList );
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
		 * This method returns the actual array of std::string values that
		 * are the column headers for this table. The number of elements
		 * in the array is given my getNumColumns() and if the user wants
		 * to have this list outside the scope of this class they need to
		 * make a copy of it and that includes all the strings in it.
		 */
		const std::string *getColumnHeaders() const;
		/*
		 * This method returns the actual array of std::string values that
		 * are the row labels for this table. The number of elements
		 * in the array is given my getNumRows() and if the user wants
		 * to have this list outside the scope of this class they need to
		 * make a copy of it and that includes all the strings in it.
		 */
		const std::string *getRowLabels() const;

		/********************************************************
		 *
		 *               Text/String Parsing Methods
		 *
		 ********************************************************/
		/*
		 * This method looks at the character buffer and parses out the
		 * integer value from the start of the buffer to the first instance
		 * of the character 'delim' and then returns that value. The
		 * buffer contents itself is untouched.
		 *
		 * On exit, the argument 'buff' will be moved to one character
		 * PAST the delimiter so that it's ready for another call to this
		 * method, if needed.
		 */
		static int parseIntFromBufferToDelim( char * & aBuff, char aDelim );
		/*
		 * This method looks at the character buffer and parses out the
		 * hexadecimal integer value from the start of the buffer to the
		 * first instance of the character 'delim' and then returns that
		 * value. The buffer contents itself is untouched.
		 *
		 * On exit, the argument 'buff' will be moved to one character
		 * PAST the delimiter so that it's ready for another call to this
		 * method, if needed.
		 */
		static int parseHexIntFromBufferToDelim( char * & aBuff, char aDelim );
		/*
		 * This method looks at the character buffer and parses out the
		 * long integer value from the start of the buffer to the first
		 * instance of the character 'delim' and then returns that value.
		 * The buffer contents itself is untouched.
		 *
		 * On exit, the argument 'buff' will be moved to one character
		 * PAST the delimiter so that it's ready for another call to this
		 * method, if needed.
		 */
		static long parseLongFromBufferToDelim( char * & aBuff, char aDelim );
		/*
		 * This method looks at the character buffer and parses out the
		 * double value from the start of the buffer to the first
		 * instance of the character 'delim' and then returns that value.
		 * The buffer contents itself is untouched.
		 *
		 * On exit, the argument 'buff' will be moved to one character
		 * PAST the delimiter so that it's ready for another call to this
		 * method, if needed.
		 */
		static double parseDoubleFromBufferToDelim( char * & aBuff, char aDelim );
		/*
		 * This method looks at the character buffer and parses out the
		 * charater string value from the start of the buffer to the first
		 * instance of the character 'delim' and then returns *a copy* of
		 * that value. The buffer contents itself is untouched, and calling
		 * 'delete []' on the returned value is the responsibility of the
		 * caller of this method.
		 *
		 * On exit, the argument 'buff' will be moved to one character
		 * PAST the delimiter so that it's ready for another call to this
		 * method, if needed.
		 */
		static char *parseStringFromBufferToDelim( char * & aBuff, char aDelim );
		/*
		 * This method is used in the creation of the encoded strings that
		 * assist in the translation of the objects from the C++ to Java
		 * environments. Basically, the (char *) buffer that's passed in
		 * contains delimiters that are '\x01' but that need to be changed
		 * to a printable ASCII character. This method scans the entire
		 * string for the presence of delimiters and then selects one
		 * character that's not used in the string and replaces all the
		 * "placeholder" delimiters with this value and returns. If it was
		 * impossible to find a delimiter, this method will return false
		 * otherwise it will return true.
		 */
		static bool chooseAndApplyDelimiter( char *aBuff );

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
		CKVariant		*mTable;
		/*
		 * This is a array of std::string values that are the column
		 * headers. The reason for picking the std::string is that it allows
		 * us a lot of freedom in the assignment, sizing and memory
		 * management of the strings and it's just as easy to get the
		 * C-string equivalent from them when using the encoding/decoding.
		 */
		std::string		*mColumnHeaders;
		/*
		 * This is a array of std::string values that are the row
		 * labels. The reason for picking the std::string is that it allows
		 * us a lot of freedom in the assignment, sizing and memory
		 * management of the strings and it's just as easy to get the
		 * C-string equivalent from them when using the encoding/decoding.
		 */
		std::string		*mRowLabels;
		/*
		 * This is the current number of rows expressed in the table's
		 * data structure. It has been used in the creation of the data
		 * table, above, and if the value is -1, then this indicates that
		 * no table has been defined.
		 */
		int				mNumRows;
		/*
		 * This is the current number of columns expressed in the table's
		 * data structure. It has been used in the creation of the data
		 * table, above, and if the value is -1, then this indicates that
		 * no table has been defined.
		 */
		int				mNumColumns;

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
		void createTable( const std::vector<std::string> & aRowLabels,
						  const std::vector<std::string> & aColHeaders );
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

#endif	// __CKTABLE_H
