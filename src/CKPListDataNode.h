/*
 * CKPListDataNode.h - this file defines a subclass of CKDataNode that's
 *                     focused on getting the data for the tree from a plist
 *                     file. Unlike the CKDBDataNode and CKFileDataNode
 *                     classes, there will NOT be any methods to process the
 *                     data from the file as the plist definition contains
 *                     all the structure necessary for building the tree.
 *
 * $Id: CKPListDataNode.h,v 1.1 2008/01/17 16:43:30 drbob Exp $
 */
#ifndef __CKPLISTDATANODE_H
#define __CKPLISTDATANODE_H

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
class CKPListDataNode :
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
		CKPListDataNode();
		/*
		 * This version of the constructor allows the file to be specified,
		 * which will load the data from the file into the tree starting
		 * at this node.
		 */
		CKPListDataNode( const CKString & aFileName );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKPListDataNode( CKPListDataNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKPListDataNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKPListDataNode & operator=( CKPListDataNode & anOther );

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
		 * This method gets the filename that will be used to load the
		 * data. This doesn't start the loading, just sets the filename.
		 */
		const CKString & getFileName() const;

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
		 * makes sure the file is there, reads in the lines and converts
		 * them to the proper tree structure.
		 */
		virtual bool load();
		/*
		 * This method is used to load the data from the file into the
		 * node/tree structure for later use. This one method
		 * makes sure the file is there, reads in the lines converting
		 * the structure in the file to the node structure.
		 */
		virtual bool load( const CKString & aFileName );
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
		virtual void asyncLoad( const CKString & aFileName );
		virtual void asyncLoad( const CKString & aFileName, const CKString & aName );

		/*
		 * This method takes the existing structure rooted at this
		 * node and writes it out to the filename set previously for
		 * this instance. This is atomic because we don't want someone
		 * to mess with things while we're trying to write them out to
		 * the filesystem.
		 */
		virtual bool save();
		/*
		 * This method takes the existing structure rooted at this
		 * node and writes it out to the filename provided. This is
		 * atomic because we don't want someone to mess with things
		 * while we're trying to write them out to the filesystem.
		 */
		virtual bool save( const CKString & aFileName );

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
		bool operator==( const CKPListDataNode & anOther ) const;
		/*
		 * This method checks to see if the two CKPListDataNode are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKPListDataNode & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;
		/*
		 * This method simply generates the plist string value for the tree
		 * rooted at this node. It's what might be written to a file, but
		 * it can be used for debugging and logging as well.
		 */
		virtual CKString toPListString() const;

	protected:
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
		virtual bool fromPList( const CKString & aPList );
		/*
		 * This method assumes that the pointer it's given is sitting on
		 * a '{' to mark the start of a plist definition for this node.
		 * If the first character is NOT a '{', then it's a syntax error.
		 * The (char *) argument will be 'moved' (incremented) as this
		 * method parses the string for values, and will be returned
		 * sitting on the '}' that closes out the plist (dictionary)
		 * entry.
		 */
		bool parseFromPList( char * & aStr );
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
		bool parseList( char * & aStr, CKVariantList & aList );

		/*
		 * This method converts the existing data structure to a plist
		 * string capable of handing to loadPList() and recreating the same
		 * structure. This can then be used to send to something, or write
		 * it out to a file.
		 */
		virtual CKString toPList();
		/*
		 * This method takes the provided node and adds it's data to the
		 * plist string provided. The 'level' is used to indicate the
		 * level of "indentation" on the string - strictly for readability.
		 */
		bool appendAsPList( CKDataNode *aNode, CKString &aPList, int aLevel );
		/*
		 * This method takes the provided value and adds it to the
		 * plist string provided. The 'level' is used to indicate the
		 * level of "indentation" on the string - strictly for readability.
		 */
		bool appendAsPList( CKVariant & aValue, CKString &aPList, int aLevel );

		/*
		 * This method looks at the string 'aBuff' and places the correct
		 * value into the variant 'aValue'. The flag is used to indicate
		 * if the value must be a string regardless of what it may look
		 * like. If everything goes as planned, the method will return a
		 * 'true', but if there are any errors, then it'll return a 'false'.
		 */
		bool precipitateValue( const CKString & aBuff, bool mustBeString, CKVariant & aValue );

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
		static CKString loadFileToString( const CKString & aFileName );

		/*
		 * This method takes the supplied string and writes it to the
		 * supplied filename. This is just a way to blast a file with
		 * the contents of a string. It's stateless, so that's why it's
		 * static.
		 */
		static bool writeStringToFile( const CKString & aString,
									   const CKString & aFileName );

	private:
		/*
		 * The only thing we need for the plist file loading is the
		 * file name. So here it is.
		 */
		CKString			mFileName;
		/*
		 * This mutex will be used on the load() method to make sure that
		 * we are safe about loading all the data before starting to do
		 * any processing.
		 */
		CKFWMutex			mLoadLock;
};

#endif	// __CKPLISTDATANODE_H
