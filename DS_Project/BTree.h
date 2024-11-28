#pragma once
#include <iostream>
#include <queue>
using namespace std;
int ComparisonCount = 0;

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
	void splitNode(BNode<T, order>*& curr, BNode<T, order>*& temp, BNode<T, order>*& root) {
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
					ComparisonCount++;
					queue.push(current->children[i]);
				}cout << " " << current->keys[i] << ' ';
				cout << endl;
			}
			if (current->leaf == false)  //*
			{
				ComparisonCount++;
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
