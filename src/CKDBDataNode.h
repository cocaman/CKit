/*
 * CKDBDataNode.h - this file defines a subclass of CKDataNode that's focused
 *                  on getting the data for the tree from a database via the
 *                  SQLAPI++ toolkit. There will be virtual methods for
 *                  getting the database server, login and password as well
 *                  as the SQL to execute to load up the data. Then there
 *                  will be a virtual method for processing the current
 *                  record in an SACommand and that's where each subclass
 *                  will convert the data in the record to a path and value
 *                  for insertion into the tree.
 *
 * $Id: CKDBDataNode.h,v 1.1 2008/01/17 16:01:21 drbob Exp $
 */
#ifndef __CKDBDATANODE_H
#define __CKDBDATANODE_H

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
#include <SQLAPI.h>

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKDBDataNode :
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
		 * it. This default instance needs to have the server, username
		 * and password set as well as the SQL used to get the data.
		 * Then the user needs to call the load() method to do the
		 * work of loading the data from the database.
		 */
		CKDBDataNode();
		/*
		 * This version of the constructor allows the database
		 * parameters to be set so that the instance is ready to load
		 * up the data the user wants. Since the SQL query isn't in the
		 * arg list, the load can't be done, but a simple call to
		 * load() solves that.
		 */
		CKDBDataNode( const CKString & aServer,
					  const CKString & aUsername,
					  const CKString & aPassword );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKDBDataNode( CKDBDataNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKDBDataNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKDBDataNode & operator=( CKDBDataNode & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * These methods are the simple setters for the database
		 * connection parameters - server, username, and password.
		 * These are all necessary and can be specified in the
		 * constructor, or these setters can be used. It's important
		 * to note that these parameters will only effect the *next*
		 * time the connection is made as that's when the methods
		 * use them. So changing one of these and expecting it to
		 * reconnect to a different server is right out.
		 */
		void setServer( const CKString & aServer );
		void setUsername( const CKString & aUsername );
		void setPassword( const CKString & aPassword );

		/*
		 * This method is the setter for the SQL query that will be
		 * used to fetch the data and populate the tree/nodes. This
		 * has to be set *before* the call to load() as that's when
		 * this is read and the process of loading is done. So don't
		 * expect to change this and have the data immediately change.
		 */
		void setSQL( const CKString & aSQLQuery );

		/*
		 * These methods return the database connection parameters
		 * that will be used the *next* time a connection is made to
		 * the database. This is not necessarily the same as those
		 * that are currently in use as someone may have come in and
		 * changed these without reloading the data in which case they
		 * are not at all what was used to get the data.
		 */
		const CKString getServer() const;
		const CKString getUsername() const;
		const CKString getPassword() const;

		/*
		 * This method returns the SQL string that's used in the load()
		 * method to get the data out of the database and into the tree
		 * structure of the nodes. Again, this can be changed after the
		 * load is done, and therefore you shoudl *not* assume that this
		 * is guaranteed to be what was used to load the data.
		 */
		const CKString getSQL();

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
		 *                Database Methods
		 *
		 ********************************************************/
		/*
		 * When the connection is made to the database, most often times
		 * there is the optional parameter of telling the interface what
		 * application this is - so that this data can be viewed in the
		 * database logs, etc. This method simply needs to be overridden
		 * and a simple value returned to set this optional app name.
		 */
		virtual const char *getAppName();
		/*
		 * As part of most database commections, we can set the hostname
		 * that we're connecting from. If there's a problem with getting
		 * the hostname from the system, this method will provide a default
		 * value for the connection. Often times, it's the name of the
		 * machine running this, or the project - something useful when
		 * looking in the database logs.
		 */
		virtual const char *getDefaultMachineName();
		
		/*
		 * This method attempts to open a connection to the database
		 * whose parameters are already stored in this instance. Of
		 * course, if there is an established connection this method
		 * will do nothing.
		 */
		bool connectToDatabase();
		/*
		 * This method attempts to open a connection to the database
		 * based on the passed-in connection parameters. If there's
		 * an established connection this method will do nothing,
		 * so make sure it's not connected before calling if you are
		 * unsure.
		 */
		bool connectToDatabase( const CKString & aServer,
								const CKString & aUsername,
								const CKString & aPassword );
		/*
		 * This method returns true if the SQLAPI++ database object
		 * believes that there's an established connection. This is not
		 * necessarily foolproof, but it's a pretty decent way to see
		 * if there's likely a connection established.
		 */
		bool isConnected();
		/*
		 * This method closes the current database activity and the
		 * established connection so that the user can connect to a
		 * different database.
		 */
		void disconnectFromDatabase();

		/*
		 * This method is used to load the data from the database
		 * into the node/tree structure for later use. This one method
		 * makes sure there's a connection to the database, issues the
		 * previously defined SQL query to fetch the data and then for
		 * each record returned from the database calls processCurrent()
		 * so that the subclasses can simply handle processing one
		 * record at a time and not have to worry about the rest of
		 * the deal. In the end, the database connection will be closed
		 * and those database-specific resources will be released.
		 */
		virtual bool load();
		/*
		 * This method is used to load the data from the database
		 * into the node/tree structure for later use. This one method
		 * makes sure there's a connection to the database, issues the
		 * provided SQL query to fetch the data and then for each record
		 * returned from the database calls processCurrent() so that the
		 * subclasses can simply handle processing one record at a time
		 * and not have to worry about the rest of the deal. In the end,
		 * the database connection will be closed and those
		 * database-specific resources will be released.
		 */
		virtual bool load( const CKString & aSQLQuery );
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
		virtual void asyncLoad( const CKString & aSQLQuery );
		virtual void asyncLoad( const CKString & aSQLQuery, const CKString & aName );

		/*
		 * This method processes the 'current' data in the SACommand
		 * that represents a single record from the database fetch. This
		 * is very useful because subclasses only really need to provide
		 * the connection parameters, and query string, and then implement
		 * this method on what to do with one row of data and that's it.
		 * This method will be called by load() for each record so as
		 * to process the entire dataset.
		 */
		virtual bool processCurrent( SACommand & aCmd );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKDBDataNodes are
		 * equal to one another based on the values they represent and *not*
		 * on the actual pointers themselves. If they are equal, then this
		 * method returns true, otherwise it returns false.
		 */
		bool operator==( const CKDBDataNode & anOther ) const;
		/*
		 * This method checks to see if the two CKDBDataNodes are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKDBDataNode & anOther ) const;
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
		 * These are the parameters that SQLAPI++ needs in order to
		 * establish a connection to the database. These can be set
		 * in the constructor, or they can be set once the instance
		 * is created.
		 */
		CKString			mServer;
		CKString			mUsername;
		CKString			mPassword;
		/*
		 * This is the SQL that's submitted to the database in order
		 * to get the data that will form the nodes/tree rooted at
		 * this instance. Again, this can be set in the constructor
		 * in which case it will be executed and loaded.
		 */
		CKString			mSQL;
		/*
		 * These are the connection and associated command from
		 * SQLAPI that are used to get the data out of the database
		 * and into the data structure.
		 */
		SAConnection		mConn;
		SACommand			mCmd;
		/*
		 * This mutex will be used on the load() method to make sure that
		 * we are safe about loading all the data before starting to do
		 * any processing.
		 */
		CKFWMutex			mLoadLock;
};

#endif	// __CKDBDATANODE_H
