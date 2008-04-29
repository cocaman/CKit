/*
 * CKFIFOCoalescingQueue.h - this file defines a template class that is nice
 *                           coalescing first-in-first-out queue of something.
 *                           This is really useful when you have keyed data
 *                           that is most likely going to come into the queue
 *                           faster than it can be taken out. This guy will
 *                           allow subsequent push() calls to replace the data
 *                           but preserve the order.
 *
 * $Id: CKFIFOCoalescingQueue.h,v 1.2 2008/04/29 19:32:26 drbob Exp $
 */
#ifndef __CKFIFOCOALESCINGQUEUE_H
#define __CKFIFOCOALESCINGQUEUE_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sstream>
#include <map>

//	Third-Party Headers

//	Other Headers
#include "CKString.h"
#include "CKFWMutex.h"
#include "CKStackLocker.h"
#include "CKFWConditional.h"
#include "CKException.h"

//	Forward Declarations
template <class K, class T> class CKFIFOCoalescingQueue;

//	Public Constants

//	Public Datatypes

//	Public Data Constants
/*
 * This is the default starting size of this queue as it's going to
 * simply be an allocated array of elements. When an operation adds more
 * that this to the size of the vector, we'll have to allocate more and
 * then this won't be the size. But for starters, this is a good size.
 */
#define	CKFIFOCOALESCINGQUEUE_DEFAULT_STARTING_SIZE		8
/*
 * When the buffer has to grow for this queue, this is the default
 * increment that it uses. Of course, this is *above* the necessary size
 * so if you're adding 2000 elements, we'll round it up to a factor of
 * the DEFAULT_INCREMENT_SIZE to make sure that we have at least a little
 * growing room.
 */
#define	CKFIFOCOALESCINGQUEUE_DEFAULT_INCREMENT_SIZE		16


/*******************************************************************
 *
 *               Queue CKFWConditional Test Classes
 *
 *******************************************************************/
/*
 * In order to have popSomething() wait as nicely as possible we need
 * to have a conditional that's based on the status of the queue. But
 * since the queue is a template, so must the conditional. So I have to
 * make it here and be careful about how it's used in the implementation.
 */
template <class K, class T> class CKFIFOCoalescingQueueNotEmptyTest :
	public ICKFWConditionalSpuriousTest
{
	public:
		CKFIFOCoalescingQueueNotEmptyTest( CKFIFOCoalescingQueue<K,T> *aQueue ) :
			mQueuePtr(aQueue)
		{
		}

		virtual ~CKFIFOCoalescingQueueNotEmptyTest()
		{
			mQueuePtr = NULL;
		}

		virtual int test()
		{
			return ((mQueuePtr != NULL) && mQueuePtr->empty());
		}

	private:
		CKFIFOCoalescingQueue<K,T>	*mQueuePtr;
};


/*
 * This is the main class definition.
 */
template <class K, class T> class CKFIFOCoalescingQueue
{
	public :
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This form of the constructor allows the user to specify the
		 * starting size of the queue as well as the growth size when
		 * the queue exceeds the starting size. Both of these default
		 * to reasonable values, but it's nice to be able to set them
		 * just the same.
		 */
		CKFIFOCoalescingQueue( int anInitialCapacity = CKFIFOCOALESCINGQUEUE_DEFAULT_STARTING_SIZE,
							   int aResizeAmount = CKFIFOCOALESCINGQUEUE_DEFAULT_INCREMENT_SIZE ) :
			mElements(),
			mKeys(anInitialCapacity, aResizeAmount),
			mMutex(),
			mConditional(mMutex)
		{
		}


		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKFIFOCoalescingQueue( CKFIFOCoalescingQueue<K,T> & anOther ) :
			mElements(),
			mKeys(),
			mMutex(),
			mConditional(mMutex)
		{
			// let the '=' operator do it
			*this = anOther;
		}


		CKFIFOCoalescingQueue( const CKFIFOCoalescingQueue<K,T> & anOther ) :
			mElements(),
			mKeys(),
			mMutex(),
			mConditional(mMutex)
		{
			// let the '=' operator do it
			*this = anOther;
		}


		/*
		 * This is the destructor for the queue and makes sure that
		 * everything is cleaned up before leaving.
		 */
		virtual ~CKFIFOCoalescingQueue()
		{
			// everything here takes care of itself.
		}


		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKFIFOCoalescingQueue<K,T> & operator=( const CKFIFOCoalescingQueue<K,T> & anOther )
		{
			return operator=((CKFIFOCoalescingQueue<K,T> &)anOther);
		}


		CKFIFOCoalescingQueue<K,T> & operator=( CKFIFOCoalescingQueue<K,T> & anOther )
		{
			// first, copy in the easy values
			mElements = anOther.mElements;
			mKeys = anOther.mKeys;

			return *this;
		}


		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This pair of methods does what you'd expect - it returns the
		 * length of the queue as it exists at the present time. It's
		 * got two names because there are so many different queue
		 * implementations that it's often convenient to use one or the
		 * other to remain consistent.
		 */
		int size() const
		{
			return mKeys.size();
		}


		int length() const
		{
			return mKeys.size();
		}


		/*
		 * This method returns the current capacity of the queue and
		 * is NOT the size per se. The capacity is what this queue
		 * will hold before having to resize it's contents.
		 */
		int capacity() const
		{
			return mKeys.capacity();
		}


		/*
		 * Because there may be times that the user wants to lock us up
		 * for change, we're going to expose this here so it's easy for them
		 * to iterate, for example.
		 */
		void lock()
		{
			mMutex.lock();
		}


		void unlock()
		{
			mMutex.unlock();
		}


		/********************************************************
		 *
		 *                Element Accessing Methods
		 *
		 ********************************************************/
		/*
		 * There needs to be a simple way to add an element to the queue.
		 * This is it.
		 */
		void push( const K & aKey, const T & anElem )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// add the data to the map - replacing or adding as it may be
			mElements[aKey] = anElem;
			// push the key onto the key stack - ignoring duplicates
			mKeys.push(aKey);
			// see if we need to wake any waiters
			if (mKeys.size() == 1) {
				mConditional.wakeWaiter();
			}
		}


		/*
		 * When you have a map of keys and values and you want to place them
		 * all onto this queue, you can call this method.
		 */
		void push( const std::map<K,T> & aMap )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// get the size of the keys right now
			int		startingSize = mKeys.size();
			// now let's rip through the map of stuff and add what's needed
			typename std::map<K,T>::const_iterator		i;
			for (i = aMap.being; i != aMap.end(); ++i) {
				// add the data to the map - replacing or adding as it may be
				mElements[i->first] = i->second;
				// push the key onto the key stack - ignoring duplicates
				mKeys.push(i->first);
			}
			// see if we need to wake any waiters
			if ((startingSize == 0) && (mKeys.size() > 0)) {
				mConditional.wakeWaiter();
			}
		}


		/*
		 * When you want to remove the next element off the queue,
		 * this method will return that element and it will be removed
		 * from the queue itself.
		 */
		T pop()
		{
			T		retval;

			// first, see if we have anything to do
			if (mKeys.empty()) {
				std::ostringstream	msg;
				msg << "CKFIFOCoalescingQueue<K,T>::pop() - there are no elements in this queue "
					"to return. Please use the size() method to verify that there "
					"is something to get.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// grab the first one in the queue
			K	key = mKeys.pop();
			// ...now get the value out of the map and remove it from the map
			retval = mElements[key];
			mElements.erase(key);

			return retval;
		}


		/*
		 * When you want to remove more than one element from the queue
		 * in fact, remove up to 'aNumber' of them, then you can call this
		 * method and it will return these elements in a CKVector<T>.
		 */
		CKVector<T> pop( int aNumber )
		{
			CKVector<T>		retval;

			// first, see if we have anything to do
			if (mKeys.empty()) {
				std::ostringstream	msg;
				msg << "CKFIFOCoalescingQueue<K,T>::pop(int) - there are no elements "
					"in this queue to return. Please use the size() method to verify "
					"that there is something to get.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// figure out how many to grab off the queue
			int		cnt = (aNumber > mKeys.size() ? mKeys.size() : aNumber);
			for (int i = 0; i < cnt; ++i) {
				// grab the first one in the queue
				K	key = mKeys.pop();
				// ...now get the value out of the map and remove it from the map
				retval.addToEnd(mElements[key]);
				mElements.erase(key);
			}

			return retval;
		}


		/*
		 * When you want to remove the next element off the queue,
		 * this method will return that element and it will be removed
		 * from the queue itself. The difference with this method is
		 * that you will be guaranteed of getting something, but you
		 * may have to wait for it. This version will wait nicely if
		 * the queue is empty, waiting for the time when something
		 * is added, and then pop it off.
		 */
		T popSomething()
		{
			T		retval;

			// make a test based on this queue
			CKFIFOCoalescingQueueNotEmptyTest<K,T>	tst(this);
			// wait until we get something in the queue
			mConditional.lockAndTest(tst);

			// grab the first one in the queue
			K	key = mKeys.pop();
			// ...now get the value out of the map and remove it from the map
			retval = mElements[key];
			mElements.erase(key);

			// now we can unlock this guy
			mMutex.unlock();

			return retval;
		}


		/*
		 * This method removes ALL copies of the argument from the queue
		 * and compresses out the empty spaces from the queue. If the
		 * argument does not exist, nothing is done.
		 */
		void remove( const T & anOther )
		{
			/*
			 * This is interesting because there will only be *one* possible
			 * hit in the values of the map because of the keying we're doing.
			 * If not, then the user has a serious issue. But aside from that,
			 * we need to find the value, remove the keys from the queue, and
			 * then remove the key/value from the map. Easy.
			 */
			typename std::map<K,T>::iterator	i;
			for (i = mElements.begin(); i != mElements.end(); ++i) {
				if (i->second == anOther) {
					mKeys.remove(i->first);
					mElements.erase(i);
					break;
				}
			}
		}


		/*
		 * This method removes ALL elements represented by the argument
		 * as a key when they were pushed onto this queue, and compresses
		 * out the empty spaces from the queue. If the argument does not
		 * exist, nothing is done.
		 */
		void remove( const K & aKey )
		{
			// lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			/*
			 * This is interesting because there will only be *one* possible
			 * hit in the values of the map because of the keying we're doing.
			 * If not, then the user has a serious issue. But aside from that,
			 * we need to find the value, remove the keys from the queue, and
			 * then remove the key/value from the map. Easy.
			 */
			if (mElements.find(aKey) != mElements.end()) {
				mKeys.remove(aKey);
				mElements.erase(aKey);
			}
		}


		/*
		 * This method returns true if the representation of the queue
		 * is really empty. This may not mean that the instance is without
		 * storage allocated, it only means that there's no information in
		 * that storage.
		 */
		bool empty()
		{
			return mKeys.empty();
		}


		bool empty() const
		{
			return mKeys.empty();
		}


		/*
		 * This method allows the user to clear out the buffer explicitly.
		 * It's handy from time to time if you're going to be re-using the
		 * same queue again and again.
		 */
		void clear()
		{
			CKStackLocker	lockem(&mMutex);

			mElements.clear();
			mKeys.clear();
		}


		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKFIFOCoalescingQueues are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKFIFOCoalescingQueue<K,T> & anOther )
		{
			bool		equal = true;

			// lock up both lists for the comparison
			mMutex.lock();
			anOther.mMutex.lock();

			// check the sizes
			if (equal) {
				if (mKeys.size() != anOther.mKeys.size()) {
					equal = false;
				}
			}

			// check the elements
			if (equal) {
				if (mElements != anOther.mElements) {
					equal = false;
				}
			}

			// check the keys (including the order
			if (equal) {
				if (mKeys != anOther.mKeys) {
					equal = false;
				}
			}

			// unlock both lists now
			anOther.mMutex.unlock();
			mMutex.unlock();

			return equal;
		}


		bool operator==( const CKFIFOCoalescingQueue<K,T> & anOther )
		{
			return operator==((CKFIFOCoalescingQueue<K,T> &)anOther);
		}


		bool operator==( CKFIFOCoalescingQueue<K,T> & anOther ) const
		{
			return ((CKFIFOCoalescingQueue<K,T> *)this)->operator==(anOther);
		}


		bool operator==( const CKFIFOCoalescingQueue<K,T> & anOther ) const
		{
			return ((CKFIFOCoalescingQueue<K,T> *)this)->operator==((CKFIFOCoalescingQueue<K,T> &)anOther);
		}


		/*
		 * This method checks to see if the two CKFIFOCoalescingQueues are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKFIFOCoalescingQueue<K,T> & anOther )
		{
			return !operator==(anOther);
		}


		bool operator!=( const CKFIFOCoalescingQueue<K,T> & anOther )
		{
			return !operator==((CKFIFOCoalescingQueue<K,T> &)anOther);
		}


		bool operator!=( CKFIFOCoalescingQueue<K,T> & anOther ) const
		{
			return !((CKFIFOCoalescingQueue<K,T> *)this)->operator==(anOther);
		}


		bool operator!=( const CKFIFOCoalescingQueue<K,T> & anOther ) const
		{
			return !((CKFIFOCoalescingQueue<K,T> *)this)->operator==((CKFIFOCoalescingQueue<K,T> &)anOther);
		}


		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		CKString toString() const
		{
			CKString	retval = "<not yet implemented>";
			return retval;
		}

	private:
		/*
		 * This is the std::map that is the core of the storage of the
		 * CKFIFOCoalescingQueue. It's simple, but it's very effective as
		 * I can put all the key,values pushed onto this queue here and
		 * then use the 'keys' FIFOQueue to preserve the order.
		 */
		std::map<K,T>	mElements;
		/*
		 * This is the "FIFO" nature of the queue as this guy will be
		 * preserving the order of the elements as they are pushed onto
		 * this queue and will be set to ignore duplicates.
		 */
		CKFIFOQueue<K>	mKeys;
		/*
		 * When it comes to messing with this queue, we're going to make
		 * sure that it can play well in a multi-threaded environment. To
		 * that end, we're going to cover the bases with a nice mutex.
		 */
		CKFWMutex		mMutex;
		/*
		 * When the user is convinced that they need to pop something off
		 * the queue no matter how long it takes, we need to have a
		 * conditional, and this is it.
		 */
		CKFWConditional	mConditional;
};

#endif	// __CKFIFOCOALESCINGQUEUE_H
