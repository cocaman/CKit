/*
 * CKFileDataNode.h - this file defines a subclass of CKDataNode that's focused
 *                    on getting the data for the tree from a flat file. Like
 *                    the CKDBDataNode class, there will be a virtual method
 *                    for processing the current line in a CKString and that's
 *                    where each subclass will convert the data in the line to
 *                    a path and value for insertion into the tree.
 *
 * $Id: CKFileDataNode.h,v 1.1 2008/01/17 16:26:59 drbob Exp $
 */
#ifndef __CKFILEDATANODE_H
#define __CKFILEDATANODE_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <string>
#include <vector>

//	Third-Party Headers
#include <CKDataNode.h>
#include <CKString.h>
#include <CKFWMutex.h>

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFileDataNode :
	public CKDataNode
{
	public:
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
		CKFileDataNode();
		/*
		 * This version of the constructor allows the file to be specified,
		 * as well as the delimiter for the data in the line and some data
		 * on the line that we're to read past *before* starting to parse
		 * the data on each subsequent line. This is very much like the flat
		 * file parser in InfoShop, and it's worked very well for us there.
		 * And with this the class is ready to load the data.
		 */
		CKFileDataNode( const CKString & aFileName, char aDelim = '\t',
						const char *aLastHeaderLine = NULL );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKFileDataNode( CKFileDataNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKFileDataNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKFileDataNode & operator=( CKFileDataNode & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method sets the filename that will be used to load the
		 * data. This doesn't start the loading, just sets the filename.
		 */
		void setFileName( const CKString & aFileName );
		/*
		 * This sets the delimiter for parsing up the line into chunks
		 * so that the processing method can use either the 'raw' line
		 * or the chunked up line to do it's work.
		 */
		void setDelimiter( char aDelimiter );
		/*
		 * This method sets the string that will be considered to be the
		 * last 'header' line in the file - if it's not NULL. If it's
		 * NULL then the first line of the file is going to be considered
		 * the first good line and processing will start there.
		 */
		void setLastHeaderLine( const char *aLastHeaderLine );

		/*
		 * This method gets the filename that will be used to load the
		 * data. This doesn't start the loading, just sets the filename.
		 */
		const CKString & getFileName() const;
		/*
		 * This gets the delimiter for parsing up the line into chunks
		 * so that the processing method can use either the 'raw' line
		 * or the chunked up line to do it's work.
		 */
		char getDelimiter() const;
		/*
		 * This method gets the string that will be considered to be the
		 * last 'header' line in the file - if it's not NULL. If it's
		 * NULL then the first line of the file is going to be considered
		 * the first good line and processing will start there.
		 */
		const char *getLastHeaderLine() const;

		/*
		 * This method can be called to make sure that the data node tree
		 * is not in the middle of a load that could make it's contents
		 * unstable. If you ignore this method's return value you do so
		 * at your own risk.
		 */
		virtual bool isLoadInProgress();
		/*
		 * This method will allow the caller to wait until the load of this
		 * data node tree is done - if it's currently in process. If the
		 * data is loaded then this will return immediately, if not, it
		 * will wait until the data tree is stable and then return.
		 */
		virtual void waitForLoad();

		/*
		 * This method clears out all the variables of the current node
		 * as well as doing a deep delete on each of the children defined
		 * on this node. That's because the most common way of populating
		 * this tree is via the load() method and that builds nodes that
		 * need to be deep deleted.
		 */
		bool clearDataTree();

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
		virtual bool load();
		/*
		 * This method is used to load the data from the file into the
		 * node/tree structure for later use. This one method
		 * makes sure the file is there, reads in the lines checking
		 * for the defined header, and then calls the processCurrent()
		 * method on every line after the header.
		 */
		virtual bool load( const CKString & aFileName, char aDelim = '\t',
						   const char *aLastHeaderLine = NULL );
		/*
		 * This method is a gem. It fires off an asynchronous load of the
		 * data node tree so that the caller can get back to doing what
		 * it wants to do without having to wait for the loading to finish
		 * before moving on.
		 *
		 * A new thread is fired off and that new thread will call the
		 * load() method to get the data loaded into the data node tree.
		 */
		virtual void asyncLoad();
		virtual void asyncLoad( const CKString & aFileName, char aDelim = '\t',
								const char *aLastHeaderLine = NULL );
		virtual void asyncLoad( const CKString & aFileName, const CKString & aName,
								char aDelim = '\t', const char *aLastHeaderLine = NULL );

		/*
		 * This method processes the 'current' data in the line that
		 * represents a single line/record from the flat file fetch. This
		 * is very useful because subclasses only really need to provide
		 * the file name and delimiter, and then implement this method on
		 * what to do with one row of data and that's it. This method will
		 * be called by load() for each record so as to process the entire
		 * file.
		 */
		virtual bool processCurrent( CKString & aLine, CKStringList & allElems );

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
		bool operator==( const CKFileDataNode & anOther ) const;
		/*
		 * This method checks to see if the two CKFileDataNode are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKFileDataNode & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	private:
		/*
		 * These are the important parameters for the flat file loading -
		 * the file name, the possible delimiter, and the possible last
		 * line of the header. If these last two are NULL then that means
		 * that this instance should not try to decode them, but rather
		 * pass everything to the subclass' processCurrent().
		 */
		CKString			mFileName;
		char				mDelimiter;
		const char			*mLastHeaderLine;
		/*
		 * This mutex will be used on the load() method to make sure that
		 * we are safe about loading all the data before starting to do
		 * any processing.
		 */
		CKFWMutex			mLoadLock;
};

#endif	// __CKFILEDATANODE_H
