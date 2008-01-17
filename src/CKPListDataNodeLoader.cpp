/*
 * CKPListDataNodeLoader.cpp - this file implements the class that has been
 *                             created to handle the asynchronous loading of
 *                             the plist file-based CKDataNode structures that
 *                             are in use in applications. The reason for this
 *                             class is becasue there are times when the
 *                             loading of a group will be very time-consuming
 *                             but the group may not be needed for some time.
 *                             If the server blocks on this load, then other
 *                             processes are unavailable while we load this one
 *                             lengthy data set.
 *
 *                             But no longer.
 *
 *                             With this we now fire off the data sets and then
 *                             when the data is requested we see if the loading
 *                             is complete. If not, then we hold off on that
 *                             request and not on any others.
 *
 * $Id: CKPListDataNodeLoader.cpp,v 1.1 2008/01/17 16:43:52 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <iostream>

//	Third-Party Headers
#include <CKException.h>
#include <CKStopwatch.h>

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
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKPListDataNodeLoader::CKPListDataNodeLoader( CKPListDataNodeLoader & anOther ) :
	CKFWThread(),
	mNode(NULL),
	mName()
{
	// let the '=' operator do all the work for me
	*this = anOther;
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKPListDataNodeLoader & CKPListDataNodeLoader::operator=( CKPListDataNodeLoader & anOther )
{
	CKFWThread::operator=(anOther);
	mNode = anOther.mNode;
	mName = anOther.mName;

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/

/********************************************************
 *
 *                Static Worker Methods
 *
 ********************************************************/
/*
 * This method is the real core of the loading thread. It takes a
 * pointer to an CKPListDataNode and fires off a new thread that
 * calls back to that instance for it's load() method. This way
 * the thread really doesn't need to know how to load anything
 * up, just to know who to call to do it.
 */
bool CKPListDataNodeLoader::doLoad( CKPListDataNode *aNode, const CKString & aName )
{
	bool		error = false;

	// first, make sure we have a node to use
	if (!error) {
		if (aNode == NULL) {
			error = true;
			std::cerr << "CKPListDataNodeLoader::doLoad(CKPListDataNode *)"
				" - the passed in data node is NULL and that means that "
				"there is no way for me to execute this load. Please make sure "
				"that the argument is not NULL before calling this method." <<
				std::endl;
		}
	}

	// next, let's create a new thread instance and populate it
	CKPListDataNodeLoader	*exec = NULL;
	if (!error) {
		try {
			exec = new CKPListDataNodeLoader();
			if (exec == NULL) {
				error = true;
				std::cerr << "CKPListDataNodeLoader::doLoad(CKPListDataNode *)"
					" - a new thread in which to execute this load command could "
					"not be created. This is a serious problem that needs to be looked "
					"into as soon as possible." << std::endl;
			} else {
				// populate this guy with the data he's going to need
				exec->mNode = aNode;
				exec->mName = aName;
				// ...and then LAUNCH!
				if (exec->start() != CKFWThread::cSuccess) {
					error = true;
					std::cerr << "CKPListDataNodeLoader::doLoad(CKPListDataNode *)"
						" - while trying to start the execution of the load an "
						"error was encountered. Please check the logs for a "
						"possible cause." << std::endl;
				}
			}
		} catch (std::exception & e) {
			error = true;
			std::cerr << "CKPListDataNodeLoader::doLoad(CKPListDataNode *)"
				" - while trying to create a new thread in which to execute the "
				"load, a std::exception was thrown: " << e.what() << std::endl;
		}
	}

	return !error;
}


/********************************************************
 *
 *             Thread Processing Methods
 *
 ********************************************************/
/*
 * This method is called within a loop in the CKFWThread's run
 * loop and if all is well for a pass, then return cSuccess. If
 * this pass indicates that we need to quit, then we can return
 * cDone.
 */
int CKPListDataNodeLoader::process()
{
	/*
	 * This method is really simple... we need to see that we have a
	 * node to work with and then simply let the node do the work. One
	 * might wonder why we're doing this in this manner and the answer
	 * is that we're now in a new thread than what called doLoad() so
	 * that the original thread is back to checking on other things to
	 * do and we're able to get going on this task eventhough the
	 * node contains all the logic, etc.
	 */
	if (mNode == NULL) {
		std::ostringstream	msg;
		msg << "CKPListDataNodeLoader::process() - the CKPListDataNode is NULL and "
			"that should never happen. This is a serious data corruption problem "
			"that needs to be looked into as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		try {
			// time this bad boy for the logs
			CKStopwatch		tick;
			// have the node load it's data (timed)
			tick.start();
			mNode->load();
			tick.stop();
			// ...and log a message that we're done
			std::cout << "[loader:" << tick.getFormattedTime() <<
				"]: finished loading of '" << mName << "'" << std::endl;
		} catch (std::exception & se) {
			std::ostringstream	msg;
			msg << "CKPListDataNodeLoader::process() - while trying to process the "
				"load, a std::exception was thrown: " << se.what();
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// we MUST return cDone as we are done with the single call
	return cDone;
}


/*
 * This method is called when the process() method returns cDone
 * and this thread is ready to terminate. All class-level cleanups
 * should go into this method.
 */
int CKPListDataNodeLoader::terminate()
{
	/*
	 * This is a CORE COMPONENT of this asynchronous processing! When
	 * the processing of the loading is done, we will be called and we
	 * need to clean up this thread as well as return "all done".
	 * If this isn't done properly, then we'll be leaking for each job
	 * that gets fired off.
	 */
	delete this;
	return cDone;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKPListDataNodeLoaders are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKPListDataNodeLoader::operator==( const CKPListDataNodeLoader & anOther ) const
{
	bool		equal = true;

	if ((mNode != anOther.mNode) ||
		(mName != anOther.mName)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKPListDataNodeLoaders are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKPListDataNodeLoader::operator!=( const CKPListDataNodeLoader & anOther ) const
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
CKString CKPListDataNodeLoader::toString()
{
	CKString		retval = "";

	retval += "<CKPListDataNodeLoader>";

	return retval;
}


/*
 * This is the default constructor for the dat aloader thread
 * and it's private because the public API for this class is to call
 * the static method doLoad() to create and load the provided data. But,
 * under the covers I need to create this thread instance, set these
 * ivars and then start the thread. So I need this guy, and so here
 * he is.
 */
CKPListDataNodeLoader::CKPListDataNodeLoader() :
	CKFWThread(),
	mNode(NULL),
	mName()
{
	// set the thread tag for this guy so I can tell them apart
	setTag("CKPListDataNodeLoader");
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called. But interestingly enough - it's NOT USED! Well, not
 * directly. There is no reason to create or delete a
 * CKPListDataNodeLoader object - ever! The static method doLoad()
 * creates one, and the terminate() method takes care of cleaning
 * things up when the work that this thread was meant to do is all
 * done. So... don't go around creating and destroying these guys
 * - the static method does all the work.
 */
CKPListDataNodeLoader::~CKPListDataNodeLoader()
{
	// nothing to do here as we have taken control of nothing
}
