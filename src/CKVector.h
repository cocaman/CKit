/*
 * CKVector.h - this file defines a template class that is a random access
 *              vector. The purpose of this is to replace the STL std::vector
 *              for simple uses and make it a lot faster and more stable.
 *              This was originally written in the MarketMash server by
 *              Jeremy.
 *
 * $Id: CKVector.h,v 1.2 2004/09/23 19:51:30 drbob Exp $
 */
#ifndef __CKVECTOR_H
#define __CKVECTOR_H

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
#include "CKException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants
/*
 * This is the default starting size of this vector as it's going to
 * simply be an allocated array of elements. When an operation adds more
 * that this to the size of the vector, we'll have to allocate more and
 * then this won't be the size. But for starters, this is a good size.
 */
#define	CKVECTOR_DEFAULT_STARTING_SIZE		8
/*
 * When the buffer has to grow for this vector, this is the default
 * increment that it uses. Of course, this is *above* the necessary size
 * so if you're adding 2000 elements, we'll round it up to a factor of
 * the DEFAULT_INCREMENT_SIZE to make sure that we have at least a little
 * growing room.
 */
#define	CKVECTOR_DEFAULT_INCREMENT_SIZE		16


/*
 * This is the main class definition.
 */
template <class T> class CKVector
{
	public :
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This form of the constructor allows the user to specify the
		 * starting size of the vector as well as the growth size when
		 * the vector exceeds the starting size. Both of these default
		 * to reasonable values, but it's nice to be able to set them
		 * just the same.
		 */
		CKVector( int anInitialCapacity = CKVECTOR_DEFAULT_STARTING_SIZE,
				  int aResizeAmount = CKVECTOR_DEFAULT_INCREMENT_SIZE ) :
			mElements(NULL),
			mSize(0),
			mCapacity(anInitialCapacity),
			mInitialCapacity(anInitialCapacity),
			mCapacityIncrement(aResizeAmount),
			mMutex()
		{
			mElements = new T[mCapacity];
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::CKVector<T>(int, int) - the initial storage "
					"for this vector was to be " << mInitialCapacity << " elements, "
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
		CKVector( CKVector<T> & anOther ) :
			mElements(NULL),
			mSize(0),
			mCapacity(0),
			mInitialCapacity(0),
			mCapacityIncrement(0),
			mMutex()
		{
			// let the '=' operator do it
			*this = anOther;
		}


		CKVector( const CKVector<T> & anOther ) :
			mElements(NULL),
			mSize(0),
			mCapacity(0),
			mInitialCapacity(0),
			mCapacityIncrement(0),
			mMutex()
		{
			// let the '=' operator do it
			*this = anOther;
		}


		/*
		 * This is the destructor for the vector and makes sure that
		 * everything is cleaned up before leaving.
		 */
		~CKVector()
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
		CKVector<T> & operator=( const CKVector<T> & anOther )
		{
			return operator=((CKVector<T> &)anOther);
		}


		CKVector<T> & operator=( CKVector<T> & anOther )
		{
			// first, copy in the easy values
			mSize = anOther.mSize;
			mCapacity = anOther.mCapacity;
			mInitialCapacity = anOther.mInitialCapacity;
			mCapacityIncrement = anOther.mCapacityIncrement;

			// next, try to get the right sized array
			mElements = new T[mSize];
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::CKVector<T>(CKVector<T> &) - the initial storage "
					"for this vector was to be " << mInitialCapacity << " elements, "
					"but the creation failed. Please look into this allocation "
					"error as soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// now let's copy in the elements one by one
			for (int i = 0; i < mSize; i++) {
				mElements[i] = anOther.mElements[i];
			}
		}


		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This pair of methods does what you'd expect - it returns the
		 * length of the vector as it exists at the present time. It's
		 * got two names because there are so many different vector
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
		 * This method returns the current capacity of the vector and
		 * is NOT the size per se. The capacity is what this vector
		 * will hold before having to resize it's contents.
		 */
		int capacity() const
		{
			return mCapacity;
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
		 * There needs to be a simple way to add an element to the end
		 * of this vector. This is it.
		 */
		void addToEnd( T anElem )
		{
			_addToEnd((T &)anElem);
		}


		void addToEnd( T & anElem )
		{
			_addToEnd(anElem);
		}


	private:
		void _addToEnd( T & anElem )
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::addToFront(T &) - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// now see if we have to resize the array for this guy
			if (mSize >= mCapacity) {
				resize(mSize + mCapacityIncrement);
			}

			// put this guy where he belongs and up the count
			mElements[mSize] = anElem;
			mSize++;
		}
	public:


		/*
		 * There needs to be a simple way to add an element to the front
		 * of this vector. This is it.
		 */
		void addToFront( T & anElem )
		{
			_addToFront(anElem);
		}


		void addToFront( T anElem )
		{
			_addToFront((T &)anElem);
		}


	private:
		void _addToFront( T & anElem )
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::addToEnd(T &) - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

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
		}
	public:


		/*
		 * When you want to remove just the last element off the vector,
		 * this method will return that element and it will be removed
		 * from the vector itself.
		 */
		T popEnd()
		{
			T		retval;

			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::popEnd() - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
			if (mSize == 0) {
				std::ostringstream	msg;
				msg << "CKVector<T>::popEnd() - there are no elements in this vector "
					"to return. Please use the size() method to verify that there "
					"is something to get.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// get the last guy in the list and decrement the size by one
			mSize--;
			retval = mElements[mSize];

			return retval;
		}


		/*
		 * When you want to remove just the first element off the vector,
		 * this method will return that element and it will be removed
		 * from the vector itself.
		 */
		T popFront()
		{
			T		retval;

			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::popFront(T &) - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
			if (mSize == 0) {
				std::ostringstream	msg;
				msg << "CKVector<T>::popFront() - there are no elements in this vector "
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
		 * This method is the simplest way to access an element in the
		 * vector. It will create space, as needed, and that's a nice
		 * way to pre-size the array.
		 */
		T & operator[]( int anIndex )
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::operator[](int) - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// see if the requested index is nonsense
			if (anIndex < 0) {
				std::ostringstream	msg;
				msg << "CKVector<T>::operator[](int) - the requested index: " <<
					anIndex << " is out of range. Please make sure you give this "
					"guy reasonable values.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// see if we need to resize this guy
			if (anIndex >= mCapacity) {
				resize(anIndex + 1);
			}

			// the size is at least this index plus one
			if (anIndex >= mSize) {
				mSize = anIndex + 1;
			}

			return mElements[anIndex]; 
		}


		/*
		 * This version of the method assumes that you cannot go adding
		 * elements to the vector, and so if you ask for one that is
		 * ouside the sizeing size of the vector, you're going to get an
		 * exception.
		 */
		const T & operator[]( int anIndex ) const
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::operator[](int) const - the storage for "
					"this vector is NULL and that is a data corruption problem "
					"that needs to be looked into as soon as possible. This "
					"should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// see if the requested index is nonsense
			if ((anIndex < 0) || (anIndex >= mSize)) {
				std::ostringstream	msg;
				msg << "CKVector<T>::operator[](int) const - the requested index: " <<
					anIndex << " is out of the accepted range for this vector: 0->" <<
					(mSize-1) << ". Please make sure you give this guy reasonable "
					"values.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			return mElements[anIndex]; 
		}


		/*
		 * When you need to remove (erase) a section of the string, this
		 * method is the one to call. You give it a starting index and
		 * optionally a number of characters to delete and they will be
		 * removed from the string. If the length is not supplied, then
		 * the characters from 'aStartingIndex' to the end of the string
		 * will be removed.
		 */
		void erase( int aStartingIndex,  int aLength = -1 )
		{
			// first, see if we have anything to do
			if (mElements == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::erase(int, int) - the storage for this vector "
					"is NULL and that is a data corruption problem that needs to "
					"be looked into as soon as possible. This should never happen.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// next, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// make sure what's being asked of us is possible
			if (aStartingIndex < 0) {
				std::ostringstream	msg;
				msg << "CKVector<T>::erase(int, int) - the provided starting index is: " <<
					aStartingIndex << " and that makes no sense. Please make sure that "
					"the index falls within the string's length.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
			if ((aLength > 0) && ((aStartingIndex + aLength) > mSize)) {
				std::ostringstream	msg;
				msg << "CKVector<T>::erase(int, int) - the provided starting index is: " <<
					aStartingIndex << " and the length is: " << aLength << " that "
					"combine to make a list longer than this vevtor is. Please make "
					"Please make sure that the erased elements exists in the "
					"current vector.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			/*
			 * Now we need to handle the erasure in one of two ways.... if it's
			 * in the middle of the string then it's a left shift, but it it's
			 * 'to the end', then it's a simpl change in the size. Both end up
			 * changing the size, so don't forget that.
			 */
			if (aLength < 0) {
				// it's a 'to the end' call
				mSize = aStartingIndex;
			} else {
				// it's within the vector, so it's a left shift
				int	cnt = mSize - aStartingIndex - aLength;
				for (int i = 0; i < aLength; i++) {
					mElements[aStartingIndex + i] =
								mElements[aStartingIndex + i + aLength];
				}
				mSize -= aLength;
			}
		}


		/*
		 * This method returns true if the representation of the vector
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
		 * same vector again and again.
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
		 * This method resizes the vector to contain exactly the
		 * specified number of elements - no more no less. If there
		 * are currently elements in the vector, they will be copied
		 * to the new vector assuming they can be copied.
		 */
		void resize( int aNewSize )
		{
			// we need to create a new buffer that's the requested size
			T	*resultant = new T[aNewSize];
			if (resultant == NULL) {
				std::ostringstream	msg;
				msg << "CKVector<T>::resize(int) - while trying to create a new "
					"buffer of " << aNewSize << " elements, an allocation error "
					"occurred. Please look into this as soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
			}

			/*
			 * Now we need to update this instance with the new buffer. First,
			 * we need to copy over any data from the existing vector (if there
			 * is one) to this new buffer, and then update all the attributes
			 * about the vector itself.
			 */
			// first, see if we have something to move into this new vector
			int		copyCnt = (mSize < aNewSize ? mSize : aNewSize);
			if (mElements != NULL) {
				// copy over just what will fit in the new vector
				for (int i = 0; i < copyCnt; i++) {
					resultant[i] = mElements[i];
				}
				// ...and delete the old vector
				delete [] mElements;
				mElements = NULL;
			}
			// next, update all the ivars that have been impacted
			mElements = resultant;
			mSize = copyCnt;
			mCapacity = aNewSize;
		}


		/*
		 * This method checks to see if the two CKVectors are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKVector<T> & anOther )
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


		bool operator==( const CKVector<T> & anOther )
		{
			return operator==((CKVector<T> &)anOther);
		}


		bool operator==( CKVector<T> & anOther ) const
		{
			return ((CKVector<T> *)this)->operator==(anOther);
		}


		bool operator==( const CKVector<T> & anOther ) const
		{
			return ((CKVector<T> *)this)->operator==((CKVector<T> &)anOther);
		}


		/*
		 * This method checks to see if the two CKVectors are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKVector<T> & anOther )
		{
			return !operator==(anOther);
		}


		bool operator!=( const CKVector<T> & anOther )
		{
			return !operator==((CKVector<T> &)anOther);
		}


		bool operator!=( CKVector<T> & anOther ) const
		{
			return !((CKVector<T> *)this)->operator==(anOther);
		}


		bool operator!=( const CKVector<T> & anOther ) const
		{
			return !((CKVector<T> *)this)->operator==((CKVector<T> &)anOther);
		}


		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 *
		 * If the default 'false' is used then the only information that's
		 * returned is with regards to the node itself and not a complete
		 * dump of the tree rooted at this node. Pass in a 'true' if you
		 * want to see the entire tree at this node.
		 */
		CKString toString() const
		{
			CKString	retval = "<not yet implemented>";
			return retval;
		}

	private:
		/*
		 * This is the element array that is the core of the storage of
		 * the CKVector. It's simple, but it's very effective as I can then
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
		 * When it comes to messing with this vector, we're going to make
		 * sure that it can play well in a multi-threaded environment. To
		 * that end, we're going to cover the bases with a nice mutex.
		 */
		CKFWMutex		mMutex;
};

#endif	// __CKVECTOR_H
