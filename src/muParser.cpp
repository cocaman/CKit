/*
  Copyright (C) 2004 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify,
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "muParser.h"
#include "CKVariant.h"
#include "CKTable.h"
#include "CKTimeSeries.h"
#include "CKPrice.h"
#include "CKTimeTable.h"

#include <cmath>
#include <algorithm>
#include <numeric>

/***/
#define PARSER_CONST_PI  3.141592653589793238462643

/** \brief The eulerian number. */
#define PARSER_CONST_E   2.718281828459045235360287

/** \brief Namespace for mathematical applications. */
namespace MathUtils
{

// Trigonometric function
Parser::value_type Parser::Sin(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(sin(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Sin(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], sin(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(sin(work.getUSD()));
					work.setNative(sin(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Sin(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Sin(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Cos(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(cos(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Cos(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], cos(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(cos(work.getUSD()));
					work.setNative(cos(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Cos(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Cos(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Tan(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(tan(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Tan(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], tan(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(tan(work.getUSD()));
					work.setNative(tan(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Tan(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Tan(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ASin(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(asin(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ASin(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], asin(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(asin(work.getUSD()));
					work.setNative(asin(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ASin(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ASin(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ACos(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(acos(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ACos(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], acos(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(acos(work.getUSD()));
					work.setNative(acos(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ACos(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ACos(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ATan(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(atan(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ATan(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], atan(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(atan(work.getUSD()));
					work.setNative(atan(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ATan(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ATan(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Sinh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(sinh(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Sinh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], sinh(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(sinh(work.getUSD()));
					work.setNative(sinh(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Sinh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Sinh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Cosh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(cosh(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Cosh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], cosh(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(cosh(work.getUSD()));
					work.setNative(cosh(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Cosh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Cosh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Tanh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(tanh(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Tanh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], tanh(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(tanh(work.getUSD()));
					work.setNative(tanh(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Tanh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Tanh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ASinh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(log(x + sqrt(x * x + 1.0)));
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ASinh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						double	x = work.get(dates[i]);
						work.put(dates[i], log(x + sqrt(x * x + 1.0)));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					double	x = work.getUSD();
					work.setUSD(log(x + sqrt(x * x + 1.0)));
					x = work.getNative();
					work.setNative(log(x + sqrt(x * x + 1.0)));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ASinh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ASinh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ACosh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(log(x + sqrt(x * x - 1.0)));
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ACosh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						double	x = work.get(dates[i]);
						work.put(dates[i], log(x + sqrt(x * x - 1.0)));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					double	x = work.getUSD();
					work.setUSD(log(x + sqrt(x * x - 1.0)));
					x = work.getNative();
					work.setNative(log(x + sqrt(x * x - 1.0)));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ACosh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ACosh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::ATanh(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue((double)0.5 * log((1.0 + x)/(1.0 - x)));
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, ATanh(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						double	x = work.get(dates[i]);
						work.put(dates[i], (double)0.5 * log((1.0 + x)/(1.0 - x)));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					double	x = work.getUSD();
					work.setUSD((double)0.5 * log((1.0 + x)/(1.0 - x)));
					x = work.getNative();
					work.setNative((double)0.5 * log((1.0 + x)/(1.0 - x)));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(ATanh(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, ATanh(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

// Logarithm functions
Parser::value_type Parser::Log2(Parser::value_type & v)
{
	// Logarithm base 2
	Parser::value_type	retval;
	// these may be used depending on the type of variant we have
	double		y = log((double) 2.0);
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(log(x)/y);
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Log2(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						double	x = work.get(dates[i]);
						work.put(dates[i], log(x)/y);
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(log(work.getUSD())/y);
					work.setNative(log(work.getNative())/y);
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Log2(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Log2(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Log10(Parser::value_type & v)
{
	// Logarithm base 10
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(log10(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Log10(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], log10(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(log10(work.getUSD()));
					work.setNative(log10(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Log10(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Log10(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Ln(Parser::value_type & v)
{
	// Logarithm base e (natural logarithm)
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(log(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Ln(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], log(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(log(work.getUSD()));
					work.setNative(log(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Ln(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Ln(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

//  misc
Parser::value_type Parser::Exp(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(exp(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Exp(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], exp(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(exp(work.getUSD()));
					work.setNative(exp(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Exp(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Exp(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Abs(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(fabs(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Abs(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], fabs(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(fabs(work.getUSD()));
					work.setNative(fabs(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Abs(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Abs(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Sqrt(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(sqrt(v.getDoubleValue()));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Sqrt(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], sqrt(work.get(dates[i])));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(sqrt(work.getUSD()));
					work.setNative(sqrt(work.getNative()));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Sqrt(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Sqrt(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Rint(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(floor(v.getDoubleValue() + (double)0.5));
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Rint(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						work.put(dates[i], floor(work.get(dates[i]) + (double)0.5));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work.setUSD(floor(work.getUSD() + (double)0.5));
					work.setNative(floor(work.getNative() + (double)0.5));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Rint(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Rint(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Sign(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(((x<0) ? (double)-1.0 : (x>0) ? (double)1.0 : (double)0.0));
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Sign(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					CKVector<double>	dates = work.getDateTimes();
					int					cnt = dates.size();
					for (int i = 0; i < cnt; i++) {
						double	x = work.get(dates[i]);
						work.put(dates[i], ((x<0) ? (double)-1.0 : (x>0) ? (double)1.0 : (double)0.0));
					}
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					double	x = work.getUSD();
					work.setUSD(((x<0) ? (double)-1.0 : (x>0) ? (double)1.0 : (double)0.0));
					x = work.getNative();
					work.setNative(((x<0) ? (double)-1.0 : (x>0) ? (double)1.0 : (double)0.0));
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Sign(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Sign(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

// string functions
Parser::value_type Parser::UpperStr(value_type & arg)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = *str;
					// now operate on the copy
					newbie.toUpper();
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, UpperStr(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(UpperStr(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, UpperStr(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::LowerStr(value_type & arg)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = *str;
					// now operate on the copy
					newbie.toLower();
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, LowerStr(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(LowerStr(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, LowerStr(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::TrimStr(value_type & arg)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = *str;
					// now operate on the copy
					newbie.trim();
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, TrimStr(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(TrimStr(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, TrimStr(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::RightStr(value_type & arg, value_type & num)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = str->right(num.getIntValue());
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, RightStr(work.getValue(r,c), num));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(RightStr(*n, num));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, RightStr(tbl.getValue(r,c), num));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::LeftStr(value_type & arg, value_type & num)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = str->left(num.getIntValue());
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, LeftStr(work.getValue(r,c), num));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(LeftStr(*n, num));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, LeftStr(tbl.getValue(r,c), num));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::MidStr(value_type & arg, value_type & start, value_type & end)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					// first, copy the string to a new string
					CKString	newbie = str->mid(start.getIntValue(), end.getIntValue());
					// ...and save it in the result
					retval.setStringValue(&newbie);
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, MidStr(work.getValue(r,c), start, end));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(MidStr(*n, start, end));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, MidStr(tbl.getValue(r,c), start, end));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::LenStr(value_type & arg)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (arg.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			{
				const CKString	*str = arg.getStringValue();
				if (str != NULL) {
					retval.setDoubleValue((double)str->length());
				}
			}
			break;
		case eNumberVariant:
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = arg.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, LenStr(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			break;
		case ePriceVariant:
			break;
		case eListVariant:
			{
				const CKVariantList	*list = arg.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(LenStr(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = arg.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, LenStr(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

// Conditional (if then else)
Parser::value_type Parser::Ite(Parser::value_type & v1, Parser::value_type & v2, Parser::value_type & v3)
{
	return (v1 != (double)0.0) ? v3 : v2;
}

// Unary operator Callbacks: Postfix operators
Parser::value_type Parser::Milli(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(x/(double)1.0e3);
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Milli(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					work *= 1.0e-3;
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work *= 1.0e-3;
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Milli(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Milli(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Nano(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(x/(double)1.0e9);
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Nano(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					work *= 1.0e-9;
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work *= 1.0e-9;
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Nano(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Nano(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Micro(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			{
				double	x = v.getDoubleValue();
				retval.setDoubleValue(x/(double)1.0e6);
			}
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, Micro(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the price
					CKTimeSeries	work = *ser;
					// now operate on each element in the price
					work *= 1.0e-6;
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the time series
					CKPrice		work = *pr;
					// now operate on each element in the table
					work *= 1.0e-6;
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(Micro(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, Micro(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

// Unary operator Callbacks: Infix operators
Parser::value_type Parser::UnaryMinus(Parser::value_type & v)
{
	Parser::value_type	retval;
	// now decide what to do based on the type of variant we have
	switch (v.getType()) {
		case eUnknownVariant:
			break;
		case eStringVariant:
			break;
		case eNumberVariant:
			retval.setDoubleValue(-1.0 * v.getDoubleValue());
			break;
		case eDateVariant:
			break;
		case eTableVariant:
			{
				const CKTable	*tbl = v.getTableValue();
				if (tbl != NULL) {
					// first, copy the table
					CKTable		work = *tbl;
					// now operate on each element in the table
					int	rows = work.getNumRows();
					int	cols = work.getNumColumns();
					for (int r = 0; r < rows; r++) {
						for (int c = 0; c < cols; c++) {
							work.setValue(r, c, UnaryMinus(work.getValue(r,c)));
						}
					}
					// finally, save this as the result
					retval.setTableValue(&work);
				}
			}
			break;
		case eTimeSeriesVariant:
			{
				const CKTimeSeries	*ser = v.getTimeSeriesValue();
				if (ser != NULL) {
					// first, copy the time series
					CKTimeSeries	work = *ser;
					// now operate on each element in the series
					work *= -1.0;
					// finally, save this as the result
					retval.setTimeSeriesValue(&work);
				}
			}
			break;
		case ePriceVariant:
			{
				const CKPrice	*pr = v.getPriceValue();
				if (pr != NULL) {
					// first, copy the price
					CKPrice		work = *pr;
					// now operate on each element in the price
					work *= -1.0;
					// finally, save this as the result
					retval.setPriceValue(&work);
				}
			}
			break;
		case eListVariant:
			{
				const CKVariantList	*list = v.getListValue();
				if (list != NULL) {
					// make a new list for copy into
					CKVariantList	work;
					// now process the source a node at a time
					for (CKVariantNode *n = list->getHead(); n != NULL; n = n->getNext()) {
						work.addToEnd(UnaryMinus(*n));
					}
					// finally, save this as the result
					retval.setListValue(&work);
				}
			}
			break;
		case eTimeTableVariant:
			{
				const CKTimeTable	*timeTbl = v.getTimeTableValue();
				if (timeTbl != NULL) {
					// make a new list for copy into
					CKTimeTable	work;
					// get the list of dates in the source table
					CKVector<long>	dates = timeTbl->getDateValues();
					for (int d = 0; d < dates.size(); d++) {
						// get the next date in the series for the table
						long	when = dates[d];
						CKTable	*what = timeTbl->getTableForDate(when);
						if (what != NULL) {
							// first, copy the table
							CKTable		tbl = *what;
							// now operate on each element in the table
							int	rows = tbl.getNumRows();
							int	cols = tbl.getNumColumns();
							for (int r = 0; r < rows; r++) {
								for (int c = 0; c < cols; c++) {
									tbl.setValue(r, c, UnaryMinus(tbl.getValue(r,c)));
								}
							}
							// finally, save this as the result
							work.setTableForDate(when, tbl);
						}
					}
					// finally, save this as the result
					retval.setTimeTableValue(&work);
				}
			}
			break;
	}
	return retval;
}

Parser::value_type Parser::Not(Parser::value_type & v)
{
	return Parser::value_type((double)(v == 0));
}

// Functions with variable number of arguments
// sum
Parser::value_type Parser::Sum(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function sum.");

	Parser::value_type	sum((double)0.0);
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		sum += (*i);
	}
	return sum;
}

// mean value
Parser::value_type Parser::Avg(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function avg.");

	Parser::value_type	sum((double)0.0);
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		sum += (*i);
	}
	sum /= (double)a_vArg.size();
	return sum;
}

// minimum
Parser::value_type Parser::Min(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function min.");

	double	min = NAN;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		if (isnan(min) || (min > i->getDoubleValue())) {
			min = i->getDoubleValue();
		}
	}
	return Parser::value_type(min);
}

// maximum
Parser::value_type Parser::Max(const std::vector<Parser::value_type> &a_vArg)
{
	if (!a_vArg.size())
		throw MathUtils::ParserException("too few arguments for function max.");

	double	max = NAN;
	std::vector<Parser::value_type>::const_iterator	i;
	for (i = a_vArg.begin(); i != a_vArg.end(); ++i) {
		if (isnan(max) || (max < i->getDoubleValue())) {
			max = i->getDoubleValue();
		}
	}
	return Parser::value_type(max);
}


//---------------------------------------------------------------------------
/** \brief Constructor.

  Call ParserBase class constructor and trigger Function, Operator and Constant initialization.
*/
Parser::Parser() :
	ParserBase(),
	m_fEpsilon((Parser::value_type)1e-7)
{
	InitFun();
	InitConst();
	InitOprt();
}

//---------------------------------------------------------------------------
/** \brief Initialize the default functions. */
void Parser::InitFun()
{
	// trigonometric functions
	AddFun("sin", Sin);
	AddFun("cos", Cos);
	AddFun("tan", Tan);
	// arcus functions
	AddFun("asin", ASin);
	AddFun("acos", ACos);
	AddFun("atan", ATan);
	// hyperbolic functions
	AddFun("sinh", Sinh);
	AddFun("cosh", Cosh);
	AddFun("tanh", Tanh);
	// arcus hyperbolic functions
	AddFun("asinh", ASinh);
	AddFun("acosh", ACosh);
	AddFun("atanh", ATanh);
	// Logarithm functions
	AddFun("log2", Log2);
	AddFun("log10", Log10);
	AddFun("log", Log10);
	AddFun("ln", Ln);
	// misc
	AddFun("exp", Exp);
	AddFun("sqrt", Sqrt);
	AddFun("sign", Sign);
	AddFun("rint", Rint);
	AddFun("abs", Abs);
	AddFun("if", Ite);
	// string functions
	AddFun("upper$", UpperStr);
	AddFun("lower$", LowerStr);
	AddFun("trim$", TrimStr);
	AddFun("right$", RightStr);
	AddFun("left$", LeftStr);
	AddFun("mid$", MidStr);
	AddFun("length", LenStr);

	// Functions with variable number of arguments
	AddFun("sum", Sum);
	AddFun("avg", Avg);
	AddFun("min", Min);
	AddFun("max", Max);
}

//---------------------------------------------------------------------------
/** \brief Initialize operators. */
void Parser::InitConst()
{
	AddConst("_pi", (Parser::value_type)PARSER_CONST_PI);
	AddConst("_e", (Parser::value_type)PARSER_CONST_E);
}

//---------------------------------------------------------------------------
/** \brief Initialize operators. */
void Parser::InitOprt()
{
	AddPostfixOp("m", Milli);
	AddPostfixOp("n", Nano);
	AddPostfixOp("mu", Micro);

	AddPrefixOp("-", UnaryMinus);
	AddPrefixOp("!", Not);
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for identifier names. */
const Parser::char_type* Parser::ValidNameChars() const
{
	return "$0123456789_.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for operator identifiers. */
const Parser::char_type* Parser::ValidOprtChars() const
{
	return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-*^/?<>=#!$%&|~'_";
}

//---------------------------------------------------------------------------
/** \brief Return characters valid for prefix operators. */
const Parser::char_type* Parser::ValidPrefixOprtChars() const
{
	return "/+-*^?<>=#!$%&|~'_";
}

//---------------------------------------------------------------------------
/** \brief Numerically differentiate with regard to a variable. */
Parser::value_type Parser::Diff(Parser::value_type *a_Var, Parser::value_type a_fPos) const
{
	assert(m_fEpsilon.getDoubleValue());
	Parser::value_type	fEpsilon( (a_fPos==0) ? (double)1e-10 : m_fEpsilon.getDoubleValue() * a_fPos.getDoubleValue() );
	Parser::value_type	fRes(0);
	Parser::value_type	fBuf(*a_Var);
	Parser::value_type	f[4] = {0,0,0,0};

	*a_Var = a_fPos.getDoubleValue() + 2.0*fEpsilon.getDoubleValue();
	f[0] = Calc();
	*a_Var = a_fPos.getDoubleValue() + 1.0*fEpsilon.getDoubleValue();
	f[1] = Calc();
	*a_Var = a_fPos.getDoubleValue() - 1.0*fEpsilon.getDoubleValue();
	f[2] = Calc();
	*a_Var = a_fPos.getDoubleValue() - 2.0*fEpsilon.getDoubleValue();
	f[3] = Calc();
	*a_Var = fBuf; // restore variable

	fRes = (-1.0*f[0].getDoubleValue() + 8.0*f[1].getDoubleValue() - 8.0*f[2].getDoubleValue() + f[3].getDoubleValue()) / (12.0*fEpsilon.getDoubleValue());
	return fRes;
}


} // namespace MathUtils
