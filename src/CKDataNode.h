/*
 * CKDataNode.h - this file defines a class that can be used to represent a
 *                general tree of data where each node contains a map of
 *                key/value pairs where the key is a CKString (name) and
 *                the value is a CKVariant that can hold almost anything
 *                you need to hold. In addition to the data, this node has
 *                a list of children nodes (pointers to CKDataNodes) and a
 *                pointer to a parent node. This means that this guy can
 *                be the basis of a complete tree of data and this is
 *                very important to many applications.
 *
 * $Id: CKDataNode.h,v 1.10 2004/09/16 09:34:13 drbob Exp $
 */
#ifndef __CKDATANODE_H
#define __CKDATANODE_H

//	System Headers
#ifdef GPP2
#include <algo.h>
#endif
#include <string>
#include <list>
#include <vector>
#include <map>
#ifdef GPP2
#include <ostream.h>
#else
#include <ostream>
#endif

//	Third-Party Headers

//	Other Headers
#include "CKVariant.h"
#include "CKString.h"
#include "CKFWMutex.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/*
 * This is the main class definition.
 */
class CKDataNode
{
	public:
		/********************************************************
		 *
		 *                Constructors/Destructor
		 *
		 ********************************************************/
		/*
		 * This is the default constructor that creates a nice, empty
		 * node ready for the user to dump data into as needed.
		 */
		CKDataNode();
		/*
		 * This constructor creates a new node and sets the reference to
		 * the node's parent to the supplied pointer. This is OK because
		 * each node *never* controls the memory of it's parent.
		 */
		CKDataNode( CKDataNode *aParent );
		/*
		 * This constructor makes a node with the given parent and the
		 * identifying name provided as well. This is useful when the
		 * structure of a tree is being built and the data will be
		 * populated later. The parent node is not controlled by the
		 * instance because no parent nodes are controlled by the nodes.
		 */
		CKDataNode( CKDataNode *aParent, const CKString & aName );
		/*
		 * This constructor creates a node with the given parent node
		 * reference as well as the provided identifying name and also
		 * sets one key/value pair in the node. This can be really helpful
		 * in setting up a grouping with time-series data, for instance.
		 * the name might be the symbol's name, the key might be "price"
		 * and the value could be a variant time-series of the price
		 * data. This would make creating a tree of nodes very easy.
		 */
		CKDataNode( CKDataNode *aParent, const CKString & aName,
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
		CKDataNode( const CKDataNode & anOther );
		/*
		 * This is the standard destructor and needs to be virtual to make
		 * sure that if we subclass off this the right destructor will be
		 * called. It's important to note that this is a SHALLOW delete
		 * and none of the children nodes will be deleted. This is the
		 * same as the deleteNodeShallow() call, and care needs to be
		 * taken with the children to make sure they don't end up as a
		 * memory leak.
		 */
		virtual ~CKDataNode();

		/*
		 * When we want to process the result of an equality we need to
		 * make sure that we do this right by always having an equals
		 * operator on all classes.
		 */
		CKDataNode & operator=( const CKDataNode & anOther );

		/********************************************************
		 *
		 *                Accessor Methods
		 *
		 ********************************************************/
		/*
		 * This method sets the reference to the parent node of this
		 * node to the provided value. At no time will this instance
		 * be responsible for the memory management of the parent node.
		 * It's more of a link than anything else. This method will
		 * also add this instance as a child of the parent node so that
		 * the doubly-linked scheme is in-place with one call.
		 */
		void setParent( CKDataNode *aNode );
		/*
		 * This method sets the identifying name for this node. This
		 * is useful because most data sets (trees) have an identifying
		 * name for each data point.
		 */
		void setName( const CKString & aName );

		/*
		 * This method returns the pointer that is the actual reference
		 * to the parent node of this node. If this node happens to be
		 * a root node, this will return NULL indicating that it's a
		 * root.
		 */
		CKDataNode *getParent() const;
		/*
		 * This method returns the identifying name of this node. In most
		 * data structures it makes sense to name the nodes and this is
		 * the way to see what this node's name is.
		 */
		CKString getName() const;

		/*
		 * Each node can have many variables (attributes) stored in a map
		 * as a CKString name and CKVariant value. This method returns a
		 * pointer to the actual named value so if you want to do something
		 * with it, make a copy. If there is no variable with this name the
		 * method will return a NULL.
		 */
		CKVariant *getVar( const CKString & aName );
		/*
		 * Each node can have many variables (attributes) stored in a map
		 * as a CKString name and CKVariant value. This method places a
		 * value into that map for this instance at the name provided.
		 */
		void putVar( const CKString & aName, const CKVariant & aValue );
		/*
		 * Since each node can hold many variables (attributes), it's
		 * sometimes necessary to clean out the old ones. This method
		 * removes the named variable from this node if it exists.
		 */
		void removeVar( const CKString & aName );
		/*
		 * When you want to clear out all the variables (attributes) from
		 * this node, call this method and the entire map of variables will
		 * be cleared out. It's non-reversable, so be carful with this.
		 */
		void clearVars();

		/*
		 * This method adds the provided node as a child of the current
		 * node - assuming it's not already a child node. If it's a new
		 * child node then this method will instruct the child node to
		 * add this instance as it's new parent. This might seem like a
		 * loop with setParent(), but since the check is made for a *new*
		 * child node, the loop is broken after the first pass. As an
		 * aside, this makes sure that it's easy to add a node as a child
		 * and not have to worry about the reverse link.
		 *
		 * The memory management of the newly linked child node is shared
		 * by this node and the creator. There are methods on the class
		 * for both shallow and deep deletions so it's up to the designer
		 * of the application instance to decide where the memory management
		 * is going to be.
		 */
		void addChild( CKDataNode *aNode );
		/*
		 * This method removes the provided node from the list of child
		 * nodes of this node - if it's actually in the list. If it's
		 * being removed this method will also instruct the now ex-child
		 * node to NULL out it's parent node so that there's no confusion
		 * on the issue.
		 */
		void removeChild( const CKDataNode *aNode );
		/*
		 * This method returns the complete list (as a vector) of all the
		 * child nodes identifying names. This is a very handy way to
		 * iterate over the children assuming they all have distinct names
		 * as would be the case in most data sets.
		 */
		std::vector<CKString> getChildNames();
		/*
		 * This method returns the pointer to the actual node that is
		 * both a child of this node and has the identifying name that
		 * is the same as that passed into this method. This is the actual
		 * pointer, so if you want to do anything with it, you need to
		 * make a copy.
		 */
		CKDataNode *findChild( const CKString & aName );
		/*
		 * This method returns the number of child nodes this node has. This
		 * is useful for a lot of things, and among them is the core of the
		 * isLeaf() method, below.
		 */
		int getChildCount();
		/*
		 * This method will return true if there are no child nodes attached
		 * to this node. This then, would make this node a leaf node in the
		 * tree.
		 */
		bool isLeaf();

		/*
		 * One of the more interesting capabilities of this class is it's
		 * ability to understand that trees are wonderful to store data
		 * but a pain to navigate. This class has methods that make this
		 * navigation very easy. This method, for instance, takes a string
		 * that is a series of node identifying names spearated by a '/'
		 * to represent a "path" to the variable (attribute) of a node.
		 *
		 * For example, say the path was:
		 *
		 *     SectionA/Subsection1/GroupQ/Item212/size
		 *
		 * This method would look for the child identified by the name
		 * "SectionA" and then have it look for a child called "Subsection1"
		 * and it's child called "GroupQ" and it's child called "Item212".
		 * At this point the method would look into the variables (attributes)
		 * of the "Item212" node and look for the named variable "size" which
		 * has to be a CKVariant. That is what it would return.
		 *
		 * It's important to note that if any node in the path is NOT FOUND
		 * then this method returns a NULL.
		 *
		 * If the path string includes a leading '/' then the path is
		 * taken from the root node of the tree that this node is but a
		 * part of. So, even if this node is *not* in the path, the value
		 * will be returned if it's in the tree.
		 */
		CKVariant *getVarAtPath( const CKString & aPath );
		/*
		 * This method takes a vector of strings as the path as opposed
		 * to a single string delimited with '/'. This makes it a little
		 * easier if you're building up the path by data elements and
		 * you don't really want to make a single string just to store
		 * the value.
		 */
		CKVariant *getVarAtPath( const std::vector<CKString> & aSteps );

		/*
		 * This method is part of the "pathing" capabilities of this class
		 * and is intended to store values in nodes in a tree structure.
		 * If, for example, the path is:
		 *
		 *     Parts/Metal/Gear/Chainring/count
		 *
		 * then it would look for the child node identified by the name
		 * "Parts" and then to it's child node named "Metal" and to it's
		 * child named "Gear" and to it's child named "Chainring". At this
		 * point the supplied CKVariant value would be placed in the
		 * "Chainring" node's variables (attributes) map under the name
		 * "count".
		 *
		 * One of the nicest features of this method is that the nodes in
		 * the path DO NOT have to exist prior to the call. As the method
		 * is walking down the tree, if no child is there of the proper
		 * name, one will be created and the name assigned and the process
		 * continued. This makes populating the tree with data very easy
		 * indeed.
		 *
		 * A final note - if the path starts with a '/' then the path is
		 * referenced to the root node of the tree that this node is but
		 * a part in. This is very powerful because it means that as long
		 * as you have a reference to *any* node in the tree you can set
		 * any value in any node in that same tree without having to assume
		 * a currently defined structure.
		 */
		void putVarAtPath( const CKString & aPath, const CKVariant & aValue );
		/*
		 * This version of the method takes a vector of strings that is
		 * the path as opposed to a single string delimited by the '/'.
		 * This is useful when you have the data organized in something
		 * like a vector and you don't want to put it all together only
		 * to have this method break it up.
		 */
		void putVarAtPath( const std::vector<CKString> & aSteps,
						   const CKVariant & aValue );

		/*
		 * This method returns a vector of the node identifiers in this
		 * tree leading to the current node. This is basically walking
		 * 'up' the tree to the root, building accumulating the steps
		 * along the way.
		 */
		std::vector<CKString> getSteps() const;
		/*
		 * This method returns a string path to the current node in a
		 * very similar way to the getSteps() method. The path lets the
		 * caller know where in this tree this particular node lies.
		 */
		CKString getPath() const;

		/*
		 * This method is very nice in that it takes a single string that
		 * represents a path and breaks it up into it's components, placing
		 * each in the returned vector in the proper order. Leading and
		 * trailing '/' characters are removed and any component in the
		 * path escaped by double-quotes will be kept intact. This is the
		 * way for a component of the path to include a '/' character.
		 */
		static std::vector<CKString> pathToSteps( const CKString & aPath );
		/*
		 * This method is useful in that it takes a vector of path steps,
		 * or components, and then assembles them into a single string
		 * that is properly escaped for the presence of '/' characters in
		 * any one of the steps.
		 */
		static CKString stepsToPath( const std::vector<CKString> & aPath );

		/*
		 * There are times that we want to know the identifying names of
		 * all the leaf nodes of a certain section of the tree. This is
		 * really useful when we need to gather data from an external
		 * source, and need a list of names to request that data for. This
		 * method does a great job of getting a unique vector of names
		 * of all the leaf nodes under it, and all it's children.
		 */
		std::vector<CKString>	getUniqueLeafNodeNames();
		/*
		 * This method is interesting in that it will return the list of
		 * unique leaf node names that are *missing* the provided variable
		 * name in their list of variables. This is really nice in that
		 * it allows us to ask the question: Who needs 'price'? and have
		 * a list of node names that is returned.
		 */
		std::vector<CKString>	getUniqueLeafNodeNamesWithoutVar(
											const CKString & aVarName );
		/*
		 * This method is interesting in that it will return the list of
		 * unique leaf node names that contain the provided variable
		 * name in their list of variables. This is really nice in that
		 * it allows us to ask the question: Who has 'price'? and have
		 * a list of node names that is returned.
		 */
		std::vector<CKString>	getUniqueLeafNodeNamesWithVar(
											const CKString & aVarName );

		/*
		 * This method will return the number of steps that need to be
		 * taken from this node to a leaf node. For example, if this node
		 * contained a child node that also contained a child node, then
		 * this method would return 2. If this node was a leaf node, then
		 * this method would return 0.
		 */
		int getNumOfStepsToLeaf();

		/********************************************************
		 *
		 *                Copy Methods
		 *
		 ********************************************************/
		/*
		 * This method creates a new CKDataNode whose pointer is
		 * returned to the caller, and whose memory control is passed
		 * to the caller. This is a SHALLOW copy of the incoming node
		 * meaning that the copy will point to the same parent and
		 * children nodes so care needs to be exercised in deleting
		 * the original and the copy.
		 */
		static CKDataNode *newNodeByShallowCopy( const CKDataNode *aNode );
		/*
		 * This method creates a new CKDataNode whose pointer is
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
		static CKDataNode *newNodeByDeepCopy( const CKDataNode *aNode,
											  const CKDataNode *aParent = NULL );

		/*
		 * This method deletes the passed node but does not delete
		 * the parent or ANY of the children nodes. This method
		 * simply cleans up the single node itself. It also tells
		 * the parent to remove itself so that it's not thinking
		 * there's a child where there isn't any longer. The pointer
		 * is modified in the process to point to NULL so that it
		 * cannot be deleted again.
		 */
		static void deleteNodeShallow( CKDataNode * & aNode );
		/*
		 * This method deletes this node AND ALL CHILDREN. This is
		 * a way to delete an entire tree starting at the root of
		 * that tree. It also tells the parent to remove itself so
		 * that it's not thinking there's a child where there isn't
		 * any longer.  The pointer is modified in the process to
		 * point to NULL so that it cannot be deleted again.
		 */
		static void deleteNodeDeep( CKDataNode * & aNode );

		/********************************************************
		 *
		 *                Utility Methods
		 *
		 ********************************************************/
		/*
		 * This method checks to see if the two CKDataNodes are equal to one
		 * another based on the values they represent and *not* on the actual
		 * pointers themselves. If they are equal, then this method returns a
		 * value of true, otherwise, it returns a false.
		 */
		bool operator==( const CKDataNode & anOther ) const;
		/*
		 * This method checks to see if the two CKDataNodes are not equal to
		 * one another based on the values they represent and *not* on the
		 * actual pointers themselves. If they are not equal, then this method
		 * returns a value of true, otherwise, it returns a false.
		 */
		bool operator!=( const CKDataNode & anOther ) const;
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

	protected:
		/*
		 * This method returns the actual pointer to the mutex that is
		 * being used to control access to the map of variables in this
		 * node. This is here primarily as an accessor tool for the
		 * subclasses that might be implemented on top of this guy.
		 */
		CKFWMutex *getVarsMutex();
		/*
		 * This method returns the actual pointer to the STL map of the
		 * variables and it's useful for subclasses that have to do
		 * something with the variables that I didn't originally think
		 * of.
		 */
		std::map<CKString, CKVariant> *getVars();

		/*
		 * This method returns the actual pointer to the mutex that is
		 * being used to control access to the list of child nodes in this
		 * node. This is here primarily as an accessor tool for the
		 * subclasses that might be implemented on top of this guy.
		 */
		CKFWMutex *getKidsMutex();
		/*
		 * This method returns the actual pointer to the STL list of the
		 * children and it's useful for subclasses that have to do
		 * something with the children that I didn't originally think
		 * of.
		 */
		std::list<CKDataNode*> *getKids();

		/*
		 * This is the tokenizer/parser that wasn't in the STL string
		 * class for some unknown reason. It takes a source and a
		 * delimiter and breaks up the source into chunks that are
		 * all separated by the delimiter string. Each chunk is put
		 * into the returned vector for accessing by the caller. Since
		 * the return value is created on the stack, the user needs to
		 * save it if they want it to stay around.
		 */
		static std::vector<CKString> parseIntoChunks( const CKString & aString,
													  const CKString & aDelim );

	private:
		/*
		 * This is the parent of this node, and is just another node in
		 * the tree. Of course, it can be NULL, and in that case we're
		 * the root node of a tree.
		 */
		CKDataNode							*mParent;
		/*
		 * Navigating a tree is a lot easier if the nodes are identified
		 * some how. Then, a simple path construct can be used. To this
		 * end, we're going to add a name here and let the user save it
		 * and use it as they see fit. Clearly, this does not have to be
		 * done, but it's a nice touch that will make using the class a
		 * little bit nicer.
		 */
		CKString							mName;
		/*
		 * This is the STL map that holds the variables for the node
		 * each is identified by a CKString and holds a CKVariant
		 * so that it can actually hold almost any data you can think
		 * of. It's very slick. :)
		 */
		std::map<CKString, CKVariant>		mVars;
		/*
		 * This list holds all the children of this node and there will
		 * be plenty of helper methods to make it easy to get nodes in
		 * and out of this structure.
		 */
		std::list<CKDataNode*>				mKids;
		/*
		 * These mutexes are used to ensure that we're not corrupting the
		 * data as it's being put into the underlying structures. This is
		 * really important as we're going to be doing a lot of stuff to
		 * the data and we don't want it shifting out from under us.
		 */
		CKFWMutex							mVarsMutex;
		CKFWMutex							mKidsMutex;
};

/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKDataNode & aNode );

#endif	// __CKDATANODE_H
