/*
 * CKTimeTable.cpp - this file implements a time-series map of CKTables
 *                   organized by the date (stored as a long of the form
 *                   YYYYMMDD) and operated on as a single entity. This
 *                   object will fit nicely into the CKVariant scheme and
 *                   will be able to represent a series of tabular results
 *                   - one per day.
 *
 * $Id: CKTimeTable.cpp,v 1.3 2005/10/27 19:24:28 drbob Exp $
 */

//	System Headers
#include <iostream>
#include <sstream>
#include <fstream>

//	Third-Party Headers

//	Other Headers
#include "CKTimeTable.h"
#include "CKStackLocker.h"

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
 * This is the default constructor that really doesn't contain
 * anything. This isn't so bad, as the setters allow you to
 * populate this guy later with anything that you could want.
 */
CKTimeTable::CKTimeTable() :
	mTables(),
	mTablesMutex(),
	mDefaultRowCount(0),
	mDefaultColumnCount(0),
	mDefaultRowLabels(),
	mDefaultColumnHeaders()
{
}


/*
 * This version of the constructor takes the size of each table
 * that will be created to hold all the data. There needs to be
 * a singular table size as that's just the way things go, and it
 * makes most sense, anyway.
 */
CKTimeTable::CKTimeTable( int aRowCount, int aColumnCount ) :
	mTables(),
	mTablesMutex(),
	mDefaultRowCount(0),
	mDefaultColumnCount(0),
	mDefaultRowLabels(),
	mDefaultColumnHeaders()
{
	// set the row and column counts - no labels on this guy
	mDefaultRowCount = aRowCount;
	mDefaultColumnCount = aColumnCount;
}


/*
 * This version of the constructor takes the list of row labels
 * and column headers that will be applied to each table in the
 * time series, and creates a new object that is ready for storing
 * this type of data for different dates.
 */
CKTimeTable::CKTimeTable( const CKStringList & aRowLabelList,
						  const CKStringList & aColumnHeaderList ) :
	mTables(),
	mTablesMutex(),
	mDefaultRowCount(0),
	mDefaultColumnCount(0),
	mDefaultRowLabels(),
	mDefaultColumnHeaders()
{
	// set the headers and labels...
	mDefaultRowLabels = aRowLabelList;
	mDefaultColumnHeaders = aColumnHeaderList;
	// ...and then set the counts from these
	mDefaultRowCount = mDefaultRowLabels.size();
	mDefaultColumnCount = mDefaultColumnHeaders.size();
}


/*
 * This constructor is interesting in that it takes the data as
 * it comes from another CKTimeTable's generateCodeFromValues() method
 * and parses it into a time table of values directly. This is very
 * useful for serializing the time table's data from one host to
 * another across a socket, for instance.
 */
CKTimeTable::CKTimeTable( const CKString & aCode ) :
	mTables(),
	mTablesMutex(),
	mDefaultRowCount(0),
	mDefaultColumnCount(0),
	mDefaultRowLabels(),
	mDefaultColumnHeaders()
{
	// first, make sure we have something to do
	if (aCode.empty()) {
		std::ostringstream	msg;
		msg << "CKTimeTable::CKTimeTable(const CKString &) - the provided argument "
			"is empty and that means that nothing can be done. Please make sure "
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
CKTimeTable::CKTimeTable( const CKTimeTable & anOther ) :
	mTables(),
	mTablesMutex(),
	mDefaultRowCount(0),
	mDefaultColumnCount(0),
	mDefaultRowLabels(),
	mDefaultColumnHeaders()
{
	// let the '=' operator do all the work for us
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKTimeTable::~CKTimeTable()
{
	// the data elements clear themselves out fine without my help
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKTimeTable & CKTimeTable::operator=( const CKTimeTable & anOther )
{
	// first, let's lock up both against change
	((CKTimeTable &)anOther).mTablesMutex.lock();
	mTablesMutex.lock();

	// now we can set the ivars from him to me
	mTables = anOther.mTables;
	mDefaultRowCount = anOther.mDefaultRowCount;
	mDefaultColumnCount = anOther.mDefaultColumnCount;
	mDefaultRowLabels = anOther.mDefaultRowLabels;
	mDefaultColumnHeaders = anOther.mDefaultColumnHeaders;

	// finally, let's unlock them in the right order
	mTablesMutex.unlock();
	((CKTimeTable &)anOther).mTablesMutex.unlock();

	return *this;
}


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
void CKTimeTable::setDefaultRowCount( int aCount )
{
	mDefaultRowCount = aCount;
}


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
void CKTimeTable::setDefaultColumnCount( int aCount )
{
	mDefaultColumnCount = aCount;
}


/*
 * The data in this instance is organized by date, row and column
 * where for each date there is a CKTable that holds the values.
 * These methods allow the user to specify the value (a CKVariant)
 * at any index using either numeric or label-based indexing. The
 * value argument is not touched and so remains the responsibility
 * of the caller to dispose of.
 */
void CKTimeTable::setValue( long aDate, int aRow, int aCol, const CKVariant & aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValue"
			"(long, int, int, CKVariant &) - while trying to get or create a "
			"table for the date provided, an allocation error occurred and "
			"none is available. This is a serious problem that needs to be "
			"looked into.");
	} else {
		tbl->setValue(aRow, aCol, aValue);
	}
}


void CKTimeTable::setValue( long aDate, int aRow, const CKString & aColHeader, const CKVariant & aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValue"
			"(long, int, const CKString &, CKVariant &) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setValue(aRow, i, aValue);
			}
		}
	}
}


void CKTimeTable::setValue( long aDate, const CKString & aRowLabel, int aCol, const CKVariant & aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValue"
			"(long, const CKString &, int, CKVariant &) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setValue(i, aCol, aValue);
			}
		}
	}
}


void CKTimeTable::setValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKVariant & aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValue"
			"(long, const CKString &, const CKString &, CKVariant &) - while "
			"trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setValue(r, c, aValue);
					}
				}
			}
		}
	}
}


/*
 * This method sets the value at this date, row and column based on
 * the type of data that's represented in the supplied string. This
 * string will *not* be modified or retained in any way, as a copy
 * will be made for this instance, if it's necessary to have one.
 * This is the most general form of the setter for this instance.
 */
void CKTimeTable::setValueAsType( long aDate, int aRow, int aCol,
								  CKVariantType aType, const char *aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValueAsType"
			"(long, int, int, CKVariantType, const char *) - while trying to "
			"get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		tbl->setValueAsType(aRow, aCol, aType, aValue);
	}
}


void CKTimeTable::setValueAsType( long aDate, int aRow, const CKString & aColHeader,
								  CKVariantType aType, const char *aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValueAsType"
			"(long, int, const CKString &, CKVariantType, const char*) - while "
			"trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setValueAsType(aRow, i, aType, aValue);
			}
		}
	}
}


void CKTimeTable::setValueAsType( long aDate, const CKString & aRowLabel, int aCol,
								  CKVariantType aType, const char *aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValueAsType"
			"(long, const CKString &, int, CKVariantType, const char*) - while "
			"trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setValueAsType(i, aCol, aType, aValue);
			}
		}
	}
}


void CKTimeTable::setValueAsType( long aDate, const CKString & aRowLabel, const CKString & aColHeader,
								  CKVariantType aType, const char *aValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setValueAsType"
			"(long, const CKString &, const CKString &, CKVariantType, const char*) - "
			"while trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setValueAsType(r, c, aType, aValue);
					}
				}
			}
		}
	}
}


/*
 * This sets the value stored in this date, row and column as a
 * string, but a local copy will be made so that the caller doesn't
 * have to worry about holding on to the parameter, and is free to
 * delete it.
 */
void CKTimeTable::setStringValue( long aDate, int aRow, int aCol, const char *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, int, int, const char *) - while trying to get or create a "
			"table for the date provided, an allocation error occurred and none "
			"is available. This is a serious problem that needs to be looked "
			"into.");
	} else {
		tbl->setStringValue(aRow, aCol, aStringValue);
	}
}


void CKTimeTable::setStringValue( long aDate, int aRow, const CKString & aColHeader, const char *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, int, const CKString &, const char*) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setStringValue(aRow, i, aStringValue);
			}
		}
	}
}


void CKTimeTable::setStringValue( long aDate, const CKString & aRowLabel, int aCol, const char *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, const CKString &, int, const char*) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setStringValue(i, aCol, aStringValue);
			}
		}
	}
}


void CKTimeTable::setStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const char *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, const CKString &, const CKString &, const char*) - while "
			"trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setStringValue(r, c, aStringValue);
					}
				}
			}
		}
	}
}


void CKTimeTable::setStringValue( long aDate, int aRow, int aCol, const CKString *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, int, int, const CKString *) - while trying to get or create "
			"a table for the date provided, an allocation error occurred and "
			"none is available. This is a serious problem that needs to be "
			"looked into.");
	} else {
		tbl->setStringValue(aRow, aCol, aStringValue);
	}
}


void CKTimeTable::setStringValue( long aDate, int aRow, const CKString & aColHeader, const CKString *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, int, const CKString &, const CKString*) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setStringValue(aRow, i, aStringValue);
			}
		}
	}
}


void CKTimeTable::setStringValue( long aDate, const CKString & aRowLabel, int aCol, const CKString *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, const CKString &, int, const CKString*) - while trying to "
			"get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setStringValue(i, aCol, aStringValue);
			}
		}
	}
}


void CKTimeTable::setStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKString *aStringValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setStringValue"
			"(long, const CKString &, const CKString &, const CKString*) - "
			"while trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setStringValue(r, c, aStringValue);
					}
				}
			}
		}
	}
}


/*
 * This method sets the value stored in this date, row and column
 * as a date of the form YYYYMMDD - stored as a long.
 */
void CKTimeTable::setDateValue( long aDate, int aRow, int aCol, long aDateValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDateValue"
			"(long, int, int, long) - while trying to get or create a table for "
			"the date provided, an allocation error occurred and none is "
			"available. This is a serious problem that needs to be looked into.");
	} else {
		tbl->setDateValue(aRow, aCol, aDateValue);
	}
}


void CKTimeTable::setDateValue( long aDate, int aRow, const CKString & aColHeader, long aDateValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDateValue"
			"(long, int, const CKString &, long) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setDateValue(aRow, i, aDateValue);
			}
		}
	}
}


void CKTimeTable::setDateValue( long aDate, const CKString & aRowLabel, int aCol, long aDateValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDateValue"
			"(long, const CKString &, int, long) - while trying to get or "
			"create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setDateValue(i, aCol, aDateValue);
			}
		}
	}
}


void CKTimeTable::setDateValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, long aDateValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDateValue"
			"(long, const CKString &, const CKString &, long) - while trying to "
			"get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setDateValue(r, c, aDateValue);
					}
				}
			}
		}
	}
}


/*
 * This method sets the value stored in this date, row and column
 * as a double.
 */
void CKTimeTable::setDoubleValue( long aDate, int aRow, int aCol, double aDoubleValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDoubleValue"
			"(long, int, int, double) - while trying to get or create a table "
			"for the date provided, an allocation error occurred and none is "
			"available. This is a serious problem that needs to be looked into.");
	} else {
		tbl->setDoubleValue(aRow, aCol, aDoubleValue);
	}
}


void CKTimeTable::setDoubleValue( long aDate, int aRow, const CKString & aColHeader, double aDoubleValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDoubleValue"
			"(long, int, const CKString &, double) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setDoubleValue(aRow, i, aDoubleValue);
			}
		}
	}
}


void CKTimeTable::setDoubleValue( long aDate, const CKString & aRowLabel, int aCol, double aDoubleValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDoubleValue"
			"(long, const CKString &, int, double) - while trying to get or "
			"create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setDoubleValue(i, aCol, aDoubleValue);
			}
		}
	}
}


void CKTimeTable::setDoubleValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, double aDoubleValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setDoubleValue"
			"(long, const CKString &, const CKString &, double) - while trying "
			"to get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setDoubleValue(r, c, aDoubleValue);
					}
				}
			}
		}
	}
}


/*
 * This sets the value stored in this date, row and column as a
 * table, but a local copy will be made so that the caller doesn't
 * have to worry about holding on to the parameter, and is free to
 * delete it.
 */
void CKTimeTable::setTableValue( long aDate, int aRow, int aCol, const CKTable *aTableValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTableValue"
			"(long, int, int, const CKTable*) - while trying to get or create a "
			"table for the date provided, an allocation error occurred and none "
			"is available. This is a serious problem that needs to be looked "
			"into.");
	} else {
		tbl->setTableValue(aRow, aCol, aTableValue);
	}
}


void CKTimeTable::setTableValue( long aDate, int aRow, const CKString & aColHeader, const CKTable *aTableValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTableValue"
			"(long, int, const CKString &, const CKTable*) - while trying to "
			"get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setTableValue(aRow, i, aTableValue);
			}
		}
	}
}


void CKTimeTable::setTableValue( long aDate, const CKString & aRowLabel, int aCol, const CKTable *aTableValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTableValue"
			"(long, const CKString &, int, const CKTable*) - while trying to "
			"get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setTableValue(i, aCol, aTableValue);
			}
		}
	}
}


void CKTimeTable::setTableValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKTable *aTableValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTableValue"
			"(long, const CKString &, const CKString &, const CKTable*) - while "
			"trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setTableValue(r, c, aTableValue);
					}
				}
			}
		}
	}
}


/*
 * This sets the value stored in this date, row and column as a
 * time series, but a local copy will be made so that the caller
 * doesn't have to worry about holding on to the parameter, and is
 * free to delete it. This makes a time-dependent series of time
 * series data so you have to be careful to keep it straight.
 */
void CKTimeTable::setTimeSeriesValue( long aDate, int aRow, int aCol, const CKTimeSeries *aSeriesValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTimeSeriesValue"
			"(long, int, int, const CKTimeSeries*) - while trying to get or "
			"create a table for the date provided, an allocation error occurred "
			"and none is available. This is a serious problem that needs to be "
			"looked into.");
	} else {
		tbl->setTimeSeriesValue(aRow, aCol, aSeriesValue);
	}
}


void CKTimeTable::setTimeSeriesValue( long aDate, int aRow, const CKString & aColHeader, const CKTimeSeries *aSeriesValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTimeSeriesValue"
			"(long, int, const CKString &, const CKTimeSeries*) - while trying "
			"to get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setTimeSeriesValue(aRow, i, aSeriesValue);
			}
		}
	}
}


void CKTimeTable::setTimeSeriesValue( long aDate, const CKString & aRowLabel, int aCol, const CKTimeSeries *aSeriesValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTimeSeriesValue"
			"(long, const CKString &, int, const CKTimeSeries*) - while trying "
			"to get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setTimeSeriesValue(i, aCol, aSeriesValue);
			}
		}
	}
}


void CKTimeTable::setTimeSeriesValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKTimeSeries *aSeriesValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setTimeSeriesValue"
			"(long, const CKString &, const CKString &, const CKTimeSeries*) - "
			"while trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setTimeSeriesValue(r, c, aSeriesValue);
					}
				}
			}
		}
	}
}


/*
 * This sets the value stored in this date, row and column as a
 * price, but a local copy will be made so that the caller doesn't
 * have to worry about holding on to the parameter, and is free to
 * delete it.
 */
void CKTimeTable::setPriceValue( long aDate, int aRow, int aCol, const CKPrice *aPriceValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setPriceValue"
			"(long, int, int, const CKPrice*) - while trying to get or create a "
			"table for the date provided, an allocation error occurred and none "
			"is available. This is a serious problem that needs to be looked "
			"into.");
	} else {
		tbl->setPriceValue(aRow, aCol, aPriceValue);
	}
}


void CKTimeTable::setPriceValue( long aDate, int aRow, const CKString & aColHeader, const CKPrice *aPriceValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setPriceValue"
			"(long, int, const CKString &, const CKPrice*) - while trying to get "
			"or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of columns
		 * where the named header is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumColumns();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getColumnHeader(i) == aColHeader) {
				tbl->setPriceValue(aRow, i, aPriceValue);
			}
		}
	}
}


void CKTimeTable::setPriceValue( long aDate, const CKString & aRowLabel, int aCol, const CKPrice *aPriceValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setPriceValue"
			"(long, const CKString &, int, const CKPrice*) - while trying "
			"to get or create a table for the date provided, an allocation error "
			"occurred and none is available. This is a serious problem that "
			"needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a set of row labels
		 * where the named label is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		cnt = tbl->getNumRows();
		for (int i = 0; i < cnt; i++) {
			if (tbl->getRowLabel(i) == aRowLabel) {
				tbl->setPriceValue(i, aCol, aPriceValue);
			}
		}
	}
}


void CKTimeTable::setPriceValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader, const CKPrice *aPriceValue )
{
	CKTable		*tbl = getOrCreateTableForDate(aDate);
	if (tbl == NULL) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::setPriceValue"
			"(long, const CKString &, const CKString &, const CKPrice*) - "
			"while trying to get or create a table for the date provided, an "
			"allocation error occurred and none is available. This is a serious "
			"problem that needs to be looked into.");
	} else {
		/*
		 * There may be times when the user has created a rows and columns
		 * where the named location is repeated several times for their
		 * convenience and in these cases we need to make sure that the
		 * data is placed in *all* the locations where you'd expect it to
		 * properly 'fit'.
		 */
		int		rows = tbl->getNumRows();
		int		cols = tbl->getNumColumns();
		for (int r = 0; r < rows; r++) {
			if (tbl->getRowLabel(r) == aRowLabel) {
				for (int c = 0; c < cols; c++) {
					if (tbl->getColumnHeader(c) == aColHeader) {
						tbl->setPriceValue(r, c, aPriceValue);
					}
				}
			}
		}
	}
}


/*
 * This method returns the size, in rows, of the next table to be
 * created in response to a new date being added to this instance.
 * This is important because it needs to be set before the first
 * date comes in, and that typically means in the constructor.
 */
int CKTimeTable::getDefaultRowCount() const
{
	return mDefaultRowCount;
}


/*
 * This method returns the size, in columns, of the next table to be
 * created in response to a new date being added to this instance.
 * This is important because it needs to be set before the first
 * date comes in, and that typically means in the constructor.
 */
int CKTimeTable::getDefaultColumnCount() const
{
	return mDefaultColumnCount;
}


/*
 * The data in this instance is organized by date, row and column
 * where for each date there is a CKTable that holds the values.
 * These methods allow the user to access any value in the series
 * of tables by simply providing the date and index values using
 * either numeric or label-based indexing. The return value is
 * the reference of the requested value so if you want to use it
 * later, it's best to make a copy.
 */
const CKVariant & CKTimeTable::getValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValue(aRow, aCol);
}


const CKVariant & CKTimeTable::getValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValue(aRow, aColHeader);
}


const CKVariant & CKTimeTable::getValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValue(aRowLabel, aCol);
}


const CKVariant & CKTimeTable::getValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValue(aRowLabel, aColHeader);
}


/*
 * This method returns the enumerated type of the data that this
 * date, row and column is currently holding.
 */
CKVariantType CKTimeTable::getType( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getType(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getType(aRow, aCol);
}


CKVariantType CKTimeTable::getType( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getType(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getType(aRow, aColHeader);
}


CKVariantType CKTimeTable::getType( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getType(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getType(aRowLabel, aCol);
}


CKVariantType CKTimeTable::getType( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getType(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getType(aRowLabel, aColHeader);
}


/*
 * This method will return the integer value of the data stored in this
 * date, row and column - if the type is numeric. If the data isn't
 * numeric an exception will be thrown as it's assumed that the user
 * should make sure that this instance is numeric *before* calling
 * this method.
 */
int CKTimeTable::getIntValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getIntValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getIntValue(aRow, aCol);
}


int CKTimeTable::getIntValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getIntValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getIntValue(aRow, aColHeader);
}


int CKTimeTable::getIntValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getIntValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getIntValue(aRowLabel, aCol);
}


int CKTimeTable::getIntValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getIntValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getIntValue(aRowLabel, aColHeader);
}


/*
 * This method will return the double value of the data stored in this
 * date, row and column - if the type is numeric. If the data isn't
 * numeric an exception will be thrown as it's assumed that the user
 * should make sure that this instance is numeric *before* calling
 * this method.
 */
double CKTimeTable::getDoubleValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDoubleValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDoubleValue(aRow, aCol);
}


double CKTimeTable::getDoubleValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDoubleValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDoubleValue(aRow, aColHeader);
}


double CKTimeTable::getDoubleValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDoubleValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDoubleValue(aRowLabel, aCol);
}


double CKTimeTable::getDoubleValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDoubleValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDoubleValue(aRowLabel, aColHeader);
}


/*
 * This method will return the date value of the data stored in this
 * date, row and column in a long of the form YYYYMMDD - if the type
 * is date. If the data isn't a date an exception will be thrown as
 * it's assumed that the user should make sure that this instance is
 * a date *before* calling this method.
 */
long CKTimeTable::getDateValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDateValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDateValue(aRow, aCol);
}


long CKTimeTable::getDateValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDateValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDateValue(aRow, aColHeader);
}


long CKTimeTable::getDateValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDateValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDateValue(aRowLabel, aCol);
}


long CKTimeTable::getDateValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getDateValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getDateValue(aRowLabel, aColHeader);
}


/*
 * This method returns the actual string value of the data that
 * this date, row and column is holding. If the user wants to use
 * this value outside the scope of this class, then they need to
 * make a copy.
 */
const CKString *CKTimeTable::getStringValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getStringValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getStringValue(aRow, aCol);
}


const CKString *CKTimeTable::getStringValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getStringValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getStringValue(aRow, aColHeader);
}


const CKString *CKTimeTable::getStringValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getStringValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getStringValue(aRowLabel, aCol);
}


const CKString *CKTimeTable::getStringValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getStringValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getStringValue(aRowLabel, aColHeader);
}


/*
 * This method returns the actual table value of the data that
 * this date, row and column is holding. If the user wants to use
 * this value outside the scope of this class, then they need to
 * make a copy.
 */
const CKTable *CKTimeTable::getTableValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTableValue(long, int, int) - there is no currently "
			"defined date: " << aDate << " (YYYYMMDD) in the current instance. "
			"This is a serious error as you can only 'get' data that's already "
			"been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTableValue(aRow, aCol);
}


const CKTable *CKTimeTable::getTableValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTableValue(long, int, const CKString &) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTableValue(aRow, aColHeader);
}


const CKTable *CKTimeTable::getTableValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTableValue(long, const CKString &, int) - there is "
			"no currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTableValue(aRowLabel, aCol);
}


const CKTable *CKTimeTable::getTableValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTableValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTableValue(aRowLabel, aColHeader);
}


/*
 * This method returns the actual time series value of the data that
 * this date, row and column is holding. If the user wants to use
 * this value outside the scope of this class, then they need to
 * make a copy.
 */
const CKTimeSeries *CKTimeTable::getTimeSeriesValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTimeSeriesValue(long, int, int) - there is no "
			"currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTimeSeriesValue(aRow, aCol);
}


const CKTimeSeries *CKTimeTable::getTimeSeriesValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTimeSeriesValue(long, int, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTimeSeriesValue(aRow, aColHeader);
}


const CKTimeSeries *CKTimeTable::getTimeSeriesValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTimeSeriesValue(long, const CKString &, int) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTimeSeriesValue(aRowLabel, aCol);
}


const CKTimeSeries *CKTimeTable::getTimeSeriesValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getTimeSeriesValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getTimeSeriesValue(aRowLabel, aColHeader);
}


/*
 * This method returns the actual price value of the data that
 * this date, row and column is holding. If the user wants to use
 * this value outside the scope of this class, then they need to
 * make a copy.
 */
const CKPrice *CKTimeTable::getPriceValue( long aDate, int aRow, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getPriceValue(long, int, int) - there is no "
			"currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getPriceValue(aRow, aCol);
}


const CKPrice *CKTimeTable::getPriceValue( long aDate, int aRow, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getPriceValue(long, int, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getPriceValue(aRow, aColHeader);
}


const CKPrice *CKTimeTable::getPriceValue( long aDate, const CKString & aRowLabel, int aCol )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getPriceValue(long, const CKString &, int) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getPriceValue(aRowLabel, aCol);
}


const CKPrice *CKTimeTable::getPriceValue( long aDate, const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getPriceValue(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getPriceValue(aRowLabel, aColHeader);
}


/*
 * This method returns a pointer to the complete CKTable that this
 * instance holds for the provided date. This is important because
 * this method will return NULL if no such date is already in the
 * response, and if one is desired, try getOrCreateTableForDate().
 * If the caller wants to do something with this return value they
 * need to make a copy as this is our original and can't be touched.
 */
CKTable *CKTimeTable::getTableForDate( long aDate )
{
	bool			error = false;
	CKTable			*retval = NULL;

	// first, make sure the date makes some kind of sense
	if (!error) {
		if ((floor(aDate/10000) < 1900) || (floor(aDate/10000) > 2100) ||
			(aDate - floor(aDate/10000)*10000 < 101) ||
			(aDate - floor(aDate/10000)*10000 > 1231)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeTable::getTableForDate(long) - the provided date: "
				<< aDate << " does not appear to be of the proper format: "
				"YYYYMMDD. Please confirm this before trying again.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if we have anything in the map for this date
	if (!error) {
		// lock up the list against change
		CKStackLocker	lockem(&mTablesMutex);
		// try to find the right date for this guy
		CKDateTableMap::iterator	i = mTables.find(aDate);
		if (i != mTables.end()) {
			retval = &(i->second);
		}
	}

	return error ? NULL : retval;
}


CKTable *CKTimeTable::getTableForDate( long aDate ) const
{
	return ((CKTimeTable *)this)->getTableForDate(aDate);
}


/*
 * When the user needs to know what dates are in this response, this
 * method is a nice way to get at those values. Each of the entries
 * is a long representing a date of the format YYYYMMDD and has a
 * corresponding CKTable in this instance's data structures.
 */
const CKVector<long> CKTimeTable::getDateValues()
{
	CKVector<long>		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the dates
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval.addToEnd(i->first);
	}

	return retval;
}


const CKVector<long> CKTimeTable::getDateValues() const
{
	return ((CKTimeTable *)this)->getDateValues();
}


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
CKVector<CKVariant> CKTimeTable::getDateVector( int aRow, int aCol )
{
	CKVector<CKVariant>		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval.addToEnd(i->second.getValue(aRow, aCol));
	}

	return retval;
}


CKVector<CKVariant> CKTimeTable::getDateVector( int aRow, const CKString & aColHeader )
{
	CKVector<CKVariant>		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval.addToEnd(i->second.getValue(aRow, aColHeader));
	}

	return retval;
}


CKVector<CKVariant> CKTimeTable::getDateVector( const CKString & aRowLabel, int aCol )
{
	CKVector<CKVariant>		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval.addToEnd(i->second.getValue(aRowLabel, aCol));
	}

	return retval;
}


CKVector<CKVariant> CKTimeTable::getDateVector( const CKString & aRowLabel, const CKString & aColHeader )
{
	CKVector<CKVariant>		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval.addToEnd(i->second.getValue(aRowLabel, aColHeader));
	}

	return retval;
}


/*
 * In addition to the vector-of-Vatiants format above, the following
 * methods allow the user to get *numeric* data as a CKTimeSeries.
 * These time series objects are much easier to work with and can
 * do advanced things like take place in expressions, etc.
 */
CKTimeSeries CKTimeTable::getTimeSeries( int aRow, int aCol )
{
	CKTimeSeries		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values that are numbers
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		if (i->second.getType(aRow, aCol) == eNumberVariant) {
			retval.put((double)i->first, i->second.getDoubleValue(aRow, aCol));
		}
	}

	return retval;
}


CKTimeSeries CKTimeTable::getTimeSeries( int aRow, const CKString & aColHeader )
{
	CKTimeSeries		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values that are numbers
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		if (i->second.getType(aRow, aColHeader) == eNumberVariant) {
			retval.put((double)i->first, i->second.getDoubleValue(aRow, aColHeader));
		}
	}

	return retval;
}


CKTimeSeries CKTimeTable::getTimeSeries( const CKString & aRowLabel, int aCol )
{
	CKTimeSeries		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values that are numbers
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		if (i->second.getType(aRowLabel, aCol) == eNumberVariant) {
			retval.put((double)i->first, i->second.getDoubleValue(aRowLabel, aCol));
		}
	}

	return retval;
}


CKTimeSeries CKTimeTable::getTimeSeries( const CKString & aRowLabel, const CKString & aColHeader )
{
	CKTimeSeries		retval;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);
	// now zoom through the list adding in all the values that are numbers
	for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
		if (i->second.getType(aRowLabel, aColHeader) == eNumberVariant) {
			retval.put((double)i->first, i->second.getDoubleValue(aRowLabel, aColHeader));
		}
	}

	return retval;
}


/*
 * This method returns the reference to the vector of default row
 * labels that will be used for populating all the new CKTables that
 * are created for each new date in this instance. This is important
 * because because each of the tables needs to have the same labels
 * and headers so that the character indexing works as it's supposed
 * to.
 */
const CKStringList & CKTimeTable::getDefaultRowLabels() const
{
	return mDefaultRowLabels;
}


/*
 * This method returns the reference to the vector of default column
 * headers that will be used for populating all the new CKTables that
 * are created for each new date in this instance. This is important
 * because because each of the tables needs to have the same labels
 * and headers so that the character indexing works as it's supposed
 * to.
 */
const CKStringList & CKTimeTable::getDefaultColumnHeaders() const
{
	return mDefaultColumnHeaders;
}


/*
 * This method is a nice encapsulation method that makes it easy
 * to add the supplied row label to the vector of default labels
 * that this instance maintains so that it can be used to properly
 * configure each new table that gets created for this instance.
 */
void CKTimeTable::addToDefaultRowLabels( const CKString & aRowLabel )
{
	mDefaultRowLabels.addToEnd(aRowLabel);
}


/*
 * This method is a nice encapsulation method that makes it easy
 * to add the supplied column header to the vector of default headers
 * that this instance maintains so that it can be used to properly
 * configure each new table that gets created for this instance.
 */
void CKTimeTable::addToDefaultColumnHeaders( const CKString & aColumnHeader )
{
	mDefaultColumnHeaders.addToEnd(aColumnHeader);
}


/*
 * This method clears out all the tables for all the dates
 * so that it's like starting over from scratch with this instance.
 */
void CKTimeTable::clear()
{
	// lock up the mutex so no one can do anything but me
	CKStackLocker	lockem(&mTablesMutex);
	// clear the tables and the dates
	mTables.clear();
}


/*
 * This method clears out all the existing default row labels
 * so that you can start with a clean slate.
 */
void CKTimeTable::clearDefaultRowLabels()
{
	mDefaultRowLabels.clear();
}


/*
 * This method clears out all the existing default column headers
 * so that you can start with a clean slate.
 */
void CKTimeTable::clearDefaultColumnHeaders()
{
	mDefaultColumnHeaders.clear();
}


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
bool CKTimeTable::add( double anOffset )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.add(anOffset);
			}
		}
	}

	return !error;
}


bool CKTimeTable::add( CKTable & aTable )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.add(aTable);
			}
		}
	}

	return !error;
}


bool CKTimeTable::add( const CKTable & aTable )
{
	return add((CKTable &)aTable);
}


bool CKTimeTable::add( CKTimeTable & anOther )
{
	bool		error = false;

	// I need to lock up both myself and the arg against changes
	CKStackLocker	lockme(&mTablesMutex);
	CKStackLocker	lockhim(&anOther.mTablesMutex);

	/*
	 * This is interesting in that we need to match up the dates as well
	 * as the data in the tables. So, I need to scan all *my* dates and
	 * try to find that same date in the *other* guy and add up his table
	 * to mine.
	 */
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				// see if the other guy has this date
				CKTable *his = anOther.getTableForDate(i->first);
				if (his != NULL) {
					i->second.add(*his);
				}
			}
		}
	}

	return !error;
}


bool CKTimeTable::add( const CKTimeTable & anOther )
{
	return add((CKTimeTable &)anOther);
}


/*
 * These methods allow the user to subtract values from each applicable
 * element in this time table. In the first case, it's a constant value
 * but in the second it's a table that will be used against all the
 * tables in the time table. The third method allows the point-by-point
 * subtraction of two complete time tables. The values updated in the
 * methods are only those that make sense.
 */
bool CKTimeTable::subtract( double anOffset )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.subtract(anOffset);
			}
		}
	}

	return !error;
}


bool CKTimeTable::subtract( CKTable & aTable )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.subtract(aTable);
			}
		}
	}

	return !error;
}


bool CKTimeTable::subtract( const CKTable & aTable )
{
	return subtract((CKTable &)aTable);
}


bool CKTimeTable::subtract( CKTimeTable & anOther )
{
	bool		error = false;

	// I need to lock up both myself and the arg against changes
	CKStackLocker	lockme(&mTablesMutex);
	CKStackLocker	lockhim(&anOther.mTablesMutex);

	/*
	 * This is interesting in that we need to match up the dates as well
	 * as the data in the tables. So, I need to scan all *my* dates and
	 * try to find that same date in the *other* guy and subtract his table
	 * from mine.
	 */
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				// see if the other guy has this date
				CKTable *his = anOther.getTableForDate(i->first);
				if (his != NULL) {
					i->second.subtract(*his);
				}
			}
		}
	}

	return !error;
}


bool CKTimeTable::subtract( const CKTimeTable & anOther )
{
	return subtract((CKTimeTable &)anOther);
}


/*
 * These method allows the user to multiply a constant value to
 * all elements in the time table where such an activity would produce
 * reasonable results. The second form of the method allows for the
 * element-by-element multiplication of the argument by each table
 * in the time table. The third form allows a point-by-point product
 * of two time tables.
 */
bool CKTimeTable::multiply( double aFactor )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.multiply(aFactor);
			}
		}
	}

	return !error;
}


bool CKTimeTable::multiply( CKTable & aTable )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.multiply(aTable);
			}
		}
	}

	return !error;
}


bool CKTimeTable::multiply( const CKTable & aTable )
{
	return multiply((CKTable &)aTable);
}


bool CKTimeTable::multiply( CKTimeTable & anOther )
{
	bool		error = false;

	// I need to lock up both myself and the arg against changes
	CKStackLocker	lockme(&mTablesMutex);
	CKStackLocker	lockhim(&anOther.mTablesMutex);

	/*
	 * This is interesting in that we need to match up the dates as well
	 * as the data in the tables. So, I need to scan all *my* dates and
	 * try to find that same date in the *other* guy and multiply his table
	 * to mine.
	 */
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				// see if the other guy has this date
				CKTable *his = anOther.getTableForDate(i->first);
				if (his != NULL) {
					i->second.multiply(*his);
				}
			}
		}
	}

	return !error;
}


bool CKTimeTable::multiply( const CKTimeTable & anOther )
{
	return multiply((CKTimeTable &)anOther);
}


/*
 * These method allows the user to divide each element in this
 * table by a constant value where such an activity would produce
 * reasonable results. The second form of the method allows for the
 * element-by-element division of the argument by each table
 * in the time table. The third form allows for a point-by-point
 * division of two time tables.
 */
bool CKTimeTable::divide( double aDivisor )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.divide(aDivisor);
			}
		}
	}

	return !error;
}


bool CKTimeTable::divide( CKTable & aTable )
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.divide(aTable);
			}
		}
	}

	return !error;
}


bool CKTimeTable::divide( const CKTable & aTable )
{
	return divide((CKTable &)aTable);
}


bool CKTimeTable::divide( CKTimeTable & anOther )
{
	bool		error = false;

	// I need to lock up both myself and the arg against changes
	CKStackLocker	lockme(&mTablesMutex);
	CKStackLocker	lockhim(&anOther.mTablesMutex);

	/*
	 * This is interesting in that we need to match up the dates as well
	 * as the data in the tables. So, I need to scan all *my* dates and
	 * try to find that same date in the *other* guy and divide his table
	 * into mine.
	 */
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				// see if the other guy has this date
				CKTable *his = anOther.getTableForDate(i->first);
				if (his != NULL) {
					i->second.divide(*his);
				}
			}
		}
	}

	return !error;
}


bool CKTimeTable::divide( const CKTimeTable & anOther )
{
	return divide((CKTimeTable &)anOther);
}


/*
 * This method simply takes the inverse of each value in the time
 * table so that x -> 1/x for all points. This is marginally useful
 * I'm thinking, but I added it here to be a little more complete.
 */
bool CKTimeTable::inverse()
{
	bool		error = false;

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	// now do the math a table at a time
	if (!error) {
		if (!mTables.empty()) {
			for (CKDateTableMap::iterator i = mTables.begin(); i != mTables.end(); ++i) {
				i->second.inverse();
			}
		}
	}

	return !error;
}


/*
 * These are the operator equivalents of the simple mathematical
 * operations on the time table. They are here as an aid to the
 * developer of analytic functions based on these guys.
 */
CKTimeTable & CKTimeTable::operator+=( double anOffset )
{
	add(anOffset);
	return *this;
}


CKTimeTable & CKTimeTable::operator+=( CKTable & aTable )
{
	add(aTable);
	return *this;
}


CKTimeTable & CKTimeTable::operator+=( const CKTable & aTable )
{
	add((CKTable &)aTable);
	return *this;
}


CKTimeTable & CKTimeTable::operator+=( CKTimeTable & anOther )
{
	add(anOther);
	return *this;
}


CKTimeTable & CKTimeTable::operator+=( const CKTimeTable & anOther )
{
	add((CKTimeTable &)anOther);
	return *this;
}


CKTimeTable & CKTimeTable::operator-=( double anOffset )
{
	subtract(anOffset);
	return *this;
}


CKTimeTable & CKTimeTable::operator-=( CKTable & aTable )
{
	subtract(aTable);
	return *this;
}


CKTimeTable & CKTimeTable::operator-=( const CKTable & aTable )
{
	subtract((CKTable &)aTable);
	return *this;
}


CKTimeTable & CKTimeTable::operator-=( CKTimeTable & anOther )
{
	subtract(anOther);
	return *this;
}


CKTimeTable & CKTimeTable::operator-=( const CKTimeTable & anOther )
{
	subtract((CKTimeTable &)anOther);
	return *this;
}


CKTimeTable & CKTimeTable::operator*=( double aFactor )
{
	multiply(aFactor);
	return *this;
}


CKTimeTable & CKTimeTable::operator/=( double aDivisor )
{
	divide(aDivisor);
	return *this;
}


/*
 * These are the operators for creating new table data from
 * one or two existing time tables. This is nice in the same vein
 * as the simpler operators in that it makes writing code for these
 * data sets a lot easier.
 */
CKTimeTable operator+( CKTimeTable & aTimeTable, double aValue )
{
	CKTimeTable		retval(aTimeTable);
	retval += aValue;
	return retval;
}


CKTimeTable operator+( double aValue, CKTimeTable & aTimeTable )
{
	return operator+(aTimeTable, aValue);
}


CKTimeTable operator+( CKTimeTable & aTimeTable, CKTable & aTable )
{
	CKTimeTable		retval(aTimeTable);
	retval += aTable;
	return retval;
}


CKTimeTable operator+( CKTable & aTable, CKTimeTable & aTimeTable )
{
	return operator+(aTimeTable, aTable);
}


CKTimeTable operator+( CKTimeTable & aTimeTable, CKTimeTable & anotherTimeTable )
{
	CKTimeTable		retval(aTimeTable);
	retval += anotherTimeTable;
	return retval;
}


CKTimeTable operator-( CKTimeTable & aTimeTable, double aValue )
{
	CKTimeTable		retval(aTimeTable);
	retval -= aValue;
	return retval;
}


CKTimeTable operator-( double aValue, CKTimeTable & aTimeTable )
{
	CKTimeTable		retval(aTimeTable);
	retval *= -1.0;
	retval += aValue;
	return retval;
}


CKTimeTable operator-( CKTimeTable & aTimeTable, CKTable & aTable )
{
	CKTimeTable		retval(aTimeTable);
	retval -= aTable;
	return retval;
}


CKTimeTable operator-( CKTable & aTable, CKTimeTable & aTimeTable )
{
	CKTimeTable		retval(aTimeTable);
	retval *= -1.0;
	retval += aTable;
	return retval;
}


CKTimeTable operator-( CKTimeTable & aTimeTable, CKTimeTable & anotherTimeTable )
{
	CKTimeTable		retval(aTimeTable);
	retval -= anotherTimeTable;
	return retval;
}


CKTimeTable operator*( CKTimeTable & aTimeTable, double aValue )
{
	CKTimeTable		retval(aTimeTable);
	retval *= aValue;
	return retval;
}


CKTimeTable operator*( double aValue, CKTimeTable & aTimeTable )
{
	return operator*(aTimeTable, aValue);
}


CKTimeTable operator/( CKTimeTable & aTimeTable, double aValue )
{
	CKTimeTable		retval(aTimeTable);
	retval /= aValue;
	return retval;
}


CKTimeTable operator/( double aValue, CKTimeTable & aTimeTable )
{
	CKTimeTable		retval(aTimeTable);
	retval.inverse();
	retval *= aValue;
	return retval;
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
CKString CKTimeTable::getValueAsString( long aDate, int aRow, int aCol ) const
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValueAsString(long, int, int) - there is no "
			"currently defined date: " << aDate << " (YYYYMMDD) in the current "
			"instance. This is a serious error as you can only 'get' data that's "
			"already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValueAsString(aRow, aCol);
}


CKString CKTimeTable::getValueAsString( long aDate, int aRow, const CKString & aColHeader ) const
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValueAsString(long, int, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValueAsString(aRow, aColHeader);
}


CKString CKTimeTable::getValueAsString( long aDate, const CKString & aRowLabel, int aCol ) const
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValueAsString(long, const CKString &, int) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValueAsString(aRowLabel, aCol);
}


CKString CKTimeTable::getValueAsString( long aDate, const CKString & aRowLabel, const CKString & aColHeader ) const
{
	CKTable		*tbl = getTableForDate(aDate);
	if (tbl == NULL) {
		std::ostringstream	msg;
		msg << "CKTimeTable::getValueAsString(long, const CKString &, const CKString &) - "
			"there is no currently defined date: " << aDate << " (YYYYMMDD) in "
			"the current instance. This is a serious error as you can only 'get' "
			"data that's already been 'set'.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return tbl->getValueAsString(aRowLabel, aColHeader);
}


/*
 * In order to simplify the move of this object from C++ to Java
 * it makes sense to encode the point's data into a string that
 * can be converted to a Java String and then the Java object can
 * interpret it and "reconstitue" the object from this coding.
 */
CKString CKTimeTable::generateCodeFromValues() const
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
	// lock up the map against change
	CKStackLocker	lockem(&(((CKTimeTable*)this)->mTablesMutex));

	// start by getting a buffer to build up this value
	CKString buff;

	// first, send out the default row and column counts
	buff.append("\x01").append(mDefaultRowCount).append("\x01").
		append(mDefaultColumnCount).append("\x01");

	// next, loop over all the default column headers
	int		cnt = mDefaultColumnHeaders.size();
	buff.append(cnt).append("\x01");
	for (int  i = 0; i < cnt; i++) {
		buff.append(mDefaultColumnHeaders[i]).append("\x01");
	}

	// next, loop over all the default row labels
	cnt = mDefaultRowLabels.size();
	buff.append(cnt).append("\x01");
	for (int  i = 0; i < cnt; i++) {
		buff.append(mDefaultRowLabels[i]).append("\x01");
	}

	// now loop over the data and write it all out in an easy manner
	cnt = mTables.size();
	buff.append(cnt).append("\x01");
	for (CKDateTableMap::const_iterator i = mTables.begin(); i != mTables.end(); ++i) {
		buff.append(i->first).append("\x01");
		CKString code = i->second.generateCodeFromValues();
		if (code.empty()) {
			throw CKException(__FILE__, __LINE__, "CKTimeTable::generate"
				"CodeFromValues() - the code for the table in the response "
				"could not be generated and this is a serious problem. Check "
				"on it as soon as possible.");
		} else {
			buff.append(code).append("\x01");
		}
	}

	/*
	 * OK, it's now in a simple character array that we can scan to check
	 * for acceptable delimiter values. What we'll do is to check the string
	 * for the existence of a series of possible delimiters, and as soon as
	 * we find one that's not used in the string we'll use that guy.
	 */
	if (!CKTable::chooseAndApplyDelimiter(buff)) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::generate"
			"CodeFromValues() - while trying to find an acceptable delimiter "
			"for the data in the time table we ran out of possibles before "
			"finding one that wasn't being used in the text of the code. "
			"This is a serious problem that the developers need to look "
			"into.");
	}

	return buff;
}


/*
 * This method takes a code that could have been written with the
 * generateCodeFromValues() method on either the C++ or Java
 * versions of this class and extracts all the values from the code
 * that are needed to populate this point. The argument is left
 * untouched, and is the responsible of the caller to free.
 */
void CKTimeTable::takeValuesFromCode( const CKString & aCode )
{
	// first, see if we have anything to do
	if (aCode.empty()) {
		throw CKException(__FILE__, __LINE__, "CKTimeTable::takeValuesFrom"
			"Code(const CKString &) - the passed-in code is empty which means "
			"that there's nothing I can do. Please make sure that the argument "
			"is not empty before calling this method.");
	}

	// lock up the map against change
	CKStackLocker	lockem(&mTablesMutex);

	/*
	 * The data is character-delimited and the delimiter is
	 * the first character of the field data. All subsequent
	 * values will be delimited by this character. We need to
	 * get it.
	 */
	char	delim = aCode[0];
	// ...and parse the code into chunks delimited by this
	int		bit = 0;
	CKStringList	chunks = CKStringList::parseIntoChunks(
									aCode.substr(1, aCode.size()-2), delim);
	if (chunks.size() < 3) {
		std::ostringstream	msg;
		msg << "CKTimeTable::takeValuesFromCode(const CKString &) - the provided "
			"code: '" << aCode << "' was not complete for a time table. Please "
			"check the format of the code and try again.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
	int		chunkSize = chunks.size();

	// first, read the default row and column counts
	int defaultRowCnt = chunks[bit++].intValue();
	int defaultColCnt = chunks[bit++].intValue();

	// next, loop over all the default column headers
	CKStringList	defaultColHeaders;
	// ...get the number of them I'll be reading
	int		count = chunks[bit++].intValue();
	// ...and then read each one and add it to the list
	for (int i = 0; i < count; i++) {
		if (bit >= chunkSize) {
			std::ostringstream	msg;
			msg << "CKTimeTable::takeValueFromCode(const CKString &) - the provided "
				"code: '" << aCode << "' was not complete for a time table. The "
				"default column headers aren't even here. Please check the format "
				"of the code and try again.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
		defaultColHeaders.addToEnd(chunks[bit++]);
	}

	// next, loop over all the default row labels
	CKStringList	defaultRowLabels;
	// ...get the number of them I'll be reading
	count = chunks[bit++].intValue();
	// ...and then read each one and add it to the list
	for (int i = 0; i < count; i++) {
		if (bit >= chunkSize) {
			std::ostringstream	msg;
			msg << "CKTimeTable::takeValueFromCode(const CKString &) - the provided "
				"code: '" << aCode << "' was not complete for a time table. The "
				"default row labels aren't even here. Please check the format of "
				"the code and try again.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
		defaultRowLabels.addToEnd(chunks[bit++]);
	}

	// get the number of tables I'll be reading
	count = chunks[bit++].intValue();
	// ...and then read each one and add it to the list
	for (int i = 0; i < count; i++) {
		if (bit+1 >= chunkSize) {
			std::ostringstream	msg;
			msg << "CKTimeTable::takeValueFromCode(const CKString &) - the provided "
				"code: '" << aCode << "' was not complete for a time table. The "
				"code doesn't contain enough components for all the tables. Please "
				"check the format of the code and try again.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}

		// make a date and a table from the string representations
		long	when = chunks[bit++].longValue();
		mTables[when] = CKTable(chunks[bit++]);
	}

	/*
	 * If we are here, then that means that all went well and we should
	 * save all these in this instance. But first, we need to make sure
	 * that everything is cleaned up before setting these things.
	 */
	mDefaultRowCount = defaultRowCnt;
	mDefaultColumnCount = defaultColCnt;
	mDefaultRowLabels = defaultRowLabels;
	mDefaultColumnHeaders = defaultColHeaders;
}


/*
 * Because there may be times that the user wants to lock us up
 * for change, we're going to expose this here so it's easy for them
 * to iterate, for example.
 */
void CKTimeTable::lock()
{
	mTablesMutex.lock();
}


void CKTimeTable::lock() const
{
	((CKTimeTable *)this)->lock();
}


void CKTimeTable::unlock()
{
	mTablesMutex.lock();
}


void CKTimeTable::unlock() const
{
	((CKTimeTable *)this)->unlock();
}


/*
 * This method checks to see if the two CKTimeTables are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKTimeTable::operator==( const CKTimeTable & anOther ) const
{
	bool		equal = true;

	// all of these guys are containers with good operators
	if ((mDefaultRowCount != anOther.mDefaultRowCount) ||
		(mDefaultColumnCount != anOther.mDefaultColumnCount) ||
		(mDefaultRowLabels != anOther.mDefaultRowLabels) ||
		(mDefaultColumnHeaders != anOther.mDefaultColumnHeaders) ||
		(mTables != anOther.mTables)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKTimeTables are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKTimeTable::operator!=( const CKTimeTable & anOther ) const
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
CKString CKTimeTable::toString() const
{
	// lock up the map against change
	CKStackLocker	lockem(&(((CKTimeTable*)this)->mTablesMutex));

	// put everything in between angle brackets to make it look nice
	CKString	retval = "< Default Table Size=";
	retval += mDefaultRowCount;
	retval += "x";
	retval += mDefaultColumnCount;
	retval += "\n";

	retval += "  Default Row Labels: [";
	CKStringNode		*i = NULL;
	for (i = mDefaultRowLabels.getHead(); i != NULL; i = i->getNext()) {
		retval += " '";
		retval += (*i);
		retval += "'";
	}
	retval += " ]\n";

	retval += "  Default Column Headers: [";
	for (i = mDefaultColumnHeaders.getHead(); i != NULL; i = i->getNext()) {
		retval += " '";
		retval += (*i);
		retval += "'";
	}
	retval += " ]\n";

	for (CKDateTableMap::const_iterator i = mTables.begin(); i != mTables.end(); ++i) {
		retval += "  Date: ";
		retval += i->first;
		retval += "\n";
		retval += i->second.toString();
		retval += "\n";
	}

	retval += ">\n";

	return retval;
}


/*
 * There are times that it will be nice to be able to write out
 * this response to a file in a nice, orderly fashion. Also, there
 * will need to be the same for any subclasses. So, let's make this
 * method similar to the toString() but it takes a filename and
 * writes out the response's data to that file in a nice way.
 */
bool CKTimeTable::toFile( const CKString & aFile ) const
{
	bool		error = false;

	// first off, open the file for writing
	std::ofstream	dest(aFile.c_str());
	if (!dest) {
		error = true;
		std::ostringstream	msg;
		msg << "CKTimeTable::toFile(const CKString &) - while trying to open the "
			"file '" << aFile << "' an error occurred and I could not get it "
			"opened. This is a serious problem.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// lock up the map against change
	CKStackLocker	lockem(&(((CKTimeTable*)this)->mTablesMutex));

	/*
	 * Now I need to write out a header to the file so the user knows
	 * what it is that they are getting from me.
	 */
	if (!error) {
		dest << "Date\tRow";
		for (int i = 0; i < mDefaultColumnHeaders.size(); i++) {
			dest << "\t" << mDefaultColumnHeaders[i];
		}
		dest << std::endl;
	}

	/*
	 * Now let's put out each table for each date
	 */
	if (!error) {
		for (CKDateTableMap::const_iterator i = mTables.begin(); i != mTables.end(); ++i) {
			for (int row = 0; row < mDefaultRowCount; row++) {
				// put the date and the row label for this row
				dest << i->first << "\t" << mDefaultRowLabels[row];
				for (int col = 0; col < mDefaultColumnCount; col++) {
					// write out the value of this row and column
					CKVariant	& v = i->second.getValue(mDefaultRowLabels[row],
														 mDefaultColumnHeaders[col]);
					dest << "\t" << v.getValueAsString();

					// see if there's an error
					if (dest.bad()) {
						error = true;
						dest.close();
						break;
					}
				}
				dest << std::endl;
			}
		}
	}

	/*
	 * Now we can close out this guy and that's it.
	 */
	if (dest) {
		dest.close();
	}

	return !error;
}


/*
 * This method is nice in that it shows the data in the response
 * as a table where the first column is the date and the next
 * column is the row labels which are the symbols and the remaining
 * columns are the data in the request. The column headers are nice
 * and this makes a very nice way to 'tabularize' the data for
 * easy viewing.
 */
CKString CKTimeTable::toTableString() const
{
	// lock up the map against change
	CKStackLocker	lockem(&(((CKTimeTable*)this)->mTablesMutex));

	// next, put down the column headers
	CKString		retval = "Date\tRow";
	CKStringNode	*i = NULL;
	for (i = mDefaultColumnHeaders.getHead(); i != NULL; i = i->getNext()) {
		retval.append("\t").append(*((CKString *)i));
	}
	retval.append("\n");

	for (CKDateTableMap::const_iterator i = mTables.begin(); i != mTables.end(); ++i) {
		int		rowCnt = i->second.getNumRows();
		int		colCnt = i->second.getNumColumns();
		for (int r = 0; r < rowCnt; r++) {
			// put down the date and the row label
			retval.append(i->first).append("\t").append(i->second.getRowLabel(r));
			// now do the rest of the data in the table
			for (int c = 0; c < colCnt; c++) {
				CKVariant	& v = i->second.getValue(r, c);
				retval.append("\t").append(v.getValueAsString());
			}
			retval.append("\n");
		}
	}

	return retval;
}


/*
 * This method sets the list of default row labels to be equivalent
 * to the passed in list. This first clears out the existing list and
 * then copies in all the elements from the argument into the list
 * this instance maintains.
 */
void CKTimeTable::setDefaultRowLabels( const CKStringList & aList )
{
	mDefaultRowLabels = aList;
}


/*
 * This method sets the list of default column headers to be equivalent
 * to the passed in list. This first clears out the existing list and
 * then copies in all the elements from the argument into the list
 * this instance maintains.
 */
void CKTimeTable::setDefaultColumnHeaders( const CKStringList & aList )
{
	mDefaultColumnHeaders = aList;
}


/*
 * This method takes the supplied date (YYYYMMDD) and CKTable reference
 * and places the table into this instance's data structures at the
 * proper location for easy retrieval. This is a nice little
 * encapsulation method that makes dealing with the data structures
 * a little bit easier.
 */
void CKTimeTable::setTableForDate( long aDate, const CKTable & aTable )
{
	// lock up the map against any change
	CKStackLocker	lockem(&mTablesMutex);
	// set this guy using the standard '=' operator
	mTables[aDate] = aTable;
}


/*
 * When you want to get a table for a date regardless of whether
 * or not one is already there, then call this method and one will
 * be created and a reference to it returned to you if one is not
 * already available. This is very useful in populating data when
 * not having one is simply not what you want to 'stick' with. If
 * something very bad happens, this method will return NULL.
 */
CKTable *CKTimeTable::getOrCreateTableForDate( long aDate )
{
	bool		error = false;
	CKTable		*retval = NULL;

	// first, make sure the date makes some kind of sense
	if (!error) {
		if ((floor(aDate/10000) < 1900) || (floor(aDate/10000) > 2100) ||
			(aDate - floor(aDate/10000)*10000 < 101) ||
			(aDate - floor(aDate/10000)*10000 > 1231)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKTimeTable::getTableForDate(long) - the provided date: "
				<< aDate << " does not appear to be of the proper format: "
				"YYYYMMDD. Please confirm this before trying again.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, see if we have anything in the map for this date
	if (!error) {
		// lock up the list against change
		CKStackLocker	lockem(&mTablesMutex);

		// first, see if we're missing one. If we are, make one
		CKDateTableMap::iterator	i = mTables.find(aDate);
		if (i == mTables.end()) {
			// no table for this date, so make one
			mTables[aDate] = CKTable(mDefaultRowLabels, mDefaultColumnHeaders);
			// try to find it again
			i = mTables.find(aDate);
			if (i != mTables.end()) {
				retval = &(i->second);
			} else {
				error = true;
				std::ostringstream	msg;
				msg << "CKTimeTable::getTableForDate(long) - the table for the "
					"provided date: " << aDate << " was not in the data structure "
					"and a new one could not be made. This is a serious problem "
					"that needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} else {
			// found it right off! so get the reference
			retval = &(i->second);
		}
	}

	return error ? NULL : retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKTimeTable & aResponse )
{
	aStream << aResponse.toString();

	return aStream;
}
