/*
 * CKFileDataNodeLoader.h - this file defines the class that has been created to
 *                          handle the asynchronous loading of the
 *                          flat file-based CKDataNode structures that are in
 *                          use in applications. The reason for this class
 *                          is becasue there are times when the loading of a
 *                          group will be very time-consuming but the group
 *                          may not be needed for some time.
 *                          If the server blocks on this load, then other
 *                          processes are unavailable while we load this one
 *                          lengthy data set.
 *
 *                          But no longer.
 *
 *                          With this we now fire off the data sets and then
 *                          when the data is requested we see if the loading
 *                          is complete. If not, then we hold off on that
 *                          request and not on any others.
 *
 * $Id: CKFileDataNodeLoader.h,v 1.1 2008/01/17 16:27:26 drbob Exp $
 */
#ifndef __CKFILEDATANODELOADER_H
#define __CKFILEDATANODELOADER_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers
#include <CKFWThread.h>
#include <CKString.h>

//	Other Headers

//	Forward Declarations
class CKFileDataNode;

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKFileDataNodeLoader :
	public CKFWThread
{
	public:
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
		CKFileDataNodeLoader( CKFileDataNodeLoader & anOther );

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKFileDataNodeLoader & operator=( CKFileDataNodeLoader & anOther );

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
		 * pointer to an CKFileDataNode and fires off a new thread that
		 * calls back to that instance for it's load() method. This way
		 * the thread really doesn't need to know how to load anything
		 * up, just to know who to call to do it.
		 */
		static bool doLoad( CKFileDataNode *aNode, const CKString & aName );

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
		virtual int process();
		/*
		 * This method is called when the process() method returns cDone
		 * and this thread is ready to terminate. All class-level cleanups
		 * should go into this method.
		 */
		virtual int terminate();

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKFileDataNodeLoaders are
		 * equal to one another based on the values they represent and *not*
		 * on the actual pointers themselves. If they are equal, then this
		 * method returns true, otherwise it returns false.
		 */
		bool operator==( const CKFileDataNodeLoader & anOther ) const;
		/*
		 * This method checks to see if the two CKFileDataNodeLoaders are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKFileDataNodeLoader & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString();

	private:
#ifdef GPP2
		friend class CKFileDataNode;
#endif
		/*
		 * This is the default constructor for the dat aloader thread
		 * and it's private because the public API for this class is to call
		 * the static method doLoad() to create and load the provided data. But,
		 * under the covers I need to create this thread instance, set these
		 * ivars and then start the thread. So I need this guy, and so here
		 * he is.
		 */
		CKFileDataNodeLoader();
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called. But interestingly enough - it's NOT USED! Well, not
		 * directly. There is no reason to create or delete a
		 * CKFileDataNodeLoader object - ever! The static method doLoad()
		 * creates one, and the terminate() method takes care of cleaning
		 * things up when the work that this thread was meant to do is all
		 * done. So... don't go around creating and destroying these guys
		 * - the static method does all the work.
		 */
		virtual ~CKFileDataNodeLoader();

		/*
		 * This is the guy that's going to get loaded.
		 */
		CKFileDataNode		*mNode;
		/*
		 * This is the optional name that will be logged when the load is
		 * done.
		 */
		CKString			mName;
};

#endif	// __CKFILEDATANODELOADER_H
