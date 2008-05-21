/*
 * CKXRef.h - this file defines a template class that is a cross-reference
 *            mapping from a key (K) to a value (V) where there can be many
 *            values for one key and many keys for one value - it's a standard
 *            many-to-many cross-reference table, just fast. The core data
 *            structures are STL maps and sets so the objects you're going
 *            to put in these cross-reference tables are going to have to have
 *            the minimal operator==() and operator<() defined for them - as
 *            they would have to have to be used in the STL sorted containers.
 *
 * $Id: CKXRef.h,v 1.1 2008/05/21 19:58:29 drbob Exp $
 */
#ifndef __CKXREF_H
#define __CKXREF_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <sstream>
#include <map>
#include <set>

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
template <class K, class V> class CKXRef
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
		CKXRef() :
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
		CKXRef( CKXRef<K, V> & anOther ) :
			mForward(),
			mReverse(),
			mMutex()
		{
			// let the '=' operator do it
			*this = anOther;
		}


		CKXRef( const CKXRef<K, V> & anOther ) :
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
		virtual ~CKXRef()
		{
			// thankfully, everything cleans itself up pretty nicely
		}


		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKXRef<K, V> & operator=( const CKXRef<K, V> & anOther )
		{
			return operator=((CKXRef<K, V> &)anOther);
		}


		CKXRef<K, V> & operator=( CKXRef<K, V> & anOther )
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
		 * value equally well.
		 */
		void put( const K & aKey, const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// add the data in both directions
			mForward[aKey].insert(aValue);
			mReverse[aValue].insert(aKey);
		}


		/*
		 * This method is used to get the set of all values (V) for
		 * the supplied key (K). It returns a copy so that we can be
		 * sure that this method is thread-safe and the set you get
		 * will not change or disappear underneath you. If there's
		 * nothing for the key then this method will throw an exception -
		 * you might want to call keyExists() first to make sure the
		 * key exists in the mapping.
		 */
		std::set<V> get( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the key exists in the map
			typename std::map<K, V>::iterator	i = mForward.find(aKey);
			if (i == mForward.end()) {
				std::ostringstream	msg;
				msg << "CKXRef<K,V>::get(const K &) - the requested key: " <<
					aKey << " is not in the map. Please make sure that the key "
					"exists in the map before calling this method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// return a *copy* of the value as it exists now
			return mForward[aKey];
		}


		/*
		 * This method is used to get the set of all keys (K) for
		 * the supplied value (V). It returns a copy so that we can
		 * be sure that this method is thread-safe and the set you
		 * get will not change or disappear underneath you. If there's
		 * nothing for the value then this method will throw an exception -
		 * you might want to call valueExists() first to make sure the
		 * value exists in the mapping.
		 */
		std::set<K> getKey( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the value exists in the map
			typename std::map<V, K>::iterator	i = mReverse.find(aValue);
			if (i == mReverse.end()) {
				std::ostringstream	msg;
				msg << "CKXRef<K,V>::getKey(const V &) - the requested value: " <<
					aValue << " is not in the map. Please make sure that the key "
					"exists in the map before calling this method.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}

			// return a *copy* of the key as it exists now
			return mReverse[aValue];
		}


		/*
		 * This method will test to see if the supplied key (K) is in
		 * the cross-reference table. If it is, then you can get() it's
		 * values without throwing an exception.
		 */
		bool keyExists( const K & aKey )
		{
			bool		exists = false;

			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the key exists in the map
			typename std::map< K, std::set<V> >::iterator	i = mForward.find(aKey);
			exists = (i != mForward.end());

			return exists;
		}


		/*
		 * This method will test to see if the supplied value (V) is in
		 * the cross-reference table. If it is, then you can getKey() it's
		 * keys without throwing an exception.
		 */
		bool valueExists( const V & aValue )
		{
			bool		exists = false;

			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// check to see that the value exists in the map
			typename std::map< V, std::set<K> >::iterator	i = mReverse.find(aValue);
			exists = (i != mReverse.end());

			return exists;
		}


		/*
		 * When you need to remove all key/value pairs from the
		 * cross-reference table based on a key, then this is the method
		 * to call. It removes all key/value pairs from both directions,
		 * and if the key does not exist, then nothing is done - no
		 * exception, just nothing done.
		 */
		void erase( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// erase the elements if they exist
			typename std::map< K, std::set<V> >::iterator	i = mForward.find(aKey);
			if (i != mForward.end()) {
				// iterate over all elements in the set
				typename std::set<V>::iterator	j;
				for (j = i->second.begin(); j != i->second.end(); ++j) {
					mReverse[*j].erase(aKey);
				}
				// ...now erase the key & set from the forward map
				mForward.erase(i);
			}
		}


		/*
		 * When you need to remove all key/value pairs from the
		 * cross-reference table based on the value, then this is the
		 * method to call. It removes all key/value pairs from both
		 * directions, and if the value does not exist, then nothing
		 * is done - no exception, just nothing done.
		 */
		void eraseValue( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);

			// erase the elements if they exist
			typename std::map< V, std::set<K> >::iterator	i = mReverse.find(aValue);
			if (i != mReverse.end()) {
				// iterate over all elements in the set
				typename std::set<K>::iterator	j;
				for (j = i->second.begin(); j != i->second.end(); ++j) {
					mForward[*j].erase(aValue);
				}
				// ...now erase the pair from the reverse map
				mReverse.erase(i);
			}
		}


		/*
		 * When you have a key iterator on this map and you want to
		 * remove all the key/value pairs referenced by this iterator, you
		 * can call this method. It'll remove the key/value pairs from
		 * each direction.
		 */
		void eraseKey( typename std::map< K, std::set<V> >::iterator & anIter )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			if (anIter != mForward.end()) {
				// iterate over all elements in the set
				typename std::set<V>::iterator	j;
				for (j = anIter->second.begin(); j != anIter->second.end(); ++j) {
					mReverse[*j].erase(anIter->first);
				}
				// ...now erase the pair from the forward map
				mForward.erase(anIter);
			}
		}


		/*
		 * When you have a value iterator on this map and you want to
		 * remove all the key/value pairs referenced by this iterator, you
		 * can call this method. It'll remove the key/value pairs from
		 * each direction.
		 */
		void eraseValue( typename std::map< V, std::set<K> >::iterator & anIter )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			if (anIter != mReverse.end()) {
				// iterate over all elements in the set
				typename std::set<K>::iterator	j;
				for (j = anIter->second.begin(); j != anIter->second.end(); ++j) {
					mForward[*j].erase(anIter->first);
				}
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
		typename std::map< K, std::set<V> >::iterator keysBegin()
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
		typename std::map< K, std::set<V> >::iterator keysEnd()
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
		typename std::map< V, std::set<K> >::iterator valuesBegin()
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
		typename std::map< V, std::set<K> >::iterator valuesEnd()
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
		typename std::map< K, std::set<V> >::const_iterator keysBegin() const
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
		typename std::map< K, std::set<V> >::const_iterator keysEnd() const
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
		typename std::map< V, std::set<K> >::const_iterator valuesBegin() const
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
		typename std::map< V, std::set<K> >::const_iterator valuesEnd() const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse.end();
		}


		/*
		 * This method returns the iterator on the set of the values
		 * for a given key so that the user can run through them
		 * without having to mess with getting the set, etc.
		 */
		typename std::set<V>::iterator valuesBegin( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward[aKey].begin();
		}


		/*
		 * This method returns the end of the iterator on the set of
		 * the values for a given key so that the user can run through
		 * them without having to mess with getting the set, etc.
		 */
		typename std::set<V>::iterator valuesEnd( const K & aKey )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward[aKey].end();
		}


		/*
		 * This method returns the iterator on the set of the keys
		 * for a given value so that the user can run through them
		 * without having to mess with getting the set, etc.
		 */
		typename std::set<K>::iterator keysBegin( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse[aValue].begin();
		}


		/*
		 * This method returns the end of the iterator on the set of
		 * the keys for a given value so that the user can run through
		 * them without having to mess with getting the set, etc.
		 */
		typename std::set<K>::iterator keysEnd( const V & aValue )
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse[aValue].end();
		}


		/*
		 * This method returns the iterator on the set of the values
		 * for a given key so that the user can run through them
		 * without having to mess with getting the set, etc.
		 */
		typename std::set<V>::const_iterator valuesBegin( const K & aKey ) const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward[aKey].begin();
		}


		/*
		 * This method returns the end of the iterator on the set of
		 * the values for a given key so that the user can run through
		 * them without having to mess with getting the set, etc.
		 */
		typename std::set<V>::const_iterator valuesEnd( const K & aKey ) const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mForward[aKey].end();
		}


		/*
		 * This method returns the iterator on the set of the keys
		 * for a given value so that the user can run through them
		 * without having to mess with getting the set, etc.
		 */
		typename std::set<K>::const_iterator keysBegin( const V & aValue ) const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse[aValue].begin();
		}


		/*
		 * This method returns the end of the iterator on the set of
		 * the keys for a given value so that the user can run through
		 * them without having to mess with getting the set, etc.
		 */
		typename std::set<K>::const_iterator keysEnd( const V & aValue ) const
		{
			// first, lock up this guy against changes
			CKStackLocker	lockem(&mMutex);
			// ...now generate the iterator and return it
			return mReverse[aValue].end();
		}


		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKXRefs are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKXRef<K, V> & anOther )
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


		bool operator==( const CKXRef<K, V> & anOther )
		{
			return operator==((CKXRef<K, V> &)anOther);
		}


		bool operator==( CKXRef<K, V> & anOther ) const
		{
			return ((CKXRef<K, V> *)this)->operator==(anOther);
		}


		bool operator==( const CKXRef<K, V> & anOther ) const
		{
			return ((CKXRef<K, V> *)this)->operator==((CKXRef<K, V> &)anOther);
		}


		/*
		 * This method checks to see if the two CKXRefs are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKXRef<K, V> & anOther )
		{
			return !operator==(anOther);
		}


		bool operator!=( const CKXRef<K, V> & anOther )
		{
			return !operator==((CKXRef<K, V> &)anOther);
		}


		bool operator!=( CKXRef<K, V> & anOther ) const
		{
			return !((CKXRef<K, V> *)this)->operator==(anOther);
		}


		bool operator!=( const CKXRef<K, V> & anOther ) const
		{
			return !((CKXRef<K, V> *)this)->operator==((CKXRef<K, V> &)anOther);
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
		 * This is the standard 'forward' map from K (keys) to a set of
		 * V (values). It's a decent STL map, and while not as fast on the
		 * inserts and updates, the lookups are very fast. The set is nice
		 * and ordered, so it's easy to get it to do what we need.
		 */
		std::map< K, std::set<V> >	mForward;
		/*
		 * This is the secondary 'reverse' map from V (values) to a set of
		 * K (keys). It's a decent STL map, and while not as fast on the
		 * inserts and updates, the lookups are very fast. The set is nice
		 * and ordered, so it's easy to get it to do what we need.
		 */
		std::map< V, std::set<K> >	mReverse;
		/*
		 * When it comes to messing with this guy, we're going to make
		 * sure that it can play well in a multi-threaded environment. To
		 * that end, we're going to cover the bases with a nice mutex.
		 */
		CKFWMutex		mMutex;
};

#endif	// __CKXREF_H
