/*
 * CKMailMessage.cpp - this file implements the CKit class that is used as a
 *                     mail message for sending messages out through different
 *                     delivery channels. Instances of this class will be sent
 *                     out via the different subclasses of CKMailDelivery but
 *                     all delivery mechanisms will use this one message
 *                     structure.
 *
 * $Id: CKMailMessage.cpp,v 1.3 2003/12/16 18:09:01 drbob Exp $
 */

//	System Headers
#include <sstream>
#ifdef GPP2
#include "algorithm"
#endif

//	Third-Party Headers

//	Other Headers
#include "CKMailMessage.h"
#include "CKException.h"
#include "CKUUID.h"

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
 * This is the default constructor that prepares a plain, empty
 * message for filling up.
 */
CKMailMessage::CKMailMessage() :
	mRecipients(),
	mSubject(),
	mMessageBody()
{
}


/*
 * This is the constructor that takes a vector of names, a subject
 * line, and a body text and creates the message based on that.
 */
CKMailMessage::CKMailMessage( const std::vector<std::string> & aRecipientList,
			   const std::string & aSubject,
			   const std::string & aBody ) :
	mRecipients(),
	mSubject(),
	mMessageBody()
{
	// load up the things we've been given
	setRecipients(aRecipientList);
	setSubject(aSubject);
	setMessageBody(aBody);
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKMailMessage::CKMailMessage( const CKMailMessage & anOther ) :
	mRecipients(),
	mSubject(),
	mMessageBody()
{
	// we can use the '=' operator to do the job
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKMailMessage::~CKMailMessage()
{
	// clear everything out...
	mRecipients.clear();
	mMessageBody.clear();
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKMailMessage & CKMailMessage::operator=( const CKMailMessage & anOther )
{
	// copy over all the data this guy has (skip making the copies)
	mRecipients = anOther.mRecipients;
	mSubject = anOther.mSubject;
	mMessageBody = anOther.mMessageBody;

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * This method sets the vector of recipients for this message
 * to be equal to the contents of the passed-in vector. Note
 * that a copy is made of the contents, and the list itself
 * is still under the caller's control.
 */
void CKMailMessage::setRecipients( const std::vector<std::string> & aList )
{
	mRecipients = aList;
}


/*
 * This method sets the subject line for this message when
 * it finally gets delivered.
 */
void CKMailMessage::setSubject( const std::string & aSubject )
{
	mSubject = aSubject;
}


/*
 * This method clears out the existing message components and
 * places the provided component as the only 'body' in the
 * message. Since this class understands multi-part MIME, this
 * is a simplistic view of what can be accomplished, but it's
 * also very easy to use if you have a simple message to send.
 */
void CKMailMessage::setMessageBody( const std::string & aMessage )
{
	mMessageBody.clear();
	mMessageBody.push_back(aMessage);
}


/*
 * This method returns a pointer to the actual list of recipients
 * that this message is targeted for. Since this pointer is the
 * actual data, if the caller wants to do something with it, they
 * need to make a copy before it goes out of scope.
 */
const std::vector<std::string> *CKMailMessage::getRecipients() const
{
	return &mRecipients;
}


/*
 * This method returns the subject line for this message when it's
 * sent out through the delivery channel.
 */
std::string CKMailMessage::getSubject() const
{
	return mSubject;
}


/*
 * This method returns a flattened message body for the message as
 * it's been built up. This is important because any delivery channel
 * will insist that the data become a character stream, and this
 * method accomplishes just that.
 */
std::string CKMailMessage::getMessageBody() const
{
	bool			error = false;
	bool			done = false;
	std::string		body;

	/*
	 * See if we're a simple message, because if we are
	 * then we can get this all done in no time and be
	 * done with it.
	 */
	if (!error && !done && !isMIME()) {
		// put in the Content-Type for a simple message
		if (mMessageBody.size() > 0) {
			body.append(getContentType(mMessageBody[0]));
		} else {
			body.append(getContentType(" "));
		}
		// ...and then a blank line to indicate the start of the msg
		body.append("\r\n");
		// ...and finally the message itself
		if (mMessageBody.size() > 0) {
			body.append(mMessageBody[0]);
		}
		// flag this as all done
		done = true;
	}

	/*
	 * OK... looks like we're multipart MIME, and if that's the
	 * case, then we need to get a CKUUID as a hex string to id
	 * this message from all other messages that might be included
	 * in this message. This is necessary because of the unique
	 * seperator needed in multipart MIME messages.
	 */
	std::string		separator = "unique-boundary-1";
	if (!error && !done) {
		CKUUID	id = CKUUID::newUUID();
		separator = std::string("msgID-").append(id.getStringValue());
	}

	/*
	 * OK, now let's hit the main structure of the message by
	 * putting the header and separators in the right place and
	 * add in the encodings of the parts as they appear in the
	 * message body.
	 */
	if (!error && !done) {
		// put in the Content-Type for a simple message
		body.append("Content-type: multipart/mixed; boundary=").append(separator).append("\r\n");
		// ...and then a blank line to indicate the start of the msg
		body.append("\r\n");
		// ...and finally all the parts of the message
		std::vector<std::string>::const_iterator	i;
		for (i = mMessageBody.begin(); i != mMessageBody.end(); ++i) {
			// start with the separator for this part
			body.append("--").append(separator).append("\r\n");

			// now throw in the content type for this nugget
			body.append(getContentType(*i));
			// ...and a blank line for the end of the header
			body.append("\r\n");

			// finally, encode this part of the message
			body.append(encodeMessagePart(*i));

			// ...and a blank line for the end of the part
			body.append("\r\n");
		}
		// at the end, we close off this message body
		body.append("--").append(separator).append("--\r\n");
	}

	return body;
}


/*
 * This method adds the provided recipient to the list of recipients
 * for this message. If the address already exists, then it is
 * not added as a safety precaution to getting spammed.
 */
void CKMailMessage::addToRecipients( const std::string & anAddress )
{
	if (anAddress.length() > 0) {
		if (mRecipients.size() > 0) {
			std::vector<std::string>::iterator	i;
			i = find(mRecipients.begin(), mRecipients.end(), anAddress);
			if (i == mRecipients.end()) {
				mRecipients.push_back(anAddress);
			}
		} else {
			mRecipients.push_back(anAddress);
		}
	}
}


/*
 * This method adds the provided data (string) to the message as
 * an additional part of the multi-part MIME spec and inserts it
 * in place at the 'end' of the currently being built message.
 */
void CKMailMessage::addToMessageBody( const std::string & aMessage )
{
	mMessageBody.push_back(aMessage);
}


/*
 * These methods take different arguments and add them as parts
 * of a multi-part MIME message to this message for delivery.
 * The mail attachment will flatten the message and add it as
 * a simple string so that we can keep things all together as
 * most mailers expect to see them.
 */
void CKMailMessage::addAttachment( const std::string & anAttachment )
{
	addToMessageBody(anAttachment);
}


void CKMailMessage::addAttachment( const CKMailMessage & anAttachment )
{
	addAttachment(anAttachment.getMessageBody());
}


/*
 * This method will remove the supplied address from the list of
 * recipients for this message. If this address isn't in the list
 * then nothing will happen.
 */
void CKMailMessage::removeFromRecipients( const std::string & anAddress )
{
	if (mRecipients.size() > 0) {
		std::vector<std::string>::iterator	i;
		i = find(mRecipients.begin(), mRecipients.end(), anAddress);
		if (i != mRecipients.end()) {
			mRecipients.erase(i);
		}
	}
}


/*
 * This method will return true only if the body of this message
 * is currently empty. As things are put into it, this method will
 * return false.
 */
bool CKMailMessage::isMessageBodyEmpty() const
{
	return (mMessageBody.size() > 0 ? false : true);
}


/********************************************************
 *
 *                Delivery Methods
 *
 ********************************************************/
/*
 * This method will return true if the message has all the
 * requisite parts to be sent out through the default delivery
 * channel. If it's not complete, then this method will return
 * false.
 */
bool CKMailMessage::messageIsDeliverable() const
{
	bool		messageIsDeliverable = true;

	// Make sure there is at least one person this is going to...
	if (mRecipients.size() == 0) {
		messageIsDeliverable = false;
	}

	/*
	 * You can send a valid email message with only a subject 
	 * or only a message body but it's pretty 
	 * counterproductive to send a message with both empty.
	 */
	if (messageIsDeliverable) {
		if ((getSubject().size() == 0) && isMessageBodyEmpty()) {
			messageIsDeliverable = false;
		}
	}

	return messageIsDeliverable;
}


/*
 * This method will return true if this message is a multi-part
 * MIME message. This is important as this message may be a simple
 * ASCII message and not need the overhead of a MIME message.
 */
bool CKMailMessage::isMIME() const
{
	return (mMessageBody.size() > 1 ? true : false);
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKMailMessages are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailMessage::operator==( const CKMailMessage & anOther ) const
{
	bool		equal = true;

	if ((mRecipients != anOther.mRecipients) ||
		(mSubject != anOther.mSubject) ||
		(mMessageBody != anOther.mMessageBody)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKMailMessages are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns true, otherwise it returns false.
 */
bool CKMailMessage::operator!=( const CKMailMessage & anOther ) const
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
std::string CKMailMessage::toString() const
{
	std::ostringstream	buff;
	std::vector<std::string>::const_iterator	i;

	buff << "Recipients:";
	for (i = mRecipients.begin(); i != mRecipients.end(); ++i) {
		buff << " '" << (*i) << "'";
	}
	buff << std::endl;
	buff << "Subject:" << getSubject() << std::endl;
	buff << getMessageBody();

	return buff.str();
}


/*
 * This method returns the MIME 'Content Type' string for inclusion
 * in the flattened message. This is important because having the
 * correct content type means that the mailers that eventually get
 * this message will know how to decode it.
 */
std::string CKMailMessage::getContentType( const std::string & anElement ) const
{
	std::string			retval;

	/*
	 * Simply run the tests for the data type and map it to a
	 * string to return. Start with the simple ones and then work
	 * on the harder ones.
	 */		
	if ((anElement.find("HTML") != std::string::npos) ||
		(anElement.find("html") != std::string::npos)) {
		retval = "Content-type: text/html; charset=US-ASCII\r\n"
					"Content-Transfer-Encoding: 7bit\r\n";
	} else if (anElement.find("</") != std::string::npos) {
		retval = "Content-type: text/enriched; charset=US-ASCII\r\n"
					"Content-Transfer-Encoding: 7bit\r\n";
	} else {
		retval = "Content-type: text/plain; charset=US-ASCII\r\n"
					"Content-Transfer-Encoding: 7bit\r\n";
	}
	
	return retval;
}


/*
 * This method returns the encoded data (string) for this part of
 * this message for inclusion in the flattened data for sending out
 * through the delivery channel. This is important as all parts
 * need to be encoded properly.
 */
std::string CKMailMessage::encodeMessagePart( const std::string & anElement ) const
{
	std::string		retval;

	/*
	 * Simply run the tests for the data type and mapp it to a
	 * String to return. Start with the simple ones and then work
	 * on the harder ones.
	 */		
	if (anElement.size() == 0) {
		retval.append(" ");
	} else {
		retval.append(anElement);
	}

	// make sure it all ends with a line termination
	retval.append("\r\n");
		
	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMailMessage & aMessage )
{
	aStream << aMessage.toString();

	return aStream;
}
