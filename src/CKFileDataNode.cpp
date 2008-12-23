/*
 * CKFileDataNode.h - this file defines a subclass of CKDataNode that's focused
 *                    on getting the data for the tree from a flat file. Like
 *                    the CKDBDataNode class, there will be a virtual method
 *                    for processing the current line in a CKString and that's
 *                    where each subclass will convert the data in the line to
 *                    a path and value for insertion into the tree.
 *
 * $Id: CKFileDataNode.cpp,v 1.2 2008/12/23 17:36:23 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <netdb.h>
#include <sys/param.h>

//	Third-Party Headers
#include <CKException.h>
#include <CKStackLocker.h>

//	Other Headers
#include "CKFileDataNode.h"
#include "CKFileDataNodeLoader.h"

//	Forward Declarations

//	Private Constants
/*
 * This sets the maximum line length that this guy can parse. I can't really
 * imagine of a properly formed text file with a line longer than this.
 */
#define	MAX_LINE_LENGTH		8096

//	Private Datatypes

//	Private Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that doesn't do a lot
 * other than to get ready for the user to fully initialize
 * it. This default instance needs to have the file specified.
 * Then the user needs to call the load() method to do the
 * work of loading the data from the file.
 */
CKFileDataNode::CKFileDataNode() :
	CKDataNode(),
	mFileName(),
	mDelimiter('\0'),
	mLastHeaderLine(NULL),
	mLoadLock()
{
}


/*
 * This version of the constructor allows the file to be specified,
 * as well as the delimiter for the data in the line and some data
 * on the line that we're to read past *before* starting to parse
 * the data on each subsequent line. This is very much like the flat
 * file parser in InfoShop, and it's worked very well for us there.
 * And with this the class is ready to load the data.
 */
CKFileDataNode::CKFileDataNode( const CKString & aFileName, char aDelim,
								const char *aLastHeaderLine ) :
	CKDataNode(),
	mFileName(),
	mDelimiter('\0'),
	mLastHeaderLine(NULL),
	mLoadLock()
{
	// set the values I have in the arguments
	mFileName = aFileName;
	mDelimiter = aDelim;
	mLastHeaderLine = aLastHeaderLine;
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKFileDataNode::CKFileDataNode( CKFileDataNode & anOther ) :
	CKDataNode(),
	mFileName(),
	mDelimiter('\0'),
	mLastHeaderLine(NULL),
	mLoadLock()
{
	// let the operator=() take care of all the details
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKFileDataNode::~CKFileDataNode()
{
	// clear out everything we created
	clearDataTree();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKFileDataNode & CKFileDataNode::operator=( CKFileDataNode & anOther )
{
	// first, let the super class handle all it's stuff
	CKDataNode::operator=(anOther);
	// now get a copy of what is special to us
	mFileName = anOther.mFileName;
	mDelimiter = anOther.mDelimiter;
	mLastHeaderLine = anOther.mLastHeaderLine;

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the filename that will be used to load the
 * data. This doesn't start the loading, just sets the filename.
 */
void CKFileDataNode::setFileName( const CKString & aFileName )
{
	mFileName = aFileName;
}


/*
 * This sets the delimiter for parsing up the line into chunks
 * so that the processing method can use either the 'raw' line
 * or the chunked up line to do it's work.
 */
void CKFileDataNode::setDelimiter( char aDelimiter )
{
	mDelimiter = aDelimiter;
}


/*
 * This method sets the string that will be considered to be the
 * last 'header' line in the file - if it's not NULL. If it's
 * NULL then the first line of the file is going to be considered
 * the first good line and processing will start there.
 */
void CKFileDataNode::setLastHeaderLine( const char *aLastHeaderLine )
{
	mLastHeaderLine = aLastHeaderLine;
}


/*
 * This method gets the filename that will be used to load the
 * data. This doesn't start the loading, just sets the filename.
 */
const CKString & CKFileDataNode::getFileName() const
{
	return mFileName;
}


/*
 * This gets the delimiter for parsing up the line into chunks
 * so that the processing method can use either the 'raw' line
 * or the chunked up line to do it's work.
 */
char CKFileDataNode::getDelimiter() const
{
	return mDelimiter;
}


/*
 * This method gets the string that will be considered to be the
 * last 'header' line in the file - if it's not NULL. If it's
 * NULL then the first line of the file is going to be considered
 * the first good line and processing will start there.
 */
const char *CKFileDataNode::getLastHeaderLine() const
{
	return mLastHeaderLine;
}


/*
 * This method can be called to make sure that the data node tree
 * is not in the middle of a load that could make it's contents
 * unstable. If you ignore this method's return value you do so
 * at your own risk.
 */
bool CKFileDataNode::isLoadInProgress()
{
	bool	locked = true;
	if (mLoadLock.tryLock()) {
		locked = false;
		mLoadLock.unlock();
	}
	return locked;
}


/*
 * This method will allow the caller to wait until the load of this
 * data node tree is done - if it's currently in process. If the
 * data is loaded then this will return immediately, if not, it
 * will wait until the data tree is stable and then return.
 */
void CKFileDataNode::waitForLoad()
{
	/*
	 * This is simple - if we're in the midst of a load then use the
	 * load lock to wait patiently until we are done. Once done, we
	 * will get the lock and then immediately release it because we
	 * don't need it as much as we just need to see that it's been
	 * unlocked by the loader and therefore we're ready to go.
	 */
	mLoadLock.lock();
	mLoadLock.unlock();
}


/*
 * This method clears out all the variables of the current node
 * as well as doing a deep delete on each of the children defined
 * on this node. That's because the most common way of populating
 * this tree is via the load() method and that builds nodes that
 * need to be deep deleted.
 */
bool CKFileDataNode::clearDataTree()
{
	bool		error = false;

	// first thing to do is to clear out all the variables at the top
	if (!error) {
		clearVars();
	}

	// now for each child do a deep delete
	if (!error) {
		// now lock up our list of kids
		CKStackLocker	lockem(getKidsMutex());
		// for each child, remove me as it's parent
		for (int i = 0; i < getKids()->size(); i++) {
			// delete the node - very deeply
			CKDataNode::deleteNodeDeep(getKids()->operator[](i));
		}
		// and clear out the list (all NULLs now)
		getKids()->clear();
	}

	return !error;
}


/********************************************************
 *
 *                Loading Methods
 *
 ********************************************************/
/*
 * This method is used to load the data from the file into the
 * node/tree structure for later use. This one method
 * makes sure the file is there, reads in the lines checking
 * for the defined header, and then calls the processCurrent()
 * method on every line after the header.
 */
bool CKFileDataNode::load()
{
	return load(mFileName, mDelimiter, mLastHeaderLine);
}


/*
 * This method is used to load the data from the file into the
 * node/tree structure for later use. This one method
 * makes sure the file is there, reads in the lines checking
 * for the defined header, and then calls the processCurrent()
 * method on every line after the header.
 */
bool CKFileDataNode::load( const CKString & aFileName, char aDelim,
						   const char *aLastHeaderLine )
{
	bool		error = false;

	// first thing is to indicate that a load is in progress
	CKStackLocker	lockem(&mLoadLock);

	// make sure that we have something to do
	if (!error) {
		if (aFileName.size() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFileDataNode::load(const CKString &) - the provided file "
				"name was empty and that means that there's nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// open up the file
	std::ifstream		src;
	if (!error) {
		src.open(aFileName.c_str());
		if (!src || !src.is_open()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFileDataNode::load(const CKString &) - the provided file '" <<
				aFileName << "' could not be opened for reading and that means "
				"that there's nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next thing to do is to clear out all the variables and children
	if (!error) {
		if (!clearDataTree()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKFileDataNode::load(const CKString &) - while trying to "
				"clear out the tree of all existing data we ran into a problem. "
				"Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now we need to loop on each returned row and call processCurrent()
	if (!error) {
		bool	inHeader = (aLastHeaderLine != NULL ? true : false);
		char	c;
		char	buff[MAX_LINE_LENGTH];
		while (!error && !src.eof()) {
			// read in a line into the buffer that should be long enough
			src.get(buff, MAX_LINE_LENGTH, '\n');
			// see if we didn't have a long enough buffer
			if (src.get(c) && (c != '\n')) {
				error = true;
				std::ostringstream	msg;
				msg << "CKFileDataNode::load(const CKString &) - while trying to "
					"parse the data from the file '" << aFileName << "' we ran into "
					"a line that was longer than " << MAX_LINE_LENGTH << " and this "
					"means the code needs to be fixed to allow longer lines.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// now make a string of this guy for easy manipulation
			CKString		line = buff;
			if (line.size() == 0) {
				continue;
			}

			// see if we need to skip this as it's the header
			if (inHeader) {
				if (line.find(aLastHeaderLine) >= 0) {
					inHeader = false;
				}
				continue;
			}

			// now chunk this line up into all it's parts
			CKStringList	chunks;
			if (aDelim != '\0') {
				chunks = CKStringList::parseIntoChunks(buff, aDelim);
			}

			// now let the subclass handle the processing of the line
			error = !processCurrent(line, chunks);
		}
	}

	// if all went well, then save everything
	if (!error) {
		mFileName = aFileName;
		mDelimiter = aDelim;
		mLastHeaderLine = aLastHeaderLine;
	}

	return !error;
}


/*
 * This method is a gem. It fires off an asynchronous load of the
 * data node tree so that the caller can get back to doing what
 * it wants to do without having to wait for the loading to finish
 * before moving on.
 *
 * A new thread is fired off and that new thread will call the
 * load() method to get the data loaded into the data node tree.
 */
void CKFileDataNode::asyncLoad()
{
	CKFileDataNodeLoader::doLoad(this, "CKFileDataNode");
}


void CKFileDataNode::asyncLoad( const CKString & aFileName, char aDelim,
								const char *aLastHeaderLine )
{
	// save the arguments for processing
	mFileName = aFileName;
	mDelimiter = aDelim;
	mLastHeaderLine = aLastHeaderLine;
	// now fire off the async load
	CKFileDataNodeLoader::doLoad(this, "CKFileDataNode");
}


void CKFileDataNode::asyncLoad( const CKString & aFileName, const CKString & aName,
								char aDelim, const char *aLastHeaderLine )
{
	// save the arguments for processing
	mFileName = aFileName;
	mDelimiter = aDelim;
	mLastHeaderLine = aLastHeaderLine;
	// now fire off the async load
	CKFileDataNodeLoader::doLoad(this, aName);
}


/*
 * This method processes the 'current' data in the line that
 * represents a single line/record from the flat file fetch. This
 * is very useful because subclasses only really need to provide
 * the file name and delimiter, and then implement this method on
 * what to do with one row of data and that's it. This method will
 * be called by load() for each record so as to process the entire
 * file.
 */
bool CKFileDataNode::processCurrent( CKString & aLine, CKStringList & allElems )
{
	bool		error = false;

	/*
	 * This method doesn't so much as it's up to the subclasses to
	 * deal with the data in each record - we're just building a
	 * decent framework to make it easy. Nonetheless, we shouldn't
	 * be silly and return an error...
	 */

	return !error;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKFileDataNode are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKFileDataNode::operator==( const CKFileDataNode & anOther ) const
{
	bool		equal = true;

	// check that the subclasses are the same
	if (!CKDataNode::operator==(anOther)) {
		equal = false;
	}

	// check the other things that are simple equalities
	if ((mFileName != anOther.mFileName) ||
		(mDelimiter != anOther.mDelimiter) ||
		(mLastHeaderLine != anOther.mLastHeaderLine)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKFileDataNode are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKFileDataNode::operator!=( const CKFileDataNode & anOther ) const
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
CKString CKFileDataNode::toString() const
{
	CKString		retval = "(";

	// slap the database parameters out
	retval.append("FileName='");
	retval.append(mFileName);
	retval.append("', Delimiter='");
	retval.append(mDelimiter);
	retval.append("', LastHeaderLine='");
	retval.append(mLastHeaderLine);
	retval.append("'\n");

	// now do the node itself
	retval.append(CKDataNode::toString());
	retval.append(")");

	return retval;
}
