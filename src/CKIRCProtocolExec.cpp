/*
 * CKIRCProtocolExec.cpp - this file defines the class that has been created to
 *                         handle the asynchronous processing of each chat
 *                         message that comes in. The problem was that a long
 *                         chat message (one that takes a long to come to an
 *                         answer) might interfere with the handling of another
 *                         chat command. Therefore, what was needed was an
 *                         asynchronous way to handle the individual chat
 *                         messages without a lot of extra overhead.
 *
 *                         This system is as close a blend as possible in that
 *                         all it does is spawn threads that use the
 *                         CKIRCProtocol to do the real heavy lifting.
 *                         The static method handleMessage() creates a new
 *                         thread and launches it. The thread itself will clean
 *                         itself up when it's done and there will be no
 *                         long-lasting effects of the spawned processing thread.
 * 
 * $Id: CKIRCProtocolExec.cpp,v 1.6 2004/09/20 16:19:34 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <iostream>

//	Third-Party Headers

//	Other Headers
#include "CKException.h"
#include "CKIRCProtocol.h"
#include "CKIRCProtocolExec.h"

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
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKIRCProtocolExec::CKIRCProtocolExec( CKIRCProtocolExec & anOther ) :
	CKFWThread(),
	mMessage(),
	mProtocol(NULL)
{
	// let the '=' operator do all the work for me
	*this = anOther;
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKIRCProtocolExec & CKIRCProtocolExec::operator=( CKIRCProtocolExec & anOther )
{
	CKFWThread::operator=(anOther);
	mMessage = anOther.mMessage;
	mProtocol = anOther.mProtocol;

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
 * This method is the real core of the asynchronous chat processing
 * scheme. This method will create a new CKIRCProtocolExec instance
 * and provide it with the message (CKIRCIncomingMessage) and whom to
 * use to process it (CKIRCProtocol). These are set in the new
 * instance that's created and then used for the life of the job.
 * Once it's over, these are released.
 */
bool CKIRCProtocolExec::handleMessage( CKIRCIncomingMessage & aMsg,
									   CKIRCProtocol *aBoss )
{
	bool		error = false;

	// first, make sure we have a controller to use
	if (!error) {
		if (aBoss == NULL) {
			error = true;
			std::cerr << "CKIRCProtocolExec::handleMessage(CKIRCIncomingMessage &, "
				"CKIRCProtocol *) - the passed in IRC Protocol is NULL and that "
				"means that there is no way for me to process this message. Please "
				"make sure that the argument is not NULL before calling this "
				"method." << std::endl;
		}
	}

	// next, let's create a new thread instance and populate it
	CKIRCProtocolExec	*exec = NULL;
	if (!error) {
		exec = new CKIRCProtocolExec();
		if (exec == NULL) {
			error = true;
			std::cerr << "CKIRCProtocolExec::handleMessage(CKIRCIncomingMessage &, "
				"CKIRCProtocol *) - a new thread in which to process this message "
				"could not be created. This is a serious problem that needs to be "
				"looked into as soon as possible." << std::endl;
		} else {
			// populate this guy with the data he's going to need
			exec->mMessage = aMsg;
			exec->mProtocol = aBoss;
			// ...and then LAUNCH!
			if (exec->start() != CKFWThread::cSuccess) {
				error = true;
				std::cerr << "CKIRCProtocolExec::handleMessage(CKIRCIncomingMessage &, "
					"CKIRCProtocol *) - the new thread which was to process this message "
					"could not be started. This is a serious problem that needs to be "
					"looked into as soon as possible." << std::endl;
			}
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
int CKIRCProtocolExec::process()
{
	/*
	 * This method is really simple... we need to see that we have a
	 * protocol to work with and then simply let the protocol do
	 * the work. One might wonder why we're doing this in this manner
	 * and the answer is that we're now in a new thread than what
	 * called handleMessage() so that the original thread is back to
	 * checking for other messages to process and we're able to get
	 * going on this task eventhough the protocol contains all the
	 * logic, etc.
	 */
	if (mProtocol == NULL) {
		std::ostringstream	msg;
		msg << "CKIRCProtocolExec::process() - the CKIRCProtocol is NULL and "
			"that should never happen. This is a serious data corruption problem "
			"that needs to be looked into as soon as possible.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// now have all the responders take a whack at it
		if (mProtocol->alertAllResponders(mMessage)) {
			// send it back to the originator
			mProtocol->sendMessage(mMessage.userNickname, mMessage.response);
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
int CKIRCProtocolExec::terminate()
{
	/*
	 * This is a CORE COMPONENT of this asynchronous processing! When
	 * the processing of the message is done, we will be called and we
	 * need to clean up this thread as well as return "all done".
	 * If this isn't done properly, then we'll eb leaking for each job
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
 * This method checks to see if the two CKIRCProtocolExecs are
 * equal to one another based on the values they represent and *not*
 * on the actual pointers themselves. If they are equal, then this
 * method returns true, otherwise it returns false.
 */
bool CKIRCProtocolExec::operator==( const CKIRCProtocolExec & anOther ) const
{
	bool		equal = true;

	if (mProtocol != anOther.mProtocol) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKIRCProtocolExecs are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKIRCProtocolExec::operator!=( const CKIRCProtocolExec & anOther ) const
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
CKString CKIRCProtocolExec::toString()
{
	CKString		retval = "";

	retval += "<CKIRCProtocolExec>";

	return retval;
}



/*
 * This is the default constructor for the crontab execution thread
 * and it's private because the public API for this class is to call
 * the static method goJob() to create and run the provided job. But,
 * under the covers I need to create this thread instance, set these
 * ivars and then start the thread. So I need this guy, and so here
 * he is.
 */
CKIRCProtocolExec::CKIRCProtocolExec() :
	CKFWThread(),
	mMessage(),
	mProtocol(NULL)
{
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called. But interestingly enough - it's NOT USED! Well, not
 * directly. There is no reason to create or delete a
 * CKIRCProtocolExec object - ever! The static method handleMessage()
 * creates one, and the terminate() method takes care of cleaning
 * things up when the work that this thread was meant to do is all
 * done. So... don't go around creating and destroying these guys -
 * the static method does all the work.
 */
CKIRCProtocolExec::~CKIRCProtocolExec()
{
	// the super does it all, so we don't have to do anything
}
