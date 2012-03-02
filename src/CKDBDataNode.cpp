/*
 * CKDBDataNode.cpp - this file implements a subclass of CKDataNode that's
 *                    focused on getting the data for the tree from a database
 *                    via the SQLAPI++ toolkit. There will be virtual methods
 *                    for getting the database server, login and password as
 *                    well as the SQL to execute to load up the data. Then
 *                    there will be a virtual method for processing the current
 *                    record in an SACommand and that's where each subclass
 *                    will convert the data in the record to a path and value
 *                    for insertion into the tree.
 *
 * $Id: CKDBDataNode.cpp,v 1.1 2008/01/17 16:01:21 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <sys/param.h>

//	Third-Party Headers
#include <CKException.h>
#include <CKStackLocker.h>

//	Other Headers
#include "CKDBDataNode.h"
#include "CKDBDataNodeLoader.h"

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
 * This is the default constructor that doesn't do a lot
 * other than to get ready for the user to fully initialize
 * it. This default instance needs to have the server, username
 * and password set as well as the SQL used to get the data.
 * Then the user needs to call the load() method to do the
 * work of loading the data from the database.
 */
CKDBDataNode::CKDBDataNode() :
	CKDataNode(),
	mServer(),
	mUsername(),
	mPassword(),
	mSQL(),
	mConn(),
	mCmd(),
	mLoadLock()
{
}


/*
 * This version of the constructor allows the database
 * parameters to be set so that the instance is ready to load
 * up the data the user wants. Since the SQL query isn't in the
 * arg list, the load can't be done, but a simple call to
 * load() solves that.
 */
CKDBDataNode::CKDBDataNode( const CKString & aServer,
							const CKString & aUsername,
							const CKString & aPassword ) :
	CKDataNode(),
	mServer(),
	mUsername(),
	mPassword(),
	mSQL(),
	mConn(),
	mCmd(),
	mLoadLock()
{
	// try to make the connection to the database
	if (!connectToDatabase(aServer, aUsername, aPassword)) {
		std::ostringstream	msg;
		msg << "CKDBDataNode::CKDBDataNode(const CKString &, const "
			"CKString &, const CKString &) - a connection could not be "
			"established to the database at: '" << aServer << "' as the user '" <<
			aUsername << "' with password '" << aPassword << "'. Please make "
			"sure the parameters will connect to the desired database.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKDBDataNode::CKDBDataNode( CKDBDataNode & anOther ) :
	CKDataNode(),
	mServer(),
	mUsername(),
	mPassword(),
	mSQL(),
	mConn(),
	mCmd(),
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
CKDBDataNode::~CKDBDataNode()
{
	// close out the database resources
	disconnectFromDatabase();
	// ...and clear out everything we created
	clearDataTree();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKDBDataNode & CKDBDataNode::operator=( CKDBDataNode & anOther )
{
	// first, let the super class handle all it's stuff
	CKDataNode::operator=(anOther);
	// now get a copy of what is special to us
	mServer = anOther.mServer;
	mUsername = anOther.mUsername;
	mPassword = anOther.mPassword;
	mSQL = anOther.mSQL;

	return *this;
}


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
void CKDBDataNode::setServer( const CKString & aServer )
{
	mServer = aServer;
}


void CKDBDataNode::setUsername( const CKString & aUsername )
{
	mUsername = aUsername;
}


void CKDBDataNode::setPassword( const CKString & aPassword )
{
	mPassword = aPassword;
}


/*
 * This method is the setter for the SQL query that will be
 * used to fetch the data and populate the tree/nodes. This
 * has to be set *before* the call to load() as that's when
 * this is read and the process of loading is done. So don't
 * expect to change this and have the data immediately change.
 */
void CKDBDataNode::setSQL( const CKString & aSQLQuery )
{
	mSQL = aSQLQuery;
}


/*
 * These methods return the database connection parameters
 * that will be used the *next* time a connection is made to
 * the database. This is not necessarily the same as those
 * that are currently in use as someone may have come in and
 * changed these without reloading the data in which case they
 * are not at all what was used to get the data.
 */
const CKString CKDBDataNode::getServer() const
{
	return mServer;
}


const CKString CKDBDataNode::getUsername() const
{
	return mUsername;
}


const CKString CKDBDataNode::getPassword() const
{
	return mPassword;
}


/*
 * This method returns the SQL string that's used in the load()
 * method to get the data out of the database and into the tree
 * structure of the nodes. Again, this can be changed after the
 * load is done, and therefore you shoudl *not* assume that this
 * is guaranteed to be what was used to load the data.
 */
const CKString CKDBDataNode::getSQL()
{
	return mSQL;
}


/*
 * This method can be called to make sure that the data node tree
 * is not in the middle of a load that could make it's contents
 * unstable. If you ignore this method's return value you do so
 * at your own risk.
 */
bool CKDBDataNode::isLoadInProgress()
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
void CKDBDataNode::waitForLoad()
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
bool CKDBDataNode::clearDataTree()
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
 *                Database Methods
 *
 ********************************************************/
/*
 * This method attempts to open a connection to the database
 * whose parameters are already stored in this instance. Of
 * course, if there is an established connection this method
 * will do nothing.
 */
bool CKDBDataNode::connectToDatabase()
{
	return connectToDatabase(mServer, mUsername, mPassword);
}


/*
 * When the connection is made to the database, most often times
 * there is the optional parameter of telling the interface what
 * application this is - so that this data can be viewed in the
 * database logs, etc. This method simply needs to be overridden
 * and a simple value returned to set this optional app name.
 */
const char *CKDBDataNode::getAppName()
{
	return "DataLoader";
}


/*
 * As part of most database commections, we can set the hostname
 * that we're connecting from. If there's a problem with getting
 * the hostname from the system, this method will provide a default
 * value for the connection. Often times, it's the name of the
 * machine running this, or the project - something useful when
 * looking in the database logs.
 */
const char *CKDBDataNode::getDefaultMachineName()
{
	return "DataLoaderBox";
}


/*
 * This method attempts to open a connection to the database
 * based on the passed-in connection parameters. If there's
 * an established connection this method will do nothing,
 * so make sure it's not connected before calling if you are
 * unsure.
 */
bool CKDBDataNode::connectToDatabase( const CKString & aServer,
									  const CKString & aUsername,
									  const CKString & aPassword )
{
	bool		error = false;

	// first, see if we're already connected
	if (mConn.isConnected()) {
		error = true;
		std::ostringstream	msg;
		msg << "CKDBDataNode::connectToDatabase(const CKString &, const "
			"CKString &, const CKString &) - a connection is already "
			"established to the server: '" << mServer << "' as user: '" <<
			mUsername << "' with password: '" << mPassword << "'. Please "
			"disconnect from this database before trying to make a new one.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	// next, let's convert the database connection values to SQLAPI++ ones
	SAString	server = aServer.c_str();
	SAString	user = aUsername.c_str();
	SAString	password = aPassword.c_str();

	// now let's try and make the connection
	try {
		mConn.setOption("CS_VERSION") = "CS_VERSION_110";
		mConn.setOption("CS_APPNAME") = getAppName();
		char	name[MAXHOSTNAMELEN];
		if (gethostname(name, MAXHOSTNAMELEN) < 0) {
			strcpy(name, getDefaultMachineName());
		}
		mConn.setOption("CS_HOSTNAME") = name;
		mConn.Connect(server, user, password, SA_Sybase_Client);
		if (!mConn.isConnected()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDBDataNode::connectToDatabase(const CKString &, const "
				"CKString &, const CKString &) - an SAConnection could "
				"not establish a good connection to the database on server: '" <<
				aServer << "' as user: '" << aUsername << "' with password: '" <<
				aPassword << "'. Please make sure that these connection parameters "
				"are correct.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// set the connection for the command
			mCmd.setConnection(&mConn);
			// ...and save the connection values
			setServer(aServer);
			setUsername(aUsername);
			setPassword(aPassword);
		}
	} catch (SAException sae) {
		error = true;
		CKString	excep = (const SAChar *)sae.ErrText();
		std::ostringstream	msg;
		msg << "CKDBDataNode::connectToDatabase(const CKString &, const "
			"CKString &, const CKString &) - while trying to establish "
			"a good connection to the database on server: '" << aServer <<
			"' as user: '" << aUsername << "' with password: '" << aPassword <<
			"' an SAException was thrown: " << excep;
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return !error;
}


/*
 * This method returns true if the SQLAPI++ database object
 * believes that there's an established connection. This is not
 * necessarily foolproof, but it's a pretty decent way to see
 * if there's likely a connection established.
 */
bool CKDBDataNode::isConnected()
{
	return mConn.isConnected();
}


/*
 * This method closes the current database activity and the
 * established connection so that the user can connect to a
 * different database.
 */
void CKDBDataNode::disconnectFromDatabase()
{
	// close out the database resources
	try {
		if (mCmd.isOpened()) {
			mCmd.Close();
		}
		if (mConn.isConnected()) {
			mConn.Disconnect();
		}
	} catch (SAException & sae) {
		CKString	excep = (const SAChar *)sae.ErrText();
		std::ostringstream	msg;
		msg << "CKDBDataNode::disconnectFromDatabase() - while trying to close "
			"out the command object and the connection to the database, an "
			"SAException was thrown: " << excep;
		throw CKException(__FILE__, __LINE__, msg.str());
	}
}

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
bool CKDBDataNode::load()
{
	return load(mSQL);
}


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
bool CKDBDataNode::load( const CKString & aSQLQuery )
{
	bool		error = false;

	// first thing is to indicate that a load is in progress
	CKStackLocker	lockem(&mLoadLock);

	// next thing to do is to clear out all the variables and children
	if (!error) {
		if (!clearDataTree()) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDBDataNode::load(const CKString &) - while trying to "
				"clear out the tree of all existing data we ran into a problem. "
				"Please check the logs for a possible cause.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// next, make sure that we're connected to the database
	if (!error) {
		if (!isConnected()) {
			if (!connectToDatabase()) {
				error = true;
				std::ostringstream	msg;
				msg << "CKDBDataNode::load(const CKString &) - while trying to "
					"verify the connection to the database server we ran into a "
					"problem. Please check the logs for a possible cause.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}
	}

	// next, let's execute the query and see what we get
	if (!error) {
		try {
			mCmd.setCommandText(aSQLQuery.c_str());
			mCmd.Execute();
			if (!mCmd.isResultSet()) {
				error = true;
				std::ostringstream	msg;
				msg << "CKDBDataNode::load(const CKString &) - the SQL query '" <<
					mSQL << "' did not return any rows of data. This means that "
					"there's no way to load any data. That's not the point of this "
					"object.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} catch (SAException & sae) {
			error = true;
			CKString	excep = (const SAChar *)sae.ErrText();
			std::ostringstream	msg;
			msg << "CKDBDataNode::load(const CKString &) - the SQL query '" <<
				mSQL << "' did not return any rows of data but it did throw an "
				"SAException: " << excep;
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now we need to loop on each returned row and call processCurrent()
	if (!error) {
		try {
			while (!error && mCmd.FetchNext()) {
				error = !processCurrent(mCmd);
			}
		} catch (SAException & sae) {
			error = true;
			CKString	excep = (const SAChar *)sae.ErrText();
			std::ostringstream	msg;
			msg << "CKDBDataNode::load(const CKString &) - while running "
				"through the result set of the SQL query '" << mSQL << "' an "
				"SAException was thrown: " << excep;
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// if everything was OK, save the query
	if (!error) {
		mSQL = aSQLQuery;
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
void CKDBDataNode::asyncLoad()
{
	CKDBDataNodeLoader::doLoad(this, "CKDBDataNode");
}


void CKDBDataNode::asyncLoad( const CKString & aSQLQuery )
{
	mSQL = aSQLQuery;
	CKDBDataNodeLoader::doLoad(this, "CKDBDataNode");
}


void CKDBDataNode::asyncLoad( const CKString & aSQLQuery, const CKString & aName )
{
	mSQL = aSQLQuery;
	CKDBDataNodeLoader::doLoad(this, aName);
}


/*
 * This method processes the 'current' data in the SACommand
 * that represents a single record from the database fetch. This
 * is very useful because subclasses only really need to provide
 * the connection parameters, and query string, and then implement
 * this method on what to do with one row of data and that's it.
 * This method will be called by load() for each record so as
 * to process the entire dataset.
 */
bool CKDBDataNode::processCurrent( SACommand & aCmd )
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
 * This method checks to see if the two CKDBDataNodes are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKDBDataNode::operator==( const CKDBDataNode & anOther ) const
{
	bool		equal = true;

	// check that the subclasses are the same
	if (!CKDataNode::operator==(anOther)) {
		equal = false;
	}

	// check the other things that are simple equalities
	if ((mServer != anOther.mServer) ||
		(mUsername != anOther.mUsername) ||
		(mPassword != anOther.mPassword) ||
		(mSQL != anOther.mSQL)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKDBDataNodes are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKDBDataNode::operator!=( const CKDBDataNode & anOther ) const
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
CKString CKDBDataNode::toString() const
{
	CKString		retval = "(";

	// slap the database parameters out
	retval.append("Server='");
	retval.append(mServer);
	retval.append("', Username='");
	retval.append(mUsername);
	retval.append("', Password='");
	retval.append(mPassword);
	retval.append("'\nSQL='");
	retval.append(mSQL);
	retval.append("'\n");

	// now do the node itself
	retval.append(CKDataNode::toString());
	retval.append(")");

	return retval;
}
