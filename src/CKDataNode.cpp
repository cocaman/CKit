/*
 * CKDataNode.cpp - this file implements a class that can be used to represent
 *                  a general tree of data where each node contains a map of
 *                  key/value pairs where the key is a CKString (name) and
 *                  the value is a CKVariant that can hold almost anything
 *                  you need to hold. In addition to the data, this node has
 *                  a list of children nodes (pointers to CKDataNodes) and a
 *                  pointer to a parent node. This means that this guy can
 *                  be the basis of a complete tree of data and this is
 *                  very important to many applications.
 *
 * $Id: CKDataNode.cpp,v 1.27 2008/01/14 21:47:10 drbob Exp $
 */

//	System Headers
#include <sstream>
#include <strings.h>
#include <stdio.h>

//	Third-Party Headers

//	Other Headers
#include "CKDataNode.h"
#include "CKException.h"

//	Forward Declarations

//	Public Constants

//	Public Datatypes

//	Public Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that creates a nice, empty
 * node ready for the user to dump data into as needed.
 */
CKDataNode::CKDataNode() :
	mParent(NULL),
	mName(),
	mVars(),
	mKids(),
	mVarsMutex(),
	mKidsMutex()
{
}


/*
 * This constructor creates a new node and sets the reference to
 * the node's parent to the supplied pointer. This is OK because
 * each node *never* controls the memory of it's parent.
 */
CKDataNode::CKDataNode( CKDataNode *aParent ) :
	mParent(NULL),
	mName(),
	mVars(),
	mKids(),
	mVarsMutex(),
	mKidsMutex()
{
	// first, set our pointer to him (the parent)...
	mParent = aParent;
	// ...and then add ourselves to him as a new child node
	aParent->mKidsMutex.lock();
	aParent->mKids.addToEnd(this);
	aParent->mKidsMutex.unlock();
}


/*
 * This constructor makes a node with the given parent and the
 * identifying name provided as well. This is useful when the
 * structure of a tree is being built and the data will be
 * populated later. The parent node is not controlled by the
 * instance because no parent nodes are controlled by the nodes.
 */
CKDataNode::CKDataNode( CKDataNode *aParent, const CKString & aName ) :
	mParent(NULL),
	mName(),
	mVars(),
	mKids(),
	mVarsMutex(),
	mKidsMutex()
{
	// first, set our pointer to him (the parent)...
	mParent = aParent;
	// ...and then add ourselves to him as a new child node
	aParent->mKidsMutex.lock();
	aParent->mKids.addToEnd(this);
	aParent->mKidsMutex.unlock();
	// we also need to set our name
	mName = aName;
}


/*
 * This constructor creates a node with the given parent node
 * reference as well as the provided identifying name and also
 * sets one key/value pair in the node. This can be really helpful
 * in setting up a grouping with time-series data, for instance.
 * the name might be the symbol's name, the key might be "price"
 * and the value could be a variant time-series of the price
 * data. This would make creating a tree of nodes very easy.
 */
CKDataNode::CKDataNode( CKDataNode *aParent, const CKString & aName,
						const CKString & aKey, const CKVariant & aValue ) :
	mParent(NULL),
	mName(),
	mVars(),
	mKids(),
	mVarsMutex(),
	mKidsMutex()
{
	// first, set our pointer to him (the parent)...
	mParent = aParent;
	// ...and then add ourselves to him as a new child node
	aParent->mKidsMutex.lock();
	aParent->mKids.addToEnd(this);
	aParent->mKidsMutex.unlock();
	// we also need to set our name
	mName = aName;
	// finally, add the variable to the map
	mVars[aKey] = aValue;
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around. It's important to note that it's a SHALLOW COPY and the
 * pointers to the children of the original node are simply copied
 * and therefore point to the exact same instances in the copy as
 * the original. This is equivalent to the newNodeByShallowCopy()
 * static method.
 */
CKDataNode::CKDataNode( const CKDataNode & anOther ) :
	mParent(NULL),
	mName(),
	mVars(),
	mKids(),
	mVarsMutex(),
	mKidsMutex()
{
	// let the operator=() take care of all the details
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called. It's important to note that this is a SHALLOW delete
 * and none of the children nodes will be deleted. This is the
 * same as the deleteNodeShallow() call, and care needs to be
 * taken with the children to make sure they don't end up as a
 * memory leak.
 */
CKDataNode::~CKDataNode()
{
	/*
	 * First thing to do is to remove ourselves from the tree
	 */
	// if we have a parent, have him remove us as a child
	if (mParent != NULL) {
		// remove me as a child from my parent
		mParent->mKidsMutex.lock();
		mParent->mKids.remove(this);
		mParent->mKidsMutex.unlock();
		// ...and NULL the link out to break it
		mParent = NULL;
	}

	// for each child, remove me as it's parent
	for (int i = 0; i < mKids.size(); i++) {
		// if we're his parent, then invalidate that link
		if ((mKids[i] != NULL) && (mKids[i]->mParent = this)) {
			mKids[i]->mParent = NULL;
		}
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes. This is a SHALLOW copy and as such
 * the only the values in this instance are copied and NOT what
 * the references point to. This is very dangerous as this
 * instance will have the pointers to the children, but those
 * children will still point to the original node as their parent.
 */
CKDataNode & CKDataNode::operator=( const CKDataNode & anOther )
{
	// this is a SHALLOW copy, so simply grab the values
	mParent = anOther.mParent;
	mName = anOther.mName;

	mVarsMutex.lock();
	mVars = anOther.mVars;
	mVarsMutex.unlock();

	mKidsMutex.lock();
	mKids = anOther.mKids;
	mKidsMutex.unlock();

	return *this;
}


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
void CKDataNode::setParent( CKDataNode *aNode )
{
	// first, see if we have anything to do
	if (mParent != aNode) {
		// see if we have a parent node to unlink
		if (mParent != NULL) {
			// remove me from the parent as a child
			mParent->mKidsMutex.lock();
			mParent->mKids.remove(this);
			mParent->mKidsMutex.unlock();
		}

		// next, set this guy as my parent
		mParent = aNode;

		// finally, add me as a child to this new parent
		if (aNode != NULL) {
			// add me as a child to this new parent
			aNode->addChild(this);
		}
	}
}


/*
 * This method sets the identifying name for this node. This
 * is useful because most data sets (trees) have an identifying
 * name for each data point.
 */
void CKDataNode::setName( const CKString & aName )
{
	mName = aName;
}


/*
 * This method returns the pointer that is the actual reference
 * to the parent node of this node. If this node happens to be
 * a root node, this will return NULL indicating that it's a
 * root.
 */
CKDataNode *CKDataNode::getParent() const
{
	return mParent;
}


/*
 * This method returns the identifying name of this node. In most
 * data structures it makes sense to name the nodes and this is
 * the way to see what this node's name is.
 */
CKString CKDataNode::getName() const
{
	return mName;
}


/*
 * Each node can have many variables (attributes) stored in a map
 * as a CKString name and CKVariant value. This method returns a
 * pointer to the actual named value so if you want to do something
 * with it, make a copy. If there is no variable with this name the
 * method will return a NULL.
 */
CKVariant *CKDataNode::getVar( const CKString & aName )
{
	CKVariant		*retval = NULL;

	// make sure we do this in a thread-safe manner
	mVarsMutex.lock();
	// now look for the entry
	if (!mVars.empty()) {
		std::map<CKString, CKVariant>::iterator	i = mVars.find(aName);
		if (i != mVars.end()) {
			retval = &((*i).second);
		}
	}
	// now unlock the map
	mVarsMutex.unlock();

	return retval;
}


/*
 * Each node can have many variables (attributes) stored in a map
 * as a CKString name and CKVariant value. This method places a
 * value into that map for this instance at the name provided.
 */
void CKDataNode::putVar( const CKString & aName, const CKVariant & aValue )
{
	// make sure we do this in a thread-safe manner
	mVarsMutex.lock();
	// ...save the entry
	mVars[aName] = aValue;
	// now unlock the map
	mVarsMutex.unlock();
}


/*
 * Since each node can hold many variables (attributes), it's
 * sometimes necessary to clean out the old ones. This method
 * removes the named variable from this node if it exists. If the
 * flag indicates that this is to be "deep" (recursive), then
 * this variable will be removed from all the child nodes as well.
 */
void CKDataNode::removeVar( const CKString & aName, bool aDeepFlag )
{
	bool		error = false;

	/*
	 * First, remove the variable name from this instance and
	 * do it in a thread-safe manner.
	 */
	if (!error) {
		// lock up the variable list
		CKStackLocker	lockem(&mVarsMutex);
		// ...erase all entries with this name (only one possible)
		if (!mVars.empty()) {
			std::map<CKString, CKVariant>::iterator	i = mVars.find(aName);
			if (i != mVars.end()) {
				mVars.erase(i);
			}
		}
	}

	/*
	 * Next, if the user wants us to delete this from all the kids, we
	 * need to do that as well.
	 */
	if (!error && aDeepFlag) {
		// lock up the list of kids for this
		CKStackLocker	lockem(&mKidsMutex);
		// see if we have any at all
		if (!mKids.empty()) {
			for (int i = 0; i < mKids.size(); i++) {
				if (mKids[i] == NULL) {
					error = true;
					std::ostringstream	msg;
					msg << "CKDataNode::removeVar(const CKString &, bool) - the "
						"child of '" << getName() << "' was not supposed to be NULL "
						"yet there is a NULL. Please check on this data corruption "
						"problem as soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					mKids[i]->removeVar(aName, aDeepFlag);
				}
			}
		}
	}
}


/*
 * When you want to clear out all the variables (attributes) from
 * this node, call this method and the entire map of variables will
 * be cleared out. It's non-reversable, so be carful with this. If
 * the flag indicates that this is to be "deep" (recursive), then
 * all variables will be removed from all child nodes as well.
 */
void CKDataNode::clearVars( bool aDeepFlag )
{
	bool		error = false;

	/*
	 * First, remove all the variables from this instance and
	 * do it in a thread-safe manner.
	 */
	if (!error) {
		// lock up the variable list
		CKStackLocker	lockem(&mVarsMutex);
		// ...erase all entries - period.
		if (!mVars.empty()) {
			mVars.clear();
		}
	}

	/*
	 * Next, if the user wants us to clear the variables from all the
	 * kids, we need to do that as well.
	 */
	if (!error && aDeepFlag) {
		// lock up the list of kids for this
		CKStackLocker	lockem(&mKidsMutex);
		// see if we have any at all
		if (!mKids.empty()) {
			for (int i = 0; i < mKids.size(); i++) {
				if (mKids[i] == NULL) {
					error = true;
					std::ostringstream	msg;
					msg << "CKDataNode::clearVars(bool) - the child of '" <<
						getName() << "' was not supposed to be NULL yet there is "
						"a NULL. Please check on this data corruption problem as "
						"soon as possible.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					mKids[i]->clearVars(aDeepFlag);
				}
			}
		}
	}
}


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
void CKDataNode::addChild( CKDataNode *aNode )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (aNode == NULL) {
			error = true;
			// no need to throw an exception, just ignore it
		}
	}

	// now see if we are this node's parent
	if (!error) {
		// first thing is to make me it's parent
		aNode->setParent(this);

		/*
		 * Next, verify that it's in the list of children on this
		 * node. If not, then add it to the list.
		 */
		// gotta be thread-safe on this guy
		mKidsMutex.lock();
		// look for first occurrence
		if (!mKids.contains(aNode)) {
			mKids.addToEnd(aNode);
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}
}


/*
 * This method removes the provided node from the list of child
 * nodes of this node - if it's actually in the list. If it's
 * being removed this method will also instruct the now ex-child
 * node to NULL out it's parent node so that there's no confusion
 * on the issue.
 */
void CKDataNode::removeChild( const CKDataNode *aNode )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (aNode == NULL) {
			error = true;
			// no need to throw an exception, just ignore it
		}
	}

	// now see if we have this node as a child node
	if (!error) {
		if (aNode->getParent() == this) {
			((CKDataNode*)aNode)->setParent(NULL);
		}
	}
}


/*
 * This method returns the complete list (as a vector) of all the
 * child nodes identifying names. This is a very handy way to
 * iterate over the children assuming they all have distinct names
 * as would be the case in most data sets.
 */
CKStringList CKDataNode::getChildNames()
{
	CKStringList	retval;

	// lock up the list to be safe
	mKidsMutex.lock();
	// go through all children and add their names to the vector
	for (int i = 0; i < mKids.size(); i++) {
		if (mKids[i] != NULL) {
			retval.addToEnd(mKids[i]->mName);
		}
	}
	// now unlock the list of kids for modification
	mKidsMutex.unlock();

	return retval;
}


/*
 * This method returns the pointer to the actual node that is
 * both a child of this node and has the identifying name that
 * is the same as that passed into this method. This is the actual
 * pointer, so if you want to do anything with it, you need to
 * make a copy.
 */
CKDataNode *CKDataNode::findChild( const CKString & aName )
{
	CKDataNode		*retval = NULL;

	// lock up the list to be safe
	mKidsMutex.lock();
	// go through all children and look to their names for the right one
	for (int i = 0; i < mKids.size(); i++) {
		if ((mKids[i] != NULL) && (mKids[i]->mName == aName)) {
			retval = mKids[i];
			break;
		}
	}
	// now unlock the list of kids for modification
	mKidsMutex.unlock();

	return retval;
}


/*
 * This method returns the number of child nodes this node has. This
 * is useful for a lot of things, and among them is the core of the
 * isLeaf() method, below.
 */
int CKDataNode::getChildCount()
{
	int		retval = 0;

	// lock up the list to be safe
	mKidsMutex.lock();
	// count up all the children
	if (!mKids.empty()) {
		retval = (int) mKids.size();
	}
	// now unlock the list of kids for modification
	mKidsMutex.unlock();

	return retval;
}


/*
 * This method will return true if there are no child nodes attached
 * to this node. This then, would make this node a leaf node in the
 * tree.
 */
bool CKDataNode::isLeaf()
{
	bool		retval = false;

	// lock up the list to be safe
	mKidsMutex.lock();
	// count up all the children
	if (!mKids.empty()) {
		retval = true;
	}
	// now unlock the list of kids for modification
	mKidsMutex.unlock();

	return retval;
}


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
CKVariant *CKDataNode::getVarAtPath( const CKString & aPath )
{
	bool		error = false;
	CKVariant	*retval = NULL;

	// first, see if we need to start at the root
	CKDataNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = node->mParent;
			}
		}
	}

	// next, turn the path into a vector of strings
	CKStringList	steps;
	int				stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getVarAtPath(const CKString &) - the path "
				"had insufficient steps to create a valid path. Please make sure "
				"that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			retval = node->getVarAtPath(steps);
		}
	}

	return retval;
}


/*
 * This method takes a vector of strings as the path as opposed
 * to a single string delimited with '/'. This makes it a little
 * easier if you're building up the path by data elements and
 * you don't really want to make a single string just to store
 * the value.
 */
CKVariant *CKDataNode::getVarAtPath( const CKStringList & aSteps )
{
	bool		error = false;
	CKVariant	*retval = NULL;

	// start right where we are now
	CKDataNode	*node = this;

	// travel down the path of the tree to the right node
	int		stepCnt = aSteps.size();
	if (!error) {
		// the last element of the path is the variable name
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getVarAtPath(const CKStringList &) "
				"- the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < (stepCnt - 1)); step++) {
				CKDataNode	*next = node->findChild(aSteps[step]);
				if (next == NULL) {
					error = true;
					// don't exception, just return NULL
					break;
				} else {
					// move down to the child
					node = next;
				}
			}
		}
	}

	// now look into the node for the variable we're looking for
	if (!error) {
		retval = node->getVar(aSteps[stepCnt - 1]);
	}

	return retval;
}


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
void CKDataNode::putVarAtPath( const CKString & aPath, const CKVariant & aValue )
{
	bool		error = false;

	// first, see if we need to start at the root
	CKDataNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = node->mParent;
			}
		}
	}

	// next, turn the path into a vector of strings
	CKStringList	steps;
	int				stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::putVarAtPath(const CKString &, const "
				"CKVariant &) - the path had insufficient steps to create a "
				"valid path. Please make sure that a valid path is passed to "
				"this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			node->putVarAtPath(steps, aValue);
		}
	}
}


/*
 * This version of the method takes a vector of strings that is
 * the path as opposed to a single string delimited by the '/'.
 * This is useful when you have the data organized in something
 * like a vector and you don't want to put it all together only
 * to have this method break it up.
 */
void CKDataNode::putVarAtPath( const CKStringList & aSteps,
							   const CKVariant & aValue )
{
	bool		error = false;

	// start right where we are now
	CKDataNode	*node = this;

	// move down the path step by step...
	int		stepCnt = aSteps.size();
	if (!error) {
		// the last element of the path is the variable name
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::putVarAtPath(const CKStringList &, "
				"const CKVariant &) - the path had insufficient steps to create "
				"a valid path. Please make sure that a valid path is passed to "
				"this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < (stepCnt - 1)); step++) {
				CKDataNode	*next = node->findChild(aSteps[step]);
				if (next == NULL) {
					/*
					 * With no child of the right name, we need to create
					 * one and place it in the tree so that we can continue.
					 * This is one very fast way to build the tree of data.
					 */
					next = new CKDataNode(node, aSteps[step]);
					if (next == NULL) {
						error = true;
						std::ostringstream	msg;
						msg << "CKDataNode::putVarAtPath(const "
							"CKStringList &, const CKVariant &) - "
							"while trying to extend the tree to include "
							"the step '" << aSteps[step] << "' an error occurred "
							"and that step could not be created. Please check the "
							"logs but this is likely an allocation error.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						// add this guy as a child of this node
						node->addChild(next);
						// ...and move into him for the next step
						node = next;
					}
				} else {
					// move down to the child
					node = next;
				}
			}
		}
	}

	// now put into the node's variables the variable we were given
	if (!error) {
		node->putVar(aSteps[stepCnt - 1], aValue);
	}
}


/*
 * This method takes a string that is a series of node identifying
 * names spearated by a '/' to represent a "path" to a node in the
 * data tree.
 *
 * For example, say the path was:
 *
 *     SectionA/Subsection1/GroupQ/Item212
 *
 * This method would look for the child identified by the name
 * "SectionA" and then have it look for a child called "Subsection1"
 * and it's child called "GroupQ" and it's child called "Item212".
 * What would be returned is the pointer to the "Item212" child.
 *
 * It's important to note that if any node in the path is NOT FOUND
 * then this method returns a NULL.
 *
 * If the path string includes a leading '/' then the path is
 * taken from the root node of the tree that this node is but a
 * part of. So, even if this node is *not* in the path, the value
 * will be returned if it's in the tree.
 */
CKDataNode *CKDataNode::getNodeAtPath( const CKString & aPath )
{
	bool		error = false;
	CKDataNode	*retval = NULL;

	// first, see if we need to start at the root
	CKDataNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = node->mParent;
			}
		}
	}

	// next, turn the path into a vector of strings
	CKStringList	steps;
	int				stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getNodeAtPath(const CKString &) - the path "
				"had insufficient steps to create a valid path. Please make sure "
				"that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			retval = node->getNodeAtPath(steps);
		}
	}

	return retval;
}


/*
 * This version of the method takes a vector of strings that is
 * the path as opposed to a single string delimited by the '/'.
 * This is useful when you have the data organized in something
 * like a vector and you don't want to put it all together only
 * to have this method break it up.
 */
CKDataNode *CKDataNode::getNodeAtPath( const CKStringList & aSteps )
{
	bool		error = false;
	CKDataNode	*retval = NULL;

	// start right where we are now
	CKDataNode	*node = this;

	// travel down the path of the tree to the right node
	int		stepCnt = aSteps.size();
	if (!error) {
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getNodeAtPath(const CKStringList &) "
				"- the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < stepCnt); step++) {
				CKDataNode	*next = node->findChild(aSteps[step]);
				if (next == NULL) {
					error = true;
					// don't exception, just return NULL
					break;
				} else {
					// move down to the child
					node = next;
				}
			}
		}
	}

	// if we're here, then the last node is the one we want
	if (!error) {
		retval = node;
	}

	return retval;
}


/*
 * This method takes a string that is a series of node identifying
 * names spearated by a '/' to represent a "path" to a node in the
 * data tree.
 *
 * For example, say the path was:
 *
 *     SectionA/Subsection1/GroupQ/Item212
 *
 * This method would look for the child identified by the name
 * "SectionA" and then have it look for a child called "Subsection1"
 * and it's child called "GroupQ" and it's child called "Item212".
 * What would be returned is the pointer to the "Item212" child.
 *
 * It's important to note that if any node in the path is NOT FOUND
 * then this method will *create* that node so that the path will
 * exist when complete unless a serious error occurs.
 *
 * If the path string includes a leading '/' then the path is
 * taken from the root node of the tree that this node is but a
 * part of. So, even if this node is *not* in the path, the value
 * will be returned if it's in the tree.
 */
CKDataNode *CKDataNode::getOrCreateNodeAtPath( const CKString & aPath )
{
	bool		error = false;
	CKDataNode	*retval = NULL;

	// first, see if we need to start at the root
	CKDataNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = node->mParent;
			}
		}
	}

	// next, turn the path into a vector of strings
	CKStringList	steps;
	int				stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getOrCreateNodeAtPath(const CKString &) - the path "
				"had insufficient steps to create a valid path. Please make sure "
				"that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			retval = node->getOrCreateNodeAtPath(steps);
		}
	}

	return retval;
}


/*
 * This version of the method takes a vector of strings that is
 * the path as opposed to a single string delimited by the '/'.
 * This is useful when you have the data organized in something
 * like a vector and you don't want to put it all together only
 * to have this method break it up.
 */
CKDataNode *CKDataNode::getOrCreateNodeAtPath( const CKStringList & aSteps )
{
	bool		error = false;

	// start right where we are now
	CKDataNode	*node = this;

	// move down the path step by step...
	int		stepCnt = aSteps.size();
	if (!error) {
		// the last element of the path is the variable name
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getOrCreateNodeAtPath(const CKStringList &) - "
				"the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < stepCnt); step++) {
				CKDataNode	*next = node->findChild(aSteps[step]);
				if (next == NULL) {
					/*
					 * With no child of the right name, we need to create
					 * one and place it in the tree so that we can continue.
					 * This is one very fast way to build the tree of data.
					 */
					next = new CKDataNode(node, aSteps[step]);
					if (next == NULL) {
						error = true;
						std::ostringstream	msg;
						msg << "CKDataNode::getOrCreateNodeAtPath(const CKStringList &) "
							"- while trying to extend the tree to include the step '"
							<< aSteps[step] << "' an error occurred and that step "
							"could not be created. Please check the logs but this "
							"is likely an allocation error.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						// add this guy as a child of this node
						node->addChild(next);
						// ...and move into him for the next step
						node = next;
					}
				} else {
					// move down to the child
					node = next;
				}
			}
		}
	}

	return node;
}


/*
 * This method takes a string that is a series of node identifying
 * names sparated by a '/' to represent a "path" to a node in the
 * data tree.
 *
 * For example, say the path was:
 *
 *     SectionA/Subsection1/GroupQ/Item212
 *
 * This method would look for the child identified by the name
 * "SectionA" and then have it look for a child called "Subsection1"
 * and it's child called "GroupQ" and it's child called "Item212".
 * At this point, the method will add the supplied node as a child
 * of the "Item212" node.
 *
 * It's important to note that if any node in the path is NOT FOUND
 * then this method will create such nodes as is necessary to make
 * the path.
 *
 * If the path string includes a leading '/' then the path is
 * taken from the root node of the tree that this node is but a
 * part of. So, even if this node is *not* in the path, the node
 * will be placed correctly if the path is in the tree.
 */
void CKDataNode::putNodeAtPath( const CKString & aPath, CKDataNode *aNode )
{
	bool		error = false;

	// first, see if we need to start at the root
	CKDataNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = node->mParent;
			}
		}
	}

	// next, turn the path into a vector of strings
	CKStringList	steps;
	int				stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::putNodeAtPath(const CKString &, CKDataNode *) - "
				"the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			node->putNodeAtPath(steps, aNode);
		}
	}
}


/*
 * This version of the method takes a vector of strings that is
 * the path as opposed to a single string delimited by the '/'.
 * This is useful when you have the data organized in something
 * like a vector and you don't want to put it all together only
 * to have this method break it up.
 */
void CKDataNode::putNodeAtPath( const CKStringList & aSteps, CKDataNode *aNode )
{
	bool		error = false;

	// start right where we are now
	CKDataNode	*node = this;

	// move down the path step by step...
	int		stepCnt = aSteps.size();
	if (!error) {
		// the last element of the path is the variable name
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::putNodeAtPath(const CKStringList &, CKDataNode *) - "
				"the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < stepCnt); step++) {
				CKDataNode	*next = node->findChild(aSteps[step]);
				if (next == NULL) {
					/*
					 * With no child of the right name, we need to create
					 * one and place it in the tree so that we can continue.
					 * This is one very fast way to build the tree of data.
					 */
					next = new CKDataNode(node, aSteps[step]);
					if (next == NULL) {
						error = true;
						std::ostringstream	msg;
						msg << "CKDataNode::putNodeAtPath(const CKStringList &, "
							"CKDataNode *) - while trying to extend the tree to "
							"include the step '" << aSteps[step] << "' an error "
							"occurred and that step could not be created. Please "
							"check the logs but this is likely an allocation error.";
						throw CKException(__FILE__, __LINE__, msg.str());
					} else {
						// add this guy as a child of this node
						node->addChild(next);
						// ...and move into him for the next step
						node = next;
					}
				} else {
					// move down to the child
					node = next;
				}
			}
		}
	}

	// if we're here, then we need to add this node as a child of last node
	if (!error) {
		node->addChild(aNode);
	}
}


/*
 * This method returns a vector of the node identifiers in this
 * tree leading to the current node. This is basically walking
 * 'up' the tree to the root, building accumulating the steps
 * along the way.
 */
CKStringList CKDataNode::getSteps() const
{
	CKStringList	retval;

	// malk up the tree inserting names at the front
	retval.addToEnd(mName);
	CKDataNode *n = mParent;
	while (n != NULL) {
		/*
		 * Insert the node names at the beginning of the list up to
		 * the point where we're at the root node. If we're at the
		 * root node, and the name is blank, then don't put it in
		 * the path - it's not necessary in that case.
		 */
		if ((n->mParent != NULL) || (n->mName != "")) {
			retval.addToFront(n->mName);
		}
		// ...and move to the parent of this node
		n = n->mParent;
	}

	return retval;
}


/*
 * This method returns a string path to the current node in a
 * very similar way to the getSteps() method. The path lets the
 * caller know where in this tree this particular node lies.
 */
CKString CKDataNode::getPath() const
{
	CKString		retval;

	// first, get the step in the path
	CKStringList	steps = getSteps();
	if (steps.size() > 0) {
		// now convert it to a path with proper escaping
		retval = stepsToPath(steps);
	}

	return retval;
}


/*
 * This method is very nice in that it takes a single string that
 * represents a path and breaks it up into it's components, placing
 * each in the returned vector in the proper order. Leading and
 * trailing '/' characters are removed and any component in the
 * path escaped by double-quotes will be kept intact. This is the
 * way for a component of the path to include a '/' character.
 */
CKStringList CKDataNode::pathToSteps( const CKString & aPath )
{
	bool			error = false;
	CKStringList	retval;
	bool			done = false;

	// first, strip any leading or trailing '/' characters
	CKString		cleanPath = aPath;
	if (!error && !done) {
		// see if there's a leading '/'
		if (cleanPath[0] == '/') {
			cleanPath.erase(0, 1);
		}
		// see if there's a trailing '/'
		int		eos = cleanPath.size() - 1;
		if (cleanPath[eos] == '/') {
			cleanPath.erase(eos, 1);
		}
	}

	// next, convert the path into a series of raw steps
	CKStringList	raw;
	int				rawCnt = 0;
	if (!error && !done) {
		raw = CKStringList::parseIntoChunks(cleanPath, "/");
		rawCnt = raw.size();
		if (rawCnt < 1) {
			done = true;
		}
	}

	// now go through the list of raw elements and correct for any escapes
	if (!error && !done) {
		for (int i = 0; i < rawCnt; i++) {
			if (raw[i][0] == '"') {
				/*
				 * OK, it's escaped, so find the next one that *ends* in a
				 * '/' and piece them all together.
				 */
				// get the start of the escaped path step
				CKString		comp = raw[i];
				// trim off the leading '"'
				comp.erase(0,1);
				// now loop on the raw steps until we find a matching bookend
				for (++i; i < rawCnt; i++) {
					// add in the '/' and the component
					comp.append(1, '/');
					comp.append(raw[i]);
					// does this end in a '"'?
					if (raw[i][raw[i].size() - 1] == '"') {
						comp.erase((comp.size() - 1), 1);
						break;
					}
				}
				// finally, add it to the path steps
				retval.addToEnd(comp);
			} else {
				// not escaped, so just add it
				retval.addToEnd(raw[i]);
			}
		}
	}

	return retval;
}


/*
 * This method is useful in that it takes a vector of path steps,
 * or components, and then assembles them into a single string
 * that is properly escaped for the presence of '/' characters in
 * any one of the steps.
 */
CKString CKDataNode::stepsToPath( const CKStringList & aPath )
{
	CKString		retval;

	// loop over all the elements adding each as necessary
	CKStringNode		*i = NULL;
	for (i = aPath.getHead(); i != NULL; i = i->getNext()) {
		// see if we need a delimiter between this and the next step
		if (i->getPrev() != NULL) {
			retval.append(1, '/');
		}

		// now see if the step needs to be escaped due to a slash
		if (i->find('/') != -1) {
			// escape this guy as it's got a '/' in it
			retval.append(1, '"');
			retval.append(*i);
			retval.append(1, '"');
		} else {
			retval.append(*i);
		}
	}

	return retval;
}


/*
 * There are times that we want to know the identifying names of
 * all the leaf nodes of a certain section of the tree. This is
 * really useful when we need to gather data from an external
 * source, and need a list of names to request that data for. This
 * method does a great job of getting a unique vector of names
 * of all the leaf nodes under it, and all it's children.
 */
CKStringList CKDataNode::getUniqueLeafNodeNames()
{
	bool			error = false;
	CKStringList	retval;

	// OK, we need to look and see if we are a leaf node
	if (!error) {
		// lock up the list of kids for this
		mKidsMutex.lock();
		// see if we have any at all
		if (!mKids.empty()) {
			// ask all the kids for their unique leaf node names
			for (int i = 0; i < mKids.size(); i++) {
				CKStringList	part = mKids[i]->getUniqueLeafNodeNames();
				if (part.size() < 1) {
					// unlock the list of kids
					mKidsMutex.unlock();
					// flag the error and throw the exception
					error = true;
					std::ostringstream	msg;
					msg << "CKDataNode::getUniqueLeafNodeNames() - the node '" <<
						mKids[i]->mName << "' (a child of '" << mName << "') had no "
						"leaf nodes under it. This is simply not possible. It's "
						"likely that there's a data corruption problem. Check "
						"on it.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					/*
					 * OK... we have some leaf nodes from the child.
					 * Let's add them to our list if they are unique.
					 */
					CKStringNode	*c = NULL;
					for (c = part.getHead(); c != NULL; c = c->getNext()) {
						if (!retval.contains(*c)) {
							retval.addToEnd(*c);
						}
					}
				}
			}
		} else {
			/*
			 * We are a leaf node! Then put our name in the vector
			 * and that's all we have to do.
			 */
			retval.addToEnd(mName);
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}

	return retval;
}


/*
 * This method is interesting in that it will return the list of
 * unique leaf node names that are *missing* the provided variable
 * name in their list of variables. This is really nice in that
 * it allows us to ask the question: Who needs 'price'? and have
 * a list of node names that is returned.
 */
CKStringList CKDataNode::getUniqueLeafNodeNamesWithoutVar( const CKString & aVarName )
{
	bool			error = false;
	CKStringList	retval;

	// OK, we need to look and see if we are a leaf node
	if (!error) {
		// lock up the list of kids for this
		mKidsMutex.lock();
		// see if we have any at all
		if (!mKids.empty()) {
			// ask all the kids for their unique leaf node names
			for (int i = 0; i < mKids.size(); i++) {
				CKStringList	part = mKids[i]->getUniqueLeafNodeNamesWithoutVar(aVarName);
				if (part.size() >= 1) {
					/*
					 * OK... we have some leaf nodes that have this
					 * variable. Let's add them to our list if they
					 * are unique.
					 */
					CKStringNode	*c = NULL;
					for (c = part.getHead(); c != NULL; c = c->getNext()) {
						if (!retval.contains(*c)) {
							retval.addToEnd(*c);
						}
					}
				}
			}
		} else {
			/*
			 * We are a leaf node! Put our name in the list ONLY if
			 * we DO NOT have the variable named 'aVarName' in our list.
			 */
			if (getVar(aVarName) == NULL) {
				retval.addToEnd(mName);
			}
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}

	return retval;
}


/*
 * This method is interesting in that it will return the list of
 * unique leaf node names that contain the provided variable
 * name in their list of variables. This is really nice in that
 * it allows us to ask the question: Who has 'price'? and have
 * a list of node names that is returned.
 */
CKStringList CKDataNode::getUniqueLeafNodeNamesWithVar( const CKString & aVarName )
{
	bool			error = false;
	CKStringList	retval;

	// OK, we need to look and see if we are a leaf node
	if (!error) {
		// lock up the list of kids for this
		mKidsMutex.lock();
		// see if we have any at all
		if (!mKids.empty()) {
			// ask all the kids for their unique leaf node names
			for (int i = 0; mKids.size(); i++) {
				CKStringList	part = mKids[i]->getUniqueLeafNodeNamesWithVar(aVarName);
				if (part.size() >= 1) {
					/*
					 * OK... we have some leaf nodes that have this
					 * variable. Let's add them to our list if they
					 * are unique.
					 */
					CKStringNode	*c = NULL;
					for (c = part.getHead(); c != NULL; c = c->getNext()) {
						if (!retval.contains(*c)) {
							retval.addToEnd(*c);
						}
					}
				}
			}
		} else {
			/*
			 * We are a leaf node! Put our name in the list ONLY if
			 * we have the variable named 'aVarName' in our list.
			 */
			if (getVar(aVarName) != NULL) {
				retval.addToEnd(mName);
			}
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}

	return retval;
}


/*
 * This method will return the number of steps that need to be
 * taken from this node to a leaf node. For example, if this node
 * contained a child node that also contained a child node, then
 * this method would return 2. If this node was a leaf node, then
 * this method would return 0.
 */
int CKDataNode::getNumOfStepsToLeaf()
{
	bool		error = false;
	int			retval = 0;

	// we need to recursively count the number of first child nodes
	if (!error) {
		// lock up the list of kids for this
		mKidsMutex.lock();
		// see if we have any at all
		if (!mKids.empty()) {
			// ask all the kids for their unique leaf node names
			if (mKids[0] != NULL) {
				retval = mKids[0]->getNumOfStepsToLeaf() + 1;
			}
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}

	return retval;
}


/********************************************************
 *
 *               Accessor Convenience Methods
 *
 ********************************************************/
/*
 * It's nice to be able to have the level of control that the
 * basic getVarAtPath() methods provide, but some times you
 * just want to use the variant anyplace in the code, and to
 * do that easily, we need a reference and a simplified way
 * of getting at the data.
 *
 * This overloads the '[]' operator so that a simple path can
 * be specified on the node and a CKVariant reference will be
 * returned. The catch is that when there's no element at
 * that path, an empty CKVariant will be created at that path
 * and that's what will be returned. This basically makes sure
 * that the tree fills out as it's accessed, and that you can
 * then use this anywhere a CKVariant can be used.
 */
CKVariant & CKDataNode::operator[]( const CKString & aPath )
{
	// try to get the pointer to the value at the path
	CKVariant	*retval = getVarAtPath(aPath);
	if (retval == NULL) {
		// not there... so add in an empty value
		putVarAtPath(aPath, CKVariant());
		// ...and get it's pointer after insertion
		retval = getVarAtPath(aPath);
	}

	return *retval;
}


CKVariant & CKDataNode::operator[]( const CKStringList & aSteps )
{
	// try to get the pointer to the value at the path
	CKVariant	*retval = getVarAtPath(aSteps);
	if (retval == NULL) {
		// not there... so add in an empty value
		putVarAtPath(aSteps, CKVariant());
		// ...and get it's pointer after insertion
		retval = getVarAtPath(aSteps);
	}

	return *retval;
}


/*
 * This method will return 'true' if the path provided exists
 * and references a valid CKVariant. This means that the path
 * must point to a leaf node - not a tree node, and that there
 * needs to be a value at that location. If there is, then this
 * guy returns true, false otherwise.
 *
 * This is a nice and simple way to see if the value exists
 * without having to get it and check it for NULL, etc.
 */
bool CKDataNode::hasValue( const CKString & aPath )
{
	return (getVarAtPath(aPath) != NULL);
}


bool CKDataNode::hasValue( const CKStringList & aSteps )
{
	return (getVarAtPath(aSteps) != NULL);
}


/*
 * This method returns the type of the value at the path
 * specified. If there is no value there, eUnknownVariant
 * will be returned. This can also be returned if the value
 * at the path is actually an empty variant, but that's
 * life.
 */
CKVariantType CKDataNode::getType( const CKString & aPath )
{
	CKVariantType	retval = eUnknownVariant;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val != NULL) {
		retval = val->getType();
	}

	return retval;
}


CKVariantType CKDataNode::getType( const CKStringList & aSteps )
{
	CKVariantType	retval = eUnknownVariant;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val != NULL) {
		retval = val->getType();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or non-numeric, then an exception
 * will be thrown. But if it is numeric, then we'll return
 * the value as an integer.
 */
int CKDataNode::getInt( const CKString & aPath )
{
	int		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getInt(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getInt(const CKString &) - the value at the path '"
			<< aPath << "' was not a numeric value, and so we can't get the "
			"integer value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getIntValue();
	}

	return retval;
}


int CKDataNode::getInt( const CKStringList & aSteps )
{
	int		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getInt(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getInt(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a numeric value, and so we can't get the "
			"integer value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getIntValue();
	}

	return retval;
}


/*
 * This method will not thrown an exception in the case of the
 * missing value at the path, or a non-numeric value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
int CKDataNode::getInt( const CKString & aPath, int aDefault )
{
	int		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eNumberVariant)) {
		retval = val->getIntValue();
	}

	return retval;
}


int CKDataNode::getInt( const CKStringList & aSteps, int aDefault )
{
	int		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eNumberVariant)) {
		retval = val->getIntValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or non-numeric, then an exception
 * will be thrown. But if it is numeric, then we'll return
 * the value as a double.
 */
double CKDataNode::getDouble( const CKString & aPath )
{
	double		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDouble(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDouble(const CKString &) - the value at the path '"
			<< aPath << "' was not a numeric value, and so we can't get the "
			"double value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getDoubleValue();
	}

	return retval;
}


double CKDataNode::getDouble( const CKStringList & aSteps )
{
	double		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDouble(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eNumberVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDouble(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a numeric value, and so we can't get the "
			"double value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getDoubleValue();
	}

	return retval;
}


/*
 * This method will not thrown an exception in the case of the
 * missing value at the path, or a non-numeric value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
double CKDataNode::getDouble( const CKString & aPath, double aDefault )
{
	double		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eNumberVariant)) {
		retval = val->getDoubleValue();
	}

	return retval;
}


double CKDataNode::getDouble( const CKStringList & aSteps, double aDefault )
{
	double		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eNumberVariant)) {
		retval = val->getDoubleValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a date, then an exception
 * will be thrown. But if it is a date, then we'll return
 * the value as a long of the format YYYYMMDD.
 */
long CKDataNode::getDate( const CKString & aPath )
{
	long		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDate(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eDateVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDate(const CKString &) - the value at the path '"
			<< aPath << "' was not a date value, and so we can't get the "
			"date value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getDateValue();
	}

	return retval;
}


long CKDataNode::getDate( const CKStringList & aSteps )
{
	long		retval = 0;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDate(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eDateVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getDate(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a date value, and so we can't get the "
			"date value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getDateValue();
	}

	return retval;
}


/*
 * This method will not thrown an exception in the case of the
 * missing value at the path, or not a date value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
long CKDataNode::getDate( const CKString & aPath, long aDefault )
{
	long		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eDateVariant)) {
		retval = val->getDateValue();
	}

	return retval;
}


long CKDataNode::getDate( const CKStringList & aSteps, long aDefault )
{
	long		retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eDateVariant)) {
		retval = val->getDateValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a string, then an exception
 * will be thrown. But if it is a string, then we'll return
 * the the pointer to the actual CKString value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKString *CKDataNode::getString( const CKString & aPath )
{
	const CKString		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getString(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eStringVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getString(const CKString &) - the value at the path '"
			<< aPath << "' was not a string value, and so we can't get the "
			"string value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getStringValue();
	}

	return retval;
}


const CKString *CKDataNode::getString( const CKStringList & aSteps )
{
	const CKString		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getString(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eStringVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getString(const CKString &) - the value at the path '"
			<< aSteps << "' was not a string value, and so we can't get the "
			"string value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getStringValue();
	}

	return retval;
}


/*
 * This method will not thrown an exception in the case of the
 * missing value at the path, or not a string value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKString *CKDataNode::getString( const CKString & aPath, const CKString *aDefault )
{
	const CKString		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eStringVariant)) {
		retval = val->getStringValue();
	}

	return retval;
}


const CKString *CKDataNode::getString( const CKStringList & aSteps, const CKString *aDefault )
{
	const CKString		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eStringVariant)) {
		retval = val->getStringValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a table, then an exception
 * will be thrown. But if it is a table, then we'll return
 * the the pointer to the actual CKTable value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKTable *CKDataNode::getTable( const CKString & aPath )
{
	const CKTable		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTable(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTableVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTable(const CKString &) - the value at the path '"
			<< aPath << "' was not a table value, and so we can't get the "
			"table value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTableValue();
	}

	return retval;
}


const CKTable *CKDataNode::getTable( const CKStringList & aSteps )
{
	const CKTable		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTable(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTableVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTable(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a table value, and so we can't get the "
			"table value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTableValue();
	}

	return retval;
}


/*
 * This method will not throw an exception in the case of the
 * missing value at the path, or not a table value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKTable *CKDataNode::getTable( const CKString & aPath, const CKTable *aDefault )
{
	const CKTable		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eTableVariant)) {
		retval = val->getTableValue();
	}

	return retval;
}


const CKTable *CKDataNode::getTable( const CKStringList & aSteps, const CKTable *aDefault )
{
	const CKTable		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eTableVariant)) {
		retval = val->getTableValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a timeseries, then an exception
 * will be thrown. But if it is a timeseries, then we'll return
 * the the pointer to the actual CKTimeSeries value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKTimeSeries *CKDataNode::getTimeSeries( const CKString & aPath )
{
	const CKTimeSeries		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeSeries(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTimeSeriesVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeSeries(const CKString &) - the value at the path '"
			<< aPath << "' was not a timeseries value, and so we can't get the "
			"timeseries value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTimeSeriesValue();
	}

	return retval;
}


const CKTimeSeries *CKDataNode::getTimeSeries( const CKStringList & aSteps )
{
	const CKTimeSeries		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeSeries(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTimeSeriesVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeSeries(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a timeseries value, and so we can't get the "
			"timeseries value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTimeSeriesValue();
	}

	return retval;
}


/*
 * This method will not throw an exception in the case of the
 * missing value at the path, or not a timeseries value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKTimeSeries *CKDataNode::getTimeSeries( const CKString & aPath, const CKTimeSeries *aDefault )
{
	const CKTimeSeries		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eTimeSeriesVariant)) {
		retval = val->getTimeSeriesValue();
	}

	return retval;
}


const CKTimeSeries *CKDataNode::getTimeSeries( const CKStringList & aSteps, const CKTimeSeries *aDefault )
{
	const CKTimeSeries		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eTimeSeriesVariant)) {
		retval = val->getTimeSeriesValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a price, then an exception
 * will be thrown. But if it is a price, then we'll return
 * the the pointer to the actual CKPrice value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKPrice *CKDataNode::getPrice( const CKString & aPath )
{
	const CKPrice		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getPrice(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != ePriceVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getPrice(const CKString &) - the value at the path '"
			<< aPath << "' was not a price value, and so we can't get the "
			"price value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getPriceValue();
	}

	return retval;
}


const CKPrice *CKDataNode::getPrice( const CKStringList & aSteps )
{
	const CKPrice		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getPrice(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != ePriceVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getPrice(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a price value, and so we can't get the "
			"price value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getPriceValue();
	}

	return retval;
}


/*
 * This method will not throw an exception in the case of the
 * missing value at the path, or not a price value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKPrice *CKDataNode::getPrice( const CKString & aPath, const CKPrice *aDefault )
{
	const CKPrice		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == ePriceVariant)) {
		retval = val->getPriceValue();
	}

	return retval;
}


const CKPrice *CKDataNode::getPrice( const CKStringList & aSteps, const CKPrice *aDefault )
{
	const CKPrice		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == ePriceVariant)) {
		retval = val->getPriceValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a list, then an exception
 * will be thrown. But if it is a list, then we'll return
 * the the pointer to the actual CKVariantList value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKVariantList *CKDataNode::getList( const CKString & aPath )
{
	const CKVariantList		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eListVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKString &) - the value at the path '"
			<< aPath << "' was not a list value, and so we can't get the "
			"list value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getListValue();
	}

	return retval;
}


const CKVariantList *CKDataNode::getList( const CKStringList & aSteps )
{
	const CKVariantList		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eListVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a list value, and so we can't get the "
			"list value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getListValue();
	}

	return retval;
}


/*
 * This method will not throw an exception in the case of the
 * missing value at the path, or not a list value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKVariantList *CKDataNode::getList( const CKString & aPath, const CKVariantList *aDefault )
{
	const CKVariantList		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eListVariant)) {
		retval = val->getListValue();
	}

	return retval;
}


const CKVariantList *CKDataNode::getList( const CKStringList & aSteps, const CKVariantList *aDefault )
{
	const CKVariantList		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eListVariant)) {
		retval = val->getListValue();
	}

	return retval;
}


/*
 * This method looks to the value at the path provided and
 * if it's either not there, or not a time table, then an exception
 * will be thrown. But if it is a time table, then we'll return
 * the the pointer to the actual CKTimeTable value that is in the
 * tree. If you want to keep this value, then you need to make
 * a copy.
 */
const CKTimeTable *CKDataNode::getTimeTable( const CKString & aPath )
{
	const CKTimeTable		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeTable(const CKString &) - "
			"there was no value available at the path '" << aPath
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTimeTableVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKString &) - the value at the path '"
			<< aPath << "' was not a time table value, and so we can't get the "
			"time table value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTimeTableValue();
	}

	return retval;
}


const CKTimeTable *CKDataNode::getTimeTable( const CKStringList & aSteps )
{
	const CKTimeTable		*retval = NULL;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if (val == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNode::getTimeTable(const CKStringList &) - "
			"there was no value available at the path '" << aSteps
			<< "'. Please check that this value exists or use the method "
			"that allows for a default value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else if (val->getType() != eTimeTableVariant) {
		std::ostringstream	msg;
		msg << "CKDataNode::getList(const CKStringList &) - the value at the path '"
			<< aSteps << "' was not a time table value, and so we can't get the "
			"time table value.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		retval = val->getTimeTableValue();
	}

	return retval;
}


/*
 * This method will not throw an exception in the case of the
 * missing value at the path, or not a time table value - rather,
 * it will return the provided default value in both these
 * cases. This means that it's possible to simply not store
 * values equal to the default and use this to get all values
 * out of the tree.
 */
const CKTimeTable *CKDataNode::getTimeTable( const CKString & aPath, const CKTimeTable *aDefault )
{
	const CKTimeTable		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aPath);
	if ((val != NULL) && (val->getType() == eTimeTableVariant)) {
		retval = val->getTimeTableValue();
	}

	return retval;
}


const CKTimeTable *CKDataNode::getTimeTable( const CKStringList & aSteps, const CKTimeTable *aDefault )
{
	const CKTimeTable		*retval = aDefault;

	// try to get the pointer to the value at the path
	CKVariant	*val = getVarAtPath(aSteps);
	if ((val != NULL) && (val->getType() == eTimeTableVariant)) {
		retval = val->getTimeTableValue();
	}

	return retval;
}


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
CKDataNode *CKDataNode::newNodeByShallowCopy( const CKDataNode *aNode )
{
	CKDataNode	*retval = NULL;

	if (aNode != NULL) {
		retval = new CKDataNode(*aNode);
		if (retval == NULL) {
			std::ostringstream	msg;
			msg << "CKDataNode::newNodeByShallowCopy(const CKDataNode*) - "
				"while trying to copy the core node, an allocation error "
				"occurred. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


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
CKDataNode *CKDataNode::newNodeByDeepCopy( const CKDataNode *aNode,
										   const CKDataNode *aParent )
{
	bool		error = false;
	CKDataNode	*retval = NULL;

	// first thing to do is to make a shallow copy of the node
	if (!error) {
		if (aNode != NULL) {
			retval = new CKDataNode(*aNode);
			if (retval == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKDataNode::newNodeByDeepCopy(const CKDataNode*, "
					"const CKDataNode*) - while trying to copy the core node, "
					"an allocation error occurred. Please check into this as "
					"soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} else {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::newNodeByDeepCopy(const CKDataNode*, "
				"const CKDataNode*) - the passed-in node to copy was NULL and "
				"that's not an acceptable input. Please make sure that the node "
				"to copy is not NULL.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	/*
	 * Now we need to be careful... if the user has specified a parent
	 * node then it's probably because they AREN'T calling this method
	 * but it's being called recursively from this method itself. So...
	 * if the parent argument IS NULL, then we need to link this new
	 * copy of the original into the tree off the same parent as the
	 * original. If not, then we just set the parent of the copy to the
	 * argument that's passed to us.
	 */
	if (!error) {
		if (aParent == NULL) {
			// first, tell the parent we're to be a child node
			if (retval->mParent != NULL) {
				retval->mParent->addChild(retval);

				// next, modify the name a bit to make it fit in
				retval->mName = "Copy of " + retval->mName;
			}
		} else {
			// this puts the node in the deep copied tree
			retval->mParent = (CKDataNode*)aParent;
		}
	}

	/*
	 * Now we need to 'replace' each of the children with a deep copy
	 * of each. This will recursively finish the deep copy until we
	 * get to just the leaf nodes and then we'll be done.
	 */
	if (!error) {
		// make a copy of the children we need to copy
		CKVector<CKDataNode*>	oldKids = retval->mKids;
		// ...and clear out the existing list of children
		retval->mKids.clear();
		// now iterate on the list of kids and deep copy each one
		for (int i = 0; oldKids.size(); i++) {
			// copy each one with this guy as it's new parent
			CKDataNode	*n = NULL;
			try {
				n = newNodeByDeepCopy(oldKids[i], retval);
			} catch (CKException & cke) {
				n = NULL;
			}
			if (n == NULL) {
				// flag this as an error
				error = true;
				// clean up all the things we've allocated to this point
				deleteNodeDeep(retval);
				// now log the problem and throw the exception
				std::ostringstream	msg;
				msg << "CKDataNode::newNodeByDeepCopy(const CKDataNode*, "
					"const CKDataNode*) - while trying to copy the children of "
					"the core node, a problem arose. Please check the logs for "
					"a possible cause and try to fix it right away.";
				throw CKException(__FILE__, __LINE__, msg.str());
			} else {
				// OK, it's good... add it to the node as a good kid
				retval->mKids.addToEnd(n);
			}
		}
	}

	return retval;
}


/*
 * This method deletes the passed node but does not delete
 * the parent or ANY of the children nodes. This method
 * simply cleans up the single node itself. It also tells
 * the parent to remove itself so that it's not thinking
 * there's a child where there isn't any longer. The pointer
 * is modified in the process to point to NULL so that it
 * cannot be deleted again.
 */
void CKDataNode::deleteNodeShallow( CKDataNode * & aNode )
{
	if (aNode != NULL) {
		delete aNode;
		aNode = NULL;
	}
}


/*
 * This method deletes this node AND ALL CHILDREN. This is
 * a way to delete an entire tree starting at the root of
 * that tree. It also tells the parent to remove itself so
 * that it's not thinking there's a child where there isn't
 * any longer.  The pointer is modified in the process to
 * point to NULL so that it cannot be deleted again.
 */
void CKDataNode::deleteNodeDeep( CKDataNode * & aNode )
{
	bool		error = false;

	// first, see if we have anything to do
	if (!error) {
		if (aNode == NULL) {
			error = true;
			// no need to log as it's already gone
		}
	}

	// we need to delete all the children - deeply
	if (!error) {
		// lock up the list of kids as it's the target
		aNode->mKidsMutex.lock();
		// iterate on all that is there
		if (!aNode->mKids.empty()) {
			for (int i = 0; i < aNode->mKids.size(); i++) {
				deleteNodeDeep(aNode->mKids[i]);
			}
			// now that they are deleted, clear out the list itself
			aNode->mKids.clear();
		}
		// unlock the list of kids as we're all done
		aNode->mKidsMutex.unlock();
	}

	// finally, delete this node as well
	if (!error) {
		// unlink this node from it's parent to delete cleanly
		aNode->mParent = NULL;
		// ...and now reclaim the space
		delete aNode;
		aNode = NULL;
	}
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method can be called to make sure that the data node tree
 * is not in the middle of a load that could make it's contents
 * unstable. If you ignore this method's return value you do so
 * at your own risk.
 */
bool CKDataNode::isLoadInProgress()
{
	return false;
}


/*
 * This method will allow the caller to wait until the load of this
 * data node tree is done - if it's currently in process. If the
 * data is loaded the this will return immediately, if not, it
 * will wait until the data tree is stable and then return.
 */
void CKDataNode::waitForLoad()
{
	// this guy does nothing now, but a subclass might
}


/*
 * This method checks to see if the two CKDataNodes are equal to one
 * another based on the values they represent and *not* on the actual
 * pointers themselves. If they are equal, then this method returns a
 * value of true, otherwise, it returns a false.
 */
bool CKDataNode::operator==( const CKDataNode & anOther ) const
{
	bool		equal = true;

	// check that the parents are the same - two NULLs match in my opinion
	if (mParent == NULL) {
		if (anOther.mParent != NULL) {
			equal = false;
		}
	} else {
		if (anOther.mParent == NULL) {
			equal = false;
		} else {
			if (mParent != anOther.mParent) {
				equal = false;
			}
		}
	}

	// check the other things that are simple equalities
	if ((mName != anOther.mName) ||
		(mVars != anOther.mVars) ||
		(mKids != anOther.mKids)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKDataNodes are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKDataNode::operator!=( const CKDataNode & anOther ) const
{
	return !(this->operator==(anOther));
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
CKString CKDataNode::toString( bool aDeepFlag ) const
{
	CKString		retval = "(";

	// slap the name of the node out
	retval.append("Name=");
	retval.append(mName);
	retval.append("\n");

	// put the parent's name, if it's available
	retval.append("Parent=");
	if (mParent == NULL) {
		retval.append("<NULL>");
	} else {
		retval.append(mParent->mName);
	}
	retval.append("\n");

	// put in each of the variables on this node
	retval.append("Values:\n");
	std::map<CKString, CKVariant>::const_iterator	i;
	for (i = mVars.begin(); i != mVars.end(); ++i) {
		retval.append("   ");
		retval.append((*i).first);
		retval.append(" : ");
		retval.append((*i).second.toString());
		retval.append("\n");
	}

	// put in the names of each of the children
	retval.append("Children:\n");
	for (int j = 0; j < mKids.size(); j++) {
		retval.append("   ");
		retval.append(mKids[j]->mName);
		if (aDeepFlag) {
			retval.append(mKids[j]->toString(aDeepFlag));
		}
		retval.append("\n");
	}

	retval.append(")\n");

	return retval;
}


/*
 * This method returns the actual pointer to the mutex that is
 * being used to control access to the map of variables in this
 * node. This is here primarily as an accessor tool for the
 * subclasses that might be implemented on top of this guy.
 */
CKFWMutex *CKDataNode::getVarsMutex()
{
	return & mVarsMutex;
}


/*
 * This method returns the actual pointer to the STL map of the
 * variables and it's useful for subclasses that have to do
 * something with the variables that I didn't originally think
 * of.
 */
std::map<CKString, CKVariant> *CKDataNode::getVars()
{
	return & mVars;
}


/*
 * This method returns the actual pointer to the mutex that is
 * being used to control access to the list of child nodes in this
 * node. This is here primarily as an accessor tool for the
 * subclasses that might be implemented on top of this guy.
 */
CKFWMutex *CKDataNode::getKidsMutex()
{
	return & mKidsMutex;
}


/*
 * This method returns the actual pointer to the STL list of the
 * children and it's useful for subclasses that have to do
 * something with the children that I didn't originally think
 * of.
 */
CKVector<CKDataNode*> *CKDataNode::getKids()
{
	return & mKids;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKDataNode & aNode )
{
	aStream << aNode.toString();

	return aStream;
}




/*
 * ----------------------------------------------------------------------------
 * This is the low-level node in the doubly-linked list that will be used
 * to organize the data nodes. This is nice in that it's easy to use, easy
 * to deal with, and the destructor takes care of cleaning up the data nodes
 * itself.
 *
 * We base it off the data node so that it appears to be a normal nodein
 * all regards - save the ability to exist in a doubly-linked list.
 * ----------------------------------------------------------------------------
 */
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
CKDataNodeListElem::CKDataNodeListElem() :
	CKDataNode(),
	mPrev(NULL),
	mNext(NULL)
{
}


/*
 * This is a "promotion" constructor that takes a data point and
 * creates a new data point node based on the data in that point.
 * This is important because it'll be an easy way to add data
 * points to the list.
 */
CKDataNodeListElem::CKDataNodeListElem( const CKDataNode & anOther,
					CKDataNodeListElem *aPrev,
					CKDataNodeListElem *aNext ) :
	CKDataNode(anOther),
	mPrev(aPrev),
	mNext(aNext)
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKDataNodeListElem::CKDataNodeListElem( const CKDataNodeListElem & anOther ) :
	CKDataNode(),
	mPrev(NULL),
	mNext(NULL)
{
	// let the operator==() take care of this for me
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKDataNodeListElem::~CKDataNodeListElem()
{
	// the super takes are of deleting all but the pointers
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKDataNodeListElem & CKDataNodeListElem::operator=( const CKDataNodeListElem & anOther )
{
	// start by letting the super do it's copying
	CKDataNode::operator=(anOther);
	// just copy in all the values from the other
	mPrev = anOther.mPrev;
	mNext = anOther.mNext;

	return *this;
}


/*
 * At times it's also nice to be able to set a data point to this
 * node so that there's not a ton of casting in the code.
 */
CKDataNodeListElem & CKDataNodeListElem::operator=( const CKDataNode & anOther )
{
	// just copy in the node value and leave the pointers alone
	CKDataNode::operator=(anOther);

	return *this;
}


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
void CKDataNodeListElem::setPrev( CKDataNodeListElem *aNode )
{
	mPrev = aNode;
}


void CKDataNodeListElem::setNext( CKDataNodeListElem *aNode )
{
	mNext = aNode;
}


/*
 * These are the simple getters for the links to the previous and
 * next nodes in the list. There's nothing special here, so we're
 * exposing them directly.
 */
CKDataNodeListElem *CKDataNodeListElem::getPrev()
{
	return mPrev;
}


CKDataNodeListElem *CKDataNodeListElem::getNext()
{
	return mNext;
}


/*
 * This method is used to 'unlink' the node from the list it's in.
 * This will NOT delete the node, merely take it out the the list
 * and now it becomes the responsibility of the caller to delete
 * this node, or add him to another list.
 */
void CKDataNodeListElem::removeFromList()
{
	// first, point the next's "prev" to the prev
	if (mNext != NULL) {
		mNext->mPrev = mPrev;
	}
	// next, point the prev's "next" to the next
	if (mPrev != NULL) {
		mPrev->mNext = mNext;
	}
	// finally, I'm not linked to *anyone* anymore
	mPrev = NULL;
	mNext = NULL;
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKDataNodeListElems are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKDataNodeListElem::operator==( const CKDataNodeListElem & anOther ) const
{
	bool		equal = true;

	// first, see if the strings are equal
	if (equal) {
		if (!CKDataNode::operator==(anOther)) {
			equal = false;
		}
	}
	// ...now check the pointers
	if (equal) {
		if ((mPrev != anOther.mPrev) ||
			(mNext != anOther.mNext)) {
			equal = false;
		}
	}

	return equal;
}


/*
 * This method checks to see if the two CKDataNodeListElems are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKDataNodeListElem::operator!=( const CKDataNodeListElem & anOther ) const
{
	return !(this->operator==(anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKDataNodeListElem::toString() const
{
	// put everything in between angle brackets to make it look nice
	CKString	retval = "<DataNode=";
	retval.append(CKDataNode::toString()).append(", ");
	char	buff[80];
	bzero(buff, 80);
	snprintf(buff, 79, "Prev=%lx, Next=%lx>", (unsigned long)mPrev,
			(unsigned long)mNext);
	retval.append(buff);

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKDataNodeListElem & aNode )
{
	aStream << aNode.toString();

	return aStream;
}




/*
 * ----------------------------------------------------------------------------
 * This is the high-level interface to a list of CKDataNode objects. It
 * is organized as a doubly-linked list of CKDataNodeListElems and the interface
 * to the list if controlled by a nice CKFWMutex. This is a nice and clean
 * replacement to the STL std::list.
 * ----------------------------------------------------------------------------
 */
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
CKDataNodeList::CKDataNodeList() :
	mHead(NULL),
	mTail(NULL),
	mMutex()
{
}


/*
 * This is the standard copy constructor and needs to be in every
 * class to make sure that we don't have too many things running
 * around.
 */
CKDataNodeList::CKDataNodeList( CKDataNodeList & anOther ) :
	mHead(NULL),
	mTail(NULL),
	mMutex()
{
	// let the operator==() take care of this for me
	*this = anOther;
}


/*
 * This is the standard destructor and needs to be virtual to make
 * sure that if we subclass off this the right destructor will be
 * called.
 */
CKDataNodeList::~CKDataNodeList()
{
	// we need to delete the head as long as there is one
	while (mHead != NULL) {
		CKDataNodeListElem	*next = mHead->mNext;
		delete mHead;
		mHead = next;
		if (mHead != NULL) {
			mHead->mPrev = NULL;
		}
	}
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKDataNodeList & CKDataNodeList::operator=( CKDataNodeList & anOther )
{
	// make sure that we don't do this to ourselves
	if (this != & anOther) {
		// first, clear out anything we might have right now
		clear();

		// now, do a deep copy of the source list
		copyToEnd(anOther);
	}
	return *this;
}


CKDataNodeList & CKDataNodeList::operator=( const CKDataNodeList & anOther )
{
	this->operator=((CKDataNodeList &) anOther);

	return *this;
}


/********************************************************
 *
 *                Accessor Methods
 *
 ********************************************************/
/*
 * These are the easiest ways to get at the head and tail of this
 * list. After that, the CKDataNodeListElem's getPrev() and getNext()
 * do a good job of moving you around the list.
 */
CKDataNodeListElem *CKDataNodeList::getHead()
{
	return mHead;
}


CKDataNodeListElem *CKDataNodeList::getHead() const
{
	return mHead;
}


CKDataNodeListElem *CKDataNodeList::getTail()
{
	return mTail;
}


CKDataNodeListElem *CKDataNodeList::getTail() const
{
	return mTail;
}


/*
 * Because there may be times that the user wants to lock us up
 * for change, we're going to expose this here so it's easy for them
 * to iterate, for example.
 */
void CKDataNodeList::lock()
{
	mMutex.lock();
}


void CKDataNodeList::lock() const
{
	((CKDataNodeList *)this)->mMutex.lock();
}


void CKDataNodeList::unlock()
{
	mMutex.unlock();
}


void CKDataNodeList::unlock() const
{
	((CKDataNodeList *)this)->mMutex.unlock();
}


/*
 * This method is a simple indexing operator so that we can easily
 * get the individual strings in the list. If the argument
 * is -1, then the default is to get the *LAST* non-NULL
 * string in the list.
 */
CKDataNode & CKDataNodeList::operator[]( int aPosition )
{
	CKDataNodeListElem	*node = NULL;

	// first, see if the arg is -1, and if so, return the last one
	if (aPosition == -1) {
		node = mTail;
	} else {
		// first, lock up this guy against changes
		mMutex.lock();
		// all we need to do is count until we get there or the end
		int		cnt = 0;
		for (node = mHead; (cnt < aPosition) && (node != NULL); node = node->mNext) {
			cnt++;
		}
		// now we can release the lock
		mMutex.unlock();
	}

	// make sure that we have the node we're interested in
	if (node == NULL) {
		std::ostringstream	msg;
		msg << "CKDataNodeList::operator[](int) - the requested index: " << aPosition <<
			" was not available in the list. Please make sure that you are asking "
			"for a valid index in the list.";
		throw CKException(__FILE__, __LINE__, msg.str());
	}

	return (CKDataNode &)(*node);
}


CKDataNode & CKDataNodeList::operator[]( int aPosition ) const
{
	return ((CKDataNodeList *)this)->operator[](aPosition);
}


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
int CKDataNodeList::size()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// all we need to do is count them all up
	int		cnt = 0;
	for (CKDataNodeListElem *node = mHead; node != NULL; node = node->mNext) {
		cnt++;
	}
	// now we can release the lock
	mMutex.unlock();

	return cnt;
}


int CKDataNodeList::size() const
{
	return ((CKStringList *)this)->size();
}


/*
 * This is used to tell the caller if the list is empty. It's
 * faster than checking for a size() == 0.
 */
bool CKDataNodeList::empty()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// all we need to do is count them all up
	bool	empty = false;
	if (mHead == NULL) {
		empty = true;
	}
	// now we can release the lock
	mMutex.unlock();

	return empty;
}


bool CKDataNodeList::empty() const
{
	return ((CKStringList *)this)->empty();
}


/*
 * This method clears out the entire list and deletes all it's
 * contents. After this, all node pointers to nodes in this list
 * will be pointing to nothing, so watch out.
 */
void CKDataNodeList::clear()
{
	// first, lock up this guy against changes
	mMutex.lock();
	// we need to delete the head as long as there is one
	while (mHead != NULL) {
		CKDataNodeListElem	*next = mHead->mNext;
		delete mHead;
		mHead = next;
		if (mHead != NULL) {
			mHead->mPrev = NULL;
		}
	}
	// now make sure to reset the head and tail
	mHead = NULL;
	mTail = NULL;
	// now we can release the lock
	mMutex.unlock();
}


void CKDataNodeList::clear() const
{
	((CKStringList *)this)->clear();
}


/*
 * When I want to add a point to the front or back of the list,
 * these are the simplest ways to do that. The passed-in data node
 * is left untouched, and a copy is made of it at the proper point
 * in the list.
 */
void CKDataNodeList::addToFront( CKDataNode & aNode )
{
	// first, lock up this guy against changes
	mMutex.lock();

	// we need to create a new list element based on this guy
	CKDataNodeListElem	*node = new CKDataNodeListElem(aNode, NULL, mHead);
	if (node == NULL) {
		// first we need to release the lock
		mMutex.unlock();
		// now we can throw the exception
		std::ostringstream	msg;
		msg << "CKDataNodeList::addToFront(const CKDataNode &) - a "
			"new data list node could not be created for the passed in node: " <<
			aNode << " and that's a serious allocation problem that needs to "
			"be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// finish linking this guy into the list properly
		if (mHead == NULL) {
			mTail = node;
		} else {
			mHead->mPrev = node;
		}
		mHead = node;
	}

	// now we can release the lock
	mMutex.unlock();
}


void CKDataNodeList::addToFront( const CKDataNode & aNode )
{
	addToFront((CKDataNode &)aNode);
}


void CKDataNodeList::addToFront( CKDataNode & aNode ) const
{
	((CKDataNodeList *)this)->addToFront(aNode);
}


void CKDataNodeList::addToFront( const CKDataNode & aNode ) const
{
	((CKDataNodeList *)this)->addToFront((CKDataNode &)aNode);
}


void CKDataNodeList::addToEnd( CKDataNode & aNode )
{
	// first, lock up this guy against changes
	mMutex.lock();

	// we need to create a new list element based on this guy
	CKDataNodeListElem	*node = new CKDataNodeListElem(aNode, mTail, NULL);
	if (node == NULL) {
		// first we need to release the lock
		mMutex.unlock();
		// now we can throw the exception
		std::ostringstream	msg;
		msg << "BBGDataPointList::addToEnd(const char *) - a "
			"new data list element could not be created for the passed in node: " <<
			aNode << " and that's a serious allocation problem that needs to "
			"be looked into.";
		throw CKException(__FILE__, __LINE__, msg.str());
	} else {
		// finish linking this guy into the list properly
		if (mTail == NULL) {
			mHead = node;
		} else {
			mTail->mNext = node;
		}
		mTail = node;
	}

	// now we can release the lock
	mMutex.unlock();
}


void CKDataNodeList::addToEnd( const CKDataNode & aNode )
{
	addToEnd((CKDataNode &)aNode);
}


void CKDataNodeList::addToEnd( CKDataNode & aNode ) const
{
	((CKDataNodeList *)this)->addToEnd(aNode);
}


void CKDataNodeList::addToEnd( const CKDataNode & aNode ) const
{
	((CKDataNodeList *)this)->addToEnd((CKDataNode &)aNode);
}


/*
 * These methods take control of the passed-in arguments and place
 * them in the proper place in the list. This is different in that
 * the control of the node is passed to the list, but that's why
 * we've created them... to make it easy to add in nodes by just
 * changing the links.
 */
void CKDataNodeList::putOnFront( CKDataNodeListElem *aNode )
{
	// first, make sure we have something to do
	if (aNode != NULL) {
		// next, lock up this guy against changes
		mMutex.lock();

		// we simply need to link this bad boy into the list
		aNode->mPrev = NULL;
		aNode->mNext = mHead;
		if (mHead == NULL) {
			mTail = aNode;
		} else {
			mHead->mPrev = aNode;
		}
		mHead = aNode;

		// now we can release the lock
		mMutex.unlock();
	}
}


void CKDataNodeList::putOnFront( const CKDataNodeListElem *aNode )
{
	putOnFront((CKDataNodeListElem *)aNode);
}


void CKDataNodeList::putOnFront( CKDataNodeListElem *aNode ) const
{
	((CKDataNodeList *)this)->putOnFront(aNode);
}


void CKDataNodeList::putOnFront( const CKDataNodeListElem *aNode ) const
{
	((CKDataNodeList *)this)->putOnFront((CKDataNodeListElem *)aNode);
}


void CKDataNodeList::putOnEnd( CKDataNodeListElem *aNode )
{
	// first, make sure we have something to do
	if (aNode != NULL) {
		// next, lock up this guy against changes
		mMutex.lock();

		// we simply need to link this bad boy into the list
		aNode->mPrev = mTail;
		aNode->mNext = NULL;
		if (mTail == NULL) {
			mHead = aNode;
		} else {
			mTail->mNext = aNode;
		}
		mTail = aNode;

		// now we can release the lock
		mMutex.unlock();
	}
}


void CKDataNodeList::putOnEnd( const CKDataNodeListElem *aNode )
{
	putOnEnd((CKDataNodeListElem *)aNode);
}


void CKDataNodeList::putOnEnd( CKDataNodeListElem *aNode ) const
{
	((CKDataNodeList *)this)->putOnEnd(aNode);
}


void CKDataNodeList::putOnEnd( const CKDataNodeListElem *aNode ) const
{
	((CKDataNodeList *)this)->putOnEnd((CKDataNodeListElem *)aNode);
}


/*
 * When you have a list that you want to add to this list, these
 * are the methods to use. It's important to note that the arguments
 * will NOT be altered - which is why this is called the 'copy' as
 * opposed to the 'splice'.
 */
void CKDataNodeList::copyToFront( CKDataNodeList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	/*
	 * I need to go through all the source data, but backwards because
	 * I'll be putting these new nodes on the *front* of the list, and
	 * if I go through the source in the forward order, I'll reverse the
	 * order of the elements in the source as I add them. So I'll go
	 * backwards... no biggie...
	 */
	for (CKDataNodeListElem *src = aList.mTail; src != NULL; src = src->mPrev) {
		// first, make a copy of this guy
		CKDataNodeListElem	*node = new CKDataNodeListElem(*src, NULL, mHead);
		if (node == NULL) {
			// first we need to release the locks
			aList.mMutex.unlock();
			mMutex.unlock();
			// now we can throw the exception
			std::ostringstream	msg;
			msg << "CKDataNodeList::copyToFront(CKDataNodeList &) - a "
				"new list element could not be created for the node: " <<
				src << " and that's a serious allocation problem that needs to "
				"be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}

		// now, add this guy to the front of the list
		if (mHead == NULL) {
			mTail = node;
		} else {
			mHead->mPrev = node;
		}
		mHead = node;
	}

	// now I can release both locks
	aList.mMutex.unlock();
	mMutex.unlock();
}


void CKDataNodeList::copyToFront( const CKDataNodeList & aList )
{
	copyToFront((CKDataNodeList &)aList);
}


void CKDataNodeList::copyToFront( CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->copyToFront(aList);
}


void CKDataNodeList::copyToFront( const CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->copyToFront((CKDataNodeList &)aList);
}


void CKDataNodeList::copyToEnd( CKDataNodeList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	/*
	 * I need to go through all the source data. I'll be putting these new
	 * nodes on the *end* of the list so the order is preserved.
	 */
	for (CKDataNodeListElem *src = aList.mHead; src != NULL; src = src->mNext) {
		// first, make a copy of this guy
		CKDataNodeListElem	*node = new CKDataNodeListElem(*src, mTail, NULL);
		if (node == NULL) {
			// first we need to release the locks
			aList.mMutex.unlock();
			mMutex.unlock();
			// now we can throw the exception
			std::ostringstream	msg;
			msg << "CKDataNodeList::copyToEnd(CKDataNodeList &) - a "
				"new list element could not be created for the node: " <<
				src << " and that's a serious allocation problem that needs to "
				"be looked into.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}

		// now, add this guy to the end of the list
		if (mTail == NULL) {
			mHead = node;
		} else {
			mTail->mNext = node;
		}
		mTail = node;
	}

	// now I can release both locks
	aList.mMutex.unlock();
	mMutex.unlock();
}


void CKDataNodeList::copyToEnd( const CKDataNodeList & aList )
{
	copyToEnd((CKDataNodeList &)aList);
}


void CKDataNodeList::copyToEnd( CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->copyToEnd(aList);
}


void CKDataNodeList::copyToEnd( const CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->copyToEnd((CKDataNodeList &)aList);
}


/*
 * When you have a list that you want to merge into this list, these
 * are the methods to use. It's important to note that the argument
 * lists will be EMPTIED - which is why this is called the 'splice'
 * as opposed to the 'copy'.
 */
void CKDataNodeList::spliceOnFront( CKDataNodeList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// add the source, in total, to the head of this list
	if (mHead == NULL) {
		// take their list in toto - mine is empty
		mHead = aList.mHead;
		mTail = aList.mTail;
	} else {
		mHead->mPrev = aList.mTail;
		if (aList.mTail != NULL) {
			aList.mTail->mNext = mHead;
		}
		if (aList.mHead != NULL) {
			mHead = aList.mHead;
		}
	}
	// ...and empty the source list
	aList.mHead = NULL;
	aList.mTail = NULL;

	// now I can release both locks
	aList.mMutex.unlock();
	mMutex.unlock();
}


void CKDataNodeList::spliceOnFront( const CKDataNodeList & aList )
{
	spliceOnFront((CKDataNodeList &)aList);
}


void CKDataNodeList::spliceOnFront( CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->spliceOnFront(aList);
}


void CKDataNodeList::spliceOnFront( const CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->spliceOnFront((CKDataNodeList &)aList);
}


void CKDataNodeList::spliceOnEnd( CKDataNodeList & aList )
{
	// first, I need to lock up both me and the source
	mMutex.lock();
	aList.mMutex.lock();

	// add the source, in total, to the end of this list
	if (mTail == NULL) {
		// take their list in toto - mine is empty
		mHead = aList.mHead;
		mTail = aList.mTail;
	} else {
		mTail->mNext = aList.mHead;
		if (aList.mHead != NULL) {
			aList.mHead->mPrev = mTail;
		}
		if (aList.mTail != NULL) {
			mTail = aList.mTail;
		}
	}
	// ...and empty the source list
	aList.mHead = NULL;
	aList.mTail = NULL;

	// now I can release both locks
	aList.mMutex.unlock();
	mMutex.unlock();
}


void CKDataNodeList::spliceOnEnd( const CKDataNodeList & aList )
{
	spliceOnEnd((CKDataNodeList &)aList);
}


void CKDataNodeList::spliceOnEnd( CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->spliceOnEnd(aList);
}


void CKDataNodeList::spliceOnEnd( const CKDataNodeList & aList ) const
{
	((CKDataNodeList *)this)->spliceOnEnd((CKDataNodeList &)aList);
}


/********************************************************
 *
 *                Utility Methods
 *
 ********************************************************/
/*
 * This method checks to see if the two CKDataNodeLists are equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKDataNodeList::operator==( CKDataNodeList & anOther )
{
	bool		equal = true;

	// first, lock up both lists against changes
	mMutex.lock();
	anOther.mMutex.lock();

	/*
	 * We need to compare each element in the list as nodes and
	 * NOT as list elements as the pointers will never be the same
	 * but the data will.
	 */
	CKDataNodeListElem	*me = mHead;
	CKDataNodeListElem	*him = anOther.mHead;
	while (equal) {
		// see if we're at the end
		if ((me == NULL) && (him == NULL)) {
			break;
		}

		// see if the two lists are of different lengths
		if (((me == NULL) && (him != NULL)) ||
			((me != NULL) && (him == NULL))) {
			equal = false;
			break;
		}

		// compare the values by data contents only
		if (!me->CKDataNode::operator==(*(CKDataNode*)him)) {
			equal = false;
			break;
		}

		// now move to the next point in each list
		me = me->mNext;
		him = him->mNext;
	}

	// now we're OK to unlock these lists and let them be free
	anOther.mMutex.unlock();
	mMutex.unlock();

	return equal;
}


bool CKDataNodeList::operator==( const CKDataNodeList & anOther )
{
	return this->operator==((CKDataNodeList &) anOther);
}


bool CKDataNodeList::operator==( CKDataNodeList & anOther ) const
{
	return ((CKDataNodeList *)this)->operator==(anOther);
}


bool CKDataNodeList::operator==( const CKDataNodeList & anOther ) const
{
	return ((CKDataNodeList *)this)->operator==((CKDataNodeList &) anOther);
}


/*
 * This method checks to see if the two CKDataNodeLists are not equal
 * to one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKDataNodeList::operator!=( CKDataNodeList & anOther )
{
	return !(this->operator==(anOther));
}


bool CKDataNodeList::operator!=( const CKDataNodeList & anOther )
{
	return !(this->operator==((CKDataNodeList &) anOther));
}


bool CKDataNodeList::operator!=( CKDataNodeList & anOther ) const
{
	return !(((CKDataNodeList *)this)->operator==(anOther));
}


bool CKDataNodeList::operator!=( const CKDataNodeList & anOther ) const
{
	return !(((CKDataNodeList *)this)->operator==((CKDataNodeList &) anOther));
}


/*
 * Because there are times when it's useful to have a nice
 * human-readable form of the contents of this instance. Most of the
 * time this means that it's used for debugging, but it could be used
 * for just about anything. In these cases, it's nice not to have to
 * worry about the ownership of the representation, so this returns
 * a CKString.
 */
CKString CKDataNodeList::toString()
{
	// lock this guy up so he doesn't change
	mMutex.lock();

	CKString		retval = "[";
	// put each data point out on the output
	for (CKDataNodeListElem *node = mHead; node != NULL; node = node->mNext) {
		retval += node->CKDataNode::toString();
		retval += "\n";
	}
	retval += "]";

	// unlock him now
	mMutex.unlock();

	return retval;
}


/*
 * Setting the head or the tail is a bit dicey and so we're not
 * going to let just anyone change these guys.
 */
void CKDataNodeList::setHead( CKDataNodeListElem *aNode )
{
	mHead = aNode;
}


void CKDataNodeList::setTail( CKDataNodeListElem *aNode )
{
	mTail = aNode;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, CKDataNodeList & aList )
{
	aStream << aList.toString();

	return aStream;
}
