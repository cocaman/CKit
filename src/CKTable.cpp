/*
 * CKTable.cpp - this file defines a general two-dimensional table class
 *               that holds rows and columns of CKVariant objects - which, of
 *               course, can be CKTable objects themselves. So this object
 *               really allows us to have a very general table structure of
 *               objects and manipulate them very easily.
 *
 * $Id: CKTable.cpp,v 1.25 2008/05/13 20:11:33 drbob Exp $
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
	mColumnHeadersIndex(),
	mRowLabels(NULL),
	mRowLabelsIndex(),
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
	mColumnHeadersIndex(),
	mRowLabels(NULL),
	mRowLabelsIndex(),
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
	mColumnHeadersIndex(),
	mRowLabels(NULL),
	mRowLabelsIndex(),
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
CKTable::CKTable( const CKString & aCode ) :
	mTable(NULL),
	mColumnHeaders(NULL),
	mColumnHeadersIndex(),
	mRowLabels(NULL),
	mRowLabelsIndex(),
	mNumRows(-1),
	mNumColumns(-1)
{
	// first, make sure we have something to do
	if (aCode.empty()) {
		std::ostringstream	msg;
		msg << "CKTable::CKTable(const CKString &) - the provided argument is "
			"empty and that means that nothing can be done. Please make sure "
			"that the argument is not empty before calling this constructor.";
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
	mColumnHeadersIndex(),
	mRowLabels(NULL),
	mRowLabelsIndex(),
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
	// make sure we don't do this to ourselves
	if (this != & anOther) {
		// now see if the requested size makes any sense to copy
		if ((anOther.mNumRows > 0) && (anOther.mNumColumns > 0)) {
			// create the data table structure
			createTable(anOther.mNumRows, anOther.mNumColumns);

			// now, copy over the row labels and column headers
			mRowLabelsIndex = anOther.mRowLabelsIndex;
			for (int r = 0; r < mNumRows; r++) {
				mRowLabels[r] = anOther.mRowLabels[r];
			}
			mColumnHeadersIndex = anOther.mColumnHeadersIndex;
			for (int c = 0; c < mNumColumns; c++) {
				mColumnHeaders[c] = anOther.mColumnHeaders[c];
			}

			// finally we need to copy all the values from the table to us
			int		cnt = mNumRows * mNumColumns;
			for (int i = 0; i < cnt; i++) {
				mTable[i] = anOther.mTable[i];
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


void CKTable::setStringValue( int aRow, int aCol, const CKString *aStringValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, int, const CKString *) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, int, const CKString *) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow * mNumColumns + aCol].setStringValue(aStringValue);
}


void CKTable::setStringValue( int aRow, const CKString & aColHeader, const CKString *aStringValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(int, const CKString &, "
			"const CKString *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(aRow, col, aStringValue);
}


void CKTable::setStringValue( const CKString & aRowLabel, int aCol, const CKString *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, int, "
			"const CKString *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setStringValue(row, aCol, aStringValue);
}


void CKTable::setStringValue( const CKString & aRowLabel, const CKString & aColHeader, const CKString *aStringValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, const CKString &, "
			"const CKString *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setStringValue(const CKString &, const CKString &, "
			"const CKString *) - there is no currently defined "
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
 * This sets the value stored in this location as a price, but
 * a local copy will be made so that the caller doesn't have to worry
 * about holding on to the parameter, and is free to delete it.
 */
void CKTable::setPriceValue( int aRow, int aCol, const CKPrice *aPriceValue )
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(int, int, const CKPrice *) - the "
			"provided location: " << aRow << ", " << aCol << " lies outside "
			"the currently defined table: " << mNumRows << " by " <<
			mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(int, int, const CKPrice *) - there "
			"is no currently defined table structure in this class. This is a "
			"serious data integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now set it intelligently
	mTable[aRow * mNumColumns + aCol].setPriceValue(aPriceValue);
}


void CKTable::setPriceValue( int aRow, const CKString & aColHeader, const CKPrice *aPriceValue )
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(int, const CKString &, "
			"const CKPrice *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setPriceValue(aRow, col, aPriceValue);
}


void CKTable::setPriceValue( const CKString & aRowLabel, int aCol, const CKPrice *aPriceValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(const CKString &, int, "
			"const CKPrice *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setPriceValue(row, aCol, aPriceValue);
}


void CKTable::setPriceValue( const CKString & aRowLabel, const CKString & aColHeader, const CKPrice *aPriceValue )
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(const CKString &, const CKString &, "
			"const CKPrice *) - there is no currently defined row "
			"label '" << aRowLabel << "' please make sure the row labels are "
			"properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::setPriceValue(const CKString &, const CKString &, "
			"const CKPrice *) - there is no currently defined "
			"column header '" << aColHeader << "' please make sure the column "
			"headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	setPriceValue(row, col, aPriceValue);
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
	mColumnHeadersIndex[aHeader] = aCol;
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
	mColumnHeadersIndex[aHeader] = aCol;
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
	mRowLabelsIndex[aLabel] = aRow;
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
	mRowLabelsIndex[aLabel] = aRow;
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
const CKString *CKTable::getStringValue( int aRow, int aCol ) const
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


const CKString *CKTable::getStringValue( int aRow, const CKString & aColHeader ) const
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


const CKString *CKTable::getStringValue( const CKString & aRowLabel, int aCol ) const
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


const CKString *CKTable::getStringValue( const CKString & aRowLabel, const CKString & aColHeader ) const
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
 * This method returns the actual price value of the data that
 * this location is holding. If the user wants to use this value
 * outside the scope of this class, then they need to make a copy.
 */
const CKPrice *CKTable::getPriceValue( int aRow, int aCol ) const
{
	// first, make sure we have a place to put this data
	if ((aRow < 0) || (aRow >= mNumRows) ||
		(aCol < 0) || (aCol >= mNumColumns)) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " lies outside the currently "
			"defined table: " << mNumRows << " by " << mNumColumns;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// ...and that we have a table structure that matches
	if (mTable == NULL) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(int, int) - there is no currently "
			"defined table structure in this class. This is a serious data "
			"integrity problem that needs to be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	// don't forget to make sure the type matches
	if (getType(aRow, aCol) != ePriceVariant) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(int, int) - the provided "
			"location: " << aRow << ", " << aCol << " does not contain a "
			"price value: " << getValue(aRow, aCol);
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// now get what they are looking for
	return mTable[aRow * mNumColumns + aCol].getPriceValue();
}


const CKPrice *CKTable::getPriceValue( int aRow, const CKString & aColHeader ) const
{
	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(const CKString &, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getPriceValue(aRow, col);
}


const CKPrice *CKTable::getPriceValue( const CKString & aRowLabel, int aCol ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getPriceValue(row, aCol);
}


const CKPrice *CKTable::getPriceValue( const CKString & aRowLabel, const CKString & aColHeader ) const
{
	// convert the row label to a row index
	int		row = getRowForLabel(aRowLabel);
	if (row < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(const CKString &, const CKString &) "
			"- there is no currently defined row label '" << aRowLabel <<
			"' please make sure the row labels are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// convert the column header to a column index
	int		col = getColumnForHeader(aColHeader);
	if (col < 0) {
		std::ostringstream	msg;
		msg << "CKTable::getPriceValue(const CKString &, const CKString &) "
			"- there is no currently defined column header '" << aColHeader <<
			"' please make sure the column headers are properly defined.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// then call the index-based method
	return getPriceValue(row, col);
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

	// see if the header is in the map
	std::map<CKString, int>::const_iterator		i = mColumnHeadersIndex.find(aHeader);
	if (i != mColumnHeadersIndex.end()) {
		// found it... so get the column number
		retval = i->second;
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

	// see if the header is in the map
	std::map<CKString, int>::const_iterator		i = mRowLabelsIndex.find(aLabel);
	if (i != mRowLabelsIndex.end()) {
		// found it... so get the row number
		retval = i->second;
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
			CKString	& header = newColumnHeaders[i];
			mColumnHeaders[oldCols + i] = header;
			mColumnHeadersIndex[header] = oldCols + i; 
		}
		// now do the row labels next
		for (int i = 0; i < newRowLabels.size(); i++) {
			CKString	& label = newRowLabels[i];
			mRowLabels[oldRows + i] = label;
			mRowLabelsIndex[label] = oldRows + i;
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
			CKString	& label = aTable.mRowLabels[row];
			if (label == "") {
				targetRow.addToEnd(blankRow++);
			} else {
				targetRow.addToEnd(getRowForLabel(label));
			}
		}
		// now do the same for all the columns in the source
		for (int col = 0; col < aTable.mNumColumns; col++) {
			CKString	& header = aTable.mColumnHeaders[col];
			if (header == "") {
				targetCol.addToEnd(blankCol++);
			} else {
				targetCol.addToEnd(getColumnForHeader(header));
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
 *                Simple Math Methods
 *
 ********************************************************/
/*
 * These methods allow the user to add values to each applicable
 * element in this table. In the first case, it's a constant value
 * but in the second it's another table. The values updated in the
 * methods are only those that make sense.
 */
bool CKTable::add( double anOffset )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::add(double) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] += anOffset;
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::add( CKTable & anOther )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::add(CKTable &) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the sizes are the same - they have to be for this
	if (!error) {
		if ((mNumRows != anOther.mNumRows) ||
			(mNumColumns != anOther.mNumColumns)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::add(CKTable &) - this table is " << mNumRows << "x" <<
				mNumColumns << " and the passed-in table is " << anOther.mNumRows <<
				"x" << anOther.mNumColumns << " and this mistatch is not allowed for "
				"the addition operation. Please make sure that the sizes match.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] += anOther.mTable[row * mNumColumns + col];
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::add( const CKTable & anOther )
{
	return add((CKTable &)anOther);
}


/*
 * These methods allow the user to subtract values from each applicable
 * element in this table. In the first case, it's a constant value
 * but in the second it's another table. The values updated in the
 * methods are only those that make sense.
 */
bool CKTable::subtract( double anOffset )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::subtract(double) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] -= anOffset;
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::subtract( CKTable & anOther )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::subtract(CKTable &) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the sizes are the same - they have to be for this
	if (!error) {
		if ((mNumRows != anOther.mNumRows) ||
			(mNumColumns != anOther.mNumColumns)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::subtract(CKTable &) - this table is " << mNumRows << "x" <<
				mNumColumns << " and the passed-in table is " << anOther.mNumRows <<
				"x" << anOther.mNumColumns << " and this mistatch is not allowed for "
				"the addition operation. Please make sure that the sizes match.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] -= anOther.mTable[row * mNumColumns + col];
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::subtract( const CKTable & anOther )
{
	return subtract((CKTable &)anOther);
}


/*
 * These method allows the user to multiply a constant value to
 * all elements in the table where such an activity would produce
 * reasonable results. The second form of the method allows for the
 * element-by-element multiplication of the tables.
 */
bool CKTable::multiply( double aFactor )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::multiply(double) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] *= aFactor;
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::multiply( CKTable & anOther )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::multiply(CKTable &) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the sizes are the same - they have to be for this
	if (!error) {
		if ((mNumRows != anOther.mNumRows) ||
			(mNumColumns != anOther.mNumColumns)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::multiply(CKTable &) - this table is " << mNumRows << "x" <<
				mNumColumns << " and the passed-in table is " << anOther.mNumRows <<
				"x" << anOther.mNumColumns << " and this mistatch is not allowed for "
				"the addition operation. Please make sure that the sizes match.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] *= anOther.mTable[row * mNumColumns + col];
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::multiply( const CKTable & anOther )
{
	return multiply((CKTable &)anOther);
}


/*
 * These method allows the user to divide each element in this
 * table by a constant value where such an activity would produce
 * reasonable results. The second form of the method allows for the
 * element-by-element division of the tables.
 */
bool CKTable::divide( double aDivisor )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::divide(double) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] /= aDivisor;
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::divide( CKTable & anOther )
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::divide(CKTable &) - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// see if the sizes are the same - they have to be for this
	if (!error) {
		if ((mNumRows != anOther.mNumRows) ||
			(mNumColumns != anOther.mNumColumns)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::divide(CKTable &) - this table is " << mNumRows << "x" <<
				mNumColumns << " and the passed-in table is " << anOther.mNumRows <<
				"x" << anOther.mNumColumns << " and this mistatch is not allowed for "
				"the addition operation. Please make sure that the sizes match.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now do the math an element at a time
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col] /= anOther.mTable[row * mNumColumns + col];
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


bool CKTable::divide( const CKTable & anOther )
{
	return divide((CKTable &)anOther);
}


/*
 * This method simply takes the inverse of each value in the table
 * so that x -> 1/x for all points. This is marginally useful
 * I'm thinking, but I added it here to be a little more complete.
 */
bool CKTable::inverse()
{
	bool		error = false;

	// see if we have anything to do
	if (!error) {
		if (mTable == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTable::inverse() - the main table structure is not where "
				"it should be and this is a serious data integrity error that needs "
				"to be looked into as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now invert each element in the table skipping problems
	if (!error) {
		for (int row = 0; row < mNumRows; row++) {
			for (int col = 0; col < mNumColumns; col++) {
				try {
					mTable[row * mNumColumns + col].inverse();
				} catch (CKException & e) {
					/*
					 * At this point we really don't want to throw an
					 * exception because we said that we'd only do those
					 * elements where it made sense. So, let's eat this
					 * exception and trust that it being logged is enough.
					 */
				}
			}
		}
	}

	return !error;
}


/*
 * These are the operator equivalents of the simple mathematical
 * operations on the table. They are here as an aid to the
 * developer of analytic functions based on these guys.
 */
CKTable & CKTable::operator+=( double anOffset )
{
	add(anOffset);
	return *this;
}


CKTable & CKTable::operator+=( CKTable & aTable )
{
	add(aTable);
	return *this;
}


CKTable & CKTable::operator+=( const CKTable & aTable )
{
	add((CKTable &)aTable);
	return *this;
}


CKTable & CKTable::operator-=( double anOffset )
{
	subtract(anOffset);
	return *this;
}


CKTable & CKTable::operator-=( CKTable & aTable )
{
	subtract(aTable);
	return *this;
}


CKTable & CKTable::operator-=( const CKTable & aTable )
{
	subtract((CKTable &)aTable);
	return *this;
}


CKTable & CKTable::operator*=( double aFactor )
{
	multiply(aFactor);
	return *this;
}


CKTable & CKTable::operator/=( double aDivisor )
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
 */
CKString CKTable::generateCodeFromValues() const
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
		buff.append(mTable[i].generateCodeFromValues()).append("\x01");
	}

	/*
	 * OK, it's now in a simple character array that we can scan to check
	 * for acceptable delimiter values. What we'll do is to check the string
	 * for the existence of a series of possible delimiters, and as soon as
	 * we find one that's not used in the string we'll use that guy.
	 */
	if (!chooseAndApplyDelimiter(buff)) {
		throw CKException(__FILE__, __LINE__, "CKTable::generateCodeFrom"
			"Values() - while trying to find an acceptable delimiter for "
			"the data in the table we ran out of possibles before finding "
			"one that wasn't being used in the text of the code. This is "
			"a serious problem that the developers need to look into.");
	}

	return buff;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this table. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKTable::takeValuesFromCode( const CKString & aCode )
{
	// first, see if we have anything to do
	if (aCode.empty()) {
		throw CKException(__FILE__, __LINE__, "CKTable::takeValuesFromCode("
			"const CKString &) - the passed-in code is empty which means that "
			"there's nothing I can do. Please make sure that the argument is "
			"not empty before calling this method.");
	}

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
	if (chunks.size() < 3) {
		std::ostringstream	msg;
		msg << "CKTable::takeValuesFromCode(const CKString &) - the code: '" <<
			aCode << "' does not represent a valid table encoding. Please check "
			"on it's source as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	/*
	 * Next thing is the row count and then the columnn count.
	 * Get them right off...
	 */
	int rowCnt = chunks[bit++].intValue();
	int colCnt = chunks[bit++].intValue();
	// see if we have enough to fill in this table
	if (chunks.size() < (2 + rowCnt + colCnt + rowCnt * colCnt)) {
		std::ostringstream	msg;
		msg << "CKTable::takeValuesFromCode(const CKString &) - the code: '" <<
			aCode << "' does not represent a valid table encoding. Please check "
			"on it's source as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// OK, we have a good size, create the table for this guy
		createTable( rowCnt, colCnt );
	}

	/*
	 * Next, we need to read off the column headers that we need to
	 * apply to this newly constructed table
	 */
	mColumnHeadersIndex.clear();
	for (int j = 0; j < colCnt; j++) {
		CKString	& header = chunks[bit++];
		mColumnHeaders[j] = header;
		mColumnHeadersIndex[header] = j;
	}

	/*
	 * Next, we need to read off the row labels that we need to
	 * apply to this newly constructed table
	 */
	mRowLabelsIndex.clear();
	for (int i = 0; i < rowCnt; i++) {
		CKString	& label = chunks[bit++];
		mRowLabels[i] = label;
		mRowLabelsIndex[label] = i;
	}

	/*
	 * Now we get into the actual data for this field.
	 */
	int		cnt = rowCnt * colCnt;
	for (int i = 0; i < cnt; i++)  {
		mTable[i].takeValuesFromCode(chunks[bit++]);
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
	std::map<CKString, int>		headersIndex;
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
	std::map<CKString, int>		labelsIndex;
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
			CKString	& header = mColumnHeaders[i];
			headers[i] = header;
			headersIndex[header] = i;
		}
		// ...and finally the row labels
		for (j = 0; j < copyRows; ++j) {
			CKString	& label = mRowLabels[j];
			labels[j] = label;
			labelsIndex[label] = j;
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
	mColumnHeadersIndex = headersIndex;
	mRowLabels = labels;
	mRowLabelsIndex = labelsIndex;
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
 * To make it a little easier on the user of this table, I have
 * created these operators so that elements in the table can be
 * referenced like simply indexes: tbl(0,5) - for both the RHS
 * and LHS of the equation. This requires the CKVariant to handle
 * a lot of the work, but that's not horrible, really.
 */
CKVariant & CKTable::operator()( int aRow, int aCol )
{
	return getValue(aRow, aCol);
}


CKVariant & CKTable::operator()( int aRow, const CKString & aColHeader )
{
	return getValue(aRow, aColHeader);
}


CKVariant & CKTable::operator()( const CKString & aRowLabel, int aCol )
{
	return getValue(aRowLabel, aCol);
}


CKVariant & CKTable::operator()( const CKString & aRowLabel, const CKString & aColHeader )
{
	return getValue(aRowLabel, aColHeader);
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
bool CKTable::chooseAndApplyDelimiter( CKString & aBuff )
{
	bool	error = false;

	// first, see if we have anything to do
	if (!error) {
		if (aBuff.empty()) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKTable::chooseAndApplyDelimiter"
				"(char *) - the passed-in buffer is empty and that means that "
				"there's nothing I can do. Please make sure that the argument to "
				"this method is not empty before calling.");
		}
	}

	/*
	 * Let's check each of the possible delimiters in turn, and if
	 * one passes, then let's flag that and we can replace it next.
	 */
	if (!error) {
		bool	replaced = false;
		// these are out list of possible delimiters in a reasonable order
		char	*delimiters = ";|!~`_@#^*/'=.+-<>[]{}1234567890abcde";
		int		passCnt = (int)strlen(delimiters);
		// check each and replace if it's not found
		for (int pass = 0; pass < passCnt; ++pass) {
			if (aBuff.find(delimiters[pass]) == -1) {
				aBuff.replace('\x01', delimiters[pass]);
				replaced = true;
				break;
			}
		}
		// see if we failed
		if (!replaced) {
			error = true;
			throw CKException(__FILE__, __LINE__, "CKTable::chooseAndApplyDelimiter"
				"(char *) - while trying to find an acceptable delimiter for the "
				"data in this string we ran out of possibles before finding one "
				"that wasn't being used in the text of the code. This is a serious "
				"problem that the developers need to look into.");
		}
	}

	return !error;
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
		CKString	& header = aList[i];
		mColumnHeaders[i] = header;
		mColumnHeadersIndex[header] = i;
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
		CKString	& label = aList[i];
		mRowLabels[i] = label;
		mRowLabelsIndex[label] = i;
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
	// clear out the index of these column headers
	mColumnHeadersIndex.clear();

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
	// clear out the index of these row labels
	mRowLabelsIndex.clear();
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
		// clear out the existing index of column headers
		mColumnHeadersIndex.clear();
		// now populate the new headers from the list
		for (int i = 0; i < mNumColumns; i++) {
			CKString	& header = aColHeaders[i];
			mColumnHeaders[i] = header;
			mColumnHeadersIndex[header] = i;
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
		// clear out the existing index of row labels
		mRowLabelsIndex.clear();
		// now populate the new labels from the list
		for (int i = 0; i < mNumRows; i++) {
			CKString	& label = aRowLabels[i];
			mRowLabels[i] = label;
			mRowLabelsIndex[label] = i;
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
	// ...and clear out the index
	mColumnHeadersIndex.clear();

	// ...and the array of row labels
	if (mRowLabels != NULL) {
		delete [] mRowLabels;
		mRowLabels = NULL;
	}
	// ...and clear out the index
	mRowLabelsIndex.clear();

	// also, set the size to 'undefined'
	mNumRows = -1;
	mNumColumns = -1;
}


/*
 * These are the operators for creating new table data from
 * one or two existing tables. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKTable operator+( CKTable & aTable, double aValue )
{
	CKTable		retval(aTable);
	retval += aValue;
	return retval;
}


CKTable operator+( double aValue, CKTable & aTable )
{
	return operator+(aTable, aValue);
}


CKTable operator+( CKTable & aTable, CKTable & anotherTable )
{
	CKTable		retval(aTable);
	retval += anotherTable;
	return retval;
}


CKTable operator-( CKTable & aTable, double aValue )
{
	CKTable		retval(aTable);
	retval -= aValue;
	return retval;
}


CKTable operator-( double aValue, CKTable & aTable )
{
	CKTable		retval(aTable);
	retval *= -1.0;
	retval += aValue;
	return retval;
}


CKTable operator-( CKTable & aTable, CKTable & anotherTable )
{
	CKTable		retval(aTable);
	retval -= anotherTable;
	return retval;
}


CKTable operator*( CKTable & aTable, double aValue )
{
	CKTable		retval(aTable);
	retval *= aValue;
	return retval;
}


CKTable operator*( double aValue, CKTable & aTable )
{
	return operator*(aTable, aValue);
}


CKTable operator/( CKTable & aTable, double aValue )
{
	CKTable		retval(aTable);
	retval /= aValue;
	return retval;
}


CKTable operator/( double aValue, CKTable & aTable )
{
	CKTable		retval(aTable);
	retval.inverse();
	retval *= aValue;
	return retval;
}
