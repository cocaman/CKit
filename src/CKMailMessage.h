/*
 * CKMailMessage.h - this file defines the CKit class that is used as a
 *                   mail message for sending messages out through different
 *                   delivery channels. Instances of this class will be sent
 *                   out via the different subclasses of CKMailDelivery but
 *                   all delivery mechanisms will use this one message
 *                   structure.
 *
 * $Id: CKMailMessage.h,v 1.5 2004/09/11 21:07:47 drbob Exp $
 */
#ifndef __CKMAILMESSAGE_H
#define __CKMAILMESSAGE_H

//	System Headers
#include <string>
#include <vector>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKMailMessage
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that prepares a plain, empty
		 * message for filling up.
		 */
		CKMailMessage();
		/*
		 * This is the constructor that takes a vector of names, a subject
		 * line, and a body text and creates the message based on that.
		 */
		CKMailMessage( const std::vector<std::string> & aRecipientList,
					   const std::string & aSubject,
					   const std::string & aBody );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKMailMessage( const CKMailMessage & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKMailMessage();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKMailMessage & operator=( const CKMailMessage & anOther );

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
		void setRecipients( const std::vector<std::string> & aList );
		/*
		 * This method sets the subject line for this message when
		 * it finally gets delivered.
		 */
		void setSubject( const std::string & aSubject );
		/*
		 * This method clears out the existing message components and
		 * places the provided component as the only 'body' in the
		 * message. Since this class understands multi-part MIME, this
		 * is a simplistic view of what can be accomplished, but it's
		 * also very easy to use if you have a simple message to send.
		 */
		void setMessageBody( const std::string & aMessage );

		/*
		 * This method returns a pointer to the actual list of recipients
		 * that this message is targeted for. Since this pointer is the
		 * actual data, if the caller wants to do something with it, they
		 * need to make a copy before it goes out of scope.
		 */
		const std::vector<std::string> *getRecipients() const;
		/*
		 * This method returns the subject line for this message when it's
		 * sent out through the delivery channel.
		 */
		std::string getSubject() const;
		/*
		 * This method returns a flattened message body for the message as
		 * it's been built up. This is important because any delivery channel
		 * will insist that the data become a character stream, and this
		 * method accomplishes just that.
		 */
		std::string getMessageBody() const;

		/*
		 * This method adds the provided recipient to the list of recipients
		 * for this message. If the address already exists, then it is
		 * not added as a safety precaution to getting spammed.
		 */
		void addToRecipients( const std::string & anAddress );
		/*
		 * This method adds the provided data (string) to the message as
		 * an additional part of the multi-part MIME spec and inserts it
		 * in place at the 'end' of the currently being built message.
		 */
		void addToMessageBody( const std::string & aMessage );
		/*
		 * These methods take different arguments and add them as parts
		 * of a multi-part MIME message to this message for delivery.
		 * The mail attachment will flatten the message and add it as
		 * a simple string so that we can keep things all together as
		 * most mailers expect to see them.
		 */
		void addAttachment( const std::string & anAttachment );
		void addAttachment( const CKMailMessage & anAttachment );

		/*
		 * This method will remove the supplied address from the list of
		 * recipients for this message. If this address isn't in the list
		 * then nothing will happen.
		 */
		void removeFromRecipients( const std::string & anAddress );

		/*
		 * This method will return true only if the body of this message
		 * is currently empty. As things are put into it, this method will
		 * return false.
		 */
		bool isMessageBodyEmpty() const;

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
		bool messageIsDeliverable() const;
		/*
		 * This method will return true if this message is a multi-part
		 * MIME message. This is important as this message may be a simple
		 * ASCII message and not need the overhead of a MIME message.
		 */
		bool isMIME() const;

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
		bool operator==( const CKMailMessage & anOther ) const;
		/*
		 * This method checks to see if the two CKMailMessages are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns true, otherwise it returns false.
		 */
		bool operator!=( const CKMailMessage & anOther ) const;
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
		 * This method returns the MIME 'Content Type' string for inclusion
		 * in the flattened message. This is important because having the
		 * correct content type means that the mailers that eventually get 
		 * this message will know how to decode it.
		 */
		std::string getContentType( const std::string & anElement ) const;
		/*
		 * This method returns the encoded data (string) for this part of
		 * this message for inclusion in the flattened data for sending out
		 * through the delivery channel. This is important as all parts
		 * need to be encoded properly.
		 */
		std::string encodeMessagePart( const std::string & anElement ) const;

	private:
		/*
		 * This is the list of email addresses that will be the recipients
		 * of this message when it is finally sent through a channel. As
		 * such, it's probably important that the format of the addresses
		 * matches the delivery channel, but for now, all we have is SMTP,
		 * so it's reasonable to stick with that for now. But it's possible
		 * too deliver the message through another channel, and then this
		 * would have to require some real thought by the user of the class.
		 */
		std::vector<std::string>		mRecipients;
		/*
		 * This is going to be the subject line of the email when it is
		 * sent.
		 */
		std::string						mSubject;
		/*
		 * Since we allow multi-part MIME messages, we need to have a list
		 * of all the parts a message can have. This is going to make the
		 * sending very flexible, but at the same time, it's not a walk in
		 * the park to get this formatted properly.
		 */
		std::vector<std::string>		mMessageBody;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMailMessage & aMessage );

#endif	// __CKMAILDELIVERY_H
