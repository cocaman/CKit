/*
 * CKDataNode.cpp - this file implements a class that can be used to represent
 *                  a general tree of data where each node contains a map of
 *                  key/value pairs where the key is a std::string (name) and
 *                  the value is a CKVariant that can hold almost anything
 *                  you need to hold. In addition to the data, this node has
 *                  a list of children nodes (pointers to CKDataNodes) and a
 *                  pointer to a parent node. This means that this guy can
 *                  be the basis of a complete tree of data and this is
 *                  very important to many applications.
 *
 * $Id: CKDataNode.cpp,v 1.9 2004/07/27 20:01:21 drbob Exp $
 */

//	System Headers
#include <sstream>

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
	aParent->mKids.push_back(this);
	aParent->mKidsMutex.unlock();
}


/*
 * This constructor makes a node with the given parent and the
 * identifying name provided as well. This is useful when the
 * structure of a tree is being built and the data will be
 * populated later. The parent node is not controlled by the
 * instance because no parent nodes are controlled by the nodes.
 */
CKDataNode::CKDataNode( CKDataNode *aParent, const std::string & aName ) :
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
	aParent->mKids.push_back(this);
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
CKDataNode::CKDataNode( CKDataNode *aParent, const std::string & aName,
						const std::string & aKey, const CKVariant & aValue ) :
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
	aParent->mKids.push_back(this);
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
	std::list<CKDataNode*>::iterator	i;
	while (!mKids.empty()) {
		// get the first kid in the list
		i = mKids.begin();
		// if we're his parent, then invalidate that link
		if (((*i) != NULL) && ((*i)->mParent = this)) {
			(*i)->mParent = NULL;
		}
		// ...and drop this guy from the list
		mKids.erase(i);
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
void CKDataNode::setName( const std::string & aName )
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
std::string CKDataNode::getName() const
{
	return mName;
}


/*
 * Each node can have many variables (attributes) stored in a map
 * as a std::string name and CKVariant value. This method returns a
 * pointer to the actual named value so if you want to do something
 * with it, make a copy. If there is no variable with this name the
 * method will return a NULL.
 */
CKVariant *CKDataNode::getVar( const std::string & aName )
{
	CKVariant		*retval = NULL;

	// make sure we do this in a thread-safe manner
	mVarsMutex.lock();
	// now look for the entry
	std::map<std::string, CKVariant>::iterator		i;
	i = mVars.find(aName);
	if (i != mVars.end()) {
		retval = &((*i).second);
	}
	// now unlock the map
	mVarsMutex.unlock();

	return retval;
}


/*
 * Each node can have many variables (attributes) stored in a map
 * as a std::string name and CKVariant value. This method places a
 * value into that map for this instance at the name provided.
 */
void CKDataNode::putVar( const std::string & aName, const CKVariant & aValue )
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
 * removes the named variable from this node if it exists.
 */
void CKDataNode::removeVar( const std::string & aName )
{
	// make sure we do this in a thread-safe manner
	mVarsMutex.lock();
	// ...erase all entries with this name (only one possible)
	mVars.erase(aName);
	// now unlock the map
	mVarsMutex.unlock();
}


/*
 * When you want to clear out all the variables (attributes) from
 * this node, call this method and the entire map of variables will
 * be cleared out. It's non-reversable, so be carful with this.
 */
void CKDataNode::clearVars()
{
	// make sure we do this in a thread-safe manner
	mVarsMutex.lock();
	// ...erase all entries - period.
	mVars.clear();
	// now unlock the map
	mVarsMutex.unlock();
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
		std::list<CKDataNode*>::iterator		i;
		i = find(mKids.begin(), mKids.end(), aNode);
		if (i == mKids.end()) {
			mKids.push_back(aNode);
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
		/*
		 * Next, verify that it's in the list of children on this
		 * node. If so, then reset it's parent as well as removing it.
		 */
		// gotta be thread-safe on this guy
		mKidsMutex.lock();
		// look for first occurrence
		std::list<CKDataNode*>::iterator		i;
		i = find(mKids.begin(), mKids.end(), aNode);
		if (i != mKids.end()) {
			// first thing is to remove me as it's parent
			if ((*i)->mParent == this) {
				(*i)->mParent = NULL;
			}
			// ...and remove it from the list
			mKids.erase(i);
		}
		// unlock the list of kids
		mKidsMutex.unlock();
	}
}


/*
 * This method returns the complete list (as a vector) of all the
 * child nodes identifying names. This is a very handy way to
 * iterate over the children assuming they all have distinct names
 * as would be the case in most data sets.
 */
std::vector<std::string> CKDataNode::getChildNames()
{
	std::vector<std::string>	retval;

	// lock up the list to be safe
	mKidsMutex.lock();
	// go through all children and add their names to the vector
	std::list<CKDataNode*>::iterator	i;
	for (i = mKids.begin(); i != mKids.end(); ++i) {
		retval.push_back((*i)->mName);
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
CKDataNode *CKDataNode::findChild( const std::string & aName )
{
	CKDataNode		*retval = NULL;

	// lock up the list to be safe
	mKidsMutex.lock();
	// go through all children and look to their names for the right one
	std::list<CKDataNode*>::iterator	i;
	for (i = mKids.begin(); i != mKids.end(); ++i) {
		if ((*i)->mName == aName) {
			retval = (*i);
		}
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
CKVariant *CKDataNode::getVarAtPath( const std::string & aPath )
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
	std::vector<std::string>	steps;
	int							stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::getVarAtPath(const std::string &) - the path "
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
CKVariant *CKDataNode::getVarAtPath( const std::vector<std::string> & aSteps )
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
			msg << "CKDataNode::getVarAtPath(const std::vector<std::string> &) "
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
void CKDataNode::putVarAtPath( const std::string & aPath, const CKVariant & aValue )
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
	std::vector<std::string>	steps;
	int							stepCnt = 0;
	if (!error) {
		steps = pathToSteps(aPath);
		stepCnt = steps.size();
		if (stepCnt < 1) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::putVarAtPath(const std::string &, const "
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
void CKDataNode::putVarAtPath( const std::vector<std::string> & aSteps,
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
			msg << "CKDataNode::putVarAtPath(const std::vector<std::string> &, "
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
							"std::vector<std::string> &, const CKVariant &) - "
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
 * This method returns a vector of the node identifiers in this
 * tree leading to the current node. This is basically walking
 * 'up' the tree to the root, building accumulating the steps
 * along the way.
 */
std::vector<std::string> CKDataNode::getSteps() const
{
	std::vector<std::string>	retval;

	// malk up the tree inserting names at the front
	retval.push_back(mName);
	CKDataNode *n = mParent;
	while (n != NULL) {
		/*
		 * Insert the node names at the beginning of the list up to
		 * the point where we're at the root node. If we're at the
		 * root node, and the name is blank, then don't put it in
		 * the path - it's not necessary in that case.
		 */
		if ((n->mParent != NULL) || (n->mName != "")) {
			retval.insert(retval.begin(), n->mName);
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
std::string CKDataNode::getPath() const
{
	std::string		retval;

	// first, get the step in the path
	std::vector<std::string>	steps = getSteps();
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
std::vector<std::string> CKDataNode::pathToSteps( const std::string & aPath )
{
	bool						error = false;
	std::vector<std::string>	retval;
	bool						done = false;

	// first, strip any leading or trailing '/' characters
	std::string		cleanPath = aPath;
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
	std::vector<std::string>	raw;
	int							rawCnt = 0;
	if (!error && !done) {
		raw = parseIntoChunks(cleanPath, "/");
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
				std::string		comp = raw[i];
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
				retval.push_back(comp);
			} else {
				// not escaped, so just add it
				retval.push_back(raw[i]);
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
std::string CKDataNode::stepsToPath( const std::vector<std::string> & aPath )
{
	std::string		retval;

	// loop over all the elements adding each as necessary
	std::vector<std::string>::const_iterator	i;
	for (i = aPath.begin(); i != aPath.end(); ++i) {
		// see if we need a delimiter between this and the next step
		if (i != aPath.begin()) {
			retval.append(1, '/');
		}

		// now see if the step needs to be escaped due to a slash
		if (i->find('/') != std::string::npos) {
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
 * source, and need a list of names to data that data for. This
 * method does a great job of getting a unique vector of names
 * of all the leaf nodes under it, and all it's children.
 */
std::vector<std::string>	CKDataNode::getUniqueLeafNodeNames()
{
	bool						error = false;
	std::vector<std::string>	retval;

	// OK, we need to look and see if we are a leaf node
	if (!error) {
		// lock up the list of kids for this
		mKidsMutex.lock();
		// see if we have any at all
		if (mKids.size() > 0) {
			// ask all the kids for their unique leaf node names
			std::list<CKDataNode*>::iterator	i;
			for (i = mKids.begin(); i != mKids.end(); ++i) {
				std::vector<std::string>	part = (*i)->getUniqueLeafNodeNames();
				if (part.size() < 1) {
					// unlock the list of kids
					mKidsMutex.unlock();
					// flag the error and throw the exception
					error = true;
					std::ostringstream	msg;
					msg << "CKDataNode::getUniqueLeafNodeNames() - the node '" <<
						(*i)->mName << "' (a child of '" << mName << "') had no "
						"leaf nodes under it. This is simply not possible. It's "
						"likely that there's a data corruption problem. Check "
						"on it.";
					throw CKException(__FILE__, __LINE__, msg.str());
				} else {
					/*
					 * OK... we have some leaf nodes from the child.
					 * Let's add them to our list if they are unique.
					 */
					unsigned int	cnt = part.size();
					for (unsigned int c = 0; c < cnt; c++) {
						if (find(retval.begin(), retval.end(), part[c])
								== retval.end()) {
							retval.push_back(part[c]);
						}
					}
				}
			}
		} else {
			/*
			 * We are a leaf node! Then put our name in the vector
			 * and that's all we have to do.
			 */
			retval.push_back(mName);
		}
		// unlock the list of kids
		mKidsMutex.unlock();
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
		std::list<CKDataNode*>	oldKids = retval->mKids;
		// ...and clear out the existing list of children
		retval->mKids.clear();
		// now iterate on the list of kids and deep copy each one
		std::list<CKDataNode*>::iterator	i;
		for (i = oldKids.begin(); i != oldKids.end(); ++i) {
			// copy each one with this guy as it's new parent
			CKDataNode	*n = NULL;
			try {
				n = newNodeByDeepCopy((*i), retval);
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
				retval->mKids.push_back(n);
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
		if (aNode->mKids.size() > 0) {
			std::list<CKDataNode*>::iterator	i;
			for (i = aNode->mKids.begin(); i != aNode->mKids.end(); ++i) {
				deleteNodeDeep(*i);
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
 * a std::string.
 *
 * If the default 'false' is used then the only information that's
 * returned is with regards to the node itself and not a complete
 * dump of the tree rooted at this node. Pass in a 'true' if you
 * want to see the entire tree at this node.
 */
std::string CKDataNode::toString( bool aDeepFlag ) const
{
	std::string		retval = "(";

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
	std::map<std::string, CKVariant>::const_iterator	i;
	for (i = mVars.begin(); i != mVars.end(); ++i) {
		retval.append("   ");
		retval.append((*i).first);
		retval.append(" : ");
		retval.append((*i).second.toString());
		retval.append("\n");
	}

	// put in the names of each of the children
	retval.append("Children:\n");
	std::list<CKDataNode*>::const_iterator	j;
	for (j = mKids.begin(); j != mKids.end(); ++j) {
		retval.append("   ");
		retval.append((*j)->mName);
		if (aDeepFlag) {
			retval.append((*j)->toString(aDeepFlag));
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
std::map<std::string, CKVariant> *CKDataNode::getVars()
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
std::list<CKDataNode*> *CKDataNode::getKids()
{
	return & mKids;
}


/*
 * This is the tokenizer/parser that wasn't in the STL string
 * class for some unknown reason. It takes a source and a
 * delimiter and breaks up the source into chunks that are
 * all separated by the delimiter string. Each chunk is put
 * into the returned vector for accessing by the caller. Since
 * the return value is created on the stack, the user needs to
 * save it if they want it to stay around.
 */
std::vector<std::string> CKDataNode::parseIntoChunks( const std::string & aString,
													  const std::string & aDelim )
{
	bool						error = false;
	std::vector<std::string>	retval;

	// first, see if we have anything to do
	if (!error) {
		if (aString.length() <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::parseIntoChunks(const std::string &, "
				"const std::string &) - the length of the source string is 0 and "
				"that means that there's nothing for me to do. Please make sure "
				"that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}
	int		delimLength = 0;
	if (!error) {
		delimLength = aDelim.length();
		if (delimLength <= 0) {
			error = true;
			std::ostringstream	msg;
			msg << "CKDataNode::parseIntoChunks(const std::string &, "
				"const std::string &) - the length of the delimiter string is 0 "
				"and that means that there's nothing for me to do. Please make "
				"sure that the arguments make sense before calling this method.";
			throw CKException(__FILE__, __LINE__, msg.str());
		}
	}

	// now, copy the source to a buffer so I can consume it in the process
	std::string		buff;
	if (!error) {
		buff = aString;
	}

	/*
	 * Now loop picking off the parts bettween the delimiters. Do this by
	 * finding the first delimiter, see if it's located at buff[0], and if
	 * so, then add an empty string to the vector, otherwise, get the
	 * substring up to that delimiter and place it at the end of the vector,
	 * removing it from the buffer as you do this. Then eat up the delimiter
	 * and do it all again. In the end, there will be one more bit and that
	 * will simply be added to the end of the vector.
	 */
	while (!error) {
		// find out wherre, if anyplace, the delimiter sits
		unsigned int	pos = buff.find(aDelim);
		if (pos == std::string::npos) {
			// nothing left to parse out, bail out
			break;
		} else if (pos == 0) {
			// add an empty string to the vector
			retval.push_back(std::string(""));
		} else {
			// pick off the substring up to the delimiter
			retval.push_back(buff.substr(0, pos));
			// ...and then delete them from the buffer
			buff.erase(0, pos);
		}

		// now strip off the delimiter from the buffer
		buff.erase(0, delimLength);
	}
	// if we didn't error out, then add the remaining buff to the end
	if (!error) {
		retval.push_back(buff);
	}

	return retval;
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
