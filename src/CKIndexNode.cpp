/*
 * CKIndexNode.cpp - this file implements a class that can be used to index
 *                   data in a CKDataNode-based tree. The idea is that the
 *                   CKDataNode is a very strict construction tree where
 *                   there can be only one parent for each child. This is
 *                   important so that you can travel within the tree without
 *                   worrying about getting lost.
 *
 *                   Yet there are times when you want to be able to access
 *                   the data in a CKDataNode tree without having to travel
 *                   through the tree to find each node. Rather, it would be
 *                   nice to be able to use a 'secondary tree' that organizes
 *                   the data in a different manner that makes it easier to
 *                   get at what you need. Since this is just an organizational
 *                   grouping, we don't want this tree to manage the memory of
 *                   the leaf nodes, as the main CKDataTree does all that.
 *
 * $Id: CKIndexNode.cpp,v 1.1 2004/12/22 10:50:24 drbob Exp $
 */

//	System Headers
#include <sstream>

//	Third-Party Headers

//	Other Headers
#include "CKIndexNode.h"
#include "CKException.h"

//	Forward Declarations

//	Private Constants

//	Private Datatypes

//	Private Data Constants


/********************************************************
 *
 *                Constructors/Destructor
 *
 ********************************************************/
/*
 * This is the default constructor that creates a nice, empty
 * node ready for the user to organize as needed.
 */
CKIndexNode::CKIndexNode() :
	CKDataNode(),
	mReference(NULL)
{
}


/*
 * This constructor creates a new node and sets the reference to
 * the node's parent to the supplied pointer. This is OK because
 * each node *never* controls the memory of it's parent.
 */
CKIndexNode::CKIndexNode( CKIndexNode *aParent ) :
	CKDataNode(aParent),
	mReference(NULL)
{
}


/*
 * This constructor makes a node with the given parent and the
 * identifying name provided as well. This is useful when the
 * structure of a tree is being built and the data will be
 * populated later. The parent node is not controlled by the
 * instance because no parent nodes are controlled by the nodes.
 */
CKIndexNode::CKIndexNode( CKIndexNode *aParent, const CKString & aName ) :
	CKDataNode(aParent, aName),
	mReference(NULL)
{
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
CKIndexNode::CKIndexNode( CKIndexNode *aParent, const CKString & aName,
						  const CKString & aKey, const CKVariant & aValue ) :
	CKDataNode(aParent, aName, aKey, aValue),
	mReference(NULL)
{
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
CKIndexNode::CKIndexNode( const CKIndexNode & anOther ) :
	CKDataNode(anOther),
	mReference(NULL)
{
	// we can let the '=' operator do most of the work
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
CKIndexNode::~CKIndexNode()
{
	// the super does all the heavy lifting for me
}


/*
 * When we want to process the result of an equality we need to
 * make sure that we do this right by always having an equals
 * operator on all classes.
 */
CKIndexNode & CKIndexNode::operator=( const CKIndexNode & anOther )
{
	// let the super do it's thing
	CKDataNode::operator=(anOther);

	// now we can set our reference
	mReference = anOther.mReference;

	return *this;
}


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
CKDataNode *CKIndexNode::getReference()
{
	return mReference;
}


/*
 * This method gets the reference node at the specified path assuming
 * that the path exists in this tree, and that there is a reference
 * node at this node in the tree. If either of these isn't true, then
 * the result will be NULL.
 */
CKDataNode *CKIndexNode::getReferenceAtPath( const CKString & aPath )
{
	bool		error = false;
	CKDataNode	*retval = NULL;

	// first, see if we need to start at the root
	CKIndexNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = (CKIndexNode *) node->mParent;
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
			msg << "CKIndexNode::getReferenceAtPath(const CKString &) - the path "
				"had insufficient steps to create a valid path. Please make sure "
				"that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			retval = node->getReferenceAtPath(steps);
		}
	}

	return retval;
}


/*
 * This version of the method takes a list of path steps so that
 * there's no need to worry about escaping any node names in the path.
 */
CKDataNode *CKIndexNode::getReferenceAtPath( const CKStringList & aSteps )
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
			msg << "CKIndexNode::getReferenceAtPath(const CKStringList &) "
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

	// if we're here, then the last node has the reference we want
	if (!error) {
		retval = ((CKIndexNode *) node)->getReference();
	}

	return retval;
}


/*
 * Each index node can have a single CKDataNode as it's 'contents'.
 * If a node has this 'contents' it is here because this method
 * has been called.
 */
void CKIndexNode::putReference( CKDataNode *aNode )
{
	mReference = aNode;
}


/*
 * This method sets the reference node at the specified path even if
 * the path doesn't exist. As expected, if the path doesn't exist, it
 * will be created and the reference set.
 */
void CKIndexNode::putReferenceAtPath( const CKString & aPath, CKDataNode *aNode )
{
	bool		error = false;

	// first, see if we need to start at the root
	CKIndexNode		*node = this;
	if (!error) {
		if (aPath[0] == '/') {
			// travel to the root of this tree
			while (node->mParent != NULL) {
				node = (CKIndexNode*) node->mParent;
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
			msg << "CKIndexNode::putReferenceAtPath(const CKString &, CKDataNode *) - "
				"the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// use the other version to get the value
			node->putReferenceAtPath(steps, aNode);
		}
	}
}


/*
 * This version of the method takes a list of path steps so that
 * there's no need to worry about escaping any node names in the path.
 */
void CKIndexNode::putReferenceAtPath( const CKStringList & aSteps, CKDataNode *aNode )
{
	bool		error = false;

	// start right where we are now
	CKIndexNode	*node = this;

	// move down the path step by step...
	int		stepCnt = aSteps.size();
	if (!error) {
		// the last element of the path is the variable name
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKIndexNode::putReferenceAtPath(const CKStringList &, CKDataNode *) - "
				"the path had insufficient steps to create a valid path. Please "
				"make sure that a valid path is passed to this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		} else {
			// OK, we have nodes to move through
			for (int step = 0; !error && (step < stepCnt); step++) {
				CKIndexNode	*next = (CKIndexNode *) node->findChild(aSteps[step]);
				if (next == NULL) {
					/*
					 * With no child of the right name, we need to create
					 * one and place it in the tree so that we can continue.
					 * This is one very fast way to build the tree of data.
					 */
					next = new CKIndexNode(node, aSteps[step]);
					if (next == NULL) {
						error = true;
						std::ostringstream	msg;
						msg << "CKIndexNode::putReferenceAtPath(const CKStringList &, "
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

	// if we're here, then we need to add the reference as requested
	if (!error) {
		node->putReference(aNode);
	}
}


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
CKIndexNode *CKIndexNode::newNodeByShallowCopy( const CKIndexNode *aNode )
{
	CKIndexNode	*retval = NULL;

	if (aNode != NULL) {
		retval = new CKIndexNode(*aNode);
		if (retval == NULL) {
			std::ostringstream	msg;
			msg << "CKIndexNode::newNodeByShallowCopy(const CKIndexNode*) - "
				"while trying to copy the core node, an allocation error "
				"occurred. Please check into this as soon as possible.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	return retval;
}


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
CKIndexNode *CKIndexNode::newNodeByDeepCopy( const CKIndexNode *aNode,
											 const CKIndexNode *aParent )
{
	bool		error = false;
	CKIndexNode	*retval = NULL;

	// first thing to do is to make a shallow copy of the node
	if (!error) {
		if (aNode != NULL) {
			retval = new CKIndexNode(*aNode);
			if (retval == NULL) {
				error = true;
				std::ostringstream	msg;
				msg << "CKIndexNode::newNodeByDeepCopy(const CKIndexNode*, "
					"const CKIndexNode*) - while trying to copy the core node, "
					"an allocation error occurred. Please check into this as "
					"soon as possible.";
				throw CKException(__FILE__, __LINE__, msg.str());
			}
		} else {
			error = true;
			std::ostringstream	msg;
			msg << "CKIndexNode::newNodeByDeepCopy(const CKIndexNode*, "
				"const CKIndexNode*) - the passed-in node to copy was NULL and "
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
			CKIndexNode	*n = NULL;
			try {
				n = newNodeByDeepCopy((CKIndexNode *) oldKids[i], retval);
			} catch (CKException & cke) {
				n = NULL;
			}
			if (n == NULL) {
				// flag this as an error
				error = true;
				// clean up all the things we've allocated to this point
				deleteNodeDeep((CKDataNode *) retval);
				// now log the problem and throw the exception
				std::ostringstream	msg;
				msg << "CKIndexNode::newNodeByDeepCopy(const CKIndexNode*, "
					"const CKIndexNode*) - while trying to copy the children of "
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
bool CKIndexNode::operator==( const CKIndexNode & anOther ) const
{
	bool		equal = true;

	if ((!CKDataNode::operator==(anOther)) ||
		(mReference != anOther.mReference)) {
		equal = false;
	}

	return equal;
}


/*
 * This method checks to see if the two CKIndexNodes are not equal to
 * one another based on the values they represent and *not* on the
 * actual pointers themselves. If they are not equal, then this method
 * returns a value of true, otherwise, it returns a false.
 */
bool CKIndexNode::operator!=( const CKIndexNode & anOther ) const
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
CKString CKIndexNode::toString( bool aDeepFlag ) const
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

	// put the reference's name, if it's available
	retval.append("Reference=");
	if (mReference == NULL) {
		retval.append("<NULL>");
	} else {
		retval.append(mReference->mName);
	}
	retval.append("\n");

	retval.append(")\n");

	return retval;
}


/*
 * For debugging purposes, let's make it easy for the user to stream
 * out this value. It basically is just the value of toString() which
 * will indicate the data type and the value.
 */
std::ostream & operator<<( std::ostream & aStream, const CKIndexNode & aNode )
{
	aStream << aNode.toString();

	return aStream;
}
