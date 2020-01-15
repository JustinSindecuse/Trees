#ifndef SPLAY_H
#define SPLAY_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include "../bst/bst.h"

/**
* A templated binary search tree implemented as a Splay tree.
*/
template <class Key, class Value>
class SplayTree : public BinarySearchTree<Key, Value>
{
public:
	// Methods for inserting/removing elements from the tree. You must implement
	// both of these methods.
	SplayTree();
	virtual void insert(const std::pair<Key, Value>& keyValuePair) override;
	void remove(const Key& key);
	int report() const;

private:
	/* You'll need this for problem 5. Stores the total number of inserts where the
	   node was added at level strictly worse than 2*log n (n is the number of nodes
	   including the added node. The root is at level 0). */
	int badInserts;
	int numNodes;
	int splayer(Node<Key, Value>* x, int y);

	/* Helper functions are encouraged. */
};

/*
--------------------------------------------
Begin implementations for the SplayTree class.
--------------------------------------------
*/

template<typename Key, typename Value>
SplayTree<Key, Value>::SplayTree() : badInserts(0) { }

template<typename Key, typename Value>
int SplayTree<Key, Value>::report() const {
	return badInserts;
}

/**
* Insert function for a key value pair. Finds location to insert the node and then splays it to the top.
*/
template<typename Key, typename Value>
void SplayTree<Key, Value>::insert(const std::pair<Key, Value>& keyValuePair)
{
	if(this->internalFind(keyValuePair.first) == NULL){
		numNodes++;
	}

	BinarySearchTree<Key,Value>::insert(keyValuePair);
	if(splayer(this->internalFind(keyValuePair.first), 0) > 2*log2(numNodes))
		badInserts++;
}

/**
* Remove function for a given key. Finds the node, reattaches pointers, and then splays the parent
* of the deleted node to the top.
*/
template<typename Key, typename Value>
void SplayTree<Key, Value>::remove(const Key& key)
{
	Node<Key, Value>* holder = this->internalFind(key);
	if(holder == NULL){
		return;
	}
	if(holder->getLeft() == NULL && holder->getRight() == NULL){
		if(holder->getParent() == NULL){
			delete holder;
			this->mRoot = NULL;
		}
		else{
			Node<Key, Value>* holder2 = holder->getParent();
			if(holder2->getRight() == holder){
				holder2->setRight(NULL);
			}
			else{
				holder2->setLeft(NULL);
			}
			delete holder;
			splayer(holder2, 0);
			return;
		}
	}
	else if(holder->getLeft() != NULL && holder->getRight() != NULL){
		Node<Key, Value>* SUCC = holder->getRight();
		while(SUCC->getLeft()){
			SUCC = SUCC->getLeft();
		}
		Node<Key, Value>* succParent = SUCC->getParent();
		if(holder->getParent() == NULL){
			SUCC->setParent(NULL);
			this->mRoot = SUCC;
		}
		else{
			SUCC->setParent(holder->getParent());
		}
		Node<Key, Value>* holder2 = holder->getParent();
		SUCC->setLeft(holder->getLeft());
		if(SUCC->getLeft() != NULL){
			SUCC->getLeft()->setParent(SUCC);
		}
		if(SUCC->getRight() != NULL && holder2 != NULL){
			if(holder2->getLeft() == SUCC){
				holder2->setLeft(SUCC->getRight());
				SUCC->getRight()->setParent(holder2);
			}
		}
		else if(holder2 != NULL){
			if(holder2->getLeft() == SUCC){
				holder2->setLeft(NULL);
			}
		}
		if(holder->getRight() != SUCC){
			SUCC->setRight(holder->getRight());
		}
		delete holder;
		splayer(succParent, 0);
		return;
	}
	else{
		Node<Key, Value>* temp;
		if(holder->getLeft() != NULL){
			temp = holder->getLeft();
		}
		else{
			holder = holder->getRight();
		}
		if(holder->getParent() == NULL){
			this->mRoot = temp;
		}
		else{
			if(holder->getParent()->getLeft() == holder){
				holder->getParent()->setLeft(temp);
			}
			else{
				holder->getParent()->setRight(temp);
			}
		}

		temp->setParent(holder->getParent());
		delete holder;
		if(temp->getParent() != NULL){
			splayer(temp->getParent(), 0);
		}
	}
}


template<typename Key, typename Value>
int SplayTree<Key, Value>::splayer(Node<Key, Value>* aNode, int current)
{
	if(aNode == this->mRoot){
		return current;
	}

	else if(aNode->getParent() == this->mRoot){
		if(aNode->getParent()->getRight() == aNode){
			aNode->getParent()->setRight(aNode->getLeft());
			if(aNode->getLeft() != NULL){
				aNode->getLeft()->setParent(aNode->getParent());
			}
			aNode->setLeft(aNode->getParent());
			aNode->getParent()->setParent(aNode);
			aNode->setParent(NULL);
			this->mRoot = aNode;
			return current + 1;
		}
		else{
			aNode->getParent()->setLeft(aNode->getRight());
			if(aNode->getRight() != NULL){
				aNode->getRight()->setParent(aNode->getParent());
			}
			aNode->setRight(aNode->getParent());
			aNode->getParent()->setParent(aNode);
			aNode->setParent(NULL);
			this->mRoot = aNode;
			return current + 1;
		}
	}

	else if(aNode->getParent()->getLeft() == aNode && aNode->getParent()->getParent()->getLeft() == aNode->getParent()){
		Node<Key, Value>* parent = aNode->getParent();
		Node<Key, Value>* grand = aNode->getParent()->getParent();
		parent->setLeft(aNode->getRight());
		if(aNode->getRight() != NULL){
			aNode->getRight()->setParent(parent);
		}
		grand->setLeft(parent->getRight());
		if(parent->getRight() != NULL){
			parent->getRight()->setParent(grand);
		}
		if(grand == this->mRoot){
			aNode->setParent(NULL);
			this->mRoot = aNode;
		}
		else{
			aNode->setParent(grand->getParent());
			if(grand->getParent()->getLeft() == grand){
				grand->getParent()->setLeft(aNode);
			}
			else{
				grand->getParent()->setRight(aNode);
			}
		}
		parent->setParent(aNode);
		grand->setParent(parent);
		aNode->setRight(parent);
		parent->setRight(grand);
		if(aNode == this->mRoot){
			return current + 2;
		}
		else{
			return splayer(aNode, current + 2);
		}
	}

	else if(aNode->getParent()->getRight() == aNode && aNode->getParent()->getParent()->getRight() == aNode->getParent()){
		Node<Key, Value>* parent = aNode->getParent();
		Node<Key, Value>* grand = aNode->getParent()->getParent();
		parent->setRight(aNode->getLeft());
		if(aNode->getLeft() != NULL){
			aNode->getLeft()->setParent(parent);
		}
		grand->setRight(parent->getLeft());
		if(parent->getLeft() != NULL){
			parent->getLeft()->setParent(grand);
		}
		if(grand == this->mRoot){
			aNode->setParent(NULL);
			this->mRoot = aNode;
		}
		else{
			aNode->setParent(grand->getParent());
			if(grand->getParent()->getRight() == grand){
				grand->getParent()->setRight(aNode);
			}
			else{
				grand->getParent()->setLeft(aNode);
			}
		}
		parent->setParent(aNode);
		grand->setParent(parent);
		aNode->setLeft(parent);
		parent->setLeft(grand);
		if(aNode == this->mRoot){
			return current + 2;
		}
		else{
			return splayer(aNode, current + 2);
		}
	}

	else if(aNode->getParent()->getLeft() == aNode && aNode->getParent()->getParent()->getRight() == aNode->getParent()){
		Node<Key, Value>* parent = aNode->getParent();
		Node<Key, Value>* grand = aNode->getParent()->getParent();
		grand->setRight(aNode->getLeft());
		if(aNode->getLeft() != NULL){
			aNode->getLeft()->setParent(grand);
		}
		parent->setLeft(aNode->getRight());
		if(aNode->getRight() != NULL){
			aNode->getRight()->setParent(parent);
		}
		if(grand == this->mRoot){
			aNode->setParent(NULL);
			this->mRoot = aNode;
		}
		else{
			aNode->setParent(grand->getParent());
			if(grand->getParent()->getRight() == grand){
				grand->getParent()->setRight(aNode);
			}
			else{
				grand->getParent()->setLeft(aNode);
			}
		}
		grand->setParent(aNode);
		parent->setParent(aNode);
		aNode->setRight(parent);
		aNode->setLeft(grand);
		if(aNode == this->mRoot){
			return current + 2;
		}
		else{
			return splayer(aNode, current + 2);
		}
	}

	else if(aNode->getParent()->getLeft() == aNode && aNode->getParent()->getParent()->getRight() == aNode->getParent()){
		Node<Key, Value>* parent = aNode->getParent();
		Node<Key, Value>* grand = aNode->getParent()->getParent();
		grand->setRight(aNode->getLeft());
		if(aNode->getLeft() != NULL){
			aNode->getLeft()->setParent(grand);
		}
		parent->setLeft(aNode->getRight());
		if(aNode->getRight() != NULL){
			aNode->getRight()->setParent(parent);
		}
		if(grand == this->mRoot){
			aNode->setParent(NULL);
			this->mRoot = aNode;
		}
		else{
			aNode->setParent(grand->getParent());
			if(grand->getParent()->getRight() == grand){
				grand->getParent()->setRight(aNode);
			}
			else{
				grand->getParent()->setLeft(aNode);
			}
		}
		grand->setParent(aNode);
		parent->setParent(aNode);
		aNode->setRight(parent);
		aNode->setLeft(grand);
		if(aNode == this->mRoot){
			return current + 2;
		}
		else{
			return splayer(aNode, current + 2);
		}
	}

	else if(aNode->getParent()->getRight() == aNode && aNode->getParent()->getParent()->getLeft() == aNode->getParent()){
		Node<Key, Value>* parent = aNode->getParent();
		Node<Key, Value>* grand = aNode->getParent()->getParent();
		grand->setLeft(aNode->getRight());
		if(aNode->getRight() != NULL){
			aNode->getRight()->setParent(grand);
		}
		parent->setRight(aNode->getLeft());
		if(aNode->getLeft() != NULL){
			aNode->getLeft()->setParent(parent);
		}
		if(grand == this->mRoot){
			aNode->setParent(NULL);
			this->mRoot = aNode;
		}
		else{
			aNode->setParent(grand->getParent());
			if(grand->getParent()->getLeft() == grand){
				grand->getParent()->setLeft(aNode);
			}
			else{
				grand->getParent()->setRight(aNode);
			}
		}
		grand->setParent(aNode);
		parent->setParent(aNode);
		aNode->setLeft(parent);
		aNode->setRight(grand);
		if(aNode == this->mRoot){
			return current + 2;
		}
		else{
			return splayer(aNode, current + 2);
		}
	}
	return current;

}
/*
------------------------------------------
End implementations for the SplayTree class.
------------------------------------------
*/

#endif
