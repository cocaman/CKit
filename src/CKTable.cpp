/*
 * CKTable.cpp - this file defines a general two-dimensional table class
 *               that holds rows and columns of CKVariant objects - which, of
 *               course, can be CKTable objects themselves. So this object
 *               really allows us to have a very general table structure of
 *               objects and manipulate them very easily.
 *
 * $Id: CKTable.cpp,v 1.17 2004/09/25 16:14:39 drbob Exp $
 */

//	System Headers
#include <string>
#include <iostream>
#include <sstream>
#include <strings.h>

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
	mColumnHeaders(NULL),
	mRowLabels(NULL),
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
	mColumnHeaders(NULL),
	mRowLabels(NULL),
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
 * This version of the constructor for this class takes two lists -
 * one for the list of row labels and the other for the list of
 * column headers. These lists not only define the structure of the
 * table, but also the row labels and column headers.
 */
CKTable::CKTable( const CKStringList aRowLabels,
				  const CKStringList aColumnHeaders ) :
	mTable(NULL),
	mColumnHeaders(NULL),
	mRowLabels(NULL),
	mNumRows(-1),
	mNumColumns(-1)
{
	// see if the requestde size makes any sense
	if (aRowLabels.empty() || aColumnHeaders.empty()) {
		std::ostringstream	msg;
		msg << "CKTable::CKTable(const CKStringList &, const "
			"CKStringList &) - the requested size: "
			<< aRowLabels.size() << " by " << aColumnHeaders.size() <<
			" doesn't make any sense. Please try again.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// create the data table structure
	createTable(aRowLabels, aColumnHeaders);
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
	mColumnHeaders(NULL),
	mRowLabels(NULL),
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
	mColumnHeaders(NULL),
	mRowLabels(NULL),
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
	// first, drop the table, if we have one
	if (mTable != NULL) {
		delete [] mTable;
		mTable = NULL;
	}
	// next, drop the row and column labels, if we have them
	if (mRowLabels != NULL) {
		delete [] mRowLabels;
		mRowLabels = NULL;
	}
	if (mColumnHeaders != NULL) {
		delete [] mColumnHeaders;
		mColumnHeaders = NULL;
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTable & CKTable::operator=( const CKTable & anOther )
{
	// now see if the requested size makes any sense to copy
	if ((anOther.mNumRows > 0) && (anOther.mNumColumns > 0)) {
		// create the data table structure
		createTable(anOther.mNumRows, anOther.mNumColumns);

		// now, copy over the row labels and column headers
		for (int r = 0; r < mNumRows; r++) {
			mRowLabels[r] = anOther.mRowLabels[r];
		}
		for (int c = 0; c < mNumColumns; c++) {
			mColumnHeaders[c] = anOther.mColumnHeaders[c];
		}

		// finally we need to copy all the values from the table to us
		int		cnt = mNumRows * mNumColumns;
		for (int i = 0; i < cnt; i++) {
			mTable[i] = anOther.mTable[i];
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
	mTable[aRow * mNumColumns + aCol] = aValue;
}


void CKTable::setValue( int aRow, const CKString & aColHeader, const CKVariant & aValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(int, const CKString &, const CKVariant &)"
			" - there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValue(aRow, col, aValue);
}


void CKTable::setValue( const CKString & aRowLabel, int aCol, const CKVariant & aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const CKString &, int, const CKVariant &)"
			" - there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValue(row, aCol, aValue);
}


void CKTable::setValue( const CKString & aRowLabel, const CKString & aColHeader, const CKVariant & aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const CKString &, const CKString &, "
			"const CKVariant &) - there is no currently defined row label '" <<
			aRowLabel << "' please make sure the row labels are properly "
			"defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValue(const CKString &, const CKString &, "
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
	mTable[aRow * mNumColumns + aCol].setValueAsType(aType, aValue);
}


void CKTable::setValueAsType( int aRow, const CKString & aColHeader,
							  CKVariantType aType, const char *aValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(int, const CKString &, "
			"CKVariantType, const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValueAsType(aRow, col, aType, aValue);
}


void CKTable::setValueAsType( const CKString & aRowLabel, int aCol,
							  CKVariantType aType, const char *aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const CKString &, int, "
			"CKVariantType, const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setValueAsType(row, aCol, aType, aValue);
}


void CKTable::setValueAsType( const CKString & aRowLabel, const CKString & aColHeader,
							  CKVariantType aType, const char *aValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const CKString &, const CKString &, "
			"CKVariantType, const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setValueAsType(const CKString &, const CKString &, "
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
	mTable[aRow * mNumColumns + aCol].setStringValue(aStringValue);
}


void CKTable::setStringValue( int aRow, const CKString & aColHeader, const char *aStringValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, const CKString &, "
			"const char *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(aRow, col, aStringValue);
}


void CKTable::setStringValue( const CKString & aRowLabel, int aCol, const char *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, int, "
			"const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(row, aCol, aStringValue);
}


void CKTable::setStringValue( const CKString & aRowLabel, const CKString & aColHeader, const char *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, const CKString &, "
			"const char *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, const CKString &, "
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
	mTable[aRow * mNumColumns + aCol].setDateValue(aDateValue);
}


void CKTable::setDateValue( int aRow, const CKString & aColHeader, long aDateValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(int, const CKString &, "
			"long) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDateValue(aRow, col, aDateValue);
}


void CKTable::setDateValue( const CKString & aRowLabel, int aCol, long aDateValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const CKString &, int, "
			"long) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDateValue(row, aCol, aDateValue);
}


void CKTable::setDateValue( const CKString & aRowLabel, const CKString & aColHeader, long aDateValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const CKString &, const CKString &, "
			"long) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDateValue(const CKString &, const CKString &, "
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
	mTable[aRow * mNumColumns + aCol].setDoubleValue(aDoubleValue);
}


void CKTable::setDoubleValue( int aRow, const CKString & aColHeader, double aDoubleValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(int, const CKString &, "
			"double) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDoubleValue(aRow, col, aDoubleValue);
}


void CKTable::setDoubleValue( const CKString & aRowLabel, int aCol, double aDoubleValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const CKString &, int, "
			"double) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setDoubleValue(row, aCol, aDoubleValue);
}


void CKTable::setDoubleValue( const CKString & aRowLabel, const CKString & aColHeader, double aDoubleValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const CKString &, const CKString &, "
			"double) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setDoubleValue(const CKString &, const CKString &, "
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
	mTable[aRow * mNumColumns + aCol].setTableValue(aTableValue);
}


void CKTable::setTableValue( int aRow, const CKString & aColHeader, const CKTable *aTableValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(int, const CKString &, "
			"const CKTable *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(aRow, col, aTableValue);
}


void CKTable::setTableValue( const CKString & aRowLabel, int aCol, const CKTable *aTableValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const CKString &, int, "
			"const CKTable *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(row, aCol, aTableValue);
}


void CKTable::setTableValue( const CKString & aRowLabel, const CKString & aColHeader, const CKTable *aTableValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const CKString &, const CKString &, "
			"const CKTable *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTableValue(const CKString &, const CKString &, "
			"const CKTable *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTableValue(row, col, aTableValue);
}


/*
 * This sets the value stored in this location as a time series, but
 * a local copy will be made so that the caller doesn't have to worry
 * about holding on to the parameter, and is free to delete it.
 */
void CKTable::setTimeSeriesValue( int aRow, int aCol, const CKTimeSeries *aTimeSeriesValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(int, int, const CKTimeSeries *) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(int, int, const CKTimeSeries *) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow * mNumColumns + aCol].setTimeSeriesValue(aTimeSeriesValue);
}


void CKTable::setTimeSeriesValue( int aRow, const CKString & aColHeader, const CKTimeSeries *aTimeSeriesValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(int, const CKString &, "
			"const CKTimeSeries *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTimeSeriesValue(aRow, col, aTimeSeriesValue);
}


void CKTable::setTimeSeriesValue( const CKString & aRowLabel, int aCol, const CKTimeSeries *aTimeSeriesValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(const CKString &, int, "
			"const CKTimeSeries *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTimeSeriesValue(row, aCol, aTimeSeriesValue);
}


void CKTable::setTimeSeriesValue( const CKString & aRowLabel, const CKString & aColHeader, const CKTimeSeries *aTimeSeriesValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(const CKString &, const CKString &, "
			"const CKTimeSeries *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setTimeSeriesValue(const CKString &, const CKString &, "
			"const CKTimeSeries *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setTimeSeriesValue(row, col, aTimeSeriesValue);
}


/*
 * This method takes the supplied column number and the header and
 * assuming the table is big enough to include that column, sets the
 * column header for that column to the supplied value. A copy is
 * made of the argument so the caller retains control of the memory.
 */
void CKTable::setColumnHeader( int aCol, const CKString & aHeader )
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
void CKTable::setRowLabel( int aRow, const CKString & aLabel )
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
	return mTable[aRow * mNumColumns + aCol];
}


CKVariant & CKTable::getValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValue(aRow, col);
}


CKVariant & CKTable::getValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValue(row, aCol);
}


CKVariant & CKTable::getValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValue(const CKString &, const CKString &) "
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
	return mTable[aRow * mNumColumns + aCol].getType();
}


CKVariantType CKTable::getType( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getType(aRow, col);
}


CKVariantType CKTable::getType( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getType(row, aCol);
}


CKVariantType CKTable::getType( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getType(const CKString &, const CKString &) "
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
			"value: " << mTable[aRow * mNumColumns + aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getIntValue();
}


int CKTable::getIntValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getIntValue(aRow, col);
}


int CKTable::getIntValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getIntValue(row, aCol);
}


int CKTable::getIntValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getIntValue(const CKString &, const CKString &) "
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
			"value: " << mTable[aRow * mNumColumns + aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getDoubleValue();
}


double CKTable::getDoubleValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDoubleValue(aRow, col);
}


double CKTable::getDoubleValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDoubleValue(row, aCol);
}


double CKTable::getDoubleValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDoubleValue(const CKString &, const CKString &) "
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
			"value: " << mTable[aRow * mNumColumns + aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getDateValue();
}


long CKTable::getDateValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDateValue(aRow, col);
}


long CKTable::getDateValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getDateValue(row, aCol);
}


long CKTable::getDateValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getDateValue(const CKString &, const CKString &) "
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
			"value: " << mTable[aRow * mNumColumns + aCol];
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getStringValue();
}


const char *CKTable::getStringValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getStringValue(aRow, col);
}


const char *CKTable::getStringValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getStringValue(row, aCol);
}


const char *CKTable::getStringValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getStringValue(const CKString &, const CKString &) "
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
	return mTable[aRow * mNumColumns + aCol].getTableValue();
}


const CKTable *CKTable::getTableValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(aRow, col);
}


const CKTable *CKTable::getTableValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(row, aCol);
}


const CKTable *CKTable::getTableValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTableValue(const CKString &, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTableValue(row, col);
}


/*
 * This method returns the actual time series value of the data that
 * this location is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKTimeSeries *CKTable::getTimeSeriesValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != eTimeSeriesVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a time "
			"series value: " << getValue(aRow, aCol);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getTimeSeriesValue();
}


const CKTimeSeries *CKTable::getTimeSeriesValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(const CKString &, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTimeSeriesValue(aRow, col);
}


const CKTimeSeries *CKTable::getTimeSeriesValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTimeSeriesValue(row, aCol);
}


const CKTimeSeries *CKTable::getTimeSeriesValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getTimeSeriesValue(const CKString &, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getTimeSeriesValue(row, col);
}


/*
 * This method returns the actual CKString value that is the
 * column header for the provided column number assuming that it
 * actually exists in the table. As such, if the user wants this
 * value outside the scope fo this class they need to make a copy
 * of it.
 */
const CKString & CKTable::getColumnHeader( int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getColumnHeader(int) - the provided column "
			"number: " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mColumnHeaders[aCol];
}


/*
 * This method returns the actual CKString value that is the
 * row label for the provided row number assuming that it
 * actually exists in the table. As such, if the user wants this
 * value outside the scope fo this class they need to make a copy
 * of it.
 */
const CKString & CKTable::getRowLabel( int aRow ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows)) {
		std::ostringstream	msg;
		msg << "CKTable::getRowLabel(int) - the provided row "
			"number: " << aRow << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
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
int CKTable::getColumnForHeader( const CKString & aHeader ) const
{
	int		retval = -1;

	// scan the array for the argument
	for ( int i = 0; i < mNumColumns; ++i ) {
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
int CKTable::getRowForLabel( const CKString & aLabel ) const
{
	int		retval = -1;

	// scan the array for the argument
	for ( int i = 0; i < mNumRows; ++i ) {
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
CKVector<CKVariant> CKTable::getRow( int aRow ) const
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
	CKVector<CKVariant>		retval;

	// now pick put the data and copy it to the returned vector
	for (int i = 0; i < mNumColumns; ++i) {
		retval.addToEnd( (CKVariant)mTable[aRow * mNumColumns + i] );
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
CKVector<CKVariant> CKTable::getRow( const CKString & aRowLabel ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getRow(const CKString &) - there is no currently "
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
CKVector<CKVariant> CKTable::getColumn( int aCol ) const
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
	CKVector<CKVariant>		retval;

	// now pick put the data and copy it to the returned vector
	for (int i = 0; i < mNumRows; ++i) {
		retval.addToEnd( (CKVariant)mTable[i * mNumColumns + aCol] );
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
CKVector<CKVariant> CKTable::getColumn( const CKString & aColumnHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColumnHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getCol(const CKString &) - there is no currently "
			"defined column header '" << aColumnHeader << "' please make sure "
			"the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getColumn(col);
}


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
bool CKTable::merge( const CKTable & aTable )
{
	bool		error = false;

	/*
	 * We need to save the old size of the table as well as have
	 * a baseline for the new table. To do this, we make two copies
	 * of the dimensions - one will remain the same, the other will
	 * be increased for new rows and/or columns from the argument.
	 */
	int			oldRows = mNumRows;
	int			oldCols = mNumColumns;
	int			endingRows = mNumRows;
	int			endingCols = mNumColumns;

	/*
	 * The first thing to do is to calculate what the new size of
	 * the resulting table needs to be. We do this by seeing how
	 * name of the row labels on aTable are empty, or not in the
	 * row labels for this table. We then do the same for the
	 * column headers.
	 */
	// do the new columns first...
	CKStringList	newColumnHeaders;
	if (!error) {
		int		cnt = aTable.mNumColumns;
		for (int i = 0; i < cnt; i++) {
			CKString		label = aTable.mColumnHeaders[i];
			if (label == "") {
				endingCols++;
			} else if (getColumnForHeader(label) == -1) {
				endingCols++;
				newColumnHeaders.addToEnd(label);
			}
		}
	}
	// now do the new rows...
	CKStringList	newRowLabels;
	if (!error) {
		int		cnt = aTable.mNumRows;
		for (int i = 0; i < cnt; i++) {
			CKString		label = aTable.mRowLabels[i];
			if (label == "") {
				endingRows++;
			} else if (getRowForLabel(label) == -1) {
				endingRows++;
				newRowLabels.addToEnd(label);
			}
		}
	}

	// now we need to resize the table to be the right overall size
	if (!error) {
		resizeTable(endingRows, endingCols);
		if ((mNumRows != endingRows) || (mNumColumns != endingCols)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::mergeInTable(const CKTable &) - the end result should "
				"have been a table " << endingRows << "x" << endingCols << " in size "
				"yet the final size is not this. That suggests an allocation error "
				"that needs to be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now we can put in the new column headers and row labels
	if (!error) {
		// do the column headers first
		for (int i = 0; i < newColumnHeaders.size(); i++) {
			mColumnHeaders[oldCols + i] = newColumnHeaders[i];
		}
		// now do the row labels next
		for (int i = 0; i < newRowLabels.size(); i++) {
			mRowLabels[oldRows + i] = newRowLabels[i];
		}
	}

	/*
	 * OK, to fill in all the data we need to be careful. Some tables
	 * have all the rows and columns labeled so it's easy, look up the
	 * row and column labels from the source and place it in the new
	 * labeled location in the new table. However, there are some cases
	 * where the rows aren't labeled - or the columns. This means that
	 * we need to allow for both tables to be partially labeled. So,
	 * when we look to the source for the data we need to also look at
	 * it's labeling. If it's not there (a "") then we need to
	 * autoincrement the target row or label and put it in the table.
	 * Thie should work for all cases.
	 */
	if (!error) {
		// blank entries are past the original table
		int	blankRow = oldRows;
		int	blankCol = oldCols;
		// these will be the new locations in the merged table
		CKVector<int>	targetRow;
		CKVector<int>	targetCol;

		// map all the rows from the source to the new table
		for (int row = 0; row < aTable.mNumRows; row++) {
			CKString		label = aTable.mRowLabels[row];
			if (label == "") {
				targetRow.addToEnd(blankRow++);
			} else {
				targetRow.addToEnd(getRowForLabel(label));
			}
		}
		// now do the same for all the columns in the source
		for (int col = 0; col < aTable.mNumColumns; col++) {
			CKString		label = aTable.mColumnHeaders[col];
			if (label == "") {
				targetCol.addToEnd(blankCol++);
			} else {
				targetCol.addToEnd(getColumnForHeader(label));
			}
		}

		// now use these maps to go from the source to the new table
		for (int row = 0; row < aTable.mNumRows; row++) {
			for (int col = 0; col < aTable.mNumColumns; col++) {
				mTable[targetRow[row] * mNumColumns + targetCol[col]] =
							aTable.mTable[row * aTable.mNumColumns + col];
			}
		}
	}

	return !error;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method returns a copy of the current value as contained in
 * a string. This is returned as a CKString as it's easy to use.
 */
CKString CKTable::getValueAsString( int aRow, int aCol ) const
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
	return mTable[aRow * mNumColumns + aCol].getValueAsString();
}


CKString CKTable::getValueAsString( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(int, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValueAsString(aRow, col);
}


CKString CKTable::getValueAsString( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const CKString &, int) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getValueAsString(row, aCol);
}


CKString CKTable::getValueAsString( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getValueAsString(const CKString &, const CKString &) "
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
	CKString buff;

	// first, send out the row and column counts
	buff.append("\x01").append(mNumRows).append("\x01").append(mNumColumns).
		append("\x01");

	// next, loop over all the column headers and write them out as well
	for (int j = 0; j < mNumColumns; ++j) {
		buff.append(mColumnHeaders[j]).append("\x01");
	}

	// next, loop over all the row labels and write them out as well
	for (int j = 0; j < mNumRows; ++j) {
		buff.append(mRowLabels[j]).append("\x01");
	}

	// now loop over the data and write it all out in an easy manner
	int		cnt = mNumRows * mNumColumns;
	for (int i = 0; i < cnt; ++i)  {
		char *code = mTable[i].generateCodeFromValues();
		if (code == NULL) {
			throw CKException(__FILE__, __LINE__, "CKTable::generateCodeFromValues"
				"() - the code for the variant in this table could not be obtained. "
				"This is a serious problem that needs to be looked into.");
		} else {
			buff.append(code).append("\x01");
			delete [] code;
			code = NULL;
		}
	}

	// now create a new buffer to hold all this
	char	*retval = new char[buff.size() + 1];
	if (retval == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTable::generateCodeFromValues"
			"() - the space to hold the codified representation of this "
			"table could not be created. This is a serious allocation "
			"error.");
	} else {
		// copy over the string's contents
		bzero(retval, buff.size() + 1);
		strncpy(retval, buff.c_str(), buff.size());
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
			mColumnHeaders[j] = value;
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
			mRowLabels[i] = value;
			// ...and drop the parsed string as we no longer need it
			delete [] value;
			value = NULL;
		}
	}

	/*
	 * Now we get into the actual data for this field.
	 */
	int		cnt = rowCnt * colCnt;
	for (int i = 0; i < cnt; i++)  {
		char	*value = parseStringFromBufferToDelim(scanner, delim);
		if (value == NULL) {
			throw CKException(__FILE__, __LINE__, "CKTable::takeValues"
				"FromCode(const char *) - while trying to read the value "
				"of the next element in the table code we ran into "
				"a NULL. This is a serious problem in the code.");
		} else {
			// reconstitute a CKVariant based on the data and add it in
			mTable[i].takeValuesFromCode(value);
			// ...and drop the value as we no longer need it
			delete [] value;
			value = NULL;
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
	CKVariant	*table = new CKVariant[aNumRows * aNumColumns];
	if (table == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::resizeTable(int, int) - the array of " <<
			aNumRows << "x" << aNumColumns << " (" << aNumRows * aNumColumns <<
			" elements) values could not be created for this new table. "
			"This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the column headers.
	 */
	CKString		*headers = new CKString[aNumColumns];
	if (headers == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::resizeTable(int, int) - the array of " <<
			aNumColumns << " column headers could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < aNumColumns; i++) {
			headers[i] = "";
		}
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the row labels.
	 */
	CKString		*labels = new CKString[aNumRows];
	if (labels == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::resizeTable(int, int) - the array of " <<
			aNumRows << " row labels could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < aNumRows; i++) {
			labels[i] = "";
		}
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
		int		i;
		int		j;
		for (i = 0; i < copyRows; ++i) {
			for (j = 0; j < copyCols; ++j) {
				table[i * aNumColumns + j] = mTable[i * mNumColumns + j];
			}
		}
		// ...now the column headers
		for (i = 0; i < copyCols; ++i) {
			headers[i] = mColumnHeaders[i];
		}
		// ...and finally the row labels
		for (j = 0; j < copyRows; ++j) {
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
	dropTable();
	mNumRows = aNumRows;
	mNumColumns = aNumColumns;
	mTable = table;
	mColumnHeaders = headers;
	mRowLabels = labels;
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
	if (equal) {
		for (int i = 0; equal && (i < mNumRows); i++) {
			if (mRowLabels[i] != anOther.mRowLabels[i]) {
				equal = false;
				break;
			}
		}
	}

	// check the column headers
	if (equal) {
		for (int i = 0; equal && (i < mNumColumns); i++) {
			if (mColumnHeaders[i] != anOther.mColumnHeaders[i]) {
				equal = false;
				break;
			}
		}
	}

	// now, see if ALL the values match
	if (equal) {
		int		cnt = mNumRows * mNumColumns;
		for (int i = 0; equal && (i < cnt); i++) {
			if (mTable[i] != anOther.mTable[i]) {
				equal = false;
				break;
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
 * a CKString.
 */
CKString CKTable::toString() const
{
	CKString		retval = "";

	// make sure we have something to show...
	if ((mNumRows > 0) && (mNumColumns > 0) && (mTable != NULL)) {
		// first, put out the column headers
		retval += "\t";
		for (int j = 0; j < mNumColumns; j++) {
			retval += (j == 0 ? "" : "\t");
			retval += mColumnHeaders[j];
		}
		retval += "\n";

		// now put out the data in the table
		for (int i = 0; i < mNumRows; i++) {
			// slap the row label first
			retval += mRowLabels[i] + "\t";
			// ...and then the rest of the data for the row
			for (int j = 0; j < mNumColumns; j++) {
				retval += (j == 0 ? "" : "\t");
				retval += mTable[i * mNumColumns + j].toString();
			}
			retval += "\n";
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
void CKTable::setTable( CKVariant *aTable )
{
	if (mTable != NULL) {
		delete [] mTable;
		mTable = NULL;
	}
	if (aTable != NULL) {
		mTable = aTable;
	}
}


/*
 * This method sets an array of CKString values to be the column
 * headers for the current table. It's important to note that there
 * needs to be getNumColumns() of them and it won't be checked, so...
 * this class will take care of making sure they are there, and it's
 * probably best to let this class to this.
 */
void CKTable::setColumnHeaders( const CKStringList & aList )
{
	// first, check to see that we have something to do
	if (aList.size() != mNumColumns) {
		std::ostringstream	msg;
		msg << "CKTable::setColumnHeaders(const CKStringList &) - "
			"the passed-in vector of strings did not contain " << mNumColumns <<
			" elements which is the number of headers in this table. Please make "
			"sure the data matches before setting.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now copy in all the values
	for (int i = 0; i < mNumColumns; i++) {
		mColumnHeaders[i] = aList[i];
	}
}


/*
 * This method sets an array of CKString values to be the row
 * labels for the current table. It's important to note that there
 * needs to be getNumRows() of them and it won't be checked, so...
 * this class will take care of making sure they are there, and it's
 * probably best to let this class to this.
 */
void CKTable::setRowLabels( const CKStringList & aList )
{
	// first, check to see that we have something to do
	if (aList.size() != mNumRows) {
		std::ostringstream	msg;
		msg << "CKTable::setRowLabels(const CKStringList &) - "
			"the passed-in vector of strings did not contain " << mNumRows <<
			" elements which is the number of labels in this table. Please make "
			"sure the data matches before setting.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now copy in all the values
	for (int i = 0; i < mNumRows; i++) {
		mRowLabels[i] = aList[i];
	}
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
 * This method returns the array of CKVariants that comprises the
 * table's row-major data storage so that it can be used in this
 * class. In general, this is a dangerous thing, and it needs to
 * be used carefully, but it's important as we're trying to stick
 * to using just the setters and getters. This method returns a
 * pointer to the actual data and sould therefore be used very
 * carefully as it could change underneath the caller if they aren't
 * careful.
 */
CKVariant *CKTable::getTable() const
{
	return mTable;
}


/*
 * This method returns the actual array of CKString values that
 * are the column headers for this table. The number of elements
 * in the array is given my getNumColumns() and if the user wants
 * to have this list outside the scopr of this class they need to
 * make a copy of it and that includes all the strings in it.
 */
const CKString *CKTable::getColumnHeaders() const
{
	return mColumnHeaders;
}


/*
 * This method returns the actual array of CKString values that
 * are the row labels for this table. The number of elements
 * in the array is given my getNumRows() and if the user wants
 * to have this list outside the scopr of this class they need to
 * make a copy of it and that includes all the strings in it.
 */
const CKString *CKTable::getRowLabels() const
{
	return mRowLabels;
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
	char	*delimiters = ";|!~`_@#^*/'=.+-<>[]{}1234567890abcde";
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
 * allocation of the table's data. It's not all that complex, but
 * it's nice to have it in one place that's insulated from all the
 * other methods in this class. This method will throw an exception
 * if the number of rows and/or columns make no sense, or if there's
 * an error in the allocation of the storage.
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
	dropTable();

	/*
	 * Now we're just going to start ripping through the structure
	 * and if we run into an allocation problem the system will throw
	 * an exception for us and we're done. Otherwise, we're going to
	 * finish what we've started.
	 */
	mTable = new CKVariant[aNumRows * aNumColumns];
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(int, int) - the array of " <<
			aNumRows << "x" << aNumColumns << " (" << aNumRows*aNumColumns <<
			" elements) values could not be created for this table. "
			"This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		/*
		 * If we're still here, then it means that we've allocated all the
		 * storage and it's OK to set the size as we've created it.
		 */
		mNumRows = aNumRows;
		mNumColumns = aNumColumns;
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the column headers.
	 */
	mColumnHeaders = new CKString[mNumColumns];
	if (mColumnHeaders == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(int, int) - the array of " <<
			mNumColumns << " column headers could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the row labels.
	 */
	mRowLabels = new CKString[mNumRows];
	if (mRowLabels == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(int, int) - the array of " <<
			mNumRows << " row labels could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This private method takes care of dealing with the intelligent
 * allocation of the table's data. It's not all that complex, but
 * it's nice to have it in one place that's insulated from all the
 * other methods in this class. This method will throw an exception
 * if the number of row labels or column headers make no sense, or
 * if there's an error in the allocation of the storage.
 */
void CKTable::createTable( const CKStringList & aRowLabels,
						   const CKStringList & aColHeaders )
{
	// first, see if we have anything to do - really
	if (aRowLabels.empty() || aColHeaders.empty()) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(const CKStringList &, "
			"const CKStringList &) - the requested table "
			"size makes no sense. Please send non-empty lists.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// next, make sure that we drop what might already be there
	dropTable();

	// now let's get the row labels, column headers and sizes
	mNumRows = aRowLabels.size();
	mNumColumns = aColHeaders.size();

	/*
	 * Now we just make what we need and if it fails back this out.
	 */
	mTable = new CKVariant[mNumRows * mNumColumns];
	if (mTable == NULL) {
		// clear out what we've already set
		dropTable();
		// ...and then throw the exception
		std::ostringstream	msg;
		msg << "CKTable::createTable(const CKStringList &, "
			"const CKStringList &) - the array of " <<
			mNumRows << "x" << mNumColumns << " (" << mNumRows*mNumColumns <<
			" elements) values could not be created for this table. "
			"This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the column headers.
	 */
	mColumnHeaders = new CKString[mNumColumns];
	if (mColumnHeaders == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(const CKStringList &, "
			"const CKStringList &) - the array of " <<
			mNumColumns << " column headers could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < mNumColumns; i++) {
			mColumnHeaders[i] = aColHeaders[i];
		}
	}

	/*
	 * If we're still here then we need to create the array of CKString
	 * values that will be the row labels.
	 */
	mRowLabels = new CKString[mNumRows];
	if (mRowLabels == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::createTable(const CKStringList &, "
			"const CKStringList &) - the array of " <<
			mNumRows << " row labels could not be created for this "
			"new table. This is a serious allocation problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		for (int i = 0; i < mNumRows; i++) {
			mRowLabels[i] = aRowLabels[i];
		}
	}
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
	if (mTable != NULL) {
		delete [] mTable;
		mTable = NULL;
	}

	// also drop the array of column headers
	if (mColumnHeaders != NULL) {
		delete [] mColumnHeaders;
		mColumnHeaders = NULL;
	}

	// ...and the array of row labels
	if (mRowLabels != NULL) {
		delete [] mRowLabels;
		mRowLabels = NULL;
	}

	// also, set the size to 'undefined'
	mNumRows = -1;
	mNumColumns = -1;
}
