/*
 * CKMultiIndexNode.h - this file defines a class that can be used to index
 *                      groups of nodex in a CKDataNode-based tree. The idea
 *                      is that the CKDataNode is a very strict construction
 *                      tree where there can be only one parent for each child.
 *                      This is important so that you can travel within the
 *                      tree without worrying about getting lost.
 *
 *                      Yet there are times when you want to be able to access
 *                      the data in a CKDataNode tree without having to travel
 *                      through the tree to find each node. Rather, it would be
 *                      nice to be able to use a 'secondary tree' that
 *                      organizes the data in a different manner that makes it
 *                      easier to get at what you need. Since this is just an
 *                      organizational grouping, we don't want this tree to
 *                      manage the memory of the leaf nodes, as the main
 *                      CKDataTree does all that.
 *
 *                      There are times when the single reference of the
 *                      CKIndexNode object isn't enough to model the data
 *                      structure. Rather, we need a 1:many relationship in
 *                      the referencing. For those times, this multi-index
 *                      node is quite useful.
 *
 * $Id: CKMultiIndexNode.h,v 1.1 2005/02/14 15:44:00 drbob Exp $
 */
#ifndef __CKMULTIINDEXNODE_H
#define __CKMULTIINDEXNODE_H

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
#include "CKVector.h"
#include "CKDataNode.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKMultiIndexNode :
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
		CKMultiIndexNode();
		/*
		 * This constructor creates a new node and sets the reference to
		 * the node's parent to the supplied pointer. This is OK because
		 * each node *never* controls the memory of it's parent.
		 */
		CKMultiIndexNode( CKMultiIndexNode *aParent );
		/*
		 * This constructor makes a node with the given parent and the
		 * identifying name provided as well. This is useful when the
		 * structure of a tree is being built and the data will be
		 * populated later. The parent node is not controlled by the
		 * instance because no parent nodes are controlled by the nodes.
		 */
		CKMultiIndexNode( CKMultiIndexNode *aParent, const CKString & aName );
		/*
		 * This constructor creates a node with the given parent node
		 * reference as well as the provided identifying name and also
		 * sets one key/value pair in the node. This can be really helpful
		 * in setting up a grouping with time-series data, for instance.
		 * the name might be the symbol's name, the key might be "price"
		 * and the value could be a variant time-series of the price
		 * data. This would make creating a tree of nodes very easy.
		 */
		CKMultiIndexNode( CKMultiIndexNode *aParent, const CKString & aName,
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
		CKMultiIndexNode( const CKMultiIndexNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called. It's important to note that this is a SHALLOW delete
		 * and none of the children nodes will be deleted. This is the
		 * same as the deleteNodeShallow() call, and care needs to be
		 * taken with the children to make sure they don't end up as a
		 * memory leak.
		 */
		virtual ~CKMultiIndexNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKMultiIndexNode & operator=( const CKMultiIndexNode & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * Each index node can have a list of CKDataNodes as it's 'contents'.
		 * If a node has a member in this 'contents' it is here because it
		 * has been explicitly set by the user. The purpose of these
		 * structures is to be able to organize data in an index tree
		 * differently from how it might be stored in it's native CKDataNode
		 * tree. This then allows the user to 'tag' a node by a different
		 * path, and maintain associated data with it.
		 */
		CKVector<CKDataNode*> *getReferences();
		/*
		 * This method gets the reference list at the specified path assuming
		 * that the path exists in this tree, and that there is a reference
		 * node at this node in the tree. If either of these isn't true, then
		 * the result will be NULL.
		 */
		CKVector<CKDataNode*> *getReferencesAtPath( const CKString & aPath );
		/*
		 * This version of the method takes a list of path steps so that
		 * there's no need to worry about escaping any node names in the path.
		 */
		CKVector<CKDataNode*> *getReferencesAtPath( const CKStringList & aSteps );

		/*
		 * This method adds a CKDataNode 'reference' to this node's list of
		 * references. This is the easiest way to add something in the bucket
		 * of the list of interest.
		 */
		bool addReference( CKDataNode *aNode );
		/*
		 * This method finds the correct node in this data tree based on the
		 * provided path and then places the provided data node in that
		 * node's list of references. It's a simple way to make sure that the
		 * pathed node exists, and to add a reference all in one step.
		 */
		bool addReferenceAtPath( const CKString & aPath, CKDataNode *aNode );
		/*
		 * This method finds the correct node in this data tree based on the
		 * provided path of steps and then places the provided data node in
		 * that node's list of references. It's a simple way to make sure that
		 * the pathed node exists, and to add a reference all in one step.
		 */
		bool addReferenceAtPath( const CKStringList & aSteps, CKDataNode *aNode );

		/*
		 * This method removes the specified reference from this node's list
		 * of data node pointers (references). If this reference does not
		 * exist in this node's list, nothing will be done. This is a nice
		 * and convenient way of "removing" oneself from the node's list.
		 */
		void removeReference( CKDataNode *aNode );
		/*
		 * This method first finds the node specified by the path and then
		 * removes the data node from the list of nodes of interest to this
		 * pathed node. This is nice in that the node will be created if it
		 * doesn't exist, and there's no harm in removing something that isn't
		 * in the list.
		 */
		void removeReferenceAtPath( const CKString & aPath, CKDataNode *aNode );
		/*
		 * This method first finds the node specified by the path of steps and
		 * then removes the data node from the list of nodes of interest to
		 * this pathed node. This is nice in that the node will be created if
		 * it doesn't exist, and there's no harm in removing something that
		 * isn't in the list.
		 */
		void removeReferenceAtPath( const CKStringList & aSteps, CKDataNode *aNode );

		/*
		 * This method clears out all the references for this node. This is
		 * a basic "do over" method as all the references to the nodes will
		 * be lost, but they will not be deleted as this guy never owned them.
		 */
		void clearReferences();
		/*
		 * This method first finds the node in this tree referenced by the
		 * path and then clears out all the references for this node. This is
		 * a basic "do over" method as all the references to the nodes will
		 * be lost, but they will not be deleted as this guy never owned them.
		 */
		void clearReferencesAtPath( const CKString & aPath );
		/*
		 * This method first finds the node in this tree referenced by the
		 * path of steps and then clears out all the references for this node.
		 * This is a basic "do over" method as all the references to the nodes
		 * will be lost, but they will not be deleted as this guy never owned
		 * them.
		 */
		void clearReferencesAtPath( const CKStringList & aSteps );

		/********************************************************
		 *
		 *                Copy Methods
		 *
		 ********************************************************/
		/*
		 * This method creates a new CKMultiIndexNode whose pointer is
		 * returned to the caller, and whose memory control is passed
		 * to the caller. This is a SHALLOW copy of the incoming node
		 * meaning that the copy will point to the same parent and
		 * children nodes so care needs to be exercised in deleting
		 * the original and the copy.
		 */
		static CKMultiIndexNode *newNodeByShallowCopy( const CKMultiIndexNode *aNode );
		/*
		 * This method creates a new CKMultiIndexNode whose pointer is
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
		static CKMultiIndexNode *newNodeByDeepCopy( const CKMultiIndexNode *aNode,
													const CKMultiIndexNode *aParent = NULL );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKMultiIndexNodes are equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKMultiIndexNode & anOther ) const;
		/*
		 * This method checks to see if the two CKMultiIndexNodes are not equal
		 * to one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKMultiIndexNode & anOther ) const;
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
		 * This is what makes this class special - a vector of pointers to
		 * CKDataNodes which this instance does not 'own', but rather uses
		 * as a reference. This will be set by the accessor methods and is
		 * up to the user to maintain.
		 */
		CKVector<CKDataNode*>		mReferences;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKMultiIndexNode & aNode );

#endif	// __CKMULTIINDEXNODE_H
