/*
 * CKString.h - this file defines a class that can be used to represent a
 *              general string. This is an alternative to the STL
 *              std::string because there are several problems with STL and
 *              GCC, and rather than fight those problems, it's easier to
 *              make an object with the subset of features that we really
 *              need and leave out the problems that STL brings.
 *
 * $Id: CKString.h,v 1.1 2004/09/14 15:39:52 drbob Exp $
 */
#ifndef __CKSTRING_H
#define __CKSTRING_H

//	System Headers
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif
#include <string>

//	Third-Party Headers

//	Other Headers

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants
/*
 * This is the default starting size of this string as it's going to
 * simply be an allocated array of chars. When an operation adds more
 * that this to the size of the string, we'll have to allocate more and
 * then this won't be the size. But for starters, this is a good size.
 */
#define	DEFAULT_STARTING_SIZE		32
/*
 * When the buffer has to grow for this string, this is the default
 * increment that it uses. Of course, this is *above* the necessary size
 * so if you're adding 2000 characters, we'll round it up to a factor of
 * the DEFAULT_INCREMENT_SIZE to make sure that we have at least a little
 * growing room.
 */
#define	DEFAULT_INCREMENT_SIZE		64


/*
 * This is the main class definition.
 */
class CKString
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that creates a nice, empty
		 * string ready for the user to dump data into as needed.
		 */
		CKString();
		/*
		 * This form of the constructor takes an initial buffer capacity
		 * and this overrides the default for this class. This is useful
		 * when you know the size of the string that will be stored and
		 * you don't want to have the overhead of allocating space as
		 * the string grows. You can also specify the default increment
		 * capacity for this class, if you need to, but it defaults to the
		 * default for the class.
		 */
		CKString( int anInitialCapacity, int aCapacityIncrement = DEFAULT_INCREMENT_SIZE );
		/*
		 * These forms of the constructor take a simple NULL-terminated
		 * string and create a new CKString from this. The arguments are
		 * left unchanged, and it remains the caller's responsibility to
		 * manage the memory of these arrays.
		 */
		CKString( char *aString );
		CKString( const char *aString );
		/*
		 * These forms of the constructor take an STL std::string and
		 * create a new CKString from this. This is a convenience/bridge
		 * method to continued use of STL while working this class into
		 * the codebase.
		 */
		CKString( std::string & aString );
		CKString( const std::string & aString );
		/*
		 * These forms of the constructor take a simple NULL-terminated
		 * string and a starting and (optional) length parameter. The
		 * idea is that these forms will create a new CKString from the
		 * substring of the argument starting at the index 'aStartIndex'
		 * which is zero-biased, and including 'aLength' of characters.
		 * If 'aLength' is omitted the default is to include to the end
		 * of the source string.
		 */
		CKString( char *aString, int aStartIndex, int aLength = -1 );
		CKString( const char *aString, int aStartIndex, int aLength = -1 );
		/*
		 * These forms of the constructor take another CKString
		 * and a starting and (optional) length parameter. The
		 * idea is that these forms will create a new CKString from the
		 * substring of the argument starting at the index 'aStartIndex'
		 * which is zero-biased, and including 'aLength' of characters.
		 * If 'aLength' is omitted the default is to include to the end
		 * of the source string.
		 */
		CKString( CKString & aString, int aStartIndex, int aLength = -1 );
		CKString( const CKString & aString, int aStartIndex, int aLength = -1 );
		/*
		 * These forms of the constructor take a STL std::string
		 * and a starting and (optional) length parameter. The
		 * idea is that these forms will create a new CKString from the
		 * substring of the argument starting at the index 'aStartIndex'
		 * which is zero-biased, and including 'aLength' of characters.
		 * If 'aLength' is omitted the default is to include to the end
		 * of the source string.
		 */
		CKString( std::string & aString, int aStartIndex, int aLength = -1 );
		CKString( const std::string & aString, int aStartIndex, int aLength = -1 );
		/*
		 * This form of the constructor takes a single character and a
		 * repeat count and creates a new CKString of that many of that
		 * single character. Nice if you have to create a dividing line
		 * and need 50 '-' characters all in a row.
		 */
		CKString( char aChar, int aRepeatCount = 1 );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKString( CKString & anOther );
		CKString( const CKString & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKString();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKString & operator=( CKString & anOther );
		CKString & operator=( const CKString & anOther );
		/*
		 * These forms of the '=' operator are for easy assignment to the
		 * CKString. They try to cover what you're likely to be doing with
		 * the string, but more can always be added if they are insufficient.
		 */
		CKString & operator=( std::string & anSTLString );
		CKString & operator=( const std::string & anSTLString );
		CKString & operator=( char *aCString );
		CKString & operator=( const char *aCString );
		CKString & operator=( char aChar );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * These methods add the different kinds of strings to the
		 * end of the existing string and return 'true' if successful,
		 * or 'false' if not. There are quite a few to be as generally
		 * useful as possible.
		 */
		bool append( CKString & aString );
		bool append( const CKString & aString );
		bool append( char *aCString );
		bool append( const char *aCString );
		bool append( std::string & aSTLString );
		bool append( const std::string & aSTLString );
		/*
		 * These methods add the string representation of the different
		 * kinds of base data objects to the end of the existing string
		 * and return 'true' if successful, or 'false' if not. There are
		 * quite a few to be as generally useful as possible.
		 */
		bool append( char aChar );
		bool append( int anInteger, int aNumOfDigits = -1 );
		bool append( long aLong, int aNumOfDigits = -1 );
		bool append( double aDouble );

		/*
		 * These methods add the different kinds of strings to the
		 * beginning of the existing string and return 'true' if successful,
		 * or 'false' if not. There are quite a few to be as generally
		 * useful as possible.
		 */
		bool prepend( CKString & aString );
		bool prepend( const CKString & aString );
		bool prepend( char *aCString );
		bool prepend( const char *aCString );
		bool prepend( std::string & aSTLString );
		bool prepend( const std::string & aSTLString );
		/*
		 * These methods add the string representation of the different
		 * kinds of base data objects to the beginning of the existing string
		 * and return 'true' if successful, or 'false' if not. There are
		 * quite a few to be as generally useful as possible.
		 */
		bool prepend( char aChar );
		bool prepend( int anInteger, int aNumOfDigits = -1 );
		bool prepend( long aLong, int aNumOfDigits = -1 );
		bool prepend( double aDouble );

		/*
		 * This method returns the current size of the string in characters.
		 * This is the same as the call to length(), but it matches some of
		 * the STL-isms a bit and is therefore really a convenience method.
		 */
		int size();
		int size() const;

		/*
		 * This method returns the number of characters in the string and
		 * is the same as the call to size(). It's simply here as another
		 * way to get the same data.
		 */
		int length();
		int length() const;

		/*
		 * This method returns true if the representation of the string
		 * is really empty. This may not mean that the instance is without
		 * storage allocated, it only means that there's no information in
		 * that storage.
		 */
		bool empty();
		bool empty() const;

		/*
		 * This method returns the total currently allocated capacity of
		 * this string. This is always going to be greater than the size
		 * of the string as there is meant to be a little breathing room
		 * at the end of the string.
		 */
		int capacity();
		int capacity() const;

		/*
		 * This method can be used to clear up *most* of the excess free
		 * space that this CKString is using. It won't get any smaller
		 * than the initial size, but it's possible to free up a lot of
		 * space if this guy got very big and now is quite small.
		 */
		bool compact();
		bool compact() const;

		/*
		 * This method returns the actual pointer to the C-String that
		 * represents the contents of this CKString. Since this is the
		 * real McCoy, if you wish to keep it around for something, please
		 * make a copy of it right away as there is no guarantees as to
		 * what might happen to it if the string grows or shrinks.
		 */
		const char *c_str();
		const char *c_str() const;

		/*
		 * This method returns an STL std::string that contains a copy of
		 * the contents of this CKString. Since it's a copy, the caller is
		 * free to use it as he sees fit.
		 */
		std::string stl_str();
		std::string stl_str() const;

		/*
		 * This method is a simple indexing operator so that we can easily
		 * get the individual characters in the CKString. If the argument
		 * is -1, then the default is to get the *LAST* non-NULL
		 * character in the string. This is nice in that we're often
		 * looking at the last character and getting the length for no
		 * other reason.
		 */
		char operator[]( int aPosition );
		char operator[]( int aPosition ) const;

		/********************************************************
		 *
		 *                Manipulation Methods
		 *
		 ********************************************************/
		/*
		 * These operators add the different kinds of strings to the
		 * beginning of the existing string and return 'true' if successful,
		 * or 'false' if not. There are quite a few to be as generally
		 * useful as possible.
		 */
		CKString & operator+=( CKString & aString );
		CKString & operator+=( const CKString & aString );
		CKString & operator+=( char *aCString );
		CKString & operator+=( const char *aCString );
		CKString & operator+=( std::string & aSTLString );
		CKString & operator+=( const std::string & aSTLString );
		/*
		 * These operators add the string representation of the different
		 * kinds of base data objects to the beginning of the existing string
		 * and return 'true' if successful, or 'false' if not. There are
		 * quite a few to be as generally useful as possible.
		 */
		CKString & operator+=( int anInteger );
		CKString & operator+=( long aLong );
		CKString & operator+=( double aDouble );
		CKString & operator+=( char aChar );

		/*
		 * These operators are the general-purpose concatenation operators
		 * that you'd expect to have in a nice string class. With these
		 * you can pretty much build up a CKString in just about any way
		 * you can imagine.
		 */
		friend CKString operator+( CKString & aString, CKString & anOther );
		friend CKString operator+( const CKString & aString, CKString & anOther );
		friend CKString operator+( CKString & aString, const CKString & anOther );
		friend CKString operator+( const CKString & aString, const CKString & anOther );

		friend CKString operator+( CKString & aString, char *aCString );
		friend CKString operator+( CKString & aString, const char *aCString );
		friend CKString operator+( const CKString & aString, char *aCString );
		friend CKString operator+( const CKString & aString, const char *aCString );

		friend CKString operator+( char *aCString, CKString & aString );
		friend CKString operator+( char *aCString, const CKString & aString );
		friend CKString operator+( const char *aCString, CKString & aString );
		friend CKString operator+( const char *aCString, const CKString & aString );

		friend CKString operator+( CKString & aString, std::string & anSTLString );
		friend CKString operator+( CKString & aString, const std::string & anSTLString );
		friend CKString operator+( const CKString & aString, std::string & anSTLString );
		friend CKString operator+( const CKString & aString, const std::string & anSTLString );

		friend CKString operator+( std::string & anSTLString, CKString & aString );
		friend CKString operator+( std::string & anSTLString, const CKString & aString );
		friend CKString operator+( const std::string & anSTLString, CKString & aString );
		friend CKString operator+( const std::string & anSTLString, const CKString & aString );

		friend CKString operator+( CKString & aString, int anInteger );
		friend CKString operator+( const CKString & aString, int anInteger );
		friend CKString operator+( int anInteger, CKString & aString );
		friend CKString operator+( int anInteger, const CKString & aString );

		friend CKString operator+( CKString & aString, long aLong );
		friend CKString operator+( const CKString & aString, long aLong );
		friend CKString operator+( long aLong, CKString & aString );
		friend CKString operator+( long aLong, const CKString & aString );

		friend CKString operator+( CKString & aString, double aDouble );
		friend CKString operator+( const CKString & aString, double aDouble );
		friend CKString operator+( double aDouble, CKString & aString );
		friend CKString operator+( double aDouble, const CKString & aString );

		friend CKString operator+( CKString & aString, char aChar );
		friend CKString operator+( const CKString & aString, char aChar );
		friend CKString operator+( char aChar, CKString & aString );
		friend CKString operator+( char aChar, const CKString & aString );

		/********************************************************
		 *
		 *                Text Handling Methods
		 *
		 ********************************************************/
		/*
		 * This method goes through all the characters in the string
		 * and makes sure that they are all uppercase. It's really
		 * pretty simple, but it's awfully handy not to have to implement
		 * this in all the projects.
		 */
		bool toUpper();
		bool toUpper() const;

		/*
		 * This method goes through all the characters in the string
		 * and makes sure that they are all lowercase. It's really
		 * pretty simple, but it's awfully handy not to have to implement
		 * this in all the projects.
		 */
		bool toLower();
		bool toLower() const;

		/*
		 * This method returns a new CKString based on the substring
		 * of the current string defined to start at 'aStartingPos'
		 * and including 'aLength' number of characters (all positions
		 * in the string are zero-biased). If this substring does not
		 * exist in the string due to position or length, then an
		 * excepton will be thrown. If the length is left off, then
		 * the "end of the string" is the default behavior.
		 */
		CKString substr( int aStartingPos = 0, int aLength = -1 );
		CKString substr( int aStartingPos = 0, int aLength = -1 ) const;

		/*
		 * Often times, you want to get the first part of a string up
		 * to, and including, a certain character or string. This method
		 * does just that. If the character or string in question is not
		 * in the current string, then an empty string will be returned.
		 */
		CKString substrTo( char aChar );
		CKString substrTo( char aChar ) const;

		CKString substrTo( char *aCString );
		CKString substrTo( char *aCString ) const;
		CKString substrTo( const char *aCString );
		CKString substrTo( const char *aCString ) const;

		CKString substrTo( std::string & anSTLString );
		CKString substrTo( std::string & anSTLString ) const;
		CKString substrTo( const std::string & anSTLString );
		CKString substrTo( const std::string & anSTLString ) const;

		CKString substrTo( CKString & aString );
		CKString substrTo( CKString & aString ) const;
		CKString substrTo( const CKString & aString );
		CKString substrTo( const CKString & aString ) const;

		/*
		 * Often times, you want to get the last part of a string starting
		 * with, and including, a certain character or string. This method
		 * does just that. If the character or string in question is not
		 * in the current string, then an empty string will be returned.
		 */
		CKString substrFrom( char aChar );
		CKString substrFrom( char aChar ) const;

		CKString substrFrom( char *aCString );
		CKString substrFrom( char *aCString ) const;
		CKString substrFrom( const char *aCString );
		CKString substrFrom( const char *aCString ) const;

		CKString substrFrom( std::string & anSTLString );
		CKString substrFrom( std::string & anSTLString ) const;
		CKString substrFrom( const std::string & anSTLString );
		CKString substrFrom( const std::string & anSTLString ) const;

		CKString substrFrom( CKString & aString );
		CKString substrFrom( CKString & aString ) const;
		CKString substrFrom( const CKString & aString );
		CKString substrFrom( const CKString & aString ) const;

		/*
		 * Often times, you want to get the first part of a string up
		 * to, but NOT including, a certain character or string. This
		 * method does just that. If the character or string in question
		 * is not in the current string, then an empty string will be
		 * returned.
		 */
		CKString substrBefore( char aChar );
		CKString substrBefore( char aChar ) const;

		CKString substrBefore( char *aCString );
		CKString substrBefore( char *aCString ) const;
		CKString substrBefore( const char *aCString );
		CKString substrBefore( const char *aCString ) const;

		CKString substrBefore( std::string & anSTLString );
		CKString substrBefore( std::string & anSTLString ) const;
		CKString substrBefore( const std::string & anSTLString );
		CKString substrBefore( const std::string & anSTLString ) const;

		CKString substrBefore( CKString & aString );
		CKString substrBefore( CKString & aString ) const;
		CKString substrBefore( const CKString & aString );
		CKString substrBefore( const CKString & aString ) const;

		/*
		 * Often times, you want to get the last part of a string starting
		 * with, but NOT including, a certain character or string. This
		 * method does just that. If the character or string in question
		 * is not in the current string, then an empty string will be
		 * returned.
		 */
		CKString substrAfter( char aChar );
		CKString substrAfter( char aChar ) const;

		CKString substrAfter( char *aCString );
		CKString substrAfter( char *aCString ) const;
		CKString substrAfter( const char *aCString );
		CKString substrAfter( const char *aCString ) const;

		CKString substrAfter( std::string & anSTLString );
		CKString substrAfter( std::string & anSTLString ) const;
		CKString substrAfter( const std::string & anSTLString );
		CKString substrAfter( const std::string & anSTLString ) const;

		CKString substrAfter( CKString & aString );
		CKString substrAfter( CKString & aString ) const;
		CKString substrAfter( const CKString & aString );
		CKString substrAfter( const CKString & aString ) const;

		/*
		 * Straight out of BASIC, this method returns the 'n' left-most
		 * characters in this string as a new CKString. If 'n' characters
		 * do not exist in this string, an exception will be thrown, so
		 * please check to see what you're asking for.
		 */
		CKString left( int aNumOfChars );
		CKString left( int aNumOfChars ) const;

		/*
		 * Straight out of BASIC, this method returns the 'n' right-most
		 * characters in this string as a new CKString. If 'n' characters
		 * do not exist in this string, an exception will be thrown, so
		 * please check to see what you're asking for.
		 */
		CKString right( int aNumOfChars );
		CKString right( int aNumOfChars ) const;

		/*
		 * Straight out of BASIC, this method returns a new substring
		 * starting at 'aStartPos' through (including) 'anEndPos' -
		 * where both are zero-biased index values. This is a simple
		 * way to get a substring that fits the old BASIC calling
		 * convention.
		 */
		CKString mid( int aStartPos, int anEndPos );
		CKString mid( int aStartPos, int anEndPos ) const;

		/*
		 * This method is a 'global search and replace' on a character
		 * level for this string. It will go through the entire string
		 * searching for all occurrences of 'anOld' character and replace
		 * each with 'aNew' character. The number of times this happens
		 * is the return value for the method.
		 */
		int replace( char anOld, char aNew );
		int replace( char anOld, char aNew ) const;

		/*
		 * These methods all search for the FIRST occurrence of the
		 * argument in the current string and return the starting
		 * position of the match if one is found, or a -1 if there
		 * is nothing in the current string that matches the  argument.
		 */
		int find( char aChar );
		int find( char aChar ) const;

		int find( char *aCString );
		int find( char *aCString ) const;
		int find( const char *aCString );
		int find( const char *aCString ) const;

		int find( CKString & aString );
		int find( const CKString & aString );
		int find( CKString & aString ) const;
		int find( const CKString & aString ) const;

		int find( std::string & anSTLString );
		int find( const std::string & anSTLString );
		int find( std::string & anSTLString ) const;
		int find( const std::string & anSTLString ) const;

		/*
		 * These methods all search for the LAST occurrence of the
		 * argument in the current string and return the starting
		 * position of the match if one is found, or a -1 if there
		 * is nothing in the current string that matches the  argument.
		 */
		int findLast( char aChar );
		int findLast( char aChar ) const;

		int findLast( char *aCString );
		int findLast( char *aCString ) const;
		int findLast( const char *aCString );
		int findLast( const char *aCString ) const;

		int findLast( CKString & aString );
		int findLast( const CKString & aString );
		int findLast( CKString & aString ) const;
		int findLast( const CKString & aString ) const;

		int findLast( std::string & anSTLString );
		int findLast( const std::string & anSTLString );
		int findLast( std::string & anSTLString ) const;
		int findLast( const std::string & anSTLString ) const;

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKStrings are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( CKString & anOther );
		bool operator==( const CKString & anOther );
		bool operator==( CKString & anOther ) const;
		bool operator==( const CKString & anOther ) const;
		/*
		 * These operators check to see if the CKString is equal to a simple
		 * NULL-terminated C-string. This is nice in that we don't have to
		 * hassle with converting all string constants to CKStrings and then
		 * do the comparison.
		 */
		bool operator==( char *aCString );
		bool operator==( const char *aCString );
		bool operator==( char *aCString ) const;
		bool operator==( const char *aCString ) const;
		/*
		 * These operators check to see if the CKString is equal to an STL
		 * string. This is nice in that we don't have to hassle with converting
		 * all STL std::string to CKStrings and then do the comparison.
		 */
		bool operator==( std::string & anSTLString );
		bool operator==( const std::string & anSTLString );
		bool operator==( std::string & anSTLString ) const;
		bool operator==( const std::string & anSTLString ) const;
		/*
		 * This method checks to see if the two CKStrings are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKString & anOther );
		bool operator!=( const CKString & anOther );
		bool operator!=( CKString & anOther ) const;
		bool operator!=( const CKString & anOther ) const;
		/*
		 * These operators check to see if the CKString is not equal to a
		 * simple NULL-terminated C-string. This is nice in that we don't have
		 * to hassle with converting all string constants to CKStrings and
		 * then do the comparison.
		 */
		bool operator!=( char * aCString );
		bool operator!=( const char * aCString );
		bool operator!=( char * aCString ) const;
		bool operator!=( const char * aCString ) const;
		/*
		 * These operators check to see if the CKString is not equal to an STL
		 * string. This is nice in that we don't have to hassle with converting
		 * all STL std::string to CKStrings and then do the comparison.
		 */
		bool operator!=( std::string & anSTLString );
		bool operator!=( const std::string & anSTLString );
		bool operator!=( std::string & anSTLString ) const;
		bool operator!=( const std::string & anSTLString ) const;

		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this string. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual std::string toString() const;

	protected:
		/*
		 * This method is used in the constructors that take a substring of
		 * a C-String, because the code would be repeated far too often and
		 * lead to maintenance problems if we didn't encapsulate it here.
		 */
		void initWithSubString( char *aCString, int aStartIndex, int aLength = -1 );

		/*
		 * When the string needs to resize itself, this method is the best
		 * way to do it. If the passed-in size is greater than the current
		 * cappcity, the existing string will be copied over and the
		 * excess will be filled with NULLs. If the size is smaller than
		 * the current capacity, then the excess will be truncated, but
		 * we'll always maintain a NULL-terminated string as that's a core
		 * tenet of the data structure.
		 */
		bool resize( int aSize );

	private:
		/*
		 * This is the character array that is the core of the storage of
		 * the CKString. It's simple, but it's very effective as I can then
		 * leverage all the other string processing in C, but at the same
		 * time have a flexible storage system.
		 */
		char			*mString;
		/*
		 * This is the current size of the string and should always be
		 * kept up to date with respect to the buffer above. It's possible
		 * that this can get out of sync, which is why we make sure that
		 * the buffer is NULL-terminated in order to re-create this with
		 * a call to strlen().
		 */
		int				mSize;
		/*
		 * This is a very important value in that it's the current maximum
		 * capacity of the string and that's it. If the operation that's
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
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKString & aString );
std::ostream & operator<<( std::ostream & aStream, const CKString & aString );
/*
 * Sometimes it's useful to read an input stream into a CKString. This
 * operator<<() does just that.
 */
std::ostream & operator<<( CKString & aString, std::ostream & aStream );
std::ostream & operator<<( const CKString & aString, std::ostream & aStream );

#endif	// __CKSTRING_H
