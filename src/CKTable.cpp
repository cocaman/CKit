/*
 * CKTable.cpp - this file defines a general two-dimensional table class
 *               that holds rows and columns of CKVariant objects - which, of
 *               course, can be CKTable objects themselves. So this object
 *               really allows us to have a very general table structure of
 *               objects and manipulate them very easily.
 *
 * $Id: CKTable.cpp,v 1.3 2004/02/27 00:32:34 drbob Exp $
 */

//	System Headers
#include <string>
#include <iostream>
#include <sstream>

//	Third-Party Headers
#include <CKException.h>

//	Other Headers
#include "CKTable.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants
/*
 * Because MIN and MAX seems to have been dropped from the math.h header
 * we need to make sure that we have them. How very odd that it's not
 * included.
 */
#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif



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
CKTable::CKTable() :
	mTable(NULL),
	mColumnHeaders(),
	mRowLabels(),
	mNumRows(-1),
	mNumColumns(-1)
{
}


/*
 * The preferred constructor for this class takes the number of
 * rows and columns and creates a table of values that can then
 * be individually accessed by the accessor methods of this
 * class.
 */
CKTable::CKTable( int aNumRows, int aNumColumns ) :
	mTable(NULL),
	mColumnHeaders(),
	mRowLabels(),
	mNumRows(-1),
	mNumColumns(-1)
{
	// see if the requestde size makes any sense
	if ((aNumRows <= 0) || (aNumColumns <= 0)) {
		std::ostringstream	msg;
		msg << "CKTable::CKTable(int, int) - the requested size: "
			<< aNumRows << " by " << aNumColumns << " doesn't make any sense. "
			"Please try again.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// create the data table structure
	createTable(aNumRows, aNumColumns);
}


/*
 * This constructor is interesting in that it takes the data as
 * it comes from another CKTable's generateCodeFromValues() method
 * and parses it into a table of values directly. This is very
 * useful for serializing the table's data from one host to
 * another across a socket, for instance.
 */
CKTable::CKTable( const char *aCode ) :
	mTable(NULL),
	mColumnHeaders(),
	mRowLabels(),
	mNumRows(-1),
	mNumColumns(-1)
{
	// first, make sure we have something to do
	if (aCode == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::CKTable(const char *) - the provided argument is "
			"NULL and that means that nothing can be done. Please make sure "
			"that the argument is not NULL before calling this constructor.";
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
CKTable::CKTable( const CKTable & anOther ) :
	mTable(NULL),
	mColumnHeaders(),
	mRowLabels(),
	mNumRows(-1),
	mNumColumns(-1)
{
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKTable::~CKTable()
{
	// drop the table, if we have one
	if (mTable != NULL) {
		dropTable();
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTable & CKTable::operator=( const CKTable & anOther )
{
	// first, let's drop any table that we might already have
	if (mTable != NULL) {
		dropTable();
	}

	// now see if the requested size makes any sense to copy
	if ((anOther.mNumRows > 0) && (anOther.mNumColumns > 0)) {
		// create the data table structure
		createTable(anOther.mNumRows, anOther.mNumColumns);

		// now we need to copy all the row labels over
		mRowLabels = anOther.mRowLabels;

		// now we need to copy all the column headers over
		mColumnHeaders = anOther.mColumnHeaders;

		// finally we need to copy all the values from the table to us
		for (int i = 0; i < anOther.mNumRows; i++) {
			for (int j = 0; j < anOther.mNumColumns; j++) {
				setValue(i, j, anOther.getValue(i, j));
			}
		}
	}

	return *this;
}


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
void CKTable::setValue( int aRow, int aCol, const CKVariant & aValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(int, int, const CKVariant &) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(int, int, const CKVariant &) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	getValue(aRow, aCol) = aValue;
}


void CKTable::setValue( int aRow, const std::string & aColHeader, const CKVariant & aValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(int, const std::string &, const CKVariant &)"
			" - there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValue(aRow, col, aValue);
}


void CKTable::setValue( const std::string & aRowLabel, int aCol, const CKVariant & aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const std::string &, int, const CKVariant &)"
			" - there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValue(row, aCol, aValue);
}


void CKTable::setValue( const std::string & aRowLabel, const std::string & aColHeader, const CKVariant & aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const std::string &, const std::string &, "
			"const CKVariant &) - there is no currently defined row label '" <<
			aRowLabel << "' please make sure the row labels are properly "
			"defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const std::string &, const std::string &, "
			"const CKVariant &) - there is no currently defined column header '"
			<< aColHeader << "' please make sure the column headers are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValue(row, col, aValue);
}


/*
 * This method sets the value at this location based on the type
 * of data that's represented in the supplied string. This string
 * will *not* be modified or retained in any way, as a copy will be
 * made for this instance, if it's necessary to have one. This is
 * the most general form of the setter for this instance.
 */
void CKTable::setValueAsType( int aRow, int aCol, CKVariantType aType,
		const char *aValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(int, int, CKVariantType, "
			"const char *) - the provided location: " << aRow << ", " << aCol <<
			" lies outside the currently defined table: " << mNumRows <<
			" by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(int, int, CKVariantType, "
			"const char *) - there is no currently defined table structure in "
			"this class. This is a serious data integrity problem that needs "
			"to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow][aCol]->setValueAsType(aType, aValue);
}


void CKTable::setValueAsType( int aRow, const std::string & aColHeader,
		CKVariantType aType, const char *aValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(int, const std::string &, "
			"CKVariantType, const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValueAsType(aRow, col, aType, aValue);
}


void CKTable::setValueAsType( const std::string & aRowLabel, int aCol,
		CKVariantType aType, const char *aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const std::string &, int, "
			"CKVariantType, const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValueAsType(row, aCol, aType, aValue);
}


void CKTable::setValueAsType( const std::string & aRowLabel, const std::string & aColHeader,
		CKVariantType aType, const char *aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const std::string &, const std::string &, "
			"CKVariantType, const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const std::string &, const std::string &, "
			"CKVariantType, const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValueAsType(row, col, aType, aValue);
}


/*
 * This sets the value stored in this location as a string, but a local
 * copy will be made so that the caller doesn't have to worry about
 * holding on to the parameter, and is free to delete it.
 */
void CKTable::setStringValue( int aRow, int aCol, const char *aStringValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, int, const char *) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, int, const char *) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow][aCol]->setStringValue(aStringValue);
}


void CKTable::setStringValue( int aRow, const std::string & aColHeader, const char *aStringValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, const std::string &, "
			"const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(aRow, col, aStringValue);
}


void CKTable::setStringValue( const std::string & aRowLabel, int aCol, const char *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const std::string &, int, "
			"const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(row, aCol, aStringValue);
}


void CKTable::setStringValue( const std::string & aRowLabel, const std::string & aColHeader, const char *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const std::string &, const std::string &, "
			"const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const std::string &, const std::string &, "
			"const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(row, col, aStringValue);
}


/*
 * This method sets the value stored in this location as a date of the
 * form YYYYMMDD - stored as a long.
 */
void CKTable::setDateValue( int aRow, int aCol, long aDateValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(int, int, long) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(int, int, long) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow][aCol]->setDateValue(aDateValue);
}


void CKTable::setDateValue( int aRow, const std::string & aColHeader, long aDateValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(int, const std::string &, "
			"long) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDateValue(aRow, col, aDateValue);
}


void CKTable::setDateValue( const std::string & aRowLabel, int aCol, long aDateValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const std::string &, int, "
			"long) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDateValue(row, aCol, aDateValue);
}


void CKTable::setDateValue( const std::string & aRowLabel, const std::string & aColHeader, long aDateValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const std::string &, const std::string &, "
			"long) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const std::string &, const std::string &, "
			"long) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDateValue(row, col, aDateValue);
}


/*
 * This method sets the value stored in this location as a double.
 */
void CKTable::setDoubleValue( int aRow, int aCol, double aDoubleValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(int, int, long) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(int, int, long) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow][aCol]->setDoubleValue(aDoubleValue);
}


void CKTable::setDoubleValue( int aRow, const std::string & aColHeader, double aDoubleValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(int, const std::string &, "
			"double) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDoubleValue(aRow, col, aDoubleValue);
}


void CKTable::setDoubleValue( const std::string & aRowLabel, int aCol, double aDoubleValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const std::string &, int, "
			"double) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDoubleValue(row, aCol, aDoubleValue);
}


void CKTable::setDoubleValue( const std::string & aRowLabel, const std::string & aColHeader, double aDoubleValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const std::string &, const std::string &, "
			"double) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const std::string &, const std::string &, "
			"double) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDoubleValue(row, col, aDoubleValue);
}


/*
 * This sets the value stored in this location as a table, but a local
 * copy will be made so that the caller doesn't have to worry about
 * holding on to the parameter, and is free to delete it.
 */
void CKTable::setTableValue( int aRow, int aCol, const CKTable *aTableValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(int, int, const CKTable *) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(int, int, const CKTable *) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow][aCol]->setTableValue(aTableValue);
}


void CKTable::setTableValue( int aRow, const std::string & aColHeader, const CKTable *aTableValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(int, const std::string &, "
			"const CKTable *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(aRow, col, aTableValue);
}


void CKTable::setTableValue( const std::string & aRowLabel, int aCol, const CKTable *aTableValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const std::string &, int, "
			"const CKTable *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(row, aCol, aTableValue);
}


void CKTable::setTableValue( const std::string & aRowLabel, const std::string & aColHeader, const CKTable *aTableValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const std::string &, const std::string &, "
			"const CKTable *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const std::string &, const std::string &, "
			"const CKTable *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(row, col, aTableValue);
}


/*
 * This method takes the supplied column number and the header and
 * assuming the table is big enough to include that column, sets the
 * column header for that column to the supplied value. A copy is
 * made of the argument so the caller retains control of the memory.
 */
void CKTable::setColumnHeader( int aCol, const std::string & aHeader )
{
	setColumnHeader(aCol, aHeader.c_str());
}


/*
 * This method takes the supplied column number and the header and
 * assuming the table is big enough to include that column, sets the
 * column header for that column to the supplied value. The header
 * will be left untouched as we'll be making a copy of it for our
 * internal use.
 */
void CKTable::setColumnHeader( int aCol, const char *aHeader )
{
	// first, make sure we have a place to put this data
	if ((aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setColumnHeader(int, const char *) - the "
			"provided column: " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mColumnHeaders[aCol] = aHeader;
}


/*
 * This method takes the supplied row number and the label and
 * assuming the table is big enough to include that row, sets the
 * row label for that row to the supplied value. A copy is
 * made of the argument so the caller retains control of the memory.
 */
void CKTable::setRowLabel( int aRow, const std::string & aLabel )
{
	setRowLabel(aRow, aLabel.c_str());
}


/*
 * This method takes the supplied row number and the label and
 * assuming the table is big enough to include that row, sets the
 * row label for that row to the supplied value. The label
 * will be left untouched as we'll be making a copy of it for our
 * internal use.
 */
void CKTable::setRowLabel( int aRow, const char *aLabel )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows)) {
		std::ostringstream	msg;
		msg << "CKTable::setRowLabel(int, const char *) - the "
			"provided row: " << aRow << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mRowLabels[aRow] = aLabel;
}


/*
 * This method returns a reference to the *actual* element in the
 * table's data structure and so the caller should be *VERY* careful
 * what he does with it. It's nice for scanning through the data and
 * seeing what's there, but the caller cannot delete it as it's
 * controlled by this table's methods.
 */
CKVariant & CKTable::getValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(int, int) - the provided location: " <<
			aRow << ", " << aCol << " lies outside the currently defined table: " <<
			mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they want
	return *(mTable[aRow][aCol]);
}


CKVariant & CKTable::getValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValue(aRow, col);
}


CKVariant & CKTable::getValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValue(row, aCol);
}


CKVariant & CKTable::getValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValue(row, col);
}


/*
 * This method returns the enumerated type of the data that this
 * location is currently holding.
 */
CKVariantType CKTable::getType( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getType(int, int) - the provided location: " <<
			aRow << ", " << aCol << " lies outside the currently defined table: " <<
			mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getType(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getType();
}


CKVariantType CKTable::getType( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getType(aRow, col);
}


CKVariantType CKTable::getType( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getType(row, aCol);
}


CKVariantType CKTable::getType( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getType(row, col);
}


/*
 * This method will return the integer value of the data stored in this
 * location - if the type is numeric. If the data isn't numeric an
 * exception will be thrown as it's assumed that the user should make
 * sure that this instance is numeric *before* calling this method.
 */
int CKTable::getIntValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a numeric "
			"value: " << mTable[aRow][aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getIntValue();
}


int CKTable::getIntValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getIntValue(aRow, col);
}


int CKTable::getIntValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getIntValue(row, aCol);
}


int CKTable::getIntValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getIntValue(row, col);
}


/*
 * This method will return the double value of the data stored in this
 * location - if the type is numeric. If the data isn't numeric an
 * exception will be thrown as it's assumed that the user should make
 * sure that this instance is numeric *before* calling this method.
 */
double CKTable::getDoubleValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a numeric "
			"value: " << mTable[aRow][aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getDoubleValue();
}


double CKTable::getDoubleValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDoubleValue(aRow, col);
}


double CKTable::getDoubleValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDoubleValue(row, aCol);
}


double CKTable::getDoubleValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDoubleValue(row, col);
}


/*
 * This method will return the date value of the data stored in this
 * location in a long of the form YYYYMMDD - if the type is date. If
 * the data isn't date an exception will be thrown as it's assumed
 * that the user should make sure that this instance is date *before*
 * calling this method.
 */
long CKTable::getDateValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eDateVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a date "
			"value: " << mTable[aRow][aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getDateValue();
}


long CKTable::getDateValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDateValue(aRow, col);
}


long CKTable::getDateValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDateValue(row, aCol);
}


long CKTable::getDateValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDateValue(row, col);
}


/*
 * This method returns the actual string value of the data that
 * this location is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy,
 * or call the getValueAsString() method that returns a copy.
 */
const char *CKTable::getStringValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eStringVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a string "
			"value: " << mTable[aRow][aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getStringValue();
}


const char *CKTable::getStringValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getStringValue(aRow, col);
}


const char *CKTable::getStringValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getStringValue(row, aCol);
}


const char *CKTable::getStringValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getStringValue(row, col);
}


/*
 * This method returns the actual table value of the data that
 * this location is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKTable *CKTable::getTableValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eTableVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a table "
			"value: " << getValue(aRow, aCol);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getTableValue();
}


const CKTable *CKTable::getTableValue( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(aRow, col);
}


const CKTable *CKTable::getTableValue( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(row, aCol);
}


const CKTable *CKTable::getTableValue( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(row, col);
}


/*
 * This method returns the actual std::string value that is the
 * column header for the provided column number assuming that it
 * actually exists in the table. As such, if the user wants this
 * value outside the scope fo this class they need to make a copy
 * of it.
 */
const std::string & CKTable::getColumnHeader( int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aCol < 0) || (aCol >= mNumColumns) ||
		((unsigned int)aCol >= mColumnHeaders.size())) {
		std::ostringstream	msg;
		msg << "CKTable::getColumnHeader(int) - the provided column "
			"number: " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns <<
			" with " << mColumnHeaders.size() << " column headers.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mColumnHeaders[aCol];
}


/*
 * This method returns the actual std::string value that is the
 * row label for the provided row number assuming that it
 * actually exists in the table. As such, if the user wants this
 * value outside the scope fo this class they need to make a copy
 * of it.
 */
const std::string & CKTable::getRowLabel( int aRow ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		((unsigned int)aRow >= mRowLabels.size())) {
		std::ostringstream	msg;
		msg << "CKTable::getRowLabel(int) - the provided row "
			"number: " << aRow << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns <<
			" with " << mRowLabels.size() << " row labels";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mRowLabels[aRow];
}


/*
 * This method will return the current number of rows in the table.
 * If no table is defined at this time, this method will return a -1.
 */
int CKTable::getNumRows() const
{
	return mNumRows;
}


/*
 * This method will return the current number of columns in the
 * table. If no table is defined at this time, this method will
 * return a -1.
 */
int CKTable::getNumColumns() const
{
	return mNumColumns;
}


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
int CKTable::getColumnForHeader( const std::string & aHeader ) const
{
	int		retval = -1;

	// scan the array for the argument
	for ( unsigned int i = 0; i < mColumnHeaders.size(); ++i ) {
		if (mColumnHeaders[i] == aHeader) {
			retval = i;
			break;
		}
	}

	return retval;
}


/*
 * This method returns the row index for the specified label.
 * If this label is not a valid row label for this table, then
 * this method will return a -1, please check for it.
 */
int CKTable::getRowForLabel( const std::string & aLabel ) const
{
	int		retval = -1;

	// scan the array for the argument
	for ( unsigned int i = 0; i < mRowLabels.size(); ++i ) {
		if (mRowLabels[i] == aLabel) {
			retval = i;
			break;
		}
	}

	return retval;
}


/*
 * This method returns a complete vector of the CKVariants that
 * make up the supplied row in the table. This is nice if you
 * want to take a 'slice' in the table and just return a vector
 * of data for processing. The management of the returned value
 * is up to the caller.
 */
std::vector<CKVariant> CKTable::getRow( int aRow ) const
{
	// first, make sure we have data to grab
	if ((aRow < 0) || (aRow >= mNumRows)) {
		std::ostringstream	msg;
		msg << "CKTable::getRow(int) - the provided row: " << aRow <<
			" lies outside the currently defined table: " << mNumRows <<
			" by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getRow(int) - there is no currently defined table "
			"structure in this class. This is a serious data integrity problem "
			"that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// make the return value first on the stack as it's easiest
	std::vector<CKVariant>		retval;

	// now pick put the data and copy it to the returned vector
	for (int i = 0; i < mNumColumns; ++i) {
		retval.push_back( *(mTable[aRow][i]) );
	}

	return retval;
}


/*
 * This method returns a complete vector of the CKVariants that
 * make up the supplied row label in the table. This is nice if you
 * want to take a 'slice' in the table and just return a vector
 * of data for processing. The management of the returned value
 * is up to the caller.
 */
std::vector<CKVariant> CKTable::getRow( const std::string & aRowLabel ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getRow(const std::string &) - there is no currently "
			"defined row label '" << aRowLabel << "' please make sure the row "
			"labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getRow(row);
}


/*
 * This method returns a complete vector of the CKVariants that
 * make up the supplied column in the table. This is nice if you
 * want to take a 'slice' in the table and just return a vector
 * of data for processing. The management of the returned value
 * is up to the caller.
 */
std::vector<CKVariant> CKTable::getColumn( int aCol ) const
{
	// first, make sure we have data to grab
	if ((aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getColumn(int) - the provided column: " << aCol <<
			" lies outside the currently defined table: " << mNumRows <<
			" by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getColumn(int) - there is no currently defined table "
			"structure in this class. This is a serious data integrity problem "
			"that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// make the return value first on the stack as it's easiest
	std::vector<CKVariant>		retval;

	// now pick put the data and copy it to the returned vector
	for (int i = 0; i < mNumRows; ++i) {
		retval.push_back( *(mTable[i][aCol]) );
	}

	return retval;
}


/*
 * This method returns a complete vector of the CKVariants that
 * make up the supplied column headerin the table. This is nice
 * if you want to take a 'slice' in the table and just return a
 * vector of data for processing. The management of the returned
 * value is up to the caller.
 */
std::vector<CKVariant> CKTable::getColumn( const std::string & aColumnHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColumnHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getCol(const std::string &) - there is no currently "
			"defined column header '" << aColumnHeader << "' please make sure "
			"the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getColumn(col);
}


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
char *CKTable::getValueAsString( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(int, int) - there is no "
			"currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow][aCol]->getValueAsString();
}


char *CKTable::getValueAsString( int aRow, const std::string & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(int, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValueAsString(aRow, col);
}


char *CKTable::getValueAsString( const std::string & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const std::string &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValueAsString(row, aCol);
}


char *CKTable::getValueAsString( const std::string & aRowLabel, const std::string & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const std::string &, const std::string &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const std::string &, const std::string &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValueAsString(row, col);
}


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
char *CKTable::generateCodeFromValues() const
{
	/*
	 * OK... this is interesting because I happen to like the idea
	 * that Bloomberg had for delimiting the data - have the first
	 * character be the delimiter for the entire string. The trick
	 * is that we'll generate the terminated string with a placeholder
	 * and then see what character we can place in the string as the
	 * delimiter that's from our list of accecptable values. It's
	 * interesting and pretty fun.
	 */

	// start by getting a buffer to build up this value
	std::ostringstream buff;

	// first, send out the row and column counts
	buff << "\x01" << mNumRows << "\x01" << mNumColumns << "\x01";

	// next, loop over all the column headers and write them out as well
	for (int j = 0; j < mNumColumns; ++j) {
		buff << mColumnHeaders[j] << "\x01";
	}

	// next, loop over all the row labels and write them out as well
	for (int j = 0; j < mNumRows; ++j) {
		buff << mRowLabels[j] << "\x01";
	}

	// now loop over the data and write it all out in an easy manner
	for (int i = 0; i < mNumRows; ++i)  {
		for (int j = 0; j < mNumColumns; ++j) {
			buff << mTable[i][j]->generateCodeFromValues() << "\x01";
		}
	}

	// now create a new buffer to hold all this
	int		len = buff.str().length();
	char	*retval = new char[len + 1];
	if (retval == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTable::generateCodeFromValues"
			"() - the space to hold the codified representation of this "
			"table could not be created. This is a serious allocation "
			"error.");
	} else {
		// copy over the string's contents
		strncpy(retval, buff.str().c_str(), len);
		// ...and make sure to NULL terminate it
		retval[len] = '\0';
	}

	/*
	 * OK, it's now in a simple character array that we can scan to check
	 * for acceptable delimiter values. What we'll do is to check the string
	 * for the existence of a series of possible delimiters, and as soon as
	 * we find one that's not used in the string we'll use that guy.
	 */
	if (retval != NULL) {
		if (!chooseAndApplyDelimiter(retval)) {
			// free up the space I had created
			delete [] retval;
			retval = NULL;
			// and throw the exception
			throw CKException(__FILE__, __LINE__, "CKTable::generateCodeFrom"
				"Values() - while trying to find an acceptable delimiter for "
				"the data in the table we ran out of possibles before finding "
				"one that wasn't being used in the text of the code. This is "
				"a serious problem that the developers need to look into.");
		}
	}

	return retval;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this table. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKTable::takeValuesFromCode( const char *aCode )
{
	// first, see if we have anything to do
	if (aCode == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTable::takeValuesFromCode("
			"const char *) - the passed-in code is NULL which means that "
			"there's nothing I can do. Please make sure that the argument is "
			"not NULL before calling this method.");
	}

	/*
	 * The data is character-delimited and the delimiter is
	 * the first character of the field data. All subsequent
	 * values will be delimited by this character. We need to
	 * get it.
	 */
	char	delim = aCode[0];
	// ...and start the scanner just after the delimiter
	char	*scanner = (char *)&(aCode[1]);

	/*
	 * Next thing is the row count and then the columnn count.
	 * Get them right off...
	 */
	int rowCnt = parseIntFromBufferToDelim(scanner, delim);
	int colCnt = parseIntFromBufferToDelim(scanner, delim);
	// also, create the table for this size
	createTable( rowCnt, colCnt );

	/*
	 * Next, we need to read off the column headers that we need to
	 * apply to this newly constructed table
	 */
	for (int j = 0; j < colCnt; j++) {
		char	*value = parseStringFromBufferToDelim(scanner, delim);
		if (value == NULL) {
			throw CKException(__FILE__, __LINE__, "CKTable::takeValues"
				"FromCode(const char *) - while trying to read the column "
				"header, a NULL was read. This is a serious problem in the "
				"code.");
		} else {
			// set the header
			setColumnHeader(j, value);
			// ...and drop the parsed string as we no longer need it
			delete [] value;
			value = NULL;
		}
	}

	/*
	 * Next, we need to read off the row labels that we need to
	 * apply to this newly constructed table
	 */
	for (int i = 0; i < rowCnt; i++) {
		char	*value = parseStringFromBufferToDelim(scanner, delim);
		if (value == NULL) {
			throw CKException(__FILE__, __LINE__, "CKTable::takeValues"
				"FromCode(const char *) - while trying to read the row "
				"label, a NULL was read. This is a serious problem in the "
				"code.");
		} else {
			// set the label
			setRowLabel(i, value);
			// ...and drop the parsed string as we no longer need it
			delete [] value;
			value = NULL;
		}
	}

	/*
	 * Now we get into the actual data for this field.
	 */
	for (int i = 0; i < rowCnt; i++)  {
		for (int j = 0; j < colCnt; j++) {
			char	*value = parseStringFromBufferToDelim(scanner, delim);
			if (value == NULL) {
				throw CKException(__FILE__, __LINE__, "CKTable::takeValues"
					"FromCode(const char *) - while trying to read the value "
					"of the next element in the table code we ran into "
					"a NULL. This is a serious problem in the code.");
			}

			// reconstitute a CKVariant based on the data and add it in
			CKVariant	val;
			val.takeValuesFromCode(value);
			setValue(i, j, val);

			// and drop the value as we no longer need it
			if (value != NULL) {
				delete [] value;
				value = NULL;
			}
		}
	}
}


/*
 * When this table needs to be resized, a call to this method will
 * do the trick. It's important to note that all the data that can
 * survive the change will survive the change. This means that if the
 * resize is such that the new table is bigger then all the data will
 * be preserved, but if the new dimensions are smaller than the
 * current ones then data will be lost.
 */
void CKTable::resizeTable( int aNumRows, int aNumColumns )
{
	// first, see if we have anything to do - really
	if ((aNumRows <= 0) || (aNumColumns <= 0)) {
		std::ostringstream	msg;
		msg << "CKTable::resizeTable(int, int) - the requested new table "
			"size of: " << aNumRows << " by " << aNumColumns <<
			" makes no sense. Please send reasonable values.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * First, we need to create a new, duplicate, table structure of
	 * the correct size. Then, we'll copy in all the values from the
	 * existing table that we can, and then drop the old table. Finally,
	 * we'll set this new table to be the 'current' table.
	 *
	 * Nice and simple.
	 *
	 * Now we're just going to start ripping through the structure
	 * and if we run into an allocation problem the system will throw
	 * an exception for us and we're done. Otherwise, we're going to
	 * finish what we've started.
	 */
	// create the array of row pointers
	CKVariant	***table = new CKVariant**[aNumRows];
	if (table == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::resizeTable(int, int) - the array of " <<
			aNumRows << " row pointers could not be created for this new table. "
			"This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < aNumRows; i++) {
			// now create the row - made up of pointers to CKVariants
			table[i] = new CKVariant*[aNumColumns];
			if (table[i] == NULL) {
				// first, delete what we've allocated to this point
				if (table != NULL) {
					for (int k = 0; k < i; ++k) {
						if (table[k] != NULL) {
							for (int l = 0; l < aNumColumns; ++l) {
								if (table[k][l] != NULL) {
									delete table[k][l];
									table[k][l] = NULL;
								}
							}
							delete [] table[k];
							table[k] = NULL;
						}
					}
					delete [] table;
					table = NULL;
				}
				// now, throw the exception
				std::ostringstream	msg;
				msg << "CKTable::resize(int, int) - row #" <<
					(i + 1) << " of " << aNumRows << " of column pointers "
					"could not be created. This is a serious allocation "
					"error.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// ...and then create each individual element
				for (int j = 0; j < aNumColumns; j++) {
					table[i][j] = new CKVariant();
					if (table[i][j] == NULL) {
						// first, delete what we've allocated to this point
						if (table != NULL) {
							for (int k = 0; k <= i; ++k) {
								if (table[k] != NULL) {
									for (int l = 0; l < aNumColumns; ++l) {
										if (table[k][l] != NULL) {
											delete table[k][l];
											table[k][l] = NULL;
										}
									}
									delete [] table[k];
									table[k] = NULL;
								}
							}
							delete [] table;
							table = NULL;
						}
						// now, throw that exception
						std::ostringstream	msg;
						msg << "CKTable::resizeTable(int, int) - the value "
							"at row=" << (i + 1) << " and col=" << (j + 1) <<
							"could not be created. This is a serious "
							"allocation error.";
						throw CKException(__FILE__, __LINE__, msg.str());
					}
				}
			}
		}
	}

	/*
	 * If we're still here then we need to create the array of std::string
	 * values that will be the column headers.
	 */
	std::vector<std::string>	headers;
	for (int i = 0; i < aNumColumns; i++) {
		headers.push_back(std::string());
	}

	/*
	 * If we're still here then we need to create the array of std::string
	 * values that will be the row labels.
	 */
	std::vector<std::string>	labels;
	for (int i = 0; i < aNumRows; i++) {
		labels.push_back(std::string());
	}

	/*
	 * OK, now we need to go through and copy in all the data from the
	 * current table to this new guy. We have to be careful of the limits
	 * and don't forget to copy over the column headers and row labels
	 * too. HOWEVER, make sure that we really have something to do.
	 */
	if (mTable != NULL) {
		int		copyCols = MIN(mNumColumns, aNumColumns);
		int		copyRows = MIN(mNumRows, aNumRows);
		for (int i = 0; i < copyCols; ++i) {
			for (int j = 0; j < copyRows; ++j) {
				*(table[i][j]) = *(mTable[i][j]);
			}
		}
		// ...now the column headers
		for (int i = 0; i < copyCols; ++i) {
			headers[i] = mColumnHeaders[i];
		}
		// ...and finally the row labels
		for (int j = 0; j < copyRows; ++j) {
			labels[j] = mRowLabels[j];
		}
	}

	/*
	 * If we're still here, then it means that we've created all new
	 * data structures and it's OK to drop the old table and then set
	 * all the new values properly. The setters (for the most part)
	 * take care of the allocation issues. The holdout is the table
	 * itself which is why we have to drop it first.
	 */
	if (getTable() != NULL) {
		dropTable();
	}
	setNumRows(aNumRows);
	setNumColumns(aNumColumns);
	setTable(table);
	setColumnHeaders(headers);
	setRowLabels(labels);
}


/*
 * This method checks to see if the two CKTables are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKTable::operator==( const CKTable & anOther ) const
{
	bool		equal = true;

	// first, see if the sizes match
	if ((mNumRows != anOther.mNumRows) ||
		(mNumColumns != anOther.mNumColumns)) {
		equal = false;
	}

	// check the row labels
	if (mRowLabels != anOther.mRowLabels) {
		equal = false;
	}

	// check the column headers
	if (mColumnHeaders != anOther.mColumnHeaders) {
		equal = false;
	}

	// now, see if ALL the values match
	if (equal == 1) {
		for (int i = 0; equal && (i < getNumRows()); i++) {
			for (int j = 0; equal && (j < getNumColumns()); j++) {
				if (getValue(i, j) != anOther.getValue(i, j)) {
					equal = false;
					break;
				}
			}
		}
	}

	return equal;
}


/*
 * This method checks to see if the two CKTables are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKTable::operator!=( const CKTable & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this table. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a std::string.
 */
std::string CKTable::toString() const
{
	std::string		retval = "";

	// make sure we have something to show...
	if ((mNumRows > 0) && (mNumColumns > 0) && (mTable != NULL)) {
		// first, put out the column headers
		retval.append("\t");
		for (int j = 0; j < getNumColumns(); j++) {
			retval.append(j == 0 ? "" : "\t").append(mColumnHeaders[j]);
		}
		retval.append("\n");

		// now put out the data in the table
		for (int i = 0; i < mNumRows; i++) {
			// slap the row label first
			retval.append(mRowLabels[i]).append("\t");
			// ...and then the rest of the data for the row
			for (int j = 0; j < getNumColumns(); j++) {
				retval.append(j == 0 ? "" : "\t").append(mTable[i][j]->toString());
			}
			retval.append("\n");
		}
	}

	return retval;
}


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
void CKTable::setTable( CKVariant ***aTable )
{
	/*
	 * We need to be VERY careful about this setter since there could
	 * possibly be a complete table already there and need to be
	 * properly dropped. But it's possible that the user is passing
	 * a NULL in the case that he's done clearing it out.
	 */
	if (((mTable != NULL) && (aTable == NULL)) ||
		((mTable == NULL) && (aTable != NULL))) {
		mTable = aTable;
	} else {
		std::ostringstream	msg;
		msg << "CKTable::setTable(CKVariant***) - there is an existing "
			"defined table structure and you're trying to place another "
			"one on top of it. This is exceptionally unsafe. Please use "
			"the dropTable() and createTable() methods to alter the "
			"table's contents directly.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This method sets an array of std::string values to be the column
 * headers for the current table. It's important to note that there
 * needs to be getNumColumns() of them and it won't be checked, so...
 * this class will take care of making sure they are there, and it's
 * probably best to let this class to this.
 */
void CKTable::setColumnHeaders( const std::vector<std::string> & aList )
{
	// first, clear out what might already be there
	mColumnHeaders.clear();
	// now set the value if we have one
	mColumnHeaders = aList;
}


/*
 * This method sets an array of std::string values to be the row
 * labels for the current table. It's important to note that there
 * needs to be getNumRows() of them and it won't be checked, so...
 * this class will take care of making sure they are there, and it's
 * probably best to let this class to this.
 */
void CKTable::setRowLabels( const std::vector<std::string> & aList )
{
	// first, clear out what might already be there
	mRowLabels.clear();
	// now set the value if we have one
	mRowLabels = aList;
}


/*
 * This method is really here to encapsulate the number of rows
 * that this data source represents. Since it's possible to have
 * the data and these sizes not match, special care should be
 * taken to ensure that the data is consistent.
 */
void CKTable::setNumRows( int aCount )
{
	mNumRows = aCount;
}


/*
 * This method is really here to encapsulate the number of columns
 * that this data source represents. Since it's possible to have
 * the data and these sizes not match, special care should be
 * taken to ensure that the data is consistent.
 */
void CKTable::setNumColumns( int aCount )
{
	mNumColumns = aCount;
}


/*
 * This method returns the array of arrays of pointers of
 * CKVariants so that it can be used in this class. In general,
 * this is a dangerous thing, and it needs to be used carefully,
 * but it's important as we're trying to stick to using just the
 * setters and getters. This method returns a pointer to the
 * actual data and sould therefore be used very carefully as it
 * could change underneath the caller if they aren't careful.
 */
CKVariant ***CKTable::getTable() const
{
	return mTable;
}


/*
 * This method returns the actual array of std::string values that
 * are the column headers for this table. The number of elements
 * in the array is given my getNumColumns() and if the user wants
 * to have this list outside the scopr of this class they need to
 * make a copy of it and that includes all the strings in it.
 */
const std::vector<std::string> *CKTable::getColumnHeaders() const
{
	return & mColumnHeaders;
}


/*
 * This method returns the actual array of std::string values that
 * are the row labels for this table. The number of elements
 * in the array is given my getNumRows() and if the user wants
 * to have this list outside the scopr of this class they need to
 * make a copy of it and that includes all the strings in it.
 */
const std::vector<std::string> *CKTable::getRowLabels() const
{
	return & mRowLabels;
}


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
int CKTable::parseIntFromBufferToDelim( char * & aBuff, char aDelim )
{
	bool		error = false;
	int			retval = 0;

	/*
	 * We're going to loop over each character in 'buff' until we
	 * get to the 'delim'. As we're going, we'll be building up
	 * the return value. If we find a character that's not a digit
	 * then we'll return a 0, just as atoi() would.
	 */
	while (*aBuff != aDelim) {
		// see if it's not a digit
		if (!isdigit(*aBuff)) {
			error = true;
		} else {
			// accumulate the value
			retval = retval * 10 + (*aBuff - '0');
		}

		// always scan to the delimiter regardless
		++aBuff;
	}

	// make sure we move past the delimiter in the buffer
	++aBuff;

	return error ? 0 : retval;
}


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
int CKTable::parseHexIntFromBufferToDelim( char * & aBuff, char aDelim )
{
	bool		error = false;
	int			retval = 0;

	/*
	 * We're going to loop over each character in 'buff' until we
	 * get to the 'delim'. As we're going, we'll be building up
	 * the return value. If we find a character that's not a hex
	 * digit then we'll return a 0, just as atoi() would.
	 */
	while ((*aBuff != aDelim) && (*aBuff != '\0')) {
		// see if it's not a digit
		if (!isxdigit(*aBuff)) {
			error = true;
		} else {
			// accumulate the value based on the digit itself
			if (isdigit(*aBuff)) {
				retval = retval * 16 + (*aBuff - '0');
			} else {
				retval = retval * 16 + (toupper(*aBuff) - 'A' + 10);
			}
		}

		// always scan to the delimiter regardless
		++aBuff;
	}

	// make sure we move past the delimiter in the buffer
	++aBuff;

	return error ? 0 : retval;
}


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
long CKTable::parseLongFromBufferToDelim( char * & aBuff, char aDelim )
{
	bool		error = false;
	long		retval = 0;

	/*
	 * We're going to loop over each character in 'buff' until we
	 * get to the 'delim'. As we're going, we'll be building up
	 * the return value. If we find a character that's not a digit
	 * then we'll return a 0, just as atoi() would.
	 */
	while (*aBuff != aDelim) {
		// see if it's not a digit
		if (!isdigit(*aBuff)) {
			error = true;
		} else {
			// accumulate the value
			retval = retval * 10 + (*aBuff - '0');
		}

		// always scan to the delimiter regardless
		++aBuff;
	}

	// make sure we move past the delimiter in the buffer
	++aBuff;

	return error ? 0 : retval;
}


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
double CKTable::parseDoubleFromBufferToDelim( char * & aBuff, char aDelim )
{
	bool		error = false;
	double		retval = NAN;

	// first thing is to get the string that is the value
	char *strVal = parseStringFromBufferToDelim(aBuff, aDelim);
	if (strVal == NULL) {
		error = true;
		// simply return a 'NaN' value no need to log
	} else {
		// convert this to a doublw
		retval = atof(strVal);
		// ...and drop the used storage as we're done with it
		delete [] strVal;
		strVal = NULL;
	}

	return error ? NAN : retval;
}


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
char *CKTable::parseStringFromBufferToDelim( char * & aBuff, char aDelim )
{
	char		*retval = NULL;

	/*
	 * What we're going to do is put a placeholder at the location
	 * in the buffer that we start, and then scan to the other
	 * delimiter - moving the buffer pointer as we go. Then, when
	 * we've hit the delimiter we'll create a string, copy in the
	 * contents and NULL terminate it.
	 */
	char	*anchor = aBuff;
	int		cnt = 0;
	while (*aBuff != aDelim) {
		// add one to the length we'll need
		++cnt;
		// ...and move the buffer over a character
		++aBuff;
	}

	// now create the correct sized buffer to return
	retval = new char[cnt + 1];
	if (retval != NULL) {
		// copy over the string contents
		memcpy( retval, anchor, cnt );
		// ...and don't forget to NULL terminate it
		retval[cnt] = '\0';
	}

	// make sure we move past the delimiter in the buffer
	++aBuff;

	return retval;
}


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
bool CKTable::chooseAndApplyDelimiter( char *aBuff )
{
	bool	error = false;

	// first, see if we have anything to do
	int		len = 0;
	if (!error) {
		if (aBuff == NULL) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKTable::chooseAndApplyDelimiter"
				"(char *) - the passed-in buffer is NULL and that means that "
				"there's nothing I can do. Please make sure that the argument to "
				"this method is not NULL before calling.");
		} else {
			// get the length of the buffer
			len = (int)strlen(aBuff);
		}
	}

	// these are out list of possible delimiters in a reasonable order
	char	*delimiters = "\n;|~`_@#^*/'=.+-<>[]{}1234567890abcde";
	int		passCnt = (int)strlen(delimiters);
	// this will be set when we find one of these acceptable
	int		goodDelim = -1;

	/*
	 * Let's check each of the possible delimiters in turn, and if
	 * one passes, then let's flag that and we can replace it next.
	 */
	if (!error) {
		for ( int pass = 0; (goodDelim < 0) && (pass < passCnt); ++pass ) {
			// get the next test delimiter and reset the 'hit' flag
			char	testDelim = delimiters[pass];
			int		hit = 0;
			// scan the string and see if the delimiter appears anywhere
			for ( int i = 0; i < len; ++i ) {
				if (aBuff[i] == testDelim) {
					++hit;
					break;
				}
			}

			/*
			 * If the delimiter was unused, then flag it and break out
			 * of the loop.
			 */
			if (hit == 0) {
				goodDelim = pass;
				break;
			}
		}
	}

	/*
	 * Decision time... if we have a good delimiter then let's replace
	 * all the '\x01' values with the value at delimiters[goodDelim].
	 * If it turns out we don't have a good delimiter, then let's throw
	 * an exception as we can't possibly create the code without one.
	 */
	if (!error) {
		if (goodDelim == -1) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKTable::chooseAndApplyDelimiter"
				"(char *) - while trying to find an acceptable delimiter for the "
				"data in this string we ran out of possibles before finding one "
				"that wasn't being used in the text of the code. This is a serious "
				"problem that the developers need to look into.");
		} else {
			// OK... time to replace the placeholder with the delimiter
			for ( int i = 0; i < len; ++i ) {
				if (aBuff[i] == '\x01') {
					aBuff[i] = delimiters[goodDelim];
				}
			}
		}
	}

	return !error;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTable & aTable )
{
	aStream << aTable.toString();

	return aStream;
}


/********************************************************
 *
 *                Private Allocation Methods
 *
 ********************************************************/
/*
 * This private method takes care of dealing with the intelligent
 * allocation of the pointers of pointers of pointers. It's not all
 * that complex, but it's nice to have it in one place that's
 * insulated from all the other methods in this class. This method
 * will throw an exception if the number of rows and/or columns make
 * no sense, or if there's an error in the allocation of the storage.
 */
void CKTable::createTable( int aNumRows, int aNumColumns )
{
	// first, see if we have anything to do - really
	if ((aNumRows <= 0) || (aNumColumns <= 0)) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(int, int) - the requested table "
			"size of: " << aNumRows << " by " << aNumColumns <<
			" makes no sense. Please send reasonable values.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// next, make sure that we drop what might already be there
	if (getTable() != NULL) {
		dropTable();
	}

	/*
	 * Now we're just going to start ripping through the structure
	 * and if we run into an allocation problem the system will throw
	 * an exception for us and we're done. Otherwise, we're going to
	 * finish what we've started.
	 */
	// create the array of row pointers
	CKVariant	***table = new CKVariant**[aNumRows];
	if (table == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(int, int) - the array of " <<
			aNumRows << " row pointers could not be created for this table. "
			"This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < aNumRows; i++) {
			// now create the row - made up of pointers to CKVariants
			table[i] = new CKVariant*[aNumColumns];
			if (table[i] == NULL) {
				// first, delete what we've allocated to this point
				if (table != NULL) {
					for (int k = 0; k < i; ++k) {
						if (table[k] != NULL) {
							for (int l = 0; l < aNumColumns; ++l) {
								if (table[k][l] != NULL) {
									delete table[k][l];
									table[k][l] = NULL;
								}
							}
							delete [] table[k];
							table[k] = NULL;
						}
					}
					delete [] table;
					table = NULL;
				}
				// now, throw that exception
				std::ostringstream	msg;
				msg << "CKTable::createTable(int, int) - row #" <<
					(i + 1) << " of " << aNumRows << " of column pointers "
					"could not be created. This is a serious allocation "
					"error.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// ...and then create each individual element
				for (int j = 0; j < aNumColumns; j++) {
					table[i][j] = new CKVariant();
					if (table[i][j] == NULL) {
						// first, delete what we've allocated to this point
						if (table != NULL) {
							for (int k = 0; k <= i; ++k) {
								if (table[k] != NULL) {
									for (int l = 0; l < aNumColumns; ++l) {
										if (table[k][l] != NULL) {
											delete table[k][l];
											table[k][l] = NULL;
										}
									}
									delete [] table[k];
									table[k] = NULL;
								}
							}
							delete [] table;
							table = NULL;
						}
						// now, throw that exception
						std::ostringstream	msg;
						msg << "CKTable::createTable(int, int) - the value "
							"at row=" << (i + 1) << " and col=" << (j + 1) <<
							"could not be created. This is a serious "
							"allocation error.";
						throw CKException(__FILE__, __LINE__, msg.str());
					}
				}
			}
		}
	}

	/*
	 * If we're still here then we need to create the array of std::string
	 * values that will be the column headers.
	 */
	std::vector<std::string>	headers;
	for (int i = 0; i < aNumColumns; i++) {
		headers.push_back(std::string());
	}

	/*
	 * If we're still here then we need to create the array of std::string
	 * values that will be the row labels.
	 */
	std::vector<std::string>	labels;
	for (int i = 0; i < aNumColumns; i++) {
		labels.push_back(std::string());
	}

	/*
	 * If we're still here, then it means that we've allocated all the
	 * storage and it's OK to set the size as we've created it.
	 */
	setNumRows(aNumRows);
	setNumColumns(aNumColumns);
	setTable(table);
	setColumnHeaders(headers);
	setRowLabels(labels);
}


/*
 * This private method takes care of dealing with the careful
 * deallocation of the pointers of pointers of pointers. It's not all
 * that complex, but it's nice to have it in one place that's
 * insulated from all the other methods in this class.
 */
void CKTable::dropTable()
{
	// first, see if we have anything to do
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::dropTable() - the passed-in table reference is "
			"NULL and that is a major problem. Please make sure that you don't "
			"call this method with a NULL.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now start at the columns and work our way out, deleting as we go
	for (int i = 0; i < mNumRows; i++) {
		if (mTable[i] != NULL) {
			for (int j = 0; j < mNumColumns; j++) {
				// if we have an element at this location, delete it
				if (mTable[i][j] != NULL) {
					delete mTable[i][j];
					mTable[i][j] = NULL;
				}
			}
			// now delete the array of pointers that is a 'row'
			delete [] mTable[i];
			mTable[i] = NULL;
		}
	}
	// finally, delete the primary column of row pointers
	delete [] mTable;
	setTable(NULL);

	// also drop the array of column headers
	mColumnHeaders.clear();

	// ...and the array of row labels
	mRowLabels.clear();

	// also, set the size to 'undefined'
	setNumRows(-1);
	setNumColumns(-1);
}