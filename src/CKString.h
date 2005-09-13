/*
 * CKString.h - this file defines a class that can be used to represent a
 *              general string. This is an alternative to the STL
 *              std::string because there are several problems with STL and
 *              GCC, and rather than fight those problems, it's easier to
 *              make an object with the subset of features that we really
 *              need and leave out the problems that STL brings.
 *
 * $Id: CKString.h,v 1.14 2005/09/13 15:46:59 drbob Exp $
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
/*
 * Because we're using the NAN value in some places in this object,
 * we need to make sure that it's defined for all the platforms that
 * will be using this object.
 */
#ifdef __linux__
#define __USE_ISOC99 1
#endif
#include <math.h>
/*
 * Oddly enough, Sun doesn't seem to have NAN defined, so we need to
 * do that here so that things run more smoothly. This is very interesting
 * because Sun has isnan() defined, but no obvious way to set a value.
 */
#ifdef __sun__
#ifndef NAN
#define	NAN	(__extension__ ((union { unsigned __l __attribute__((__mode__(__SI__))); \
			float __d; }) { __l: 0x7fc00000UL }).__d)
#endif
#endif
/*
 * This is most odd, but it seems that at least on Darwin (Mac OS X)
 * there's a problem with the definition of isnan(). So... to make it
 * easier on all parties, I'm simply going to repeat the definition
 * that's in Linux and Darwin here, and it should get picked up even
 * if the headers fail us.
 */
#ifdef __MACH__
#ifndef isnan
#define	isnan(x)	((sizeof(x) == sizeof(double)) ? __isnand(x) : \
					(sizeof(x) == sizeof(float)) ? __isnanf(x) : __isnan(x))
#endif
#endif

//	Third-Party Headers
#include <CKFWMutex.h>

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
#define	DEFAULT_STARTING_SIZE		4
/*
 * When the buffer has to grow for this string, this is the default
 * increment that it uses. Of course, this is *above* the necessary size
 * so if you're adding 2000 characters, we'll round it up to a factor of
 * the DEFAULT_INCREMENT_SIZE to make sure that we have at least a little
 * growing room.
 */
#define	DEFAULT_INCREMENT_SIZE		8


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
		CKString & append( CKString & aString );
		CKString & append( const CKString & aString );
		CKString & append( char *aCString, int aLength = -1 );
		CKString & append( const char *aCString, int aLength = -1 );
		CKString & append( std::string & aSTLString );
		CKString & append( const std::string & aSTLString );
		/*
		 * These methods add the string representation of the different
		 * kinds of base data objects to the end of the existing string
		 * and return 'true' if successful, or 'false' if not. There are
		 * quite a few to be as generally useful as possible.
		 */
		CKString & append( char aChar );
		CKString & append( int anInteger, int aNumOfDigits = -1 );
		CKString & append( long aLong, int aNumOfDigits = -1 );
		CKString & append( double aDouble, int aNumDecPlaces = -1 );
		CKString & append( void *anAddress );

		/*
		 * These methods add the different kinds of strings to the
		 * beginning of the existing string and return 'true' if successful,
		 * or 'false' if not. There are quite a few to be as generally
		 * useful as possible.
		 */
		CKString & prepend( CKString & aString );
		CKString & prepend( const CKString & aString );
		CKString & prepend( char *aCString, int aLength = -1 );
		CKString & prepend( const char *aCString, int aLength = -1 );
		CKString & prepend( std::string & aSTLString );
		CKString & prepend( const std::string & aSTLString );
		/*
		 * These methods add the string representation of the different
		 * kinds of base data objects to the beginning of the existing string
		 * and return 'true' if successful, or 'false' if not. There are
		 * quite a few to be as generally useful as possible.
		 */
		CKString & prepend( char aChar );
		CKString & prepend( int anInteger, int aNumOfDigits = -1 );
		CKString & prepend( long aLong, int aNumOfDigits = -1 );
		CKString & prepend( double aDouble, int aNumDecPlaces = -1 );
		CKString & prepend( void *anAddress );

		/*
		 * When you want to fill an existing string with a repeated value,
		 * this is a good way to do it. It replaces what's in the string
		 * and in it's place puts the requested number of copies of the
		 * given character.
		 */
		CKString & fill( char aChar, int aCount );
		CKString & fill( char aChar, int aCount ) const;

		/*
		 * When you need to remove (erase) a section of the string, this
		 * method is the one to call. You give it a starting index and
		 * optionally a number of characters to delete and they will be
		 * removed from the string. If the length is not supplied, then
		 * the characters from 'aStartingIndex' to the end of the string
		 * will be removed.
		 */
		bool erase( int aStartingIndex,  int aLength = -1 );
		bool erase( int aStartingIndex,  int aLength = -1 ) const;

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

		/*
		 * This method allows the user to clear out the buffer explicitly.
		 * It's handy from time to time if you're going to be re-using the
		 * same string again and again.
		 */
		bool clear();
		bool clear() const;

		/*
		 * There are times that you might want to see the numeric
		 * representation of the contents of this string. These methods
		 * make that easy enough to do and take care of all the dirty
		 * work for us.
		 */
		int intValue();
		int intValue() const;

		int hexIntValue();
		int hexIntValue() const;

		long longValue();
		long longValue() const;

		double doubleValue();
		double doubleValue() const;

		/********************************************************
		 *
		 *                Manipulation Methods
		 *
		 ********************************************************/
		/*
		 * There will be times when the data in a string may not exactly
		 * look like a string, but you need to make an *exact* copy
		 * anyway. This method will take the capacity, size, and data
		 * from the argument and duplicate them so that this guy is a
		 * clone of the original.
		 */
		CKString & clone( CKString & anOther );
		CKString & clone( const CKString & anOther );
		CKString & clone( CKString & anOther ) const;
		CKString & clone( const CKString & anOther ) const;

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
		CKString & toUpper();
		CKString & toUpper() const;

		/*
		 * This method goes through all the characters in the string
		 * and makes sure that they are all lowercase. It's really
		 * pretty simple, but it's awfully handy not to have to implement
		 * this in all the projects.
		 */
		CKString & toLower();
		CKString & toLower() const;

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
		 * argument in the current string *ON OR AFTER* the supplied
		 * index and return the starting position of the match if one
		 * is found, or a -1 if there is nothing in the current string
		 * that matches the  argument.
		 */
		int find( char aChar, int aStartingIndex = 0 );
		int find( char aChar, int aStartingIndex = 0 ) const;

		int find( char *aCString, int aStartingIndex = 0 );
		int find( char *aCString, int aStartingIndex = 0 ) const;
		int find( const char *aCString, int aStartingIndex = 0 );
		int find( const char *aCString, int aStartingIndex = 0 ) const;

		int find( CKString & aString, int aStartingIndex = 0 );
		int find( const CKString & aString, int aStartingIndex = 0 );
		int find( CKString & aString, int aStartingIndex = 0 ) const;
		int find( const CKString & aString, int aStartingIndex = 0 ) const;

		int find( std::string & anSTLString, int aStartingIndex = 0 );
		int find( const std::string & anSTLString, int aStartingIndex = 0 );
		int find( std::string & anSTLString, int aStartingIndex = 0 ) const;
		int find( const std::string & anSTLString, int aStartingIndex = 0 ) const;

		/*
		 * These methods all search for the LAST occurrence of the
		 * argument in the current string and return the starting
		 * position of the match if one is found, or a -1 if there
		 * is nothing in the current string that matches the  argument.
		 */
		int findLast( char aChar, int aStartingIndex = -1 );
		int findLast( char aChar, int aStartingIndex = -1 ) const;

		int findLast( char *aCString, int aStartingIndex = -1 );
		int findLast( char *aCString, int aStartingIndex = -1 ) const;
		int findLast( const char *aCString, int aStartingIndex = -1 );
		int findLast( const char *aCString, int aStartingIndex = -1 ) const;

		int findLast( CKString & aString, int aStartingIndex = -1 );
		int findLast( const CKString & aString, int aStartingIndex = -1 );
		int findLast( CKString & aString, int aStartingIndex = -1 ) const;
		int findLast( const CKString & aString, int aStartingIndex = -1 ) const;

		int findLast( std::string & anSTLString, int aStartingIndex = -1 );
		int findLast( const std::string & anSTLString, int aStartingIndex = -1 );
		int findLast( std::string & anSTLString, int aStartingIndex = -1 ) const;
		int findLast( const std::string & anSTLString, int aStartingIndex = -1 ) const;

		/*
		 * This method trims all the whitespace off the right-hand end of
		 * the string so that the last character in the string is something
		 * that visibly printable.
		 */
		CKString & trimRight();
		CKString & trimRight() const;

		/*
		 * This method trims all the whitespace off the left-hand end of
		 * the string so that the first character in the string is something
		 * that visibly printable.
		 */
		CKString & trimLeft();
		CKString & trimLeft() const;

		/*
		 * This method trims all the whitespace off the both ends of the
		 * string so that string is freed of all the "junk".
		 */
		CKString & trim();
		CKString & trim() const;

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
		 * This method checks to see if the argument CKString is greather
		 * than or equal to this string (thus this one being less than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<( CKString & anOther );
		bool operator<( const CKString & anOther );
		bool operator<( CKString & anOther ) const;
		bool operator<( const CKString & anOther ) const;
		/*
		 * This method checks to see if the argument string is greather
		 * than or equal to this string (thus this one being less than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<( char *aCString );
		bool operator<( const char *aCString );
		bool operator<( char *aCString ) const;
		bool operator<( const char *aCString ) const;
		/*
		 * This method checks to see if the argument STL string is greather
		 * than or equal to this string (thus this one being less than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<( std::string & anSTLString );
		bool operator<( const std::string & anSTLString );
		bool operator<( std::string & anSTLString ) const;
		bool operator<( const std::string & anSTLString ) const;

		/*
		 * This method checks to see if the argument CKString is greather
		 * than this string (thus this one being less than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<=( CKString & anOther );
		bool operator<=( const CKString & anOther );
		bool operator<=( CKString & anOther ) const;
		bool operator<=( const CKString & anOther ) const;
		/*
		 * This method checks to see if the argument string is greather
		 * than this string (thus this one being less than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<=( char *aCString );
		bool operator<=( const char *aCString );
		bool operator<=( char *aCString ) const;
		bool operator<=( const char *aCString ) const;
		/*
		 * This method checks to see if the argument STL string is greather
		 * than this string (thus this one being less than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator<=( std::string & anSTLString );
		bool operator<=( const std::string & anSTLString );
		bool operator<=( std::string & anSTLString ) const;
		bool operator<=( const std::string & anSTLString ) const;

		/*
		 * This method checks to see if the argument CKString is less
		 * than or equal to this string (thus this one being greater than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>( CKString & anOther );
		bool operator>( const CKString & anOther );
		bool operator>( CKString & anOther ) const;
		bool operator>( const CKString & anOther ) const;
		/*
		 * This method checks to see if the argument string is less
		 * than or equal to this string (thus this one being greater than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>( char *aCString );
		bool operator>( const char *aCString );
		bool operator>( char *aCString ) const;
		bool operator>( const char *aCString ) const;
		/*
		 * This method checks to see if the argument STL string is less
		 * than or equal to this string (thus this one being greater than),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>( std::string & anSTLString );
		bool operator>( const std::string & anSTLString );
		bool operator>( std::string & anSTLString ) const;
		bool operator>( const std::string & anSTLString ) const;

		/*
		 * This method checks to see if the argument CKString is less
		 * than this string (thus this one being greater than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>=( CKString & anOther );
		bool operator>=( const CKString & anOther );
		bool operator>=( CKString & anOther ) const;
		bool operator>=( const CKString & anOther ) const;
		/*
		 * This method checks to see if the argument string is less
		 * than this string (thus this one being greater than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>=( char *aCString );
		bool operator>=( const char *aCString );
		bool operator>=( char *aCString ) const;
		bool operator>=( const char *aCString ) const;
		/*
		 * This method checks to see if the argument STL string is less
		 * than this string (thus this one being greater than or equal),
		 * based on the values they represent and *not* on the actual
		 * pointers themselves.
		 */
		bool operator>=( std::string & anSTLString );
		bool operator>=( const std::string & anSTLString );
		bool operator>=( std::string & anSTLString ) const;
		bool operator>=( const std::string & anSTLString ) const;

		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this string. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a std::string.
		 */
		virtual CKString toString() const;

		/*
		 * There are times that it'd be nice to see the hex characters of
		 * the string laid out so that you can actually see the binary data
		 * in the string. This is the method that does that.
		 */
		virtual CKString toHexString() const;

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
		friend class CKStringNode;
		friend class CKStringList;

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
 * ----------------------------------------------------------------------------
 * This is the low-level node in the doubly-linked list that will be used
 * to organize a list of strings. This is nice in that it's easy to use, easy
 * to deal with, and the destructor takes care of cleaning up the strings
 * itself.
 *
 * We base it off the CKString so that it appears to be a normal string in
 * all regards - save the ability to exist in a doubly-linked list.
 *
 * The reason for this is that the STL std::vector and stl::list are no good
 * in their implementations in GCC, so rather than try to fix that code, we
 * chose to write our own classes.
 * ----------------------------------------------------------------------------
 */
class CKStringNode :
	public CKString
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that really doesn't contain
		 * anything. This isn't so bad, as the setters allow you to
		 * populate this guy later with anything that you could want.
		 */
		CKStringNode();
		/*
		 * This is a "promotion" constructor that takes a string and
		 * creates a new string node based on the data in that string.
		 * This is important because it'll be an easy way to add strings
		 * to the list.
		 */
		CKStringNode( const CKString & anOther,
					  CKStringNode *aPrev = NULL,
					  CKStringNode *aNext = NULL );
		CKStringNode( const char * aCString,
					  CKStringNode *aPrev = NULL,
					  CKStringNode *aNext = NULL );
		CKStringNode( const std::string & anSTLString,
					  CKStringNode *aPrev = NULL,
					  CKStringNode *aNext = NULL );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKStringNode( const CKStringNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKStringNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKStringNode & operator=( const CKStringNode & anOther );
		/*
		 * At times it's also nice to be able to set a string to this
		 * node so that there's not a ton of casting in the code.
		 */
		CKStringNode & operator=( const CKString & anOther );
		CKStringNode & operator=( const char * aCString );
		CKStringNode & operator=( const std::string & aCString );


		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * These are the simple setters for the links to the previous and
		 * next nodes in the list. There's nothing special here, so we're
		 * exposing them directly.
		 */
		void setPrev( CKStringNode *aNode );
		void setNext( CKStringNode *aNode );

		/*
		 * These are the simple getters for the links to the previous and
		 * next nodes in the list. There's nothing special here, so we're
		 * exposing them directly.
		 */
		CKStringNode *getPrev();
		CKStringNode *getNext();

		/*
		 * This method is used to 'unlink' the node from the list it's in.
		 * This will NOT delete the node, merely take it out the the list
		 * and now it becomes the responsibility of the caller to delete
		 * this node, or add him to another list.
		 */
		void removeFromList();

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKStringNodes are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKStringNode & anOther ) const;
		/*
		 * This method checks to see if the two CKStringNodes are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKStringNode & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString() const;

	private:
		friend class CKStringList;

		/*
		 * Since we're a doubly-linked list, I'm just going to have a
		 * prev and next pointers and that will take care of the linking.
		 */
		CKStringNode		*mPrev;
		CKStringNode		*mNext;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKStringNode & aNode );




/*
 * ----------------------------------------------------------------------------
 * This is the high-level interface to a list of CKString objects. It
 * is organized as a doubly-linked list of CKStringNodes and the interface
 * to the list if controlled by a nice CKFWMutex. This is a nice and clean
 * replacement to the STL std::list.
 * ----------------------------------------------------------------------------
 */
class CKStringList
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that really doesn't contain
		 * anything. This isn't so bad, as the setters allow you to
		 * populate this guy later with anything that you could want.
		 */
		CKStringList();
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around.
		 */
		CKStringList( CKStringList & anOther );
		CKStringList( const CKStringList & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called.
		 */
		virtual ~CKStringList();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKStringList & operator=( CKStringList & anOther );
		CKStringList & operator=( const CKStringList & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * These are the easiest ways to get at the head and tail of this
		 * list. After that, the CKStringNode's getPrev() and getNext()
		 * do a good job of moving you around the list.
		 */
		CKStringNode *getHead();
		CKStringNode *getHead() const;
		CKStringNode *getTail();
		CKStringNode *getTail() const;

		/*
		 * These methods return copies of the first and last strings in the
		 * list. This is useful if you want to look at the elements of the
		 * list but don't want to do the size() - 1 for the last one. If the
		 * list is empty then the empty string will be returned, so it's best
		 * to check empty() before using the results of this method for sure.
		 */
		CKString getFirst();
		CKString getFirst() const;

		CKString getLast();
		CKString getLast() const;

		/*
		 * Because there may be times that the user wants to lock us up
		 * for change, we're going to expose this here so it's easy for them
		 * to iterate, for example.
		 */
		void lock();
		void lock() const;
		void unlock();
		void unlock() const;

		/*
		 * This method is a simple indexing operator so that we can easily
		 * get the individual strings in the list. If the argument
		 * is -1, then the default is to get the *LAST* non-NULL
		 * string in the list.
		 */
		CKString & operator[]( int aPosition );
		CKString & operator[]( int aPosition ) const;

		/********************************************************
		 *
		 *                List Methods
		 *
		 ********************************************************/
		/*
		 * This method gets the size of the list in a thread-safe
		 * way. This means that it will block until it can get the
		 * lock on the data, so be warned.
		 */
		int size();
		int size() const;

		/*
		 * This is used to tell the caller if the list is empty. It's
		 * faster than checking for a size() == 0.
		 */
		bool empty();
		bool empty() const;

		/*
		 * This method clears out the entire list and deletes all it's
		 * contents. After this, all node pointers to nodes in this list
		 * will be pointing to nothing, so watch out.
		 */
		void clear();
		void clear() const;

		/*
		 * When I want to add a string to the front or back of the list,
		 * these are the simplest ways to do that. The passed-in string
		 * is left untouched, and a copy is made of it at the proper point
		 * in the list.
		 */
		void addToFront( CKString & aString );
		void addToFront( const CKString & aString );
		void addToFront( CKString & aString ) const;
		void addToFront( const CKString & aString ) const;

		void addToFront( std::string & anSTLString );
		void addToFront( const std::string & anSTLString );
		void addToFront( std::string & anSTLString ) const;
		void addToFront( const std::string & anSTLString ) const;

		void addToFront( char * aCString );
		void addToFront( const char * aCString );
		void addToFront( char * aCString ) const;
		void addToFront( const char * aCString ) const;

		void addToEnd( CKString & aString );
		void addToEnd( const CKString & aString );
		void addToEnd( CKString & aString ) const;
		void addToEnd( const CKString & aString ) const;

		void addToEnd( std::string & anSTLString );
		void addToEnd( const std::string & anSTLString );
		void addToEnd( std::string & anSTLString ) const;
		void addToEnd( const std::string & anSTLString ) const;

		void addToEnd( char *aCString );
		void addToEnd( const char *aCString );
		void addToEnd( char *aCString ) const;
		void addToEnd( const char *aCString ) const;

		/*
		 * These methods take control of the passed-in arguments and place
		 * them in the proper place in the list. This is different in that
		 * the control of the node is passed to the list, but that's why
		 * we've created them... to make it easy to add in nodes by just
		 * changing the links.
		 */
		void putOnFront( CKStringNode *aNode );
		void putOnFront( const CKStringNode *aNode );
		void putOnFront( CKStringNode *aNode ) const;
		void putOnFront( const CKStringNode *aNode ) const;

		void putOnEnd( CKStringNode *aNode );
		void putOnEnd( const CKStringNode *aNode );
		void putOnEnd( CKStringNode *aNode ) const;
		void putOnEnd( const CKStringNode *aNode ) const;

		/*
		 * When you have a list that you want to add to this list, these
		 * are the methods to use. It's important to note that the arguments
		 * will NOT be altered - which is why this is called the 'copy' as
		 * opposed to the 'splice'.
		 */
		void copyToFront( CKStringList & aList );
		void copyToFront( const CKStringList & aList );
		void copyToFront( CKStringList & aList ) const;
		void copyToFront( const CKStringList & aList ) const;

		void copyToEnd( CKStringList & aList );
		void copyToEnd( const CKStringList & aList );
		void copyToEnd( CKStringList & aList ) const;
		void copyToEnd( const CKStringList & aList ) const;

		/*
		 * When you have a list that you want to merge into this list, these
		 * are the methods to use. It's important to note that the argument
		 * lists will be EMPTIED - which is why this is called the 'splice'
		 * as opposed to the 'copy'.
		 */
		void spliceOnFront( CKStringList & aList );
		void spliceOnFront( const CKStringList & aList );
		void spliceOnFront( CKStringList & aList ) const;
		void spliceOnFront( const CKStringList & aList ) const;

		void spliceOnEnd( CKStringList & aList );
		void spliceOnEnd( const CKStringList & aList );
		void spliceOnEnd( CKStringList & aList ) const;
		void spliceOnEnd( const CKStringList & aList ) const;

		/*
		 * This method removes the specified node from this list - actually,
		 * it's just guaranteed to remove it from the list it's in as the
		 * erasure simply removes this node from it's list and then deletes
		 * it.
		 */
		void erase( CKStringNode * & aNode );
		void erase( CKStringNode * & aNode ) const;

		void erase( CKString & aString );
		void erase( const CKString & aString );
		void erase( CKString & aString ) const;
		void erase( const CKString & aString ) const;

		void erase( char *aCString );
		void erase( const char *aCString );
		void erase( char *aCString ) const;
		void erase( const char *aCString ) const;

		void erase( std::string & anSTLString );
		void erase( const std::string & anSTLString );
		void erase( std::string & anSTLString ) const;
		void erase( const std::string & anSTLString ) const;

		/*
		 * This method is useful in that it will tell us if this list
		 * contains the provided string and that is a nice test if we
		 * want to be making list of unique elements.
		 */
		bool contains( CKString & aString );
		bool contains( const CKString & aString );
		bool contains( CKString & aString ) const;
		bool contains( const CKString & aString ) const;

		bool contains( char *aCString );
		bool contains( const char *aCString );
		bool contains( char *aCString ) const;
		bool contains( const char *aCString ) const;

		bool contains( std::string & anSTLString );
		bool contains( const std::string & anSTLString );
		bool contains( std::string & anSTLString ) const;
		bool contains( const std::string & anSTLString ) const;

		/*
		 * This method is useful in that it will locate the CKStringNode
		 * in the list, if it exists, that matches the passed-in string.
		 * This is a nice way to find a location in the list.
		 */
		CKStringNode *find( CKString & aString );
		CKStringNode *find( const CKString & aString );
		CKStringNode *find( CKString & aString ) const;
		CKStringNode *find( const CKString & aString ) const;

		CKStringNode *find( char *aCString );
		CKStringNode *find( const char *aCString );
		CKStringNode *find( char *aCString ) const;
		CKStringNode *find( const char *aCString ) const;

		CKStringNode *find( std::string & anSTLString );
		CKStringNode *find( const std::string & anSTLString );
		CKStringNode *find( std::string & anSTLString ) const;
		CKStringNode *find( const std::string & anSTLString ) const;

		/*
		 * These methods remove the first and last strings from the list
		 * and return them to the callers. The idea is that many times
		 * when the processing of a list is done a line at a time and this
		 * makes it easy to do this. If there are no lines in the list
		 * this method will return the empty string, so it's important to
		 * use the size() or empty() methods to see when to stop popping
		 * off the strings.
		 */
		CKString popOffFront();
		CKString popOffFront() const;

		CKString popOffEnd();
		CKString popOffEnd() const;

		/*
		 * This is the tokenizer/parser that wasn't in the STL string
		 * class for some unknown reason. It takes a source and a
		 * delimiter and breaks up the source into chunks that are
		 * all separated by the delimiter string. Each chunk is put
		 * into the returned vector for accessing by the caller. Since
		 * the return value is created on the stack, the user needs to
		 * save it if they want it to stay around.
		 */
		static CKStringList parseIntoChunks( const CKString & aString,
											 const CKString & aDelim );

		/*
		 * This method is really useful when dealing with a list of
		 * strings and wanting to make a single string out of them.
		 * This might be in a debug print statement, it might also be
		 * a way to "piece together" something. In any case, the elements
		 * of the string list will be put into the resultant string each
		 * one separated by the passed-in separator.
		 */
		CKString concatenate( const CKString & aSeparator );
		CKString concatenate( const CKString & aSeparator ) const;

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKStringLists are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator==( CKStringList & anOther );
		bool operator==( const CKStringList & anOther );
		bool operator==( CKStringList & anOther ) const;
		bool operator==( const CKStringList & anOther ) const;
		/*
		 * This method checks to see if the two CKStringLists are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( CKStringList & anOther );
		bool operator!=( const CKStringList & anOther );
		bool operator!=( CKStringList & anOther ) const;
		bool operator!=( const CKStringList & anOther ) const;
		/*
		 * Because there are times when it's useful to have a nice
		 * human-readable form of the contents of this instance. Most of the
		 * time this means that it's used for debugging, but it could be used
		 * for just about anything. In these cases, it's nice not to have to
		 * worry about the ownership of the representation, so this returns
		 * a CKString.
		 */
		virtual CKString toString();

	protected:
		/*
		 * Setting the head or the tail is a bit dicey and so we're not
		 * going to let just anyone change these guys.
		 */
		void setHead( CKStringNode *aNode );
		void setTail( CKStringNode *aNode );

	private:
		/*
		 * A Doubly-linked list is pretty easy - there's a head and a
		 * tail and that's about it.
		 */
		CKStringNode			*mHead;
		CKStringNode			*mTail;
		/*
		 * This is the mutex that is going to protect all the dangerous
		 * operations so that this list is thread-safe.
		 */
		CKFWMutex				mMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKStringList & aList );

#endif	// __CKSTRING_H
