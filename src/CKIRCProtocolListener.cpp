/*
 * CKIRCProtocolListener.cpp - this file implements the class that is used in
 *                             concert with the CKIRCProtocol to monitor the
 *                             incoming IRC messages and interpret them correctly
 *                             so that those that are destined for the chat
 *                             user the protocol is presenting are interpreted
 *                             and passed to the listeners properly.
 *
 * $Id: CKIRCProtocolListener.cpp,v 1.4 2004/05/24 18:19:41 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKIRCProtocolListener.h"
#include "CKIRCProtocolExec.h"
#include "CKException.h"

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
 * This method of the constructor takes a name of a server and the
 * port to communicate on for IRC messages. This is a very common
 * form of the constructor because it creates the protocol object
 * and connects to a specific IRC server.
 */
CKIRCProtocolListener::CKIRCProtocolListener( CKIRCProtocol *aProtocol ) :
	CKFWThread(),
	mProtocol(NULL),
	mIsRunning(false),
	mTimeToDie(false)
{
	// set the args as we've been given them
	setProtocol(aProtocol);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKIRCProtocolListener::CKIRCProtocolListener( const CKIRCProtocolListener & anOther ) :
	CKFWThread(),
	mProtocol(NULL),
	mIsRunning(false),
	mTimeToDie(false)
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKIRCProtocolListener::~CKIRCProtocolListener()
{
	/*
	 * While we "have" a CKIRCProtocol, that's really out prent and we
	 * don't want to free him. So, there's really nothing we have to
	 * delete, so let's not.
	 */
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKIRCProtocolListener & CKIRCProtocolListener::operator=( const CKIRCProtocolListener & anOther )
{
	// set everything that the other one has
	mProtocol = anOther.mProtocol;
	mIsRunning = anOther.mIsRunning;
	mTimeToDie = anOther.mTimeToDie;

	return *this;
}


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
void CKIRCProtocolListener::setProtocol( CKIRCProtocol *aProtocol )
{
	/*
	 * If there's something already here, then don't worry about it
	 * as it's only a placeholder and we don't need to delete the
	 * IRCProtocol as it's the 'parent' of this guy. Instead, just
	 * save the new value as the new 'parent' of this guy.
	 */
	mProtocol = aProtocol;
}


/*
 * This method sets the flag for this instance that it's in the
 * midst of it's monitoring loop, and therefore should be treated
 * with some care. When the monitoring is done, this method will
 * be called with a 'false' argument to indicate that this instance
 * is just sitting there, awaiting instructions.
 */
void CKIRCProtocolListener::setIsRunning( bool aFlag )
{
	mIsRunning = aFlag;
}


/*
 * This method is set by the protocol thread in order to tell this
 * thread that it's time to shutdown and quit. This is important
 * because we'd like to have well-behaved threads wherever possible
 * so that things start up and shutdown cleanly.
 */
void CKIRCProtocolListener::setTimeToDie( bool aFlag )
{
	mTimeToDie = aFlag;
}


/*
 * This method returns the pointer to the CKIRCProtocol that this
 * instance is doing all the monitoring for. This is important to be
 * a pointer because we don't want to do any copying, but rather
 * direct referencing. If you are interested in using this value
 * outside the scope of this class, then please make a copy.
 */
CKIRCProtocol *CKIRCProtocolListener::getProtocol() const
{
	return mProtocol;
}


/*
 * This method gets the flagged state of this instance. If it returns
 * true, this means that the instance is in the middle of it's
 * processing loop, and we need to be careful with it. If it returns
 * false, then that means that the instance is basically sitting idle.
 */
bool CKIRCProtocolListener::isRunning() const
{
	return mIsRunning;
}


/*
 * This method tells this instance if the protocol thread thinks it
 * is time to clean up and die. This is important because we'd like
 * to have well-behaved threads wherever possible so that things
 * start up and shutdown cleanly.
 */
bool CKIRCProtocolListener::timeToDie() const
{
	return mTimeToDie;
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
int CKIRCProtocolListener::process()
{
	bool		error = false;
	int			retval = cSuccess;

	// first, let's make sure to let everyone know we're running
	setIsRunning(true);

	// next, let's see if we have a line from the IRC Server to process
	std::string		line;
	bool			processed = false;
	if (!error && !timeToDie()) {
		try {
			line = mProtocol->getReply();
			processed = false;
		} catch (CKException & e) {
			/*
			 * This means that we timed out on the socket connection and
			 * so we're done for this round. No biggie, we can be back
			 * here again next time.
			 */
			processed = true;
		}
	}

	/*
	 * First, let's see if this message is one that we know we
	 * have to respond to. If so, then do it now without having
	 * to make the client respond to these messages.
	 */
	if (!error && !timeToDie() && !processed && (line.size() > 0)) {
		processed = mProtocol->isReflexChat(line);
	}

	/*
	 * Next, let's see if it's a message that's meant for this user...
	 */
	if (!error && !timeToDie() && !processed && (line.size() > 0)) {
		// now, compute the tagline for this protocol
		std::string		tag = "PRIVMSG ";
		tag += mProtocol->getNickname();
		tag += " :";
		unsigned int	pos = std::string::npos;

		if ((line[0] == ':') &&
			((pos = line.find(tag)) != std::string::npos)) {
			// build up the message packet for tossing around
			CKIRCIncomingMessage	msg;
			int		bang = line.find('!');
			msg.userNickname = line.substr(1, (bang - 1));
			msg.message = line.substr(pos + tag.size());
			msg.response = "";

			// now process it outside of this thread
			CKIRCProtocolExec::handleMessage(msg, mProtocol);
			processed = true;
		}
	}

	// flag if we're going to die now
	if (timeToDie()) {
		setIsRunning(false);
		retval = cDone;
	}

	return retval;
}


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
bool CKIRCProtocolListener::operator==( const CKIRCProtocolListener & anOther ) const
{
	bool		equal = true;

	if ((*mProtocol != *anOther.mProtocol) ||
		(mIsRunning != anOther.mIsRunning) ||
		(mTimeToDie != anOther.mTimeToDie)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKIRCProtocolListeners are
 * not equal to one another based on the values they represent and
 * *not* on the actual pointers themselves. If they are not equal,
 * then this method returns true, otherwise it returns false.
 */
bool CKIRCProtocolListener::operator!=( const CKIRCProtocolListener & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a std::string.
 */
std::string CKIRCProtocolListener::toString() const
{
	std::ostringstream	buff;

	buff << "< IRCProtocol=" << mProtocol->toString() << ", " <<
		" Running? " << (mIsRunning ? "Yes" : "No") << ", " <<
		" TimeToDie? " << (mTimeToDie ? "Yes" : "No") <<
		">" << std::endl;

	return buff.str();
}


/*
 * This is the default constructor that really shouldn't ever need
 * to be called, as this class is a helper class for the CKIRCProtocol
 * class, and lives almost completly in it's implementation.
 */
CKIRCProtocolListener::CKIRCProtocolListener() :
	CKFWThread(),
	mProtocol(NULL),
	mIsRunning(false),
	mTimeToDie(false)
{
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIRCProtocolListener & aListener )
{
	aStream << aListener.toString();

	return aStream;
}
