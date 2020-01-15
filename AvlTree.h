#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>
#include "../bst/bst.h"

/**
* A special kind of node for an AVL tree, which adds the height as a data member, plus 
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
	// Constructor/destructor.
	AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
	virtual ~AVLNode();

	// Getter/setter for the node's height.
	int getHeight() const;
	void setHeight(int height);

	// Getters for parent, left, and right. These need to be redefined since they 
	// return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
	// for more information.
	virtual AVLNode<Key, Value>* getParent() const override;
	virtual AVLNode<Key, Value>* getLeft() const override;
	virtual AVLNode<Key, Value>* getRight() const override;

protected:
	int mHeight;
};

/*
--------------------------------------------
Begin implementations for the AVLNode class.
--------------------------------------------
*/

/**
* Constructor for an AVLNode. Nodes are initialized with a height of 0.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent)
	: Node<Key, Value>(key, value, parent)
	, mHeight(0)
{

}

/**
* Destructor.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* Getter function for the height. 
*/
template<typename Key, typename Value>
int AVLNode<Key, Value>::getHeight() const
{
	return mHeight;
}

/**
* Setter function for the height. 
*/
template<typename Key, typename Value>
void AVLNode<Key, Value>::setHeight(int height)
{
	mHeight = height;
}

/**
* Getter function for the parent. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getParent() const
{
	return static_cast<AVLNode<Key,Value>*>(this->mParent);
}

/**
* Getter function for the left child. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getLeft() const
{
	return static_cast<AVLNode<Key,Value>*>(this->mLeft);
}

/**
* Getter function for the right child. Used since the node inherits from a base node.
*/
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLNode<Key, Value>::getRight() const
{
	return static_cast<AVLNode<Key,Value>*>(this->mRight);
}

/*
------------------------------------------
End implementations for the AVLNode class.
------------------------------------------
*/

/**
* A templated balanced binary search tree implemented as an AVL tree.
*/
template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
	// Methods for inserting/removing elements from the tree. You must implement
	// both of these methods. 
	virtual void insert(const std::pair<Key, Value>& keyValuePair) override;
	void remove(const Key& key);

private:
	bool isBalanced(AVLNode<Key, Value>* x);
	int getBalance(AVLNode<Key, Value>* y);
	void balance(AVLNode<Key, Value>* z);
	void leftLeft(AVLNode<Key, Value>* a);
	void leftRight(AVLNode<Key, Value>* b);
	void rightRight(AVLNode<Key, Value>* c);
	void rightLeft(AVLNode<Key, Value>* d);

	/* Helper functions are strongly encouraged to help separate the problem
	   into smaller pieces. You should not need additional data members. */
};

/*
--------------------------------------------
Begin implementations for the AVLTree class.
--------------------------------------------
*/

/**
* Insert function for a key value pair. Finds location to insert the node and then balances the tree. 
*/
template<typename Key, typename Value>
void AVLTree<Key, Value>::insert(const std::pair<Key, Value>& keyValuePair)
{
	//if there's no root, set new node as root
	if(this->mRoot == NULL){
		AVLNode<Key, Value>* tempnode = new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, NULL);
		this->mRoot = tempnode;
		tempnode->setHeight(1);
		return;
	}
	AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->mRoot);
	//inserts node appropriately
	while(1){
		if(temp->getKey() == keyValuePair.first){
			temp->setValue(keyValuePair.second);
			break;
		}
		else if(keyValuePair.first > temp->getKey() && temp->getRight() != NULL){
			temp = temp->getRight();
		}
		else if(keyValuePair.first > temp->getKey()){
			temp->setRight(new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, temp));
			temp->getRight()->setHeight(1);
			temp = temp->getRight();
			break;
		}
		else if(keyValuePair.first < temp->getKey() && temp->getLeft() != NULL){
			temp = temp->getLeft();
		}
		else if(keyValuePair.first < temp->getKey()){
			temp->setLeft(new AVLNode<Key, Value>(keyValuePair.first, keyValuePair.second, temp));
			temp->getLeft()->setHeight(1);
			temp = temp->getLeft();
			break;
		}
	}
	AVLNode<Key, Value>* traveler = temp;
	//readjusts heights
	while(temp != this->mRoot){
		if(temp->getHeight() + 1 > temp->getParent()->getHeight()){
			temp->getParent()->setHeight(temp->getHeight() + 1);
			temp = temp->getParent();
		}
		else{
			temp = temp->getParent();
		}
	}
	//finds unbalanced node
	while(traveler != this->mRoot){
		if(isBalanced(traveler->getParent())){
			traveler = traveler->getParent();
		}
		else{
			balance(traveler->getParent());
			break;
		}
	}
}

/**
* Remove function for a given key. Finds the node, reattaches pointers, and then balances when finished. 
*/
template<typename Key, typename Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
	AVLNode<Key, Value>* aNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
	if(aNode == NULL){
		return;
	}


	//two children case
	else if(aNode->getRight() != NULL && aNode->getLeft() != NULL){
		AVLNode<Key, Value>* aSuccessor = aNode->getRight();
		AVLNode<Key, Value>* aParent;
		AVLNode<Key, Value>* traveler;
		while(aSuccessor->getLeft()){
			aSuccessor = aSuccessor->getLeft();
		} 
		//set parent's child to successor
		if(aNode->getParent() == NULL){
			this->mRoot = aSuccessor;
			aSuccessor->setParent(NULL);
		}
		else{
			if(aNode->getParent()->getRight() == aNode){
				aNode->getParent()->setRight(aSuccessor);
			}
			else{
				aNode->getParent()->setLeft(aSuccessor);
			}
			aSuccessor->setParent(aNode->getParent());
		}

		aSuccessor->setLeft(aNode->getLeft());
		if(aSuccessor->getLeft() != NULL){
			aSuccessor->getLeft()->setParent(aSuccessor);
		}

		if(aSuccessor->getRight() != NULL){
			if(aSuccessor->getParent()->getLeft() == aSuccessor){
				aSuccessor->getParent()->setLeft(aSuccessor->getRight());
				aSuccessor->getRight()->setParent(aSuccessor->getParent());
			}
		}
		else{
			if(aSuccessor->getParent()->getLeft() == aSuccessor)
				aSuccessor->getParent()->setLeft(NULL);
		}

		delete aNode;


		aParent = aSuccessor;
		traveler = aSuccessor;





		//height readjustment
		while(aParent != this->mRoot){
			if(aParent->getHeight() + 1 > aParent->getParent()->getHeight()){
				aParent->getParent()->setHeight(aParent->getHeight() + 1);
				aParent = aParent->getParent();
			}
			else{
				aParent = aParent->getParent();
			}
		}






		//check for balance
		while(traveler != NULL){
			if(isBalanced(traveler)){
				traveler = traveler->getParent();
			}
			else{
				balance(traveler);
				traveler = traveler->getParent();
			}
		}
	}



	//one child case
	else if(aNode->getRight() != NULL || aNode->getLeft() != NULL){
		AVLNode<Key, Value>* aBrother;
		if(aNode->getRight() != NULL){
			aBrother = aNode->getRight();
		}
		else
			aBrother = aNode->getLeft();

		AVLNode<Key, Value>* aParent = aBrother;
		AVLNode<Key, Value>* traveler = aParent;

		if(aNode->getParent() == NULL){
			this->mRoot = aBrother;
			aBrother->setParent(NULL);
		}
		else{
			if(aNode->getParent()->getRight() == aNode){
				aNode->getParent()->setRight(aBrother);
			}
			else{
				aNode->getParent()->setLeft(aBrother);
			}
			aBrother->setParent(aNode->getParent());
		}
		delete aNode;





		//height adjustment
		if(aParent->getRight() != NULL){
			aParent->setHeight(aParent->getRight()->getHeight() + 1);
		}
		else if(aParent->getLeft() != NULL){
			aParent->setHeight(aParent->getLeft()->getHeight() + 1);
		}
		else
			aParent->setHeight(1);

		while(aParent != this->mRoot){
			if(aParent->getHeight() + 1 > aParent->getParent()->getHeight()){
				aParent->getParent()->setHeight(aParent->getHeight() + 1);
				aParent = aParent->getParent();
			}
			else{
				aParent = aParent->getParent();
			}
		}




		//check for balance
		while(traveler != NULL){
			if(isBalanced(traveler)){
				traveler = traveler->getParent();
			}
			else{
				balance(traveler);
				traveler = traveler->getParent();
			}
		}

	}





	//no child case :(
	else{
		if(aNode == this->mRoot){
			delete aNode;
			this->mRoot = NULL;
		}
		else{
			AVLNode<Key, Value>* aParent = static_cast<AVLNode<Key, Value>*>(aNode->getParent());
			AVLNode<Key, Value>* traveler = aParent;
			if(aNode->getParent()->getRight() == aNode){
				aNode->getParent()->setRight(NULL);
			}
			else{
				aNode->getParent()->setLeft(NULL);
			}
			delete aNode;




			//height adjustment
			if(aParent->getRight() != NULL){
				aParent->setHeight(aParent->getRight()->getHeight() + 1);
			}
			else if(aParent->getLeft() != NULL){
				aParent->setHeight(aParent->getLeft()->getHeight() + 1);
			}
			else
				aParent->setHeight(1);

			while(aParent != this->mRoot){
				if(aParent->getHeight() + 1 > aParent->getParent()->getHeight()){
					aParent->getParent()->setHeight(aParent->getHeight() + 1);
					aParent = aParent->getParent();
				}
				else{
					aParent = aParent->getParent();
				}
			}




			//check for balance
			while(traveler != NULL){
				if(isBalanced(traveler)){
					traveler = traveler->getParent();
				}
				else{
					balance(traveler);
					traveler = traveler->getParent();
				}
			}

		}
	}
}

template<typename Key, typename Value>
int AVLTree<Key, Value>::getBalance(AVLNode<Key, Value>* testNode)  
{
	if(testNode == NULL){
    	return 0;
    }
	if(testNode->getRight() == NULL && testNode->getLeft() == NULL){
		return 0;
	}
	else if(testNode->getRight() == NULL){
		return testNode->getLeft()->getHeight();
	}
	else if(testNode->getLeft() == NULL){
		return testNode->getRight()->getHeight() * -1;
	}
    else{
    	return testNode->getLeft()->getHeight() - testNode->getRight()->getHeight();
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::balance(AVLNode<Key, Value>* badNode){
	int nodeBalance = getBalance(badNode);
	if(nodeBalance > 1){
		int leftHeight;
		int rightHeight;
		if(badNode->getLeft()->getLeft() == NULL){
			leftHeight = 0;
		}
		else
			leftHeight = badNode->getLeft()->getLeft()->getHeight();
		if(badNode->getLeft()->getRight() == NULL){
			rightHeight = 0;
		}
		else
			rightHeight = badNode->getLeft()->getRight()->getHeight();
		
		//LeftLeft Case
		if(leftHeight >= rightHeight){
			leftLeft(badNode);
		}
		//LeftRight Case
		else if(leftHeight < rightHeight){
			leftRight(badNode);
		}
	}

	else if(nodeBalance < -1){
		int leftHeight;
		int rightHeight;
		if(badNode->getRight()->getLeft() == NULL){
			leftHeight = 0;
		}
		else
			leftHeight = badNode->getRight()->getLeft()->getHeight();
		if(badNode->getRight()->getRight() == NULL){
			rightHeight = 0;
		}
		else
			rightHeight = badNode->getRight()->getRight()->getHeight();
		
		//rightleft case
		if(leftHeight >= rightHeight){
			rightLeft(badNode);
		}
		//rightright case
		else if(leftHeight < rightHeight){
			rightRight(badNode);
		}
	}
}

//Perform appropriate rotation for the Left Left case
template<typename Key, typename Value>
void AVLTree<Key, Value>::leftLeft(AVLNode<Key, Value>* badNode){
    AVLNode<Key, Value>* holder1 = badNode->getLeft();

    //Rotate Nodes
    if(badNode->getParent() != NULL){
    	if(badNode->getParent()->getLeft() == badNode){
    		badNode->getParent()->setLeft(holder1);
    	}
    	else
    		badNode->getParent()->setRight(holder1);
    }
    else
    	this->mRoot = holder1;
    holder1->setParent(badNode->getParent());

    badNode->setLeft(holder1->getRight());
    if(holder1->getRight() != NULL)
		holder1->getRight()->setParent(badNode);

    holder1->setRight(badNode);
    badNode->setParent(holder1);

    //set their new Heights
    int badTemp;
    int badTemp1;
    int holderTemp;
    int holderTemp1;
    if(badNode->getLeft() == NULL)
    	badTemp = 0;
    else
    	badTemp = badNode->getLeft()->getHeight();
    if(badNode->getRight() == NULL)
    	badTemp1 = 0;
    else{
    	badTemp1 = badNode->getRight()->getHeight();
    }
	badNode->setHeight(std::max(badTemp, badTemp1) + 1);

    if(holder1->getLeft() == NULL)
    	holderTemp = 0;
    else
    	holderTemp = holder1->getLeft()->getHeight();
    if(holder1->getRight() == NULL)
    	holderTemp1 = 0;
    else
    	holderTemp1 = holder1->getRight()->getHeight();
    holder1->setHeight(std::max(holderTemp, holderTemp1) + 1);


	AVLNode<Key, Value>* temp = holder1;
	while(temp != this->mRoot){
		if(temp->getParent()->getLeft() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else if(temp->getParent()->getRight() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else{
			temp->getParent()->setHeight(std::max(temp->getParent()->getLeft()->getHeight()+1, temp->getParent()->getRight()->getHeight()+1));
		}
		temp = temp->getParent();
	}    
}

//Perform appropriate rotation for Right Right case
template<typename Key, typename Value>
void AVLTree<Key, Value>::rightRight(AVLNode<Key, Value>* badNode){
    AVLNode<Key, Value>* holder1 = badNode->getRight();

    //Rotate Nodes
    if(badNode->getParent() != NULL){
    	if(badNode->getParent()->getLeft() == badNode){
    		badNode->getParent()->setLeft(holder1);
    	}
    	else
    		badNode->getParent()->setRight(holder1);
    }
    else
    	this->mRoot = holder1;
    holder1->setParent(badNode->getParent());
    badNode->setRight(holder1->getLeft());
    if(holder1->getLeft() != NULL)
		holder1->getLeft()->setParent(badNode);
    holder1->setLeft(badNode);
    badNode->setParent(holder1);

    //Set their new Heights
	int badTemp;
    int badTemp1;
    int holderTemp;
    int holderTemp1;
    if(badNode->getLeft() == NULL)
    	badTemp = 0;
    else
    	badTemp = badNode->getLeft()->getHeight();
    if(badNode->getRight() == NULL)
    	badTemp1 = 0;
    else
    	badTemp1 = badNode->getRight()->getHeight();

    badNode->setHeight(std::max(badTemp, badTemp1) + 1);

    if(holder1->getLeft() == NULL)
    	holderTemp = 0;
    else
    	holderTemp = holder1->getLeft()->getHeight();
    if(holder1->getRight() == NULL)
    	holderTemp1 = 0;
    else
    	holderTemp1 = holder1->getRight()->getHeight();
    
    
    holder1->setHeight(std::max(holderTemp, holderTemp1) + 1);    

    AVLNode<Key, Value>* temp = holder1;
	while(temp != this->mRoot){
		if(temp->getParent()->getLeft() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else if(temp->getParent()->getRight() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else
			temp->getParent()->setHeight(std::max(temp->getParent()->getLeft()->getHeight()+1, temp->getParent()->getRight()->getHeight()+1));
		temp = temp->getParent();
	}
}

//Perform appropriate rotation for Right Left case
template<typename Key, typename Value>
void AVLTree<Key, Value>::rightLeft(AVLNode<Key, Value>* badNode){ 
    AVLNode<Key, Value>* holder1 = badNode->getRight();
    AVLNode<Key, Value>* holder2 = holder1->getLeft();

    //Rotate Nodes
    if(badNode->getParent() != NULL){
    	if(badNode->getParent()->getLeft() == badNode){
    		badNode->getParent()->setLeft(holder2);
    	}
    	else
    		badNode->getParent()->setRight(holder2);
    	holder2->setParent(badNode->getParent());
    }
    else {
    	this->mRoot = holder2;
    	holder2->setParent(NULL);
    }
    badNode->setRight(holder2->getLeft());
    if(holder2->getLeft() != NULL){
    	holder2->getLeft()->setParent(badNode);
    }
    holder1->setLeft(holder2->getRight());
    if(holder2->getRight() != NULL){
    	holder2->getRight()->setParent(holder1);
    }
    holder2->setRight(holder1);
    holder2->setLeft(badNode);
    holder1->setParent(holder2);
    badNode->setParent(holder2);

    //Set their new Heights
    int badTemp;
    int badTemp1;
    int holderTemp;
    int holderTemp1;
    if(badNode->getLeft() == NULL)
    	badTemp = 0;
    else
    	badTemp = badNode->getLeft()->getHeight();
    if(badNode->getRight() == NULL)
    	badTemp1 = 0;
    else
    	badTemp1 = badNode->getRight()->getHeight();

    badNode->setHeight(std::max(badTemp, badTemp1) + 1);

    if(holder1->getLeft() == NULL)
    	holderTemp = 0;
    else
    	holderTemp = holder1->getLeft()->getHeight();
    if(holder1->getRight() == NULL)
    	holderTemp1 = 0;
    else
    	holderTemp1 = holder1->getRight()->getHeight();
    
    
    holder1->setHeight(std::max(holderTemp, holderTemp1) + 1);
    holder2->setHeight(std::max(holder2->getLeft()->getHeight(), holder2->getRight()->getHeight()) + 1);

    AVLNode<Key, Value>* temp = holder2;
	while(temp != this->mRoot){
		if(temp->getParent()->getLeft() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else if(temp->getParent()->getRight() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else
			temp->getParent()->setHeight(std::max(temp->getParent()->getLeft()->getHeight()+1, temp->getParent()->getRight()->getHeight()+1));
		temp = temp->getParent();
	}
}

//Perform appropriate rotation for Left Right case
template<typename Key, typename Value>
void AVLTree<Key, Value>::leftRight(AVLNode<Key, Value>* badNode){
    AVLNode<Key, Value>* holder1 = badNode->getLeft();
    AVLNode<Key, Value>* holder2 = holder1->getRight();

    //Rotate Nodes
    if(badNode->getParent() != NULL){
    	if(badNode->getParent()->getLeft() == badNode){
    		badNode->getParent()->setLeft(holder2);
    	}
    	else
    		badNode->getParent()->setRight(holder2);
    	holder2->setParent(badNode->getParent());
    }
    else {
    	this->mRoot = holder2;
    	holder2->setParent(NULL);
    }
    badNode->setLeft(holder2->getRight());
    if(holder2->getRight() != NULL){
    	holder2->getRight()->setParent(badNode);
    }
    holder1->setRight(holder2->getLeft());
    if(holder2->getLeft() != NULL){
    	holder2->getLeft()->setParent(holder1);
    }
    holder2->setLeft(holder1);
    holder2->setRight(badNode);
    holder1->setParent(holder2);
    badNode->setParent(holder2);

    //Set their new Heights
    int badTemp;
    int badTemp1;
    int holderTemp;
    int holderTemp1;
    if(badNode->getLeft() == NULL)
    	badTemp = 0;
    else
    	badTemp = badNode->getLeft()->getHeight();
    if(badNode->getRight() == NULL)
    	badTemp1 = 0;
    else
    	badTemp1 = badNode->getRight()->getHeight();

    badNode->setHeight(std::max(badTemp, badTemp1) + 1);

    if(holder1->getLeft() == NULL)
    	holderTemp = 0;
    else
    	holderTemp = holder1->getLeft()->getHeight();
    if(holder1->getRight() == NULL)
    	holderTemp1 = 0;
    else
    	holderTemp1 = holder1->getRight()->getHeight();
    
    
    holder1->setHeight(std::max(holderTemp, holderTemp1) + 1);
    holder2->setHeight(std::max(holder2->getLeft()->getHeight(), holder2->getRight()->getHeight()) + 1);
    AVLNode<Key, Value>* temp = holder2;
	while(temp != this->mRoot){
		if(temp->getParent()->getLeft() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else if(temp->getParent()->getRight() == NULL)
			temp->getParent()->setHeight(temp->getHeight() + 1);
		else
			temp->getParent()->setHeight(std::max(temp->getParent()->getLeft()->getHeight()+1, temp->getParent()->getRight()->getHeight()+1));
		temp = temp->getParent();
	}
}

template<typename Key, typename Value>
bool AVLTree<Key, Value>::isBalanced(AVLNode<Key, Value>* testNode)  
{

	int holder = getBalance(testNode);
	if(holder == 1 || holder == 0 || holder == -1){
		return true;
	}
	else{
		return false;
	}
}
/*
------------------------------------------
End implementations for the AVLTree class.
------------------------------------------
*/

#endif
