#pragma once
#pragma once
#include<iostream>
#include<filesystem>
#include<fstream>
#include<string>
#include<cmath>
#include<queue>
#include"Tree.h"
namespace fs = std::filesystem;
using namespace std;




template<class T>
class AVLNode {
public:
	AVLNode<T>* left;
	AVLNode<T>* right;
	AVLNode<T>* parent;
	T data;
	int height;
	string hash;
	string fileName;
	AVLNode(T c) :data(c), left(nullptr), right(nullptr), parent(nullptr), height(0),hash("") {
		fileName = to_string_generic(data) + ".txt";
	}

};
template<class T>
class AVL :public Tree<T>{
public:
	AVLNode<T>* root;
	int nNodes;
	Repository<T> repo;
	AVL() :root(nullptr), nNodes(0), repo(this) {
		repo.create();
		repo.main();
		computeHash();
	}

	void deleteByVal(T val) {
		deleteNode(root, val);
		//deleteFile(val);
	}

	void display() {
		printTree(root);
	}


	//File Handling

	//Creates a file in that directory(basically an AVLNode)

	void createFile(const string& fileName, AVLNode<T>* n) {
		fstream file;
		file.open(repo.name + "/" + repo.currBranch + "/" + fileName, ios::out);
		cout << "File " << fileName << " has been created\n";
		file << n->data << endl;
		if (n->parent) {
			file << n->parent->data << ".txt\n";
		}
		else {
			file << "NULL ";
		}
		if (n->left) {
			file << n->left->data << ".txt ";
		}
		else {
			file << "NULL ";
		}
		if (n->right) {
			file << n->right->data << ".txt" << endl;
		}
		else {
			file << "NULL" << endl;
		}
	}

	void updateNodeFile(AVLNode<T>* node) {
		if (!node) return;
		fstream file;
		file.open(repo.name + "/" + repo.currBranch + "/" + node->fileName, ios::out);
		file << node->data << endl;
		if (node->parent) {
			file << node->parent->data << ".txt\n";
		}
		else {
			file << "NULL ";
		}
		if (node->left) {
			file << node->left->data << ".txt ";
		}
		else {
			file << "NULL ";
		}
		if (node->right) {
			file << node->right->data << ".txt" << endl;
		}
		else {
			file << "NULL" << endl;
		}
		file.close();
	}


	void deleteFile(T val) {
		cout << "deleting " << val << ".txt\n";
		string filePath = repo.name + "/" + repo.currBranch + "/" + to_string_generic(val) + ".txt";
		remove(filePath.c_str());
	}


	int Height(AVLNode<T>* k1) {
		if (k1 == nullptr) {
			return -1;
		}
		return k1->height;
	}

	AVLNode<T>* rotateLeft(AVLNode<T>*& k1) {
		cout << "Rotating left\n";
		AVLNode<T>* temp = k1->right;
		k1->right = temp->left;
		if (k1->right) {
			k1->right->parent = k1;
		}
		temp->left = k1;
		updateNodeFile(k1->parent);
		temp->parent = k1->parent;
		k1->parent = temp;
		k1->height = max(Height(k1->left), Height(k1->right)) + 1;
		temp->height = max(Height(temp->right), k1->height) + 1;

		updateNodeFile(k1);
		updateNodeFile(temp);
		if (temp->parent) updateNodeFile(temp->parent);
		if (k1->right) updateNodeFile(k1->right);

		return temp;
	}

	AVLNode<T>* rotateRight(AVLNode<T>*& k1) {
		cout << "Rotating Right\n";

		AVLNode<T>* temp = k1->left;
		k1->left = temp->right;
		if (k1->left) {
			k1->left->parent = k1;
		}
		temp->right = k1;
		updateNodeFile(k1->parent);
		temp->parent = k1->parent;
		k1->parent = temp;
		k1->height = max(Height(k1->left), Height(k1->right)) + 1;
		temp->height = max(Height(temp->left), k1->height) + 1;

		updateNodeFile(k1);
		updateNodeFile(temp);
		if (temp->parent) updateNodeFile(temp->parent);
		if (k1->left) updateNodeFile(k1->left);

		return temp;
	}

	AVLNode<T>* rightLeft(AVLNode<T>*& k1) {
		cout << "Rotating rightleft\n";

		k1->left = rotateLeft(k1->left);
		if (k1->left)
			k1->left->parent = k1;
		k1 = rotateRight(k1);

		updateNodeFile(k1);
		if (k1->left) updateNodeFile(k1->left);
		if (k1->right) updateNodeFile(k1->right);
		if (k1->parent) updateNodeFile(k1->parent);

		return k1;
	}



	AVLNode<T>* leftRight(AVLNode<T>*& k1) {
		k1->right = rotateRight(k1->right);
		if (k1->right)
			k1->right->parent = k1;
		k1 = rotateLeft(k1);

		updateNodeFile(k1);
		if (k1->left) updateNodeFile(k1->left);
		if (k1->right) updateNodeFile(k1->right);
		if (k1->parent) updateNodeFile(k1->parent);

		return k1;
	}

	void insert(T c) {
		if (root == nullptr) {
			root = new AVLNode<T>(c);
			nNodes++;
			root->height = max(Height(root->left), Height(root->right)) + 1;
			root->parent = nullptr;
			createFile(root->fileName, root);
			return;
		}
		else {
			insertNode(root, root->parent, c);
			insertNode(root, root->parent, c);
		}
		computeHash();
	}

	void insertNode(AVLNode<T>*& tRoot, AVLNode<T>*& parent, T c) {
		if (tRoot == nullptr) {
			tRoot = new AVLNode<T>(c);
			nNodes++;
			tRoot->height = max(Height(tRoot->left), Height(tRoot->right)) + 1;
			tRoot->parent = parent;
			createFile(tRoot->fileName, tRoot);
			createFile(tRoot->parent->fileName, tRoot->parent);
			return;
		}
		else if (Tree<T>::isEqual(c, tRoot->data) == -1) {
			insertNode(tRoot->left, tRoot, c);
			if (abs(Height(tRoot->left) - Height(tRoot->right)) == 2) {
				if (Tree<T>::isEqual(c, tRoot->left->data) == -1) {
					tRoot = rotateRight(tRoot);
				}
				else {
					tRoot = rightLeft(tRoot);
				}
			}

		}
		else if (Tree<T>::isEqual(c, tRoot->data) == 1) {
			insertNode(tRoot->right, tRoot, c);
			if (abs(Height(tRoot->right) - Height(tRoot->left)) == 2) {
				if (Tree<T>::isEqual(c, tRoot->right->data) == 1) {
					tRoot = rotateLeft(tRoot);
				}
				else {
					tRoot = leftRight(tRoot);
				}
			}
		}
		tRoot->height = max(Height(tRoot->left), Height(tRoot->right)) + 1;
	}



	void print() {
		if (!root) {
			cout << "NULL";
			return;
		}
		queue<AVLNode<T>*> q;
		q.push(root);
		while (!q.empty()) {
			AVLNode<T>* curr = q.front();
			q.pop();
			cout << curr->data << "->";
			if (curr->left) {
				q.push(curr->left);
			}
			if (curr->right) {
				q.push(curr->right);
			}
		}
		cout << "NULL\n";
	}
	void printTree(AVLNode<T>* node, int space = 0, int indent = 4) {
		if (node == nullptr) {
			return;
		}

		// Increase distance between levels
		space += indent;

		// Process right child first
		printTree(node->right, space);

		// Print current node after space
		std::cout << std::endl;
		for (int i = indent; i < space; ++i) {
			std::cout << " ";
		}
		std::cout << node->data << "\n";

		// Process left child
		printTree(node->left, space);
	}

	AVLNode<T>* search(AVLNode<T>* tRoot, T k) {
		if (tRoot == nullptr) {
			return nullptr;
		}
		if (isEqual(tRoot->data, k) == 0) {
			return root;
		}
		else if (isEqual(tRoot->data, k) == 1) {
			search(tRoot->left, k);
		}
		else if (isEqual(tRoot->data, k) == -1) {
			search(tRoot->right, k);
		}

	}

	T minimum() {
		AVLNode<T>* curr = root;
		while (curr->left) {
			curr = curr->left;
		}
		return curr->data;
	}

	T maximum() {
		AVLNode<T>* curr = root;
		while (curr->right) {
			curr = curr->right;
		}
		return curr->data;
	}

	void deleteNode(AVLNode<T>*& tRoot, T key) {
		if (!tRoot) {
			return;
		}

		if (Tree<T>::isEqual(key, tRoot->data) == -1) {
			deleteNode(tRoot->left, key);
		}
		else if (Tree<T>::isEqual(key, tRoot->data) == 1) {
			deleteNode(tRoot->right, key);
		}
		else {
			if (!tRoot->left || !tRoot->right) {
				AVLNode<T>* tempParent = tRoot->parent;
				AVLNode<T>* temp = tRoot->left ? tRoot->left : tRoot->right;
				if (tRoot->parent) {
					updateNodeFile(tRoot->parent);
				}
				if (temp) {
					temp->parent = tempParent;
				}
				deleteFile(tRoot->data);
				delete tRoot;
				tRoot = temp;
				nNodes--;

				if (tRoot) {
					//tRoot->parent = tempParent;
					updateNodeFile(tRoot);
				}
				if (tempParent) {
					updateNodeFile(tempParent);
				}
			}
			else {
				AVLNode<T>* succ = tRoot->right;
				while (succ->left) {
					succ = succ->left;
				}


				tRoot->data = succ->data;
				cout << "Succ = " << succ->data << endl;

				deleteNode(tRoot->right, succ->data);

				deleteFile(key);
				tRoot->fileName = to_string_generic(tRoot->data) + ".txt";
				updateNodeFile(tRoot);
				updateNodeFile(tRoot->left);
				updateNodeFile(tRoot->right);
			}
		}

		if (!tRoot) {
			return;
		}
		tRoot->height = max(Height(tRoot->left), Height(tRoot->right)) + 1;
		int balance = Height(tRoot->left) - Height(tRoot->right);
		if (balance > 1) {
			if (Height(root->left->left) >= Height(root->left->right)) {
				root = rotateRight(root);
			}
			else {
				root->left = rotateLeft(root->left);
				root = rotateRight(root);
			}
		}
		else if (balance < -1) {
			if (Height(root->right->right) >= Height(root->right->left)) {
				root = rotateLeft(root);
			}
			else {
				root->right = rotateRight(root->right);
				root = rotateLeft(root);
			}
		}

		updateNodeFile(tRoot);

		if (tRoot->parent) {
			updateNodeFile(tRoot->parent);
		}
		computeHash();
	}

	//Virtual function update
	void update(T oldData,T newData){
		updateNode(root,oldData,newData);
	}

	//Updates value of a node
	void updateNode(AVLNode<T>*& tRoot, T oldData, T newData) {
		deleteByVal(oldData);
		insert(newData);
		cout << "Updated Node with value " << oldData << " with " << newData << endl;
	}

	bool isAVL() {
		if (abs(Height(root->right) - Height(root->left)) <= 1) {
			return true;
		}
		return false;
	}

	int balanceFactor() {
		return abs(Height(root->right) - Height(root->left));
	}

	//ConstructTree
	void constructTree() {
		
	}


	//Hashing 
	
	
	//Computes Hash for the whole tree
	void computeHash() {
		computeHashHelper(root);
		cout << root->hash<<endl;
	}
	
	//Recursive function that calculates hashes
	string computeHashHelper(AVLNode<T>* node) {
		if (node == nullptr)
			return "";
		//Currently a placeholder for computing actual has, implement later
		node->hash = Tree<T>::instructorHash(node->data) + computeHashHelper(node->left) + computeHashHelper(node->right);
		return node->hash;
	}

};
