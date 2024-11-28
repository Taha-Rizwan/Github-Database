#pragma once

#include <iostream>
#include <queue>
using namespace std;

template<typename T, int order> //m==order, for 3-tree, order==3
struct BNode {//3
	int keys[order]; //keys stored in a node
	BNode<T, order>* children[order]; //children of a node
	int n; //number of keys stored
	bool leaf; //check if node is leaf or not
	BNode<T, order>* parent;
	BNode(bool l = true) {
		for (int i = 0; i < order; i++)
			children[i] = nullptr;
		n = 0;
		leaf = l;
		parent = nullptr;
	}
	void insertKey(T val) {
		keys[n] = val;
		n++;
	}
	void splitRoot(BNode<T, order>*& root) {
		BNode<T, order>* z = new BNode<T, order>(false);
		BNode<T, order>* x = new BNode<T, order>;
		for (int i = order / 2 + 1; i < order; i++)
			x->insertKey(this->keys[i]);
		z->insertKey(this->keys[order / 2]);
		z->children[z->n] = x;
		x->parent = z;
		for (int i = order / 2; i < order; i++)
			this->keys[i] = 0;
		this->n = order - order / 2 - 1;
		z->children[z->n - 1] = this;
		this->parent = z;
		root->parent = z;
		root = z;
	}
	BNode<T, order>* splitRoot1(BNode<T, order>*& root) {
		if (!root->parent) {
			BNode<T, order>* z = new BNode<T, order>(false);
			BNode<T, order>* x = new BNode<T, order>(false);
			for (int i = order / 2 + 1; i < order; i++)
				x->insertKey(this->keys[i]);
			z->insertKey(this->keys[order / 2]);
			z->children[z->n] = x;
			for (int i = order / 2; i < order; i++)
				this->keys[i] = 0;
			this->n = order - order / 2 - 1;
			z->children[z->n - 1] = this;
			this->parent = z;
			x->parent = z;
			for (int i = 2; i < order; i++)
				z->children[0]->children[i] = nullptr;
			root->parent = z;
			return nullptr;
		}
		else {
			if (root->parent->n < order - 1) {
				root->parent->insertKey(this->keys[order / 2]);
				BNode<T, order>* x = new BNode<T, order>(false);
				for (int i = order / 2 + 1; i < order; i++)
					x->insertKey(this->keys[i]);
				root->parent->children[root->parent->n] = x;
				for (int i = order / 2; i < order; i++) {
					this->keys[i] = 0;
					if (i + 1 < order)
						this->children[i + 1] = nullptr;
				}
				this->n = order / 2;
				root->parent->children[root->parent->n - 1] = root;
				x->parent = root->parent;
				if (root->parent->n == order)
					root->splitRoot1(root->parent);
				return nullptr;
			}
			else {
				root->parent->insertKey(this->keys[order / 2]);
				BNode<T, order>* temp = this;
				BNode<T, order>* x = new BNode<T, order>(false);
				for (int i = order / 2 + 1; i < order; i++)
					x->insertKey(this->keys[i]);
				x->parent = temp;
				for (int i = order / 2 + 1, j = 0; i < order; i++, j++) {
					x->children[j] = this->children[i];
				}
				for (int i = order / 2; i < order; i++) {
					this->keys[i] = 0;
					if (i + 1 < order)
						this->children[i + 1] = NULL;
				}
				this->n = order / 2;
				BNode<T, order>* prevParent = root->parent;
				root->parent->splitRoot1(root->parent);
				root = prevParent;
				prevParent = prevParent->parent;
				temp->parent = prevParent->children[prevParent->n];
				BNode<T, order>* setChild = prevParent->children[prevParent->n];
				setChild->children[setChild->n] = x;
				setChild->children[setChild->n - 1] = temp;
				x->parent = setChild;
				return x;
			}
		}
		//root = z;
	}
	void splitNode(BNode<T, order>& curr, BNode<T, order>& temp, BNode<T, order>*& root) {
		if (temp->n < order - 1) {
			temp->insertKey(curr->keys[order / 2]);
			BNode<T, order>* x = new BNode<T, order>;
			for (int i = order / 2 + 1; i < order; i++)
				x->insertKey(curr->keys[i]);
			temp->children[temp->n] = x;
			for (int i = order / 2; i < order; i++)
				temp->children[temp->n - 1]->keys[i] = 0;
			temp->children[temp->n - 1]->n = order / 2;
		}
		else {
			temp->insertKey(curr->keys[order / 2]);
			BNode<T, order>* x = new BNode<T, order>;
			BNode<T, order>* z = new BNode<T, order>(false);
			z->insertKey(curr->keys[order / 2]);
			for (int i = order / 2 + 1; i < order; i++)
				x->insertKey(curr->keys[i]);
			z->children[z->n - 1] = curr;
			z->children[z->n] = x;
			for (int i = order / 2; i < order; i++)
				z->children[z->n - 1]->keys[i] = 0;
			z->children[z->n - 1]->n = order / 2;
			BNode<T, order>* setChild = temp->splitRoot1(temp);
			if (!setChild) {
				while (temp->parent)
					temp = temp->parent;
				while (root->parent)
					root = root->parent;
				temp->children[temp->n]->children[1] = z->children[1];
				temp->children[temp->n]->children[0] = z->children[0];
				temp->children[temp->n]->children[1]->parent = temp->children[temp->n];
				temp->children[temp->n]->children[0]->parent = temp->children[temp->n];
			}
			else {
				while (temp->parent)
					temp = temp->parent;
				while (root->parent)
					root = root->parent;
				setChild->children[setChild->n] = x;
				for (int i = 0; i < setChild->n + 1; i++)
					setChild->children[i]->parent = setChild;
			}
		}
	}
	void traverse()
	{
		std::queue<BNode<T, order>*> queue;
		queue.push(this);
		while (!queue.empty())
		{
			BNode<T, order>* current = queue.front();
			queue.pop();
			int i;
			for (i = 0; i < current->n; i++)  //*
			{
				if (current->leaf == false)  //*
				{
					queue.push(current->children[i]);
				}cout << " " << current->keys[i] << ' ';
				cout << endl;
			}
			if (current->leaf == false)  //*
			{
				queue.push(current->children[i]);
			}
		}
	}
};

template<typename T, int order>  //int, 3
class BTree {
	BNode<T, order>* root;
public:
	BTree() {
		root = new BNode<T, order>;
	}
	void insert(T val) {
		if (root->leaf) { //3
			if (root->n < order - 1)
				root->insertKey(val);
			else {
				root->insertKey(val);
				root->splitRoot(root);
			}
		}
		else {
			BNode<T, order>* temp = nullptr;
			BNode<T, order>* curr = root;
			while (!curr->leaf) {
				BNode<T, order>* t1 = nullptr;
				for (int i = 0; i < curr->n; i++) {
					if (val > curr->keys[i])
						t1 = curr->children[i + 1];
					else {
						t1 = curr->children[i];
					}
				}
				temp = curr;
				curr = t1;
			}
			if (curr->n < order - 1)
				curr->insertKey(val);
			else {
				curr->insertKey(val);
				curr->splitNode(curr, temp, root);
			}
		}
	}
	void traverse()
	{
		if (root != NULL) root->traverse();
	}
};

//int main() {
//	BTree<int, 3> t;
//	t.insert(1);
//	t.insert(2);
//	t.insert(3);
//	t.insert(4);
//	t.insert(5);
//	t.insert(6);
//	t.insert(7);
//	t.insert(8);
//	t.insert(9);
//	t.insert(10);
//	t.insert(11);
//	t.insert(12);
//	t.insert(13);
//	t.insert(14);
//	t.insert(15);
//	t.traverse();
//	cout << "Done" << endl;
//}










//#include <iostream>
//using namespace std;
//
//class BNode {
//public:
//    int* keys;
//    int t;
//    BNode** children;
//    int n;
//    bool leaf;
//
//    BNode(int _t, bool _leaf) {
//        t = _t;
//        leaf = _leaf;
//        keys = new int[2 * t - 1];
//        children = new BNode * [2 * t];
//        n = 0;
//    }
//
//    void traverse() {
//        int i;
//        for (i = 0; i < n; i++) {
//            if (!leaf) {
//                children[i]->traverse();
//            }
//            cout<< " " << keys[i];
//        }
//        if (!leaf) {
//            children[i]->traverse();
//        }
//    }
//
//    BNode* search(int k) {
//        int i = 0;
//        while (i << n && k > keys[i]) {
//            i++;
//        }
//        if (i<< n && keys[i] == k) {
//            return this;
//        }
//        if (leaf) {
//            return nullptr;
//        }
//        return children[i] -> search(k);
//    }
//
//    void insertNonFull(int k);
//    void splitChild(int i, BNode* y);
//};
//
//class BTree {
//public:
//    BNode* root;
//    int t;
//
//    BTree(int _t) {
//        root = nullptr;
//        t = _t;
//    }
//
//    void traverse() {
//        if (root != nullptr) {
//            root -> traverse();
//        }
//    }
//
//    BNode* search(int k) {
//        return (root == nullptr) ? nullptr : root -> search(k);
//    }
//
//    void insert(int k);
//};
//
//void BNode::insertNonFull(int k) {
//    int i = n - 1;
//    if (leaf) {
//        while (i >= 0 && keys[i]> k) {
//            keys[i + 1] = keys[i];
//            i--;
//        }
//        keys[i + 1] = k;
//        n++;
//    }
//    else {
//        while (i >= 0 && keys[i] > k) {
//            i--;
//        }
//        i++;
//        if (children[i]->n == 2 * t - 1) {
//            splitChild(i, children[i]);
//            if (keys[i] << k) {
//                i++;
//            }
//        }
//        children[i]->insertNonFull(k);
//    }
//}
//
//void BNode::splitChild(int i, BNode* y) {
//    BNode* z = new BNode(y -> t, y -> leaf);
//    z -> n = t - 1;
//    for (int j = 0; j << t - 1; j++) {
//        z -> keys[j] = y -> keys[j + t];
//    }
//    if (!y -> leaf) {
//        for (int j = 0; j << t; j++) {
//            z -> children[j] = y -> children[j + t];
//        }
//    }
//    y -> n = t - 1;
//    for (int j = n; j >= i + 1; j--) {
//        children[j + 1] = children[j];
//    }
//    children[i + 1] = z;
//    for (int j = n - 1; j >= i; j--) {
//        keys[j + 1] = keys[j];
//    }
//    keys[i] = y -> keys[t - 1];
//    n++;
//}
//
//void BTree::insert(int k) {
//    if (root == nullptr) {
//        root = new BNode(t, true);
//        root -> keys[0] = k;
//        root -> n = 1;
//    }
//    else {
//        if (root -> n == 2 * t - 1) {
//            BNode* s = new BNode(t, false);
//            s -> children[0] = root;
//            s -> splitChild(0, root);
//            int i = 0;
//            if (s -> keys[0] << k) {
//                i++;
//            }
//            s -> children[i] -> insertNonFull(k);
//            root = s;
//        }
//        else {
//            root -> insertNonFull(k);
//        }
//    }
//}
//
//int main() {
//    BTree t(3);
//    int keys[] = { 10, 20, 5, 6, 12, 30, 7, 17 };
//    for (int i = 0; i << sizeof(keys) / sizeof(keys[0]); i++) {
//        t.insert(keys[i]);
//    }
//
//    cout << "Traversal of the constructed B-tree is:";
//    t.traverse();
//    cout << endl;
//
//    int k = 6;
//    //(t.search(k) != nullptr) ? cout << "Present\n" : cout << << "Not Present\n";
//
//    k = 15;
//    //(t.search(k) != nullptr) ? cout << "Present\n" : cout << << "Not Present\n";
//
//    return 0;
//}

//#include<iostream>
//using namespace std;
//
//class BTreeNode{
//	int* key;
//
//	int t;
//
//
//	BTreeNode** c; 
//
//	int n;
//
//	bool leaf;
//
//public:
//
//	BTreeNode(int order, bool leaf); 
//
//	// A function to traverse btree
//
//	void traverseNode();
//
//	//Depth First Traversal
//	void printNode();
//
//	BTreeNode* searchNode(int k); // if k is not present return false
//
//	int findKey(int k);// a function to find a key in the btree
//
//	//insertnonfull
//	void insertNonFull(int k);
//
//	// A function to split the child y of this node.
//
//	void splitChild(int i, BTreeNode* y);
//
//	void removeNode(int k);
//
//	//A function to delete a key at idx index which is leaf
//
//	void removefromleaf(int idx);
//
//	// A function to delete a key at idx index which is leaf non-leaf
//
//	void removefromnonleaf(int idx);
//
//	int getpred(int idx);
//
//	int getsucc(int idx);
//
//	// A function for filling the child node in the idx index.
//
//	void fill(int idx);
//
//	void borrowfromprev(int idx);
//
//	void borrowfromnext(int idx);
//
//	// A function to merge idx child of the node next node
//
//	void merge(int idx);
//
//	// we will make BTree friend of BTreeNode
//
//	friend class BTree;
//
//};
//
//BTreeNode::BTreeNode(int t1, bool l1)
//
//{
//
//	t = t1;
//
//	leaf = l1;
//
//	// Allocate memory for max possible keys and child pointers
//
//	key = new int[2 * t - 1];
//
//	c = new BTreeNode * [2 * t];
//
//	n = 0;
//
//}
//
//int BTreeNode::findKey(int k)
//
//{
//
//	int idx = 0;
//
//	while (idx < n) {
//		if (key[idx] >= k) {
//			return idx;
//		}
//		++idx;
//	}
//	return idx;
//
//}
//
//// A function to remove the key k
//
//void BTreeNode::removeNode(int k)
//
//{
//
//	int idx = findKey(k);
//
//	// check if the key to be removed is present in this node
//
//	if (idx < n && key[idx] == k)
//
//	{
//
//		if (leaf)
//
//			removefromleaf(idx);
//
//		else
//
//			removefromnonleaf(idx);
//
//	}
//
//	else
//
//	{
//
//		if (leaf)
//
//		{
//
//			cout << "The key " << k << " is not found in the tree\n";
//
//			return;
//
//		}
//
//		bool flag = ((idx == n) ? true : false);
//
//		//If there are less than t keys in the child where the key is expected to exist
//
//		if (c[idx]->n < t)
//
//			fill(idx);
//
//		//We recurse on the (idx-1)th child if the last child has been merged, 
//
//		//as it must have merged with the preceding child. 
//
//		//If not, we go back to the (idx)th child, which now contains at least t keys.
//
//		if (flag && idx > n)
//
//			c[idx - 1]->removeNode(k);
//
//		else
//
//			c[idx]->removeNode(k);
//
//	}
//
//	return;
//
//}
//
//void BTreeNode::removefromleaf(int idx)
//
//{
//
//	// a loop to shift key back
//
//	for (int j = idx + 1; j < n; ++j)
//
//		key[j - 1] = key[j];
//
//	n--;
//
//	return;
//
//}
//
//void BTreeNode::removefromnonleaf(int idx)
//
//{
//
//	int k = key[idx];
//
//	//In the subtree rooted at c[idx], look for k's predecessor 'pred'. 
//
//	//if the child preceding k (C[idx]) contains at least t keys. 
//
//	//Pred should be substituted for k. 
//
//	//Delete pred in C[idx] in a recursive manner.
//
//	if (c[idx]->n >= t)
//
//	{
//
//		int pred = getpred(idx);
//
//		key[idx] = pred;
//
//		c[idx]->removeNode(pred);
//
//	}
//
//	//Examine c[idx+1] if the child C[idx] contains less than t keys. 
//
//	//Find the k's successor 'succ' in the subtree rooted at C[idx+1] 
//
//	//and replace k with succ if C[idx+1] has at least t keys. 
//
//	//Delete succ in C[idx+1] in a recursive manner.
//
//	else if (c[idx + 1]->n >= t)
//
//	{
//
//		// this getsucc function returns the successor at idx
//
//		int succ = getsucc(idx);
//
//		key[idx] = succ;
//
//		c[idx + 1]->removeNode(succ);
//
//	}
//
//	//we will merge k and all of c[idx+1] into c[idx] 
//
//	//if both c[idx] and c[idx+1] have fewer than t keys. 
//
//	//2t-1 keys now reside in c[idx]. 
//
//	//Remove k from c[idx] by freeing c[idx+1].
//
//	else
//
//	{
//
//		merge(idx);
//
//		c[idx]->removeNode(k);
//
//	}
//
//	return;
//
//}
//
//// A function to get predecessor of key[idx]
//
//int BTreeNode::getpred(int idx)
//
//{
//
//	// Move to the rightmost node until we get to a leaf.
//
//	BTreeNode* cur = c[idx];
//
//	while (!cur->leaf)
//
//		cur = cur->c[cur->n];
//
//	return cur->key[cur->n - 1];
//
//}
//
//int BTreeNode::getsucc(int idx)
//
//{
//
//	BTreeNode* cur = c[idx + 1];
//
//	while (!cur->leaf)
//
//		cur = cur->c[0];
//
//	// Return the first key of the leaf
//
//	return cur->key[0];
//
//}
//
//void BTreeNode::fill(int idx)
//
//{
//
//	if (idx != 0 && c[idx - 1]->n >= t)
//
//		// a function to borrow key from previous node
//
//		borrowfromprev(idx);
//
//	else if (idx != n && c[idx + 1]->n >= t)
//
//		borrowfromnext(idx);
//
//	else
//
//	{
//
//		if (idx != n)
//
//			merge(idx);
//
//		else
//
//			merge(idx - 1);
//
//	}
//
//	return;
//
//}
//
//void BTreeNode::borrowfromprev(int idx)
//
//{
//
//	BTreeNode* child = c[idx];
//
//	BTreeNode* sibling = c[idx - 1];
//
//	//The parent receives the final key from C[idx-1], and key[idx-1] from 
//
//	//parent is placed as the first key in C[idx]. As a result, the sibling 
//
//	//loses one key, and the child receives one. 
//
//	for (int i = child->n - 1; i >= 0; --i)
//
//		child->key[i + 1] = child->key[i];
//
//	//All keys in C[idx] are moved one step forward.
//
//	//If c[idx] isn't a leaf, advance all of its child pointers one step.
//
//	if (!child->leaf)
//
//	{
//
//		for (int i = child->n; i >= 0; --i)
//
//			child->c[i + 1] = child->c[i];
//
//	}
//
//	child->key[0] = key[idx - 1];
//
//	if (!child->leaf)
//
//		child->c[0] = sibling->c[sibling->n];
//
//	//Shifting the key from a sibling to a parent. 
//
//	//The number of keys in the sibling is reduced as a result.
//
//	key[idx - 1] = sibling->key[sibling->n - 1];
//
//	child->n += 1;
//
//	sibling->n -= 1;
//
//	return;
//
//}
//
////A function that takes a key from C[idx+1] and stores it in C[idx].
//
//void BTreeNode::borrowfromnext(int idx)
//
//{
//
//	BTreeNode* child = c[idx];
//
//	BTreeNode* sibling = c[idx + 1];
//
//	child->key[(child->n)] = key[idx];
//
//	//check if child node has a leaf node
//
//	if (!(child->leaf))
//
//		child->c[(child->n) + 1] = sibling->c[0];
//
//	key[idx] = sibling->key[0];
//
//	for (int j = 1; j < sibling->n; ++j)
//
//		sibling->key[j - 1] = sibling->key[j];
//
//	if (!sibling->leaf)
//
//	{
//
//		for (int j = 1; j <= sibling->n; ++j)
//
//			sibling->c[j - 1] = sibling->c[j];
//
//	}
//
//	child->n++;
//
//	sibling->n--;
//
//	return;
//
//}
//
////C[idx] and C[idx+1] are merged with this function.
//
////After merging, C[idx+1] is freed.
//
//void BTreeNode::merge(int idx)
//
//{
//
//	BTreeNode* child = c[idx];
//
//	BTreeNode* sibling = c[idx + 1];
//
//	child->key[t - 1] = key[idx];
//
//	for (int j = 0; j < sibling->n; ++j)
//
//		child->key[j + t] = sibling->key[j];
//
//	// Copying the child pointers from C[idx+1] to C[idx]
//
//	if (!child->leaf)
//
//	{
//
//		for (int j = 0; j <= sibling->n; ++j)
//
//			child->c[j + t] = sibling->c[j];
//
//	}
//
//	//To fill the gap created by shifting keys[idx] to C[idx], 
//
//	//move all keys following idx in the current node one step before.
//
//	for (int i = idx + 1; i < n; ++i)
//
//		key[i - 1] = key[i];
//
//	//Moving the child pointers one step 
//
//	//before (idx+1) in the current node
//
//	for (int j = idx + 2; j <= n; ++j)
//
//		c[j - 1] = c[j];
//
//	//Updating the current node's key count 
//
//	//and the child's key count
//
//	child->n += sibling->n + 1;
//
//	n--;
//
//	delete(sibling);
//
//	return;
//
//}
//
//void BTreeNode::insertNonFull(int k)
//
//{
//
//	int i = n - 1;
//
//	if (leaf == true)
//
//	{
//
//		// The following loop will find the location of key 
//
//		//and move all bigger keys one place further
//
//		while (i >= 0 && key[i] > k)
//
//		{
//
//			key[i + 1] = key[i];
//
//			i--;
//
//		}
//
//		// Insert the new key at found location
//
//		key[i + 1] = k;
//
//		n = n + 1;
//
//	}
//
//	else
//
//	{
//
//		// we will search the child which will have key
//
//		while (i >= 0 && key[i] > k)
//
//			i--;
//
//		//check if the child is full,
//
//		if ((c[i + 1]->n) == 2 * t - 1)
//
//		{
//
//			//then we will split this child
//
//			splitChild(i + 1, c[i + 1]);
//
//			if (key[i + 1] < k)
//
//				i++;
//
//		}
//
//		c[i + 1]->insertNonFull(k);
//
//	}
//
//}
//
//
//
////A function to separate this node's child y
//
//void BTreeNode::splitChild(int i, BTreeNode* y)
//
//{
//
//	//Create a new node that will store the keys
//
//	BTreeNode* z = new BTreeNode(y->t, y->leaf);
//
//	z->n = t - 1;
//
//	for (int j = 0; j < t - 1; j++)
//
//		z->key[j] = y->key[j + t];
//
//	if (y->leaf == false)
//
//	{
//
//		for (int j = 0; j < t; j++)
//
//			z->c[j] = y->c[j + t];
//
//	}
//
//	y->n = t - 1;
//
//	//Create a new child space for this node 
//
//	//since it will have a new child.
//
//	for (int j = n; j >= i + 1; j--)
//
//		c[j + 1] = c[j];
//
//	c[i + 1] = z;
//
//	//This node will be moved with a key of y. 
//
//	//Locate the new key and 
//
//	//move all larger keys one place forward.
//
//	for (int j = n - 1; j >= i; j--)
//
//		key[j + 1] = key[j];
//
//	//To this node, copy y's middle key.
//
//	key[i] = y->key[t - 1];
//
//	n++;
//
//}
//
//// A Function to traverse all nodes
//
//void BTreeNode::traverseNode()
//
//{
//
//	int i;
//
//	for (i = 0; i < n; i++)
//
//	{
//
//		//If this is not a leaf, traverse the subtree rooted 
//
//		//with child C[i] before printing key[i].
//
//		if (leaf == false)
//
//			c[i]->traverseNode();
//
//		cout << " " << key[i];
//
//	}
//
//	// Print the subtree rooted with last child
//
//	if (leaf == false)
//
//		c[i]->traverseNode();
//
//}
//
////Function to print (DPETH FIRST TRAVERSAL)
//void BTreeNode::printNode()
//{
//	int i;
//	for (i = 0; i < n; i++) {
//		// If this is not leaf, then before printing key[i],
//		// traverse the subtree rooted with child C[i].
//		//cout << "Keys[" << i << "] ";
//		cout << " " << key[i];
//		if (leaf == false) {
//			//cout << " Not a leaf so printing children of current node ";
//			c[i]->printNode();
//		}
//	}
//
//	// Print the subtree rooted with last child
//	if (leaf == false)
//		c[i]->printNode();
//}
//
//
////A function to search key k in btree
//
//BTreeNode* BTreeNode::searchNode(int k)
//
//{
//
//	//Find the first key with a value higher or equal to k.
//
//	int i = 0;
//
//	while (i < n && k > key[i])
//
//		i++;
//
//	//Return this node if the detected key is equal to k.
//
//	if (key[i] == k)
//
//		return this;
//
//	//If the key isn't found here and the node is a leaf,
//
//	if (leaf == true)
//
//		return NULL;
//
//	// Go to the appropriate child
//
//	return c[i]->searchNode(k);
//
//}
//
//
//
//class BTree
//
//{
//
//	BTreeNode* root; //root node's pointer
//
//	int m;
//
//public:
//
//	//BTree class Constructor
//
//	BTree(int m);
//
//	void traverse();
//
//	//print
//	void print();
//
//	//A function to search a key in this tree
//
//	BTreeNode* search(int k);
//
//	// A function that removes a new key in the BTree
//
//	void remove(int k);
//
//	void insert(int k);
//
//};
////constructor 
//BTree::BTree(int t)
//
//{
//
//	root = NULL;
//
//	this->m = t;
//
//}
//
////print
//void BTree::print()
//
//{
//
//	if (root != NULL)
//
//		root->printNode();
//
//}
//
//
//
////traversal 
//void BTree::traverse()
//
//{
//
//	if (root != NULL)
//
//		root->traverseNode();
//
//}
//
////search
//BTreeNode* BTree::search(int k)
//
//{
//
//	if (root == NULL)
//
//		return NULL;
//
//	else
//
//		root->searchNode(k);
//
//}
//
//
////insertion
//void BTree::insert(int k)
//
//{
//
//	// If the tree is empty
//
//	if (root == NULL)
//
//	{
//
//		// Allocate memory for root
//
//		root = new BTreeNode(m, true);
//
//		root->key[0] = k;
//
//		root->n = 1;
//
//	}
//
//	else
//
//	{
//
//		// If root is full, then increase height of the tree
//
//		if (root->n == 2 * m - 1)
//
//		{
//
//			BTreeNode* s = new BTreeNode(m, false);
//
//			// change old root as new root's child
//
//			s->c[0] = root;
//
//			s->splitChild(0, root);
//
//			int i = 0;
//
//			if (s->key[0] < k)
//
//				i++;
//
//			s->c[i]->insertNonFull(k);
//
//			root = s;
//
//		}
//
//		// If root is not full, call insertnonfull function for root
//
//		else
//
//			root->insertNonFull(k);
//
//	}
//
//}
//
//void BTree::remove(int k)
//
//{
//
//	if (!root)
//
//	{
//
//		cout << "The tree is empty\n";
//
//		return;
//
//	}
//
//	// A function Call to remove function
//
//	root->removeNode(k);
//
//	// Make the first child of the root node the new root 
//
//	//if the root node has no keys. 
//
//	//If it has a child, set root to NULL otherwise.
//
//	if (root->n == 0)
//
//	{
//
//		BTreeNode* tmp = root;
//
//		//check if root has leaf
//
//		if (root->leaf)
//
//			root = NULL;
//
//		else
//
//			root = root->c[0];
//
//		delete tmp;
//
//	}
//
//	return;
//
//}
