/*
 * CKIndexNode.h - this file defines a class that can be used to index
 *                 data in a CKDataNode-based tree. The idea is that the
 *                 CKDataNode is a very strict construction tree where
 *                 there can be only one parent for each child. This is
 *                 important so that you can travel within the tree without
 *                 worrying about getting lost.
 *
 *                 Yet there are times when you want to be able to access
 *                 the data in a CKDataNode tree without having to travel
 *                 through the tree to find each node. Rather, it would be
 *                 nice to be able to use a 'secondary tree' that organizes
 *                 the data in a different manner that makes it easier to
 *                 get at what you need. Since this is just an organizational
 *                 grouping, we don't want this tree to manage the memory of
 *                 the leaf nodes, as the main CKDataTree does all that.
 *
 * $Id: CKIndexNode.h,v 1.1 2004/12/22 10:50:24 drbob Exp $
 */
#ifndef __CKINDEXNODE_H
#define __CKINDEXNODE_H

//	System Headers
#ifdef GPP2
#include <algo.h>
#endif
#include <map>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKDataNode.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKIndexNode :
	public CKDataNode
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that creates a nice, empty
		 * node ready for the user to organize as needed.
		 */
		CKIndexNode();
		/*
		 * This constructor creates a new node and sets the reference to
		 * the node's parent to the supplied pointer. This is OK because
		 * each node *never* controls the memory of it's parent.
		 */
		CKIndexNode( CKIndexNode *aParent );
		/*
		 * This constructor makes a node with the given parent and the
		 * identifying name provided as well. This is useful when the
		 * structure of a tree is being built and the data will be
		 * populated later. The parent node is not controlled by the
		 * instance because no parent nodes are controlled by the nodes.
		 */
		CKIndexNode( CKIndexNode *aParent, const CKString & aName );
		/*
		 * This constructor creates a node with the given parent node
		 * reference as well as the provided identifying name and also
		 * sets one key/value pair in the node. This can be really helpful
		 * in setting up a grouping with time-series data, for instance.
		 * the name might be the symbol's name, the key might be "price"
		 * and the value could be a variant time-series of the price
		 * data. This would make creating a tree of nodes very easy.
		 */
		CKIndexNode( CKIndexNode *aParent, const CKString & aName,
					 const CKString & aKey, const CKVariant & aValue );
		/*
		 * This is the standard copy constructor and needs to be in every
		 * class to make sure that we don't have too many things running
		 * around. It's important to note that it's a SHALLOW COPY and the
		 * pointers to the children of the original node are simply copied
		 * and therefore point to the exact same instances in the copy as
		 * the original. This is equivalent to the newNodeByShallowCopy()
		 * static method.
		 */
		CKIndexNode( const CKIndexNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called. It's important to note that this is a SHALLOW delete
		 * and none of the children nodes will be deleted. This is the
		 * same as the deleteNodeShallow() call, and care needs to be
		 * taken with the children to make sure they don't end up as a
		 * memory leak.
		 */
		virtual ~CKIndexNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKIndexNode & operator=( const CKIndexNode & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * Each index node can have a single CKDataNode as it's 'contents'.
		 * If a node has this 'contents' it is here because it has been
		 * explicitly set by the user. The purpose of these structures is
		 * to be able to organize data in an index tree differently from
		 * how it might be stored in it's native CKDataNode tree. This
		 * then allows the user to 'tag' a node by a different path, and
		 * maintain associated data with it.
		 */
		CKDataNode *getReference();
		/*
		 * This method gets the reference node at the specified path assuming
		 * that the path exists in this tree, and that there is a reference
		 * node at this node in the tree. If either of these isn't true, then
		 * the result will be NULL.
		 */
		CKDataNode *getReferenceAtPath( const CKString & aPath );
		/*
		 * This version of the method takes a list of path steps so that
		 * there's no need to worry about escaping any node names in the path.
		 */
		CKDataNode *getReferenceAtPath( const CKStringList & aSteps );

		/*
		 * Each index node can have a single CKDataNode as it's 'contents'.
		 * If a node has this 'contents' it is here because this method
		 * has been called.
		 */
		void putReference( CKDataNode *aNode );
		/*
		 * This method sets the reference node at the specified path even if
		 * the path doesn't exist. As expected, if the path doesn't exist, it
		 * will be created and the reference set.
		 */
		void putReferenceAtPath( const CKString & aPath, CKDataNode *aNode );
		/*
		 * This version of the method takes a list of path steps so that
		 * there's no need to worry about escaping any node names in the path.
		 */
		void putReferenceAtPath( const CKStringList & aSteps, CKDataNode *aNode );

		/********************************************************
		 *
		 *                Copy Methods
		 *
		 ********************************************************/
		/*
		 * This method creates a new CKIndexNode whose pointer is
		 * returned to the caller, and whose memory control is passed
		 * to the caller. This is a SHALLOW copy of the incoming node
		 * meaning that the copy will point to the same parent and
		 * children nodes so care needs to be exercised in deleting
		 * the original and the copy.
		 */
		static CKIndexNode *newNodeByShallowCopy( const CKIndexNode *aNode );
		/*
		 * This method creates a new CKIndexNode whose pointer is
		 * returned to the caller, and whose memory control is passed
		 * to the caller. This is a DEEP copy of the incoming node
		 * meaning that the copy will point to the same parent yet all
		 * the children nodes will be deep copies as well. This means
		 * that a duplicate tree with duplicate values is created.
		 *
		 * The typical calling scheme is to call thit with only the node
		 * to copy as the argument. The second argument is really just
		 * used in the processing of the copy as a thing to remind the
		 * new nodes where they belong. If you call this method with
		 * two paramters you had better really know what you're doing.
		 */
		static CKIndexNode *newNodeByDeepCopy( const CKIndexNode *aNode,
											   const CKIndexNode *aParent = NULL );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKIndexNodes are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKIndexNode & anOther ) const;
		/*
		 * This method checks to see if the two CKIndexNodes are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKIndexNode & anOther ) const;
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
		virtual CKString toString( bool aDeepFlag = false ) const;

	private:
		/*
		 * This is what makes this class special - a pointer to a CKDataNode
		 * which this instance does not 'own', but rather uses as a reference.
		 * This will be set by the
		 */
		CKDataNode			*mReference;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIndexNode & aNode );

#endif	// __CKINDEXNODE_H
