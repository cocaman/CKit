/*
 * CKIRCProtocolListener.h - this file defines the class that is used in
 *                           concert with the CKIRCProtocol to monitor the
 *                           incoming IRC messages and interpret them correctly
 *                           so that those that are destined for the chat
 *                           user the protocol is presenting are interpreted
 *                           and passed to the listeners properly.
 *
 * $Id: CKIRCProtocolListener.h,v 1.5 2004/09/11 21:07:46 drbob Exp $
 */
#ifndef __CKIRCPROTOCOLLISTENER_H
#define __CKIRCPROTOCOLLISTENER_H

//	System Headers
#include <string>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

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
class CKIRCProtocolListener :
	public CKFWThread
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This method of the constructor takes a name of a server and the
		 * port to communicate on for IRC messages. This is a very common
		 * form of the constructor because it creates the protocol object
		 * and connects to a specific IRC server.
		 */
		CKIRCProtocolListener( CKIRCProtocol *aProtocol );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKIRCProtocolListener( const CKIRCProtocolListener & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKIRCProtocolListener();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKIRCProtocolListener & operator=( const CKIRCProtocolListener & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method sets the pointer this instance will have to the
		 * CKIRCProtocol that it's going to be doing all this work for.
		 * This is important to be a pointer because we don't want to do
		 * any copying, but rather direct assignment.
		 */
		void setProtocol( CKIRCProtocol *aProtocol );
		/*
		 * This method sets the flag for this instance that it's in the
		 * midst of it's monitoring loop, and therefore should be treated
		 * with some care. When the monitoring is done, this method will
		 * be called with a 'false' argument to indicate that this instance
		 * is just sitting there, awaiting instructions.
		 */
		void setIsRunning( bool aFlag );
		/*
		 * This method is set by the protocol thread in order to tell this
		 * thread that it's time to shutdown and quit. This is important
		 * because we'd like to have well-behaved threads wherever possible
		 * so that things start up and shutdown cleanly.
		 */
		void setTimeToDie( bool aFlag );

		/*
		 * This method returns the pointer to the CKIRCProtocol that this
		 * instance is doing all the monitoring for. This is important to be
		 * a pointer because we don't want to do any copying, but rather
		 * direct referencing. If you are interested in using this value
		 * outside the scope of this class, then please make a copy.
		 */
		CKIRCProtocol *getProtocol() const;
		/*
		 * This method gets the flagged state of this instance. If it returns
		 * true, this means that the instance is in the middle of it's
		 * processing loop, and we need to be careful with it. If it returns
		 * false, then that means that the instance is basically sitting idle.
		 */
		bool isRunning() const;
		/*
		 * This method tells this instance if the protocol thread thinks it
		 * is time to clean up and die. This is important because we'd like
		 * to have well-behaved threads wherever possible so that things
		 * start up and shutdown cleanly.
		 */
		bool timeToDie() const;

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

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKIRCProtocolListeners are
		 * equal to one another based on the values they represent and *not*
		 * on the actual pointers themselves. If they are equal, then this
		 * method returns true, otherwise it returns false.
		 */
		bool operator==( const CKIRCProtocolListener & anOther ) const;
		/*
		 * This method checks to see if the two CKIRCProtocolListeners are
		 * not equal to one another based on the values they represent and
		 * *not* on the actual pointers themselves. If they are not equal,
		 * then this method returns true, otherwise it returns false.
		 */
		bool operator!=( const CKIRCProtocolListener & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString() const;

	protected:
		/*
		 * This is the default constructor that really shouldn't ever need
		 * to be called, as this class is a helper class for the CKIRCProtocol
		 * class, and lives almost completly in it's implementation.
		 */
		CKIRCProtocolListener();

		/*
		 * This method sets the message tagline to the value supplied in the
		 * argument. The value is copied, so there's no need to worry about
		 * who owns the memory - the caller does.
		 */
		void setMsgTagline( const std::string & aTag );

		/*
		 * This method returns a pointer to this instance's message tagline
		 * so that it can be used - without copying, in the methods in this
		 * class. This is important as it's a pointer, but it's never owned
		 * by anyone other than this class. If you want to keep it around,
		 * then make a copy.
		 */
		const std::string *getMsgTagline() const;

	private:
		/*
		 * This is the IRC Protocol that I'm working for. This is set in
		 * the standard constructor (but not the default), and once it's
		 * set, there's really no reason to change it.
		 */
		CKIRCProtocol				*mProtocol;
		/*
		 * This is the boolean flag that this class uses to indicate if it's
		 * in the middle of it's running loop monitoring for the data from
		 * the IRC socket. This is important as the main IRC Protocol needs
		 * to know what this guy is doing.
		 */
		bool						mIsRunning;
		/*
		 * This boolean flag is set by the IRC Protocol to tell this class
		 * that it's time to clean up and quit it's run loop. This is done
		 * this way because we want to have clean processes and not have to
		 * force any hacks on killing threads.
		 */
		bool						mTimeToDie;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCProtocolListener & aListener );

#endif	// __CKIRCPROTOCOLLISTENER_H
