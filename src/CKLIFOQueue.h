/*
 * CKLIFOQueue.h - this file defines a template class that is a simple
 *                 last-in-first-out queue of something. This is really
 *                 nice in that we can choose to have it ignore duplicates
 *                 or we can have duplicates included.
 *
 * $Id: CKLIFOQueue.h,v 1.4 2008/04/25 19:03:01 drbob Exp $
 */
#ifndef __CKLIFOQUEUE_H
#define __CKLIFOQUEUE_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKString.h"
#include "CKFWMutex.h"
#include "CKStackLocker.h"
#include "CKFWConditional.h"
#include "CKException.h"

//	Forward Declarations
template <class T> class CKLIFOQueue;

//	Public Constants

//	Public Datatypes

//	Public Data Constants
/*
 * This is the default starting size of this queue as it's going to
 * simply be an allocated array of elements. When an operation adds more
 * that this to the size of the vector, we'll have to allocate more and
 * then this won't be the size. But for starters, this is a good size.
 */
#define	CKLIFOQUEUE_DEFAULT_STARTING_SIZE		8
/*
 * When the buffer has to grow for this queue, this is the default
 * increment that it uses. Of course, this is *above* the necessary size
 * so if you're adding 2000 elements, we'll round it up to a factor of
 * the DEFAULT_INCREMENT_SIZE to make sure that we have at least a little
 * growing room.
 */
#define	CKLIFOQUEUE_DEFAULT_INCREMENT_SIZE		16


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
template <class T> class CKLIFOQueueNotEmptyTest :
	public ICKFWConditionalSpuriousTest
{
	public:
		CKLIFOQueueNotEmptyTest( CKLIFOQueue<T> *aQueue ) :
			mQueuePtr(aQueue)
		{
		}

		virtual ~CKLIFOQueueNotEmptyTest()
		{
			mQueuePtr = NULL;
		}

		virtual int test()
		{
			return ((mQueuePtr != NULL) && mQueuePtr->empty());
		}

	private:
		CKLIFOQueue<T>	*mQueuePtr;
};


/*
 * This is the main class definition.
 */
template <class T> class CKLIFOQueue
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
		CKLIFOQueue( int anInitialCapacity = CKLIFOQUEUE_DEFAULT_STARTING_SIZE,
					 int aResizeAmount = CKLIFOQUEUE_DEFAULT_INCREMENT_SIZE ) :
			mElements(NULL),
			mSize(0),
			mCapacity(anInitialCapacity),
			mInitialCapacity(anInitialCapacity),
			mCapacityIncrement(aResizeAmount),
			mElementsAreUnique(true),
			mMutex(),
			mConditional(mMutex)
		{
			mElements = new T[mCapacity];
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::CKLIFOQueue<T>(int, int) - the initial storage "
					"for this queue was to be " << mInitialCapacity << " elements, "
					"but the creation failed. Please look into this allocation "
					"error as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		}


		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKLIFOQueue( CKLIFOQueue<T> & anOther ) :
			mElements(NULL),
			mSize(0),
			mCapacity(0),
			mInitialCapacity(0),
			mCapacityIncrement(0),
			mElementsAreUnique(true),
			mMutex(),
			mConditional(mMutex)
		{
			// let the '=' operator do it
			*this = anOther;
		}


		CKLIFOQueue( const CKLIFOQueue<T> & anOther ) :
			mElements(NULL),
			mSize(0),
			mCapacity(0),
			mInitialCapacity(0),
			mCapacityIncrement(0),
			mElementsAreUnique(true),
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
		virtual ~CKLIFOQueue()
		{
			if (mElements != NULL) {
				delete [] mElements;
				mElements = NULL;
			}
		}


		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKLIFOQueue<T> & operator=( const CKLIFOQueue<T> & anOther )
		{
			return operator=((CKLIFOQueue<T> &)anOther);
		}


		CKLIFOQueue<T> & operator=( CKLIFOQueue<T> & anOther )
		{
			// first, copy in the easy values
			mSize = anOther.mSize;
			mCapacity = anOther.mCapacity;
			mInitialCapacity = anOther.mInitialCapacity;
			mCapacityIncrement = anOther.mCapacityIncrement;
			mElementsAreUnique = anOther.mElementsAreUnique;

			// next, try to get the right sized array
			mElements = new T[mSize];
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::CKLIFOQueue<T>(CKLIFOQueue<T> &) - the initial storage "
					"for this queue was to be " << mInitialCapacity << " elements, "
					"but the creation failed. Please look into this allocation "
					"error as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// now let's copy in the elements one by one
			for (int i = 0; i < mSize; i++) {
				mElements[i] = anOther.mElements[i];
			}

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
			return mSize;
		}


		int length() const
		{
			return mSize;
		}


		/*
		 * This method returns the current capacity of the queue and
		 * is NOT the size per se. The capacity is what this queue
		 * will hold before having to resize it's contents.
		 */
		int capacity() const
		{
			return mCapacity;
		}


		/*
		 * This method can be used to set if the push() method allows
		 * for duplicates to exist in the queue. The default is 'no',
		 * meaning that duplicates are NOT allowed in the queue, but
		 * by calling this method with a 'false' allows subsequent
		 * calls to push() to add duplicates to the queue.
		 */
		void setElementsAreUnique( bool aFlag )
		{
			// make sure this is done in a thread-safe manner
			mMutex.lock();
			mElementsAreUnique = aFlag;
			mMutex.unlock();
		}


		/*
		 * This method can be used to see if the push() method allows
		 * for duplicates to exist in the queue. If this method returns
		 * 'true' then all the elements in the queue should be unique.
		 */
		bool elementsAreUnique()
		{
			return mElementsAreUnique;
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
		void push( const T & anElem )
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::push(T &) - the storage for this queue "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// see if we are making sure they are unique
			if (!mElementsAreUnique || !contains(anElem)) {
				// now see if we have to resize the array for this guy
				if (mSize >= mCapacity) {
					resize(mSize + mCapacityIncrement);
				}

				// now we need to move everything over one to the right
				for (int i = mSize; i > 0; i--) {
					mElements[i] = mElements[i-1];
				}

				// put this guy where he belongs and up the count
				mElements[0] = anElem;
				mSize++;

				// see if we need to wake any waiters
				if (mSize == 1) {
					mConditional.wakeWaiter();
				}
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
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::pop(T &) - the storage for this queue "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
			if (mSize == 0) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::pop() - there are no elements in this queue "
					"to return. Please use the size() method to verify that there "
					"is something to get.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// grab the first one in the list
			retval = mElements[0];

			// now we need to move everything over one to the left
			mSize--;
			for (int i = 0; i < mSize; i++) {
				mElements[i] = mElements[i+1];
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

			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::pop(T &) - the storage for this queue "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// now make a test based on this queue
			CKLIFOQueueNotEmptyTest<T>	tst(this);
			// wait until we get something in the queue
			mConditional.lockAndTest(tst);

			// grab the first one in the list
			retval = mElements[0];

			// now we need to move everything over one to the left
			mSize--;
			for (int i = 0; i < mSize; i++) {
				mElements[i] = mElements[i+1];
			}

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
			int		i = 0;
			while (i < mSize) {
				// see if this guy needs to be deleted
				if (mElements[i] == anOther) {
					for (int j = i+1; j < mSize; j++) {
						mElements[j-1] = mElements[j];
					}
					// we have one less thing in the list
					mSize--;
				} else {
					// look at the mext guy in the array
					i++;
				}
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
			return (mSize == 0);
		}


		bool empty() const
		{
			return (mSize == 0);
		}


		/*
		 * This method allows the user to clear out the buffer explicitly.
		 * It's handy from time to time if you're going to be re-using the
		 * same queue again and again.
		 */
		void clear()
		{
			mSize = 0;
		}


		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKLIFOQueues are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKLIFOQueue<T> & anOther )
		{
			bool		equal = true;

			// lock up both lists for the comparison
			mMutex.lock();
			anOther.mMutex.lock();

			// check the sizes
			if (equal) {
				if (mSize != anOther.mSize) {
					equal = false;
				}
			}

			// check the elements
			if (equal) {
				for (int i = 0; i < mSize; i++) {
					if (mElements[i] != anOther.mElements[i]) {
						equal = false;
						break;
					}
				}
			}

			// unlock both lists now
			anOther.mMutex.unlock();
			mMutex.unlock();

			return equal;
		}


		bool operator==( const CKLIFOQueue<T> & anOther )
		{
			return operator==((CKLIFOQueue<T> &)anOther);
		}


		bool operator==( CKLIFOQueue<T> & anOther ) const
		{
			return ((CKLIFOQueue<T> *)this)->operator==(anOther);
		}


		bool operator==( const CKLIFOQueue<T> & anOther ) const
		{
			return ((CKLIFOQueue<T> *)this)->operator==((CKLIFOQueue<T> &)anOther);
		}


		/*
		 * This method checks to see if the two CKLIFOQueues are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKLIFOQueue<T> & anOther )
		{
			return !operator==(anOther);
		}


		bool operator!=( const CKLIFOQueue<T> & anOther )
		{
			return !operator==((CKLIFOQueue<T> &)anOther);
		}


		bool operator!=( CKLIFOQueue<T> & anOther ) const
		{
			return !((CKLIFOQueue<T> *)this)->operator==(anOther);
		}


		bool operator!=( const CKLIFOQueue<T> & anOther ) const
		{
			return !((CKLIFOQueue<T> *)this)->operator==((CKLIFOQueue<T> &)anOther);
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

	protected:
		/*
		 * This method returns true if the argument is contained in the
		 * queue. This method uses the '==' operator on the object, so
		 * you need to make sure that it is implemented.
		 */
		bool contains( const T & anOther )
		{
			bool		hitIt = false;
			for (int i = 0; i < mSize; i++) {
				if (mElements[i] == anOther) {
					hitIt = true;
					break;
				}
			}
			return hitIt;
		}


		/*
		 * When you want to find the index of the object in the queue,
		 * the find() method does the job. If the object is NOT in the
		 * queue (using the '==' operator, which must be defined), then
		 * this method will return -1.
		 */
		int find( const T & anOther )
		{
			int		index = -1;
			for (int i = 0; i < mSize; i++) {
				if (mElements[i] == anOther) {
					index = i;
					break;
				}
			}
			return index;
		}


		/*
		 * This method resizes the queue to contain exactly the
		 * specified number of elements - no more no less. If there
		 * are currently elements in the queue, they will be copied
		 * to the new queue assuming they can be copied.
		 */
		void resize( int aNewSize )
		{
			// we need to create a new buffer that's the requested size
			T	*resultant = new T[aNewSize];
			if (resultant == NULL) {
				std::ostringstream	msg;
				msg << "CKLIFOQueue<T>::resize(int) - while trying to create a new "
					"buffer of " << aNewSize << " elements, an allocation error "
					"occurred. Please look into this as soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
			}

			/*
			 * Now we need to update this instance with the new buffer. First,
			 * we need to copy over any data from the existing queue (if there
			 * is one) to this new buffer, and then update all the attributes
			 * about the queue itself.
			 */
			// first, see if we have something to move into this new queue
			int		copyCnt = (mSize < aNewSize ? mSize : aNewSize);
			if (mElements != NULL) {
				// copy over just what will fit in the new queue
				for (int i = 0; i < copyCnt; i++) {
					resultant[i] = mElements[i];
				}
				// ...and delete the old queue
				delete [] mElements;
				mElements = NULL;
			}
			// next, update all the ivars that have been impacted
			mElements = resultant;
			mSize = copyCnt;
			mCapacity = aNewSize;
		}


	private:
		/*
		 * This is the element array that is the core of the storage of
		 * the CKLIFOQueue. It's simple, but it's very effective as I can then
		 * leverage all the other array processing in C, but at the same
		 * time have a flexible storage system.
		 */
		T				*mElements;
		/*
		 * This is the current size of the array and should always be
		 * kept up to date with respect to the buffer above.
		 */
		int				mSize;
		/*
		 * This is a very important value in that it's the current maximum
		 * capacity of the array and that's it. If the operation that's
		 * about to be performed is going to cause the buffer to exceed this
		 * length, then we'll need to call resize() with a larger value
		 * and make more room.
		 */
		int				mCapacity;
		/*
		 * Because we can also shrink this guy, we need to keep around the
		 * initial capacity of the string so that this buffer never dips below
		 * that value no matter what.
		 */
		int				mInitialCapacity;
		/*
		 * When the buffer has to grow, this is the size by which it will
		 * grow. It's adjustable so that as things are more dynamic, the
		 * size can increase and cause less re-allocations to occur.
		 */
		int				mCapacityIncrement;
		/*
		 * The queue will be able to understand that for some applications
		 * the elements in the queue need to be unique, but maintain their
		 * order if a duplicate comes in. In these cases, we'll only add
		 * an element to the queue if there isn't already a match.
		 */
		bool			mElementsAreUnique;
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

#endif	// __CKLIFOQUEUE_H
