#pragma once
#pragma once
#include<iostream>
#include<filesystem>
#include<fstream>
#include<cstdio>
#include<string>
#include<cmath>
#include<queue>
namespace fs = std::filesystem;
using namespace std;
template<class T>
class Node {
public:
	Node<T>* left;
	Node<T>* right;
	Node<T>* parent;

	T data;
	int height;
	string fileName;
	Node(T c) :data(c), left(nullptr), right(nullptr), parent(nullptr), height(0) {
		fileName = std::to_string(data) + ".txt";
	}

};
template<class T>
class AVL {
public:
	Node<T>* root;
	int nNodes;
	string dirName;
	AVL() :root(nullptr), nNodes(0) {
		dirName = "Nodes";
	}

	//File Handling

	//creates a directory
	void createDirectory(const string& dirName) {
		if (!fs::exists(dirName)) {
			fs::create_directory(dirName);
			cout << "Directory " << dirName << " has been created\n";
		}
	}

	//Creates a file in that directory(basically a node)

	void createFileInDirectory(const string& dirName, const string& fileName, Node<T>* n) {
		createDirectory(dirName);
		fstream file;
		file.open(dirName + '/' + fileName, ios::out);
		cout << "File " << fileName << " has been created\n";
		file << n->data << endl;
		if (n->parent) {
			file << n->parent->data << ".txt ";
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

	void updateNodeFile(Node<T>* node) {
		if (!node) return;
		fstream file(dirName + '/' + node->fileName, ios::out);
		file << node->data << endl;
		file << (node->parent ? to_string(node->parent->data) + ".txt" : "NULL") << " ";
		file << (node->left ? to_string(node->left->data) + ".txt" : "NULL") << " ";
		file << (node->right ? to_string(node->right->data) + ".txt" : "NULL") << endl;
		file.close();
	}


	int Height(Node<T>* k1) {
		if (k1 == nullptr) {
			return -1;
		}
		return k1->height;
	}

	int isEqual(char c, char d) {
		return isEqual(int(c), int(d));
	}

	int isEqual(int c, int d) {
		if (c == d) {
			return 0;
		}
		else if (c > d) {
			return 1;
		}
		else {
			return -1;
		}
	}

	int isEqual(string c, string d) {
		int len1 = c.length();
		int len2 = d.length();
		for (int i = 0; i < min(len1, len2); i++) {
			if (c[i] > d[i]) {
				return 1;
			}
			else if (c[i] < d[i]) {
				return -1;
			}
		}
		return 0;
	}

	Node<T>* rotateLeft(Node<T>* k1) {
		cout << "Rotating left\n";
		Node<T>* temp = k1->right;
		k1->right = temp->left;
		if (k1->right)
			k1->right->parent = k1;
		if (temp->left)
			temp->left->parent = k1;
		temp->left = k1;
		temp->parent = k1->parent;
		k1->parent = temp;
		k1->height = max(Height(k1->left), Height(k1->right)) + 1;
		temp->height = max(Height(temp->right), k1->height) + 1;

		updateNodeFile(k1);
		updateNodeFile(temp);
		if (k1->parent) updateNodeFile(k1->parent);
		if (temp->parent) updateNodeFile(temp->parent);
		if (k1->right) updateNodeFile(k1->right);

		return temp;
	}

	Node<T>* rotateRight(Node<T>* k1) {
		cout << "Rotating Right\n";

		Node<T>* temp = k1->left;
		k1->left = temp->right;
		if (k1->left)
			k1->left->parent = k1;

		temp->right = k1;
		temp->parent = k1->parent;
		k1->parent = temp;
		k1->height = max(Height(k1->left), Height(k1->right)) + 1;
		temp->height = max(Height(temp->left), k1->height) + 1;

		updateNodeFile(k1);
		updateNodeFile(temp);
		if (k1->parent) updateNodeFile(k1->parent);
		if (temp->parent) updateNodeFile(temp->parent);
		if (k1->left) updateNodeFile(k1->left);

		return temp;
	}

	Node<T>* rightLeft(Node<T>* k1) {
		cout << "Rotating rightleft\n";

		k1->left = rotateLeft(k1->left);
		if (k1->left)
			k1->left->parent = k1;
		k1 = rotateRight(k1);
		return k1;
	}



	Node<T>* leftRight(Node<T>* k1) {
		k1->right = rotateRight(k1->right);
		if (k1->right)
			k1->right->parent = k1;
		k1 = rotateLeft(k1);
		return k1;
	}

	void insert(T c) {
		if (root == nullptr) {
			root = new Node<T>(c);
			nNodes++;
			root->height = max(Height(root->left), Height(root->right)) + 1;
			root->parent = nullptr;
			createDirectory(dirName);
			createFileInDirectory(dirName, root->fileName, root);
			return;
		}
		else {
			insertNode(root, root->parent, c);
		}
	}

	void insertNode(Node<T>*& tRoot, Node<T>*& parent, T c) {
		if (tRoot == nullptr) {
			tRoot = new Node<T>(c);
			nNodes++;
			tRoot->height = max(Height(tRoot->left), Height(tRoot->right)) + 1;
			tRoot->parent = parent;
			createFileInDirectory(dirName, tRoot->fileName, tRoot);
			createFileInDirectory(dirName, tRoot->parent->fileName, tRoot->parent);
			return;
		}
		else if (isEqual(c, tRoot->data) == -1) {
			insertNode(tRoot->left, tRoot, c);
			if (abs(Height(tRoot->left) - Height(tRoot->right)) == 2) {
				if (isEqual(c, tRoot->left->data) == -1) {
					tRoot = rotateRight(tRoot);
				}
				else {
					tRoot = rightLeft(tRoot);
				}
			}

		}
		else if (isEqual(c, tRoot->data) == 1) {
			insertNode(tRoot->right, tRoot, c);
			if (abs(Height(tRoot->right) - Height(tRoot->left)) == 2) {
				if (isEqual(c, tRoot->right->data) == 1) {
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
		queue<Node<T>*> q;
		q.push(root);
		while (!q.empty()) {
			Node<T>* curr = q.front();
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
	void printTree(Node<T>* node, int space = 0, int indent = 4) {
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

	void display() {
		printTree(root);
	}
	Node<T>* search(Node<T>* tRoot, T k) {
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
		Node<T>* curr = root;
		while (curr->left) {
			curr = curr->left;
		}
		return curr->data;
	}

	T maximum() {
		Node<T>* curr = root;
		while (curr->right) {
			curr = curr->right;
		}
		return curr->data;
	}

	void deleteNode(Node<T>*& tRoot, T key) {
		if (!tRoot) {
			return;
		}

		if (isEqual(key, tRoot->data) == -1) {
			deleteNode(tRoot->left, key);
		}
		else if (isEqual(key, tRoot->data) == 1) {
			deleteNode(tRoot->right, key);
		}
		else {
			if (!tRoot->left || !tRoot->right) {
				Node<T>* temp = tRoot->left ? tRoot->left : tRoot->right;
				//string filename = to_string(key) + ".txt";
				//fs::remove(filename);
				delete tRoot;
				tRoot = temp;
				nNodes--;
			}
			else {
				Node<T>* succ = tRoot->right;
				while (succ->left) {
					succ = succ->left;
				}
				tRoot->data = succ->data;
				deleteNode(tRoot->right, succ->data);
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



};
