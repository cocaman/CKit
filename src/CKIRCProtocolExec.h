/*
 * CKIRCProtocolExec.h - this file defines the class that has been created to
 *                       handle the asynchronous processing of each chat
 *                       message that comes in. The problem was that a long
 *                       chat message (one that takes a long to come to an
 *                       answer) might interfere with the handling of another
 *                       chat command. Therefore, what was needed was an
 *                       asynchronous way to handle the individual chat
 *                       messages without a lot of extra overhead.
 *
 *                       This system is as close a blend as possible in that
 *                       all it does is spawn threads that use the
 *                       CKIRCProtocol to do the real heavy lifting.
 *                       The static method handleMessage() creates a new
 *                       thread and launches it. The thread itself will clean
 *                       itself up when it's done and there will be no
 *                       long-lasting effects of the spawned processing thread.
 * 
 * $Id: CKIRCProtocolExec.h,v 1.3 2004/09/11 02:15:20 drbob Exp $
 */
#ifndef __CKIRCPROTOCOLEXEC_H
#define __CKIRCPROTOCOLEXEC_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <vector>

//	Third-Party Headers

//	Other Headers
#include "CKFWThread.h"
#include "CKIRCProtocol.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKIRCProtocolExec :
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
		CKIRCProtocolExec( CKIRCProtocolExec & anOther );

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKIRCProtocolExec & operator=( CKIRCProtocolExec & anOther );

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
		static bool handleMessage( CKIRCIncomingMessage & aMsg, CKIRCProtocol *aBoss );

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
		 * This method checks to see if the two CKIRCProtocolExecs are
		 * equal to one another based on the values they represent and *not*
		 * on the actual pointers themselves. If they are equal, then this
		 * method returns true, otherwise it returns false.
		 */
		bool operator==( const CKIRCProtocolExec & anOther ) const;
		/*
		 * This method checks to see if the two CKIRCProtocolExecs are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKIRCProtocolExec & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString();

	private:
		friend class CKIRCProtocol;

		/*
		 * This is the default constructor for the crontab execution thread
		 * and it's private because the public API for this class is to call
		 * the static method goJob() to create and run the provided job. But,
		 * under the covers I need to create this thread instance, set these
		 * ivars and then start the thread. So I need this guy, and so here
		 * he is.
		 */
		CKIRCProtocolExec();
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
		virtual ~CKIRCProtocolExec();

		/*
		 * This the the message that I'm going to be dealing with. I'm not
		 * going to be doing a lot with it as the CKIRCProtocol really does
		 * all the work, but I have to have a copy so that we can run with
		 * it.
		 */
		CKIRCIncomingMessage	mMessage;
		/*
		 * This is the guy that's going to know what to do with the message
		 * and whom to send it to for processing. This is done in just a few
		 * methods but it makes for a nice package.
		 */
		CKIRCProtocol			*mProtocol;
};

#endif	// __CKIRCPROTOCOLEXEC_H
