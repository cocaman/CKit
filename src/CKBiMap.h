/*
 * CKBiMap.h - this file defines a template class that is a bi-directional
 *             map (K, V) where the lookup of keys for a value are unique as
 *             well and in fact are just another map reversing the keys and
 *             values. The advantage of this is that it's as fast to look up
 *             one direction as the other and that makes it ideal for those
 *             cases where you have to maintain a translation between two
 *             systems and need to go quickly from one to the other.
 *
 * $Id: CKBiMap.h,v 1.1 2008/05/21 19:58:29 drbob Exp $
 */
#ifndef __CKBIMAP_H
#define __CKBIMAP_H

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
#include "CKException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
template <class K, class V> class CKBiMap
{
	public :
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This form of the constructor gets everything set up for
		 * throwing things into this map.
		 */
		CKBiMap() :
			mForward(),
			mReverse(),
			mMutex()
		{
		}


		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKBiMap( CKBiMap<K, V> & anOther ) :
			mForward(),
			mReverse(),
			mMutex()
		{
			// let the '=' operator do it
			*this = anOther;
		}


		CKBiMap( const CKBiMap<K, V> & anOther ) :
			mForward(),
			mReverse(),
			mMutex()
		{
			// let the '=' operator do it
			*this = anOther;
		}


		/*
		 * This is the destructor for the vector and makes sure that
		 * everything is cleaned up before leaving.
		 */
		virtual ~CKBiMap()
		{
			// thankfully, everything cleans itself up pretty nicely
		}


		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKBiMap<K, V> & operator=( const CKBiMap<K, V> & anOther )
		{
			return operator=((CKBiMap<K, V> &)anOther);
		}


		CKBiMap<K, V> & operator=( CKBiMap<K, V> & anOther )
		{
			// make sure that we don't do this to ourselves
			if (this != & anOther) {
				// first, copy in the easy values
				mForward = anOther.mForward;
				mReverse = anOther.mReverse;
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
		 * number of key/value pairs in the map as it exists at the
		 * present time. It's got two names because there are so many
		 * different implementations that it's often convenient to use
		 * one or the other to remain consistent.
		 */
		int size() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...get the size and return it
			return mForward.size();
		}


		int length() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...get the size and return it
			return mForward.size();
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
		 *                  Accessing Methods
		 *
		 ********************************************************/
		/*
		 * This method adds the key/value pair to the map in both
		 * directions so that it can be accessed by the key or by the
		 * value equally well. It's important to remember that the
		 * reverse map requires uniqueness, so the mapping needs to
		 * be unique in both directions. Just be warned.
		 */
		void put( const K & aKey, const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// add the data in both directions
			mForward[aKey] = aValue;
			mReverse[aValue] = aKey;
		}


		/*
		 * This method is used to get the value (V) for the supplied
		 * key (K). It returns a copy so that we can be sure that
		 * this method is thread-safe and the value you get will not
		 * change or disappear underneath you. If there's nothing
		 * for the key then this method will throw an exception -
		 * you might want to call keyExists() first to make sure the
		 * key exists in the mapping.
		 */
		V get( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the key exists in the map
			typename std::map<K, V>::iterator	i = mForward.find(aKey);
			if (i == mForward.end()) {
				std::ostringstream	msg;
				msg << "CKBiMap<K,V>::get(const K &) - the requested key: " <<
					aKey << " is not in the map. Please make sure that the key "
					"exists in the map before calling this method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// return a *copy* of the value as it exists now
			return mForward[aKey];
		}


		/*
		 * This method is used to get the key (K) for the supplied
		 * value (V). It returns a copy so that we can be sure that
		 * this method is thread-safe and the key you get will not
		 * change or disappear underneath you. If there's nothing
		 * for the value then this method will throw an exception -
		 * you might want to call valueExists() first to make sure the
		 * value exists in the mapping.
		 */
		K getKey( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the value exists in the map
			typename std::map<V, K>::iterator	i = mReverse.find(aValue);
			if (i == mReverse.end()) {
				std::ostringstream	msg;
				msg << "CKBiMap<K,V>::getKey(const V &) - the requested value: " <<
					aValue << " is not in the map. Please make sure that the key "
					"exists in the map before calling this method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// return a *copy* of the key as it exists now
			return mReverse[aValue];
		}


		/*
		 * This method will test to see if the supplied key (K) is in
		 * the bi-directional map. If it is, then you can get() it's
		 * value without throwing an exception.
		 */
		bool keyExists( const K & aKey )
		{
			bool		exists = false;

			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the key exists in the map
			typename std::map<K, V>::iterator	i = mForward.find(aKey);
			exists = (i != mForward.end());

			return exists;
		}


		/*
		 * This method will test to see if the supplied value (V) is in
		 * the bi-directional map. If it is, then you can getKey() it's
		 * key without throwing an exception.
		 */
		bool valueExists( const V & aValue )
		{
			bool		exists = false;

			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the value exists in the map
			typename std::map<V, K>::iterator	i = mReverse.find(aValue);
			exists = (i != mReverse.end());

			return exists;
		}


		/*
		 * This method is a simple way to access a *copy* of the value
		 * in the map. Normally, this would return a reference that could
		 * be used on the lhs as well as the rhs, but because we can't
		 * know the corresponding value to go with this key (in case we
		 * had to add it to the map), we can only use this as a short-hand
		 * value to "read" the values. That's why it's returning a copy.
		 */
		V operator[]( const K & aKey )
		{
			return get(aKey);
		}


		/*
		 * This method is a simple way to access a *copy* of the value
		 * in the map. Normally, this would return a reference that could
		 * be used on the lhs as well as the rhs, but because we can't
		 * know the corresponding value to go with this key (in case we
		 * had to add it to the map), we can only use this as a short-hand
		 * value to "read" the values. That's why it's returning a copy.
		 */
		const V operator[]( const K & aKey ) const
		{
			return get(aKey);
		}


		/*
		 * This method is a simple way to access a *copy* of the key
		 * in the map. Normally, this would return a reference that could
		 * be used on the lhs as well as the rhs, but because we can't
		 * know the corresponding key to go with this value (in case we
		 * had to add it to the map), we can only use this as a short-hand
		 * value to "read" the keys. That's why it's returning a copy.
		 */
		K operator()( const V & aValue )
		{
			return getKey(aValue);
		}


		/*
		 * This method is a simple way to access a *copy* of the key
		 * in the map. Normally, this would return a reference that could
		 * be used on the lhs as well as the rhs, but because we can't
		 * know the corresponding key to go with this value (in case we
		 * had to add it to the map), we can only use this as a short-hand
		 * value to "read" the keys. That's why it's returning a copy.
		 */
		const K operator()( const V & aValue ) const
		{
			return getKey(aValue);
		}


		/*
		 * When you need to remove a key/value pair from the map based
		 * on the key, then this is the method to call. It removes the
		 * key/value pair from both directions, and if the key does not
		 * exist, then nothing is done - no exception, just nothing
		 * done.
		 */
		void erase( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// erase the elements if they exist
			typename std::map<K, V>::iterator	i = mForward.find(aKey);
			if (i != mForward.end()) {
				// erase the reverse first so the iterator is valid
				mReverse.erase(i->second);
				// ...now erase the pair from the forward map
				mForward.erase(i);
			}
		}


		/*
		 * When you need to remove a key/value pair from the map based
		 * on the value, then this is the method to call. It removes the
		 * key/value pair from both directions, and if the value does not
		 * exist, then nothing is done - no exception, just nothing
		 * done.
		 */
		void eraseValue( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// erase the elements if they exist
			typename std::map<V, K>::iterator	i = mReverse.find(aValue);
			if (i != mReverse.end()) {
				// erase the forward first so the iterator is valid
				mForward.erase(i->second);
				// ...now erase the pair from the reverse map
				mReverse.erase(i);
			}
		}


		/*
		 * When you have a key iterator on this map and you want to
		 * remove the key/value pair referenced by this iterator, you
		 * can call this method. It'll remove the key/value pair from
		 * each direction.
		 */
		void eraseKey( typename std::map<K, V>::iterator & anIter )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			if (anIter != mForward.end()) {
				// erase the reverse first so the iterator is valid
				mReverse.erase(anIter->second);
				// ...now erase the pair from the forward map
				mForward.erase(anIter);
			}
		}


		/*
		 * When you have a value iterator on this map and you want to
		 * remove the key/value pair referenced by this iterator, you
		 * can call this method. It'll remove the key/value pair from
		 * each direction.
		 */
		void eraseValue( typename std::map<V, K>::iterator & anIter )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			if (anIter != mReverse.end()) {
				// erase the forward first so the iterator is valid
				mForward.erase(anIter->second);
				// ...now erase the pair from the reverse map
				mReverse.erase(anIter);
			}
		}


		/*
		 * This method returns true if the representation of the map
		 * is really empty. This may not mean that the instance is without
		 * storage allocated, it only means that there's no information in
		 * that storage.
		 */
		bool empty()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now get the status and return it
			return mForward.empty();
		}


		bool empty() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now get the status and return it
			return mForward.empty();
		}


		/*
		 * This method allows the user to clear out the map explicitly.
		 * It's handy from time to time if you're going to be re-using the
		 * same map again and again.
		 */
		void clear()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now clear out the maps
			mForward.clear();
			mReverse.clear();
		}


		/********************************************************
		 *
		 *                  Iterator Methods
		 *
		 ********************************************************/
		/*
		 * This method returns the forward iterator on the map of the
		 * keys-to-values map so that you can iterate through all the
		 * keys one at a time.
		 */
		typename std::map<K, V>::iterator keysBegin()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward.begin();
		}


		/*
		 * This method returns the end of the forward iterator on the
		 * map of the keys-to-values map so that you can stop iterating
		 * through all the keys.
		 */
		typename std::map<K, V>::iterator keysEnd()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward.end();
		}


		/*
		 * This method returns the forward iterator on the map of the
		 * values-to-keys map so that you can iterate through all the
		 * values one at a time.
		 */
		typename std::map<V, K>::iterator valuesBegin()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse.begin();
		}


		/*
		 * This method returns the end of the forward iterator on the
		 * map of the values-to-keys map so that you can stop iterating
		 * through all the values.
		 */
		typename std::map<V, K>::iterator valuesEnd()
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse.end();
		}


		/*
		 * This method returns the forward iterator on the map of the
		 * keys-to-values map so that you can iterate through all the
		 * keys one at a time.
		 */
		typename std::map<K, V>::const_iterator keysBegin() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward.begin();
		}


		/*
		 * This method returns the end of the forward iterator on the
		 * map of the keys-to-values map so that you can stop iterating
		 * through all the keys.
		 */
		typename std::map<K, V>::const_iterator keysEnd() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward.end();
		}


		/*
		 * This method returns the forward iterator on the map of the
		 * values-to-keys map so that you can iterate through all the
		 * values one at a time.
		 */
		typename std::map<V, K>::const_iterator valuesBegin() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse.begin();
		}


		/*
		 * This method returns the end of the forward iterator on the
		 * map of the values-to-keys map so that you can stop iterating
		 * through all the values.
		 */
		typename std::map<V, K>::const_iterator valuesEnd() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse.end();
		}


		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKBiMaps are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKBiMap<K, V> & anOther )
		{
			bool		equal = true;

			// lock up both maps for the comparison
			mMutex.lock();
			anOther.mMutex.lock();

			// check the maps
			if (equal) {
				if ((mForward != anOther.mForward) ||
					(mReverse != anOther.mReverse)) {
					equal = false;
				}
			}

			// unlock both maps now
			anOther.mMutex.unlock();
			mMutex.unlock();

			return equal;
		}


		bool operator==( const CKBiMap<K, V> & anOther )
		{
			return operator==((CKBiMap<K, V> &)anOther);
		}


		bool operator==( CKBiMap<K, V> & anOther ) const
		{
			return ((CKBiMap<K, V> *)this)->operator==(anOther);
		}


		bool operator==( const CKBiMap<K, V> & anOther ) const
		{
			return ((CKBiMap<K, V> *)this)->operator==((CKBiMap<K, V> &)anOther);
		}


		/*
		 * This method checks to see if the two CKBiMaps are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKBiMap<K, V> & anOther )
		{
			return !operator==(anOther);
		}


		bool operator!=( const CKBiMap<K, V> & anOther )
		{
			return !operator==((CKBiMap<K, V> &)anOther);
		}


		bool operator!=( CKBiMap<K, V> & anOther ) const
		{
			return !((CKBiMap<K, V> *)this)->operator==(anOther);
		}


		bool operator!=( const CKBiMap<K, V> & anOther ) const
		{
			return !((CKBiMap<K, V> *)this)->operator==((CKBiMap<K, V> &)anOther);
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
		 * This is the standard 'forward' map from K (keys) to V (values).
		 * It's a decent STL map, and while not as fast on the inserts and
		 * updates, the lookups are very fast.
		 */
		std::map<K, V>	mForward;
		/*
		 * This is the secondary 'reverse' map from V (values) to K (keys).
		 * It's a decent STL map, and while not as fast on the inserts and
		 * updates, the lookups are very fast.
		 */
		std::map<V, K>	mReverse;
		/*
		 * When it comes to messing with this guy, we're going to make
		 * sure that it can play well in a multi-threaded environment. To
		 * that end, we're going to cover the bases with a nice mutex.
		 */
		CKFWMutex		mMutex;
};

#endif	// __CKBIMAP_H
