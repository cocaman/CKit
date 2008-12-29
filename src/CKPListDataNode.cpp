/*
 * CKPListDataNode.cpp - this file implements a subclass of CKDataNode that's
 *                       focused on getting the data for the tree from a plist
 *                       file. Unlike the CKDBDataNode and CKFileDataNode
 *                       classes, there will NOT be any methods to process the
 *                       data from the file as the plist definition contains
 *                       all the structure necessary for building the tree.
 *
 * $Id: CKPListDataNode.cpp,v 1.2 2008/12/29 15:09:51 drbob Exp $
 */

//	System Headers
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/param.h>
#include <string.h>
#include <errno.h>

//	Third-Party Headers
#include <CKException.h>
#include <CKStackLocker.h>

//	Other Headers
#include "CKPListDataNode.h"
#include "CKPListDataNodeLoader.h"

//	Forward Declarations

//	Private Constants

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
CKPListDataNode::CKPListDataNode() :
	mFileName(),
	mLoadLock()
{
}


/*
 * This version of the constructor allows the file to be specified,
 * which will load the data from the file into the tree starting
 * at this node.
 */
CKPListDataNode::CKPListDataNode( const CKString & aFileName ) :
	mFileName(),
	mLoadLock()
{
	load(aFileName);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKPListDataNode::CKPListDataNode( CKPListDataNode & anOther ) :
	mFileName(),
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
CKPListDataNode::~CKPListDataNode()
{
	// clear out everything we created
	clearDataTree();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKPListDataNode & CKPListDataNode::operator=( CKPListDataNode & anOther )
{
	// first, let the super class handle all it's stuff
	CKDataNode::operator=(anOther);
	// now get a copy of what is special to us
	mFileName = anOther.mFileName;

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
void CKPListDataNode::setFileName( const CKString & aFileName )
{
	mFileName = aFileName;
}


/*
 * This method gets the filename that will be used to load the
 * data. This doesn't start the loading, just sets the filename.
 */
const CKString & CKPListDataNode::getFileName() const
{
	return mFileName;
}


/*
 * This method can be called to make sure that the data node tree
 * is not in the middle of a load that could make it's contents
 * unstable. If you ignore this method's return value you do so
 * at your own risk.
 */
bool CKPListDataNode::isLoadInProgress()
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
void CKPListDataNode::waitForLoad()
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
bool CKPListDataNode::clearDataTree()
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
bool CKPListDataNode::load()
{
	return load(getFileName());
}


/*
 * This method is used to load the data from the file into the
 * node/tree structure for later use. This one method
 * makes sure the file is there, reads in the lines and converts
 * them to the proper tree structure.
 */
bool CKPListDataNode::load( const CKString & aFileName )
{
	bool		error = false;

	// first thing is to indicate that a load is in progress
	CKStackLocker	lockem(&mLoadLock);

	// make sure that we have something to do
	if (!error) {
		if (aFileName.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::load(const CKString &) - the provided file "
				"name was empty and that means that there's nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * We need to read the file's contents into a string and
	 * then we can see if it's something of interest. If it is,
	 * then we need to clear out this node and everything below
	 * it and parse the string into a new structure.
	 */
	if (!error) {
		CKString	plist = loadFileToString(aFileName);
		if (plist.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::load(const CKString &) - the provided file '"
				<< aFileName << "' was empty, and nothing can be parsed from an "
				"empty file. Please check on this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// first, let's clear this guy out
			if (!clearDataTree()) {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::load(const CKString &) - while trying "
					"to clear out this instance of all data, an error occured. "
					"Please check the logs for a possible cause.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
			// now let's read in the new data
			if (!fromPList(plist)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::load(const CKString &) - the provided "
					"file '" << aFileName << "' had something, but it could not "
					"be parsed into a valid data tree. You probably need to "
					"check the logs for a clue, and then look at the file for "
					"the error in the structure.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// if all went well, then save everything
	if (!error) {
		mFileName = aFileName;
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
void CKPListDataNode::asyncLoad()
{
	CKPListDataNodeLoader::doLoad(this, "CKPListDataNode");
}


void CKPListDataNode::asyncLoad( const CKString & aFileName )
{
	// save the argument for processing
	mFileName = aFileName;
	// now fire off the async load
	CKPListDataNodeLoader::doLoad(this, "CKPListDataNode");
}


void CKPListDataNode::asyncLoad( const CKString & aFileName, const CKString & aName )
{
	// save the arguments for processing
	mFileName = aFileName;
	// now fire off the async load
	CKPListDataNodeLoader::doLoad(this, aName);
}


/*
 * This method takes the existing structure rooted at this
 * node and writes it out to the filename set previously for
 * this instance. This is atomic because we don't want someone
 * to mess with things while we're trying to write them out to
 * the filesystem.
 */
bool CKPListDataNode::save()
{
	return save(getFileName());
}


/*
 * This method takes the existing structure rooted at this
 * node and writes it out to the filename provided. This is
 * atomic because we don't want someone to mess with things
 * while we're trying to write them out to the filesystem.
 */
bool CKPListDataNode::save( const CKString & aFileName )
{
	bool		error = false;

	// make sure that we have something to do
	if (!error) {
		if (aFileName.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::save(const CKString &) - the "
				"provided file name was empty and that means that there's "
				"nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now let's make this structure into a string and save it
	if (!error) {
		CKString	plist = toPList();
		if (plist.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::save(const CKString &) - the structure "
				"rooted at this node created an empty plist - this makes no "
				"sense, and probably means there's an error in the processing. "
				"Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// write it out to the file
			if (!writeStringToFile(plist, aFileName)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::save(const CKString &) - while trying "
					"to write out the plist version of the structure rooted at "
					"this node to the file '" << aFileName << "', an error was "
					"encountered. Please check the logs for a possible cause.";
				throw CKException(__FILE__, __LINE__, msg.str());
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
 * This method checks to see if the two CKPListDataNode are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKPListDataNode::operator==( const CKPListDataNode & anOther ) const
{
	bool		equal = true;

	// check that the subclasses are the same
	if (!CKDataNode::operator==(anOther)) {
		equal = false;
	}

	// check the other things that are simple equalities
	if (mFileName != anOther.mFileName) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKPListDataNode are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKPListDataNode::operator!=( const CKPListDataNode & anOther ) const
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
CKString CKPListDataNode::toString() const
{
	CKString		retval = "(";

	// slap the database parameters out
	retval.append("FileName='");
	retval.append(mFileName);
	retval.append("'\n");

	// now do the node itself
	retval.append(CKDataNode::toString());
	retval.append(")");

	return retval;
}


/*
 * This method simply generates the plist string value for the tree
 * rooted at this node. It's what might be written to a file, but
 * it can be used for debugging and logging as well.
 */
CKString CKPListDataNode::toPListString() const
{
	return ((CKPListDataNode *)this)->toPList();
}


/********************************************************
 *
 *           PList/String Conversion Methods
 *
 ********************************************************/
/*
 * This method takes a plist held in a CKString and parses it
 * replacing everything in this tree structure with the structure
 * defined in the string. This string might be read in from a file
 * and it might come from some other source (web server?) so we
 * just make sure this is how we load it up.
 */
bool CKPListDataNode::fromPList( const CKString & aPList )
{
	bool		error = false;

	// first, make sure we have something to do
	if (!error) {
		if (aPList.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::fromPList(const CKString &) - the "
				"provided plist string is empty and that means that there's "
				"nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * The way we're going to do this is to fall back to the trusty
	 * world of C string processing. Basically, we know that the argument
	 * is NULL-terminated. That's a given. We can then use the (char *)
	 * to move through the string advancing and parsing as we go.
	 */
	if (!error) {
		for (char *pos = (char *)aPList.c_str(); pos[0] != '\0'; ++pos) {
			if (isspace(pos[0])) {
				// skip any whitespace
				continue;
			} else if (pos[0] == '{') {
				// OK, we have a plist to read
				if (!parseFromPList(pos)) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::fromPList(const CKString &) - while "
						"trying to parse a dictionary from the plist, an error "
						"was encountered. Please check the logs for a possible "
						"cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			} else {
				// this is a format error
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::fromPList(const CKString &) - while "
					"trying to parse the plist string, we came across the "
					"character '" << pos[0] << "' which is illegal in this "
					"level of the plist. Please check and make sure that the "
					"plist is properly formed.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	return !error;
}


/*
 * This method assumes that the pointer it's given is sitting on
 * a '{' to mark the start of a plist definition for this node.
 * If the first character is NOT a '{', then it's a syntax error.
 * The (char *) argument will be 'moved' (incremented) as this
 * method parses the string for values, and will be returned
 * sitting on the '}' that closes out the plist (dictionary)
 * entry.
 */
bool CKPListDataNode::parseFromPList( char * & aStr )
{
	bool		error = false;

	// first, make sure we have something to do
	if (!error) {
		if (aStr == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::parseFromPList(char * &) - the "
				"passed-in string is NULL and that should never happen. This "
				"probably means that there's been a massive parser error. "
				"Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, make sure we don't have a formatting error
			if (aStr[0] != '{') {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::parseFromPList(char * &) - the "
					"passed-in string does not start with an '{' - it starts with "
					"an '" << aStr[0] << "'. This is a formatting error as all "
					"plist dictionary entries have to start with a '{'";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// move past the '{' as we're done with it now
				++aStr;
			}
		}
	}

	/*
	 * At this point we need to scan the string picking off the keys
	 * and values as we go. If we get to the closing '}', then it's
	 * time to stop.
	 */
	if (!error) {
		CKString	buff;
		CKString	key;
		CKVariant	value;
		bool		inEscapedString = false;
		bool		forcedString = false;
		for (; aStr[0] != '\0'; ++aStr) {
			if (aStr[0] == '\\') {
				/*
				 * This is a manual escape character so I need to
				 * get the next character and put it in the buffer
				 * no matter what it is.
				 */
				if (aStr[1] != '\0') {
					buff.append(aStr[1]);
					++aStr;
				} else {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"while trying to add a manually escaped character to the "
						"buffer, a NULL was encountered. This is a serious problem "
						"that needs to be looked at as soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			} else if (inEscapedString) {
				/*
				 * If we're in a quoted string then everything goes
				 * into the buffer up to - but not including, the
				 * trailing quote.
				 */
				 if (aStr[0] == '"') {
					inEscapedString = false;
				 } else {
					buff.append(aStr[0]);
				 }
			} else if (aStr[0] == '"') {
				// this is the start of an escaped string
				inEscapedString = true;
				// ...and it'll have to be converted to a String
				forcedString = true;
			} else if (aStr[0] == '{') {
				/*
				 * The only place a dictionary fits in this scheme is as a
				 * node in the tree. This means that we already need to have
				 * seen the 'key' for this 'value'. So, check that. Then,
				 * create a new node, add it to me and give it the name of
				 * the key. Then let it parse itself from the stream.
				 */
				// make sure that we have some kind of key
				if (key.empty()) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"we ran into a '{' starting a map, but there is no defined "
						"key at this point, and there's no way a map can be a key "
						"in the CKPListDataNode. This is an illegal PList format.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
				// now create a new child node for me and name it w/ the key
				CKPListDataNode	*kid = new CKPListDataNode();
				if (kid == NULL) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"we ran into a new map for '" << key << "' but when we "
						"tried to create a new data node to represent it, we hit "
						"an allocation error. Please check on this as soon as "
						"possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					// set the parent of this new guy to me
					kid->setParent(this);
					// add him as one of my kids
					addChild(kid);
					// we also need to set his name
					kid->setName(key);
					// now we can have him parse the string for his data
					if (!kid->parseFromPList(aStr)) {
						error = true;
						std::ostringstream	msg;
						msg << "CKPListDataNode::parseFromPList(char * &) - "
							"we ran into a new map for '" << key << "' but when we "
							"tried to parse the structure into it we ran into an "
							"error. Please check the logs for a possible cause.";
						throw CKException(__FILE__, __LINE__, msg.str());
					}
				}
				// ...and clear everything out for the next one
				buff.clear();
				forcedString = false;
				key.clear();
				value.clearValue();
				// if the next thing is a ';', move to it
				if (aStr[1] == ';') {
					++aStr;
				}
			} else if ((aStr[0] == '(') || (aStr[0] == '[')) {
				/*
				 * The only place a list fits in this scheme is as a value
				 * on a node in the tree. This means that we already need to
				 * have seen the 'key' for this 'value'. So, check that.
				 * Then, parse the data into a variant list, add it to me
				 * and give it the name of the key.
				 */
				// make sure that we have some kind of key
				if (key.empty()) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"we ran into a '" << aStr[0] << "' starting a list, but "
						"there is no defined key at this point, and there's no "
						"way a list can be a key in the CKPListDataNode. This is "
						"an illegal PList format.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
				// get the right value out of the buffer
				CKVariantList		listValue;
				if (!parseList(aStr, listValue)) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"while trying to parse the value for a list we "
						"ran into a problem. Please check the logs for the cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					// save the key/value pair on this node
					putVar(key, CKVariant(&listValue));
				}
				// ...and clear everything out for the next one
				buff.clear();
				forcedString = false;
				key.clear();
				value.clearValue();
				// if the next thing is a ';', move to it
				if (aStr[1] == ';') {
					++aStr;
				}
			} else if ((aStr[0] == '=') && key.empty()) {
				/*
				 * This is the key/value separator meaning that we
				 * have the key in our posession and we need to do
				 * the right thing to it. Because of the limitations
				 * in the plist, the key has to be a string and the
				 * value has to be a variant. That means that we need
				 * to copy 'buff' into 'key' and reset 'buff' and
				 * clear the 'value' variant.
				 */
				key = buff;
				// let's clear out the temp variables we used
				buff.clear();
				forcedString = false;
				// we certainly don't have any value for this guy - yet
				value.clearValue();
			} else if (((aStr[0] == ';') || (aStr[0] == '\n')) &&
					   !key.empty() && !buff.empty()) {
				/*
				 * This is the character indicating that we're done
				 * with a key/value definition and we therefore need
				 * to process what we have on the 'value' and then
				 * put this mapping into our instance and get ready
				 * for the next go-round.
				 */
				// if it ends with a ';' drop it as it's extra crud
				if (buff[buff.size() - 1] == ';') {
					buff.erase(buff.size() - 1);
				}
				// get the right value out of the buffer
				if (!precipitateValue(buff, forcedString, value)) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseFromPList(char * &) - "
						"while trying to parse the value for '" << buff << "' we "
						"ran into a problem. Please check the logs for the cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					// save the key/value pair on this node
					putVar(key, value);
					// ...and clear everything out for the next one
					buff.clear();
					forcedString = false;
					key.clear();
					value.clearValue();
				}
			} else if (aStr[0] == '}') {
				/*
				 * If we have anything in the buffer we need to
				 * assume that the user forgot to put in the last
				 * ';' so let's do that now.
				 */
				if (!buff.empty() || !key.empty()) {
					// get the right value out of the buffer
					if (!precipitateValue(buff, forcedString, value)) {
						error = true;
						std::ostringstream	msg;
						msg << "CKPListDataNode::parseFromPList(char * &) - "
							"while trying to parse the value for '" << buff << "' we "
							"ran into a problem. Please check the logs for the cause.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						// save the key/value pair on this node
						putVar(key, value);
						// ...and clear everything out for the next one
						buff.clear();
						forcedString = false;
						key.clear();
						value.clearValue();
					}
				}
				// this is the end of the plist definition
				break;
			} else if (isspace(aStr[0])) {
				// skip all the whitespace
				continue;
			} else {
				// it's just another character to add to the buffer
				buff.append(aStr[0]);
			}
		}
	}

	return !error;
}


/*
 * This method assumes that the pointer it's given is sitting on
 * a '(' or '[' indicating the start of an array in the plist
 * definition. If the first character is NOT one of these, then
 * we have a formatting error that needs to be delt with. The
 * (char *) argument will be 'moved' (incremented) as this
 * method parses the string for values, and will be returned
 * sitting on the ')' or ']' that closes out the plist array
 * entry.
 */
bool CKPListDataNode::parseList( char * & aStr, CKVariantList & aList )
{
	bool		error = false;

	// first, make sure we have something to do
	char		terminalChar = ')';
	if (!error) {
		if (aStr == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
				"the passed-in string is NULL and that should never happen. "
				"This probably means that there's been a massive parser error. "
				"Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, make sure we don't have a formatting error
			if (aStr[0] == '[') {
				terminalChar = ']';
				// move past the '[' as we're done with it now
				++aStr;
			} else if (aStr[0] != '(') {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
					"the passed-in string does not start with an '(' or '[' - it "
					"starts with an '" << aStr[0] << "'. This is a formatting "
					"error as all plist list entries have to start with a '(' or "
					"'['.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// move past the '(' as we're done with it now
				++aStr;
			}
		}
	}

	/*
	 * Now we have a large loop where we'll be reading in the
	 * characters from the source and filling in the buffer of
	 * the value and then at a "," processing this into a
	 * new value in our List.
	 */
	if (!error) {
		CKString	buff;
		CKVariant	value;
		bool		inEscapedString = false;
		bool		forcedString = false;
		for (; aStr[0] != '\0'; ++aStr) {
			if (aStr[0] == '\\') {
				/*
				 * This is a manual escape character so I need to
				 * get the next character and put it in the buffer
				 * no matter what it is.
				 */
				if (aStr[1] != '\0') {
					buff.append(aStr[1]);
					++aStr;
				} else {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
						"while trying to add a manually escaped character to the "
						"buffer, a NULL was encountered. This is a serious problem "
						"that needs to be looked at as soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			} else if (inEscapedString) {
				/*
				 * If we're in a quoted string then everything goes
				 * into the buffer up to - but not including, the
				 * trailing quote.
				 */
				 if (aStr[0] == '"') {
					inEscapedString = false;
				 } else {
					buff.append(aStr[0]);
				 }
			} else if (aStr[0] == '"') {
				// this is the start of an escaped string
				inEscapedString = true;
				// ...and it'll have to be converted to a String
				forcedString = true;
			} else if (aStr[0] == '{') {
				/*
				 * The only place a dictionary fits in this scheme is as a
				 * node in the tree. This means that within a list, we can't
				 * have one. At least not now. We'd have to add the map as
				 * a datatype to the variant, and that's a lot of work. For
				 * now, we're simply going to log this as an error.
				 */
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
					"we ran into a '{' starting a map, but there is no defined "
					"key at this point, and there's no way a map can be a key "
					"in the CKPListDataNode. This is an illegal PList format.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else if ((aStr[0] == '(') || (aStr[0] == '[')) {
				/*
				 * This is another list, so let's parse this bad boy...
				 */
				CKVariantList		listValue;
				if (!parseList(aStr, listValue)) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
						"while trying to parse the value for a list we "
						"ran into a problem. Please check the logs for the cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					// add this sub-list to the main list
					aList.addToEnd(CKVariant(&listValue));
					// ...and clear everything out for the next one
					buff.clear();
					forcedString = false;
					value.clearValue();
				}
			} else if ((aStr[0] == ',') && !buff.empty()) {
				/*
				 * This is the character indicating that we're done
				 * with a value definition and we therefore need
				 * to process what we have on the 'value' and then
				 * add this mapping into our provided list and get
				 * ready for the next go-round.
				 */
				if (!precipitateValue(buff, forcedString, value)) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
						"while trying to parse the value for '" << buff << "' we "
						"ran into a problem. Please check the logs for the cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					// add this sub-list to the main list
					aList.addToEnd(value);
					// ...and clear everything out for the next one
					buff.clear();
					forcedString = false;
					value.clearValue();
				}
			} else if (aStr[0] == terminalChar) {
				/*
				 * If we have anything in the buffer we need to
				 * process that as we don't have a final ','
				 */
				if (!buff.empty()) {
					// get the right value out of the buffer
					if (!precipitateValue(buff, forcedString, value)) {
						error = true;
						std::ostringstream	msg;
						msg << "CKPListDataNode::parseList(char * &, CKVariantList &) - "
							"while trying to parse the final value for '" << buff
							<< "' we ran into a problem. Please check the logs "
							"for the cause.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						// add this sub-list to the main list
						aList.addToEnd(value);
						// ...and clear everything out for the next one
						buff.clear();
						forcedString = false;
						value.clearValue();
					}
				}
				// this is the end of the list definition
				break;
			} else if (isspace(aStr[0])) {
				// skip all the whitespace
				continue;
			} else {
				// it's just another character to add to the buffer
				buff.append(aStr[0]);
			}
		}
	}

	return !error;
}


/*
 * This method converts the existing data structure to a plist
 * string capable of handing to loadPList() and recreating the same
 * structure. This can then be used to send to something, or write
 * it out to a file.
 */
CKString CKPListDataNode::toPList()
{
	bool		error = false;
	CKString	retval;

	if (!error) {
		if (!appendAsPList(this, retval, 0)) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::appendAsPList(CKDataNode *, CKString &, int) - "
				"the passed-in node is NULL and that means that there's nothing "
				"I can do. Please make sure the argument is not NULL.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


/*
 * This method takes the provided node and adds it's data to the
 * plist string provided. The 'level' is used to indicate the
 * level of "indentation" on the string - strictly for readability.
 */
bool CKPListDataNode::appendAsPList( CKDataNode *aNode, CKString &aPList, int aLevel )
{
	bool		error = false;

	// make sure there's something to do
	bool	isSmall = false;
	if (!error) {
		if (aNode == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::appendAsPList(CKDataNode *, CKString &, int) - "
				"the passed-in node is NULL and that means that there's nothing "
				"I can do. Please make sure the argument is not NULL.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else if ((aNode->mKids.size() + aNode->mVars.size()) < 3) {
			isSmall = true;
		}
	}

	// start by adding in the start of the map
	if (!error) {
		aPList.append('{');
		if (!isSmall || (aLevel == 0)) {
			aPList.append('\n');
		}
	}

	/*
	 * The first thing we need to do is to run through all the key/value
	 * pairs that are assigned to this node. Nothing special, we just need
	 * to make sure that we add them all properly formatted.
	 */
	static CKString	sTabs("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");
	if (!error) {
		CKStackLocker		lockem(&aNode->mVarsMutex);

		if (!aNode->mVars.empty()) {
			std::map<CKString, CKVariant>::iterator	i;
			for (i = aNode->mVars.begin(); i != aNode->mVars.end(); ++i) {
				// put in the right level of indentation
				if (aLevel >= 0) {
					if (isSmall && (aLevel > 0)) {
						aPList.append(' ');
					} else {
						aPList.append(sTabs.substr(0,aLevel+1));
					}
				}
				// properly format the key for the output
				aPList.append(i->first);
				// add in the " = "...
				aPList.append(" = ");
				// properly format the value for the output
				if (!appendAsPList(i->second, aPList, (aLevel+1))) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::appendAsPList(CKDataNode *, CKString &, int) - "
						"while running through the values at level=" << aLevel
						<< " we were unable to add one to the plist. Please check "
						"the logs for a possible cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
				// finish off the key/value pair
				aPList.append(';');
				if (!isSmall || (aLevel == 0)) {
					aPList.append('\n');
				}
			}
		}
	}

	/*
	 * Now that we're done with the key/value pairs at this node,
	 * we need to repeat the process for each child of this node.
	 */
	if (!error) {
		CKStackLocker		lockem(&aNode->mKidsMutex);

		if (!aNode->mKids.empty()) {
			for (int i = 0; i < aNode->mKids.size(); ++i) {
				// put in the right level of indentation
				if (aLevel >= 0) {
					if (isSmall && (aLevel > 0)) {
						aPList.append(' ');
					} else {
						aPList.append(sTabs.substr(0,aLevel+1));
					}
				}
				// properly format the key for the output
				aPList.append(aNode->mKids[i]->getName());
				// add in the " = "...
				aPList.append(" = ");
				// properly format the child value for the output
				if (!appendAsPList(aNode->mKids[i], aPList, (aLevel+1))) {
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::appendAsPList(CKDataNode *, CKString &, int) - "
						"while running through the child nodes at level=" << aLevel
						<< " we were unable to add one to the plist. Please check "
						"the logs for a possible cause.";
					throw CKException(__FILE__, __LINE__, msg.str());
				}
				// finish off the key/value pair
				aPList.append(';');
				if (!isSmall || (aLevel == 0)) {
					aPList.append('\n');
				}
			}
		}
	}

	// if we're OK to this point, then we need to finish it off
	if (!error) {
		if (aLevel > 0) {
			if (isSmall) {
				aPList.append(' ');
			} else {
				aPList.append(sTabs.substr(0,aLevel));
			}
		}
		aPList.append('}');
	}

	return !error;
}


/*
 * This method takes the provided value and adds it to the
 * plist string provided. The 'level' is used to indicate the
 * level of "indentation" on the string - strictly for readability.
 */
bool CKPListDataNode::appendAsPList( CKVariant & aValue, CKString &aPList, int aLevel )
{
	bool		error = false;

	switch (aValue.getType()) {
		case eUnknownVariant:
			aPList.append("U:");
			break;
		case eStringVariant:
			/*
			 * This is interesting because we need to see if this
			 * string can be mistaken for any of the other datatypes.
			 * If it can, then we need to put double quotes around it
			 * to make sure that it isn't improperly read in next time.
			 * But to put them on everything that's a string looks
			 * clunky in the output.
			 */
			{
				const CKString	*src = aValue.getStringValue();
				if (src != NULL) {
					bool		quote = false;
					if (src->find(' ') >= 0) {
						quote = true;
					} else if ((src->find('(') >= 0) || (src->find(')') >= 0) ||
							   (src->find('[') >= 0) || (src->find(']') >= 0) ||
							   (src->find('{') >= 0) || (src->find('}') >= 0) ||
							   (src->find(',') >= 0) || (src->find(';') >= 0) ||
							   (src->find('=') >= 0)) {
						quote = true;
					} else {
						// check the string for ambiguous conversion
						bool		is_string = false;
						for (int i = 0; src->c_str()[i] != '\0'; ++i) {
							// skip everything that's a number
							if (isdigit(src->c_str()[i]) ||
								(src->c_str()[i] == '.') ||
								(tolower(src->c_str()[i]) == 'e')) {
								continue;
							}
							// if we're here, then it's a string
							is_string = true;
							break;
						}
						// if it's confused for a number, quote it
						if (!is_string) {
							quote = true;
						}
					}
					// now see if we need to qupte it
					if (quote) {
						aPList.append('"').append(src->c_str()).append('"');
					} else {
						aPList.append(src->c_str());
					}
				}
			}
			break;
		case eNumberVariant:
			aPList.append(aValue.getDoubleValue());
			break;
		case eDateVariant:
			{
				// we're going to use the default MM/DD/YYYY format
				long	when = aValue.getDateValue();
				int		yr = when/10000;
				int		mon = (when - yr*10000)/100;
				int		day = (when - (yr*100 + mon)*100);
				aPList.append(mon).append('/').append(day).append('/').append(yr);
			}
			break;
		case eTableVariant:
		case eTimeSeriesVariant:
		case ePriceVariant:
		case eTimeTableVariant:
			/*
			 * Because the encoded values for these variants can contain
			 * characters that will mess up the decoding process, we need
			 * to get the raw code and then scan it for troublesome chars
			 * and escape them so they are written out properly.
			 */
			{
				CKString	rawCode = aValue.generateCodeFromValues();
				CKString	escCode(rawCode.length());
				const char *raw = rawCode.c_str();
				for (int i = 0; raw[i] != '\0'; ++i) {
					if ((raw[i] == '(') || (raw[i] == ')') || (raw[i] == '[') ||
						(raw[i] == ']') || (raw[i] == '{') || (raw[i] == '}') ||
						(raw[i] == ',') || (raw[i] == ';') || (raw[i] == '=')) {
						escCode.append('\\');
					}
					// don't forget to add in the character
					escCode.append(raw[i]);
				}
				// now that every bad value is escaped, we can add this
				aPList.append(escCode);
			}
			break;
		case eListVariant:
			const CKVariantList	*list = aValue.getListValue();
			if (list == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::appendAsPList(CKVariant &, CKString &, int) - "
					"the value is supposed to be a list, but the list pointer "
					"is NULL. This is a serious data corruption problem that "
					"needs to be looked into.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// first, we need to add the start of the list
				aPList.append('[');
				// now we need to add in the elements
				CKVariantNode	*n = list->getHead();
				for (n = list->getHead(); n != NULL; n = n->getNext()) {
					// check to see if we need a ',' separator
					if (n != list->getHead()) {
						aPList.append(", ");
					}
					// slam down the value
					if (!appendAsPList(*n, aPList, aLevel)) {
						error = true;
						std::ostringstream	msg;
						msg << "CKPListDataNode::appendAsPList(CKVariant &, CKString &, int) - "
							"while expanding the list's elements, an error occured "
							"on: '" << ((CKVariant *)n) << "'. This is a real "
							"problem that needs to be looked at as soon as possible.";
						throw CKException(__FILE__, __LINE__, msg.str());
					}
				}
				// finally, we need to add the end of the list
				aPList.append(']');
			}
			break;
	}

	return !error;
}


/*
 * This method looks at the string 'aBuff' and places the correct
 * value into the variant 'aValue'. The flag is used to indicate
 * if the value must be a string regardless of what it may look
 * like. If everything goes as planned, the method will return a
 * 'true', but if there are any errors, then it'll return a 'false'.
 */
bool CKPListDataNode::precipitateValue( const CKString & aBuff, bool mustBeString, CKVariant & aValue )
{
	bool		error = false;
	bool		gotIt = false;

	// first, see if it's trivial - buffer is empty
	if (!error && !gotIt) {
		if (aBuff.empty()) {
			gotIt = true;
		}
	}

	// next, see if it's forced to be a String
	if (!error && !gotIt && mustBeString) {
		aValue = aBuff.c_str();
		gotIt = true;
	}

	/*
	 * First, let's see if the buffer contains an encoded variant. If
	 * it does, then decode it and we're done.
	 */
	if (!error && !gotIt) {
		const char *str = aBuff.c_str();
		if ((str[1] == ':') &&
			((str[0] == 'U') || (str[0] == 'S') || (str[0] == 'N') ||
			 (str[0] == 'D') || (str[0] == 'T') || (str[0] == 'L') ||
			 (str[0] == 'P') || (str[0] == 'A') || (str[0] == 'R'))) {
			aValue.takeValuesFromCode(aBuff);
			gotIt = true;
		}
	}

	/*
	 * OK, now it's a toughie... it's not obvious what this
	 * string is supposed to be so we're going to have a series
	 * of rules that should take care of the more obvious ones
	 * and if they don't, then we'll make them better. :)
	 */
	if (!error && !gotIt) {
		aBuff.trim();
		const char *str = aBuff.c_str();
		int			len = aBuff.size();
		if (((str[0] == '(') && (str[len-1] == ')')) ||
			((str[0] == '[') && (str[len-1] == ']'))) {
			// it's a List - comma separated values in the List
			char				*str = (char *)aBuff.c_str();
			CKVariantList		listValue;
			if (!parseList(str, listValue)) {
				error = true;
				std::ostringstream	msg;
				msg << "CKPListDataNode::precipitateValue(const CKString &, bool, CKVariant &) - "
					"while trying to parse the value for a list we "
					"ran into a problem. Please check the logs for the cause.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// set the value to this list
				aValue = listValue;
			}
		} else if ((str[0] == '<') && (str[len-1] == '>')) {
			// it's a byte stream - one big hex string
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::precipitateValue(const CKString &, bool, CKVariant &) - "
				"the value is '" << aBuff << "' which should be a series of "
				"bytes stored a hex digits. At this time this is not supported. "
				"Sorry.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			/*
			 * Now we need to scan the string itself as it could be a
			 * number or it could be a string. If it's got something other
			 * than a number in it, flag it and stop. If it's got a fraction
			 * or an exponent, then flag it as a double, but continue on.
			 */
			bool	is_string = false;
			int		slash_cnt = 0;
			int		dash_cnt = 0;
			bool	is_double = false;
			for (int i = 0; str[i] != '\0'; ++i) {
				// skip digits as they tell us nothing
				if (isdigit(str[i])) {
					continue;
				}
				// see if it has a '/' or a '-' for a date
				if (str[i] == '/') {
					++slash_cnt;
					continue;
				}
				if ((i > 0) && (str[i] == '-')) {
					++dash_cnt;
					continue;
				}
				// see if it's a fraction (possible exponent)
				if ((str[i] == '.') || (tolower(str[i]) == 'e')) {
					is_double = true;
					continue;
				}
				// if we're here, then it's a string
				is_string = true;
				break;
			}
			// now decode it...
			if (is_string) {
				aValue = aBuff;
			} else if ((slash_cnt > 0) || (dash_cnt > 0)) {
				if (((slash_cnt == 2) && (dash_cnt == 0)) ||
					((slash_cnt == 0) && (dash_cnt == 2))) {
					long	when = 0;
					// cut this into the three parts of a date
					CKStringList	parts = CKStringList::parseIntoChunks(aBuff,
													(slash_cnt == 2 ? "/" : "-"));
					// see which is biggest - first or last
					if (parts[0].intValue() > parts[2].intValue()) {
						// YYYY-MM-DD
						when = parts[0].intValue() * 10000
							  + parts[1].intValue() * 100
							  + parts[2].intValue();
					} else {
						// MM-DD-YYYY
						when = parts[2].intValue() * 10000
							  + parts[0].intValue() * 100
							  + parts[1].intValue();
					}
					// save the date
					aValue = when;
				} else {
					// the slashes or dashes weren't right - it's a string
					aValue = aBuff;
				}
			} else if (is_double) {
				aValue = aBuff.doubleValue();
			} else {
				aValue = aBuff.intValue();
			}
		}
	}

	return !error;
}


/********************************************************
 *
 *              String/File I/O Methods
 *
 ********************************************************/
/*
 * This method is simply going to take a file name, open it up
 * and read every character to the EOF into a new CKString and
 * send that back tot he caller. No state, so it's static.
 */
CKString CKPListDataNode::loadFileToString( const CKString & aFileName )
{
	bool		error = false;
	CKString	retval;

	// make sure that we have something to do
	if (!error) {
		if (aFileName.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::loadFileToString(const CKString &) - the "
				"provided file name was empty and that means that there's "
				"nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * There's no reason to get fancy. Simple C-style reading of all the
	 * characters in the file is all we need. We're simply going to read
	 * a character and then append it to the string. Nothing fancy.
	 */
	if (!error) {
		FILE	*fp = fopen(aFileName.c_str(), "r");
		if (fp == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::loadFileToString(const CKString &) - the "
				"provided file name '" << aFileName << "' could not be opened "
				"for reading: " << strerror(errno);
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			int		c;
			while ((c = fgetc(fp)) != EOF) {
				retval.append((char)c);
			}
			// close the file as we're done
			fclose(fp);
		}
	}

	return retval;
}


/*
 * This method takes the supplied string and writes it to the
 * supplied filename. This is just a way to blast a file with
 * the contents of a string. It's stateless, so that's why it's
 * static.
 */
bool CKPListDataNode::writeStringToFile( const CKString & aString,
										 const CKString & aFileName )
{
	bool		error = false;

	// make sure that we have something to do
	if (!error) {
		if (aString.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::writeFileToString(const CKString &, const CKString &) - "
				"the provided string was empty and that means that there's "
				"nothing I can do - I'm not going to write nothing to a file.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	if (!error) {
		if (aFileName.empty()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::writeFileToString(const CKString &, const CKString &) - "
				"the provided file name was empty and that means that there's "
				"nothing I can do.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * There's no reason to get fancy. Simple C-style writing of all the
	 * characters in the string to the file is all we need. We're simply
	 * going to open the file for writing and then slam it out.
	 */
	if (!error) {
		FILE	*fp = fopen(aFileName.c_str(), "w");
		if (fp == NULL) {
			error = true;
			std::ostringstream	msg;
			msg << "CKPListDataNode::writeFileToString(const CKString &, const CKString &) - "
				"the provided file name '" << aFileName << "' could not be opened "
				"for writing: " << strerror(errno);
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			const char	*data = aString.c_str();
			int			cnt = aString.size();
			for (int i = 0; i < cnt; ++i) {
				if (fputc(data[i], fp) == EOF) {
					// make sure to close what we have
					fclose(fp);
					// now log the error and throw the exception
					error = true;
					std::ostringstream	msg;
					msg << "CKPListDataNode::writeFileToString(const CKString &, const CKString &) - "
						"while trying to write the string to the provided file "
						"name '" << aFileName << "' an error occured: "
						<< strerror(errno);
					throw CKException(__FILE__, __LINE__, msg.str());
				}
			}
			// close the file as we're done
			fclose(fp);
		}
	}

	return !error;
}
