#pragma once
#pragma once
#include<iostream>
#include<filesystem>
#include<fstream>
#include<sstream>
#include<string>
#include<limits>
#include<cmath>
#include<queue>

#include"Repository.h"
namespace fs = std::filesystem;
using namespace std;



int stringToInt(const std::string& str) {
	int result = 0;
	int sign = 1;
	size_t i = 0;

	// Check for empty string
	if (str.empty()) {
		std::cerr << "Error: Empty string" << std::endl;
		return 0; // Or throw an exception or return an error code
	}

	// Handle optional '+' or '-' signs
	if (str[i] == '-') {
		sign = -1;
		i++;
	}
	else if (str[i] == '+') {
		i++;
	}

	// Process each character in the string
	for (; i < str.length(); i++) {
		char c = str[i];

		// Check if the character is a valid digit
		if (c < '0' || c > '9') {
			std::cerr << "Error: Invalid character '" << c << "' in input string" << std::endl;
			return 0; // Or handle invalid input (throw an exception, return error code, etc.)
		}

		// Convert char to integer and add to result
		result = result * 10 + (c - '0');

		// Handle overflow
		if (result < 0) {
			std::cerr << "Error: Integer overflow occurred" << std::endl;
			return 0; // Or handle overflow as needed
		}
	}

	return result * sign;
}


int isEqual(char c, char d) {
	return isEqual(int(c), int(d));
}


//int
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

//string
int isEqual(string c, string d) {
	int len1 = c.length();
	int len2 = d.length();
	bool greater = false;
	bool less = false;
	if (len1 < len2) {

	}
	for (int i = 0; i < min(len1, len2); i++) {
		if (c[i] > d[i]) {
			return 1;
		}
		else if (c[i] < d[i]) {
			return -1;
		}
	}

	if (len1 > len2) {
		return 1;
	}
	else if (len1 < len2) {
		return -1;
	}

	return 0;
}






template<class T>
class AVLNode {
public:
	string leftPath;
	string rightPath;
	string parentPath;
	string fileName;
	T data;
	int height;
	string hash;
	bool dups;
	AVLNode(T c) :data(c), leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), hash(""), dups(false) {
		fileName = to_string_generic(data) + ".txt";
	}
	AVLNode() :leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), hash(""), dups(false) {
	}
};
template<class T>
class AVL :public Tree<T> {
public:
	string rootFile;
	int nNodes;
	Repository<T> repo;
	AVL() :rootFile("NULL"),repo(this), nNodes(0) {
		//computeHash();
		repo.create();
	}

	void deleteByVal(T val) {
		deleteNode(rootFile, val);
		string fileName = to_string_generic(val) + ".txt";
		deleteFile(fileName);
	}

	void display() {
		printTree(rootFile);
	}


	//File Handling


	//Creates a file in that directory(basically an AVLNode)

	string createFile(AVLNode<T>* node) {

		// Open the file for the node
		fstream file;
		file.open(repo.name + "/" + repo.currBranch + "/" + node->fileName, std::ios::out);
		cout << "File " << node->fileName << " has been created\n";
		cout << "FILENAME: " << node->fileName << endl;
		if (!file.is_open()) {
			throw std::runtime_error("Unable to create file: " + node->fileName);
		}

		// Write node data to the file
		file << node->data << "\n";                       // Node data
		file << node->parentPath << "\n"                   // Parent path
			<< node->leftPath << "\n"                     // Left child path
			<< node->rightPath << "\n";                   // Right child path
		file << node->height << "\n";
		file.close();
		return node->fileName;
	}


	AVLNode<T>* readNodeFromFile(string filePath) {
		if (filePath == "NULL")
			return nullptr;
		cout << "reading" << endl;
		

		fstream file;
		file.open(repo.name + "/" + repo.currBranch + "/" + filePath);
		if (!file.is_open()) {
			cout << "CAN NOT OPEN " << filePath << endl;
			//throw runtime_error("Unable to open file: " + filePath);
			return nullptr;
		}
		AVLNode<T>* r = new AVLNode<T>();

		r->leftPath = "";
		r->rightPath = "";
		r->parentPath = "";
		string pathsLine;
		getline(file, pathsLine);

		if constexpr (std::is_same<T, int>::value) {
			r->data = stoi(pathsLine);  // Converts string to int
		}
		else {
			r->data = pathsLine;  // For non-int types, store the string directly
		}
		cout << "r->data: " << r->data << endl;
		r->fileName = to_string_generic(r->data) + ".txt";
		getline(file, pathsLine);
		r->parentPath = pathsLine;
		getline(file, pathsLine);
		r->leftPath = pathsLine;
		getline(file, pathsLine);
		r->rightPath = pathsLine;
		cout << "r->rightPath: " << r->rightPath << endl;
		getline(file, pathsLine);
		r->height = stringToInt(pathsLine);

		file.close();
		// cout << "Done reading" << endl;
		// cout << r->data<<endl;
		return r;
	}

	void updateNodeFile(AVLNode<T>* node) {
		if (!node) { return; }
		fstream file;
		
		file.open(repo.name + "/" + repo.currBranch + "/" + node->fileName, ios::out);
		if (!file.is_open()) {
			throw runtime_error("Unable to create file: " + node->fileName);
		}

		// Write node data to the file
		file << node->data << "\n";                       // Node data
		file << node->parentPath << "\n"                   // Parent path
			<< node->leftPath << "\n"                     // Left child path
			<< node->rightPath << "\n"                    // Right child path
			<< node->height << "\n";                      // Node height (now included)

		file.close();
	}



	void deleteFile(string fileName) {
		cout << "deleting " << fileName << "\n";
		string dirName = "Nodes";
		string filePath = dirName + "/" + fileName;
		remove(filePath.c_str());
	}


	int Height(AVLNode<T>* k1) {
		if (k1 == nullptr) {
			return -1;
		}
		return k1->height;
	}




	void insert(T data) {
		cout << endl << "iNSERTING " << data << endl << endl;
		if (rootFile == "NULL") {
			// Create root node
			AVLNode<T>* rootNode = new AVLNode<T>(data);
			rootFile = createFile(rootNode);

			return;
		}

		string currFile = rootFile;
		string parentFile = "NULL";

		// Traverse the tree to find the correct position for the new node
		while (currFile != "NULL") {
			AVLNode<T>* currentNode = readNodeFromFile(currFile);
			parentFile = currFile;

			if (isEqual(data, currentNode->data) == 0) return; // Prevent duplicate insertions

			if (isEqual(data, currentNode->data) == -1) {
				currFile = currentNode->leftPath;
				cout << "going towards left: " << currFile << endl;
			}
			else {
				cout << data << " is greater than " << currentNode->data << endl;
				currFile = currentNode->rightPath;
				cout << "goinf towards right: " << currFile << endl;
			}
		}

		// Create new node
		AVLNode<T>* newNode = new AVLNode<T>(data);
		newNode->parentPath = parentFile;
		string newNodeFile = createFile(newNode);

		// Attach the new node to the parent
		AVLNode<T>* parent = readNodeFromFile(parentFile);
		if (data < parent->data)
			parent->leftPath = newNodeFile;
		else
			parent->rightPath = newNodeFile;

		updateNodeFile(parent);

		// Start balancing the tree from the new node up to the root
		while (newNodeFile != "NULL") {
			AVLNode<T>* node = readNodeFromFile(newNodeFile);

			updateNodeHeight(*node);  // Update the height of the node
			updateNodeFile(node);     // Save the updated node to file

			int balanceFactor = getBalance(*node);

			if (balanceFactor > 1) { // Left-heavy
				AVLNode<T>* leftChild = readNodeFromFile(node->leftPath);
				if (getBalance(*leftChild) >= 0) {
					cout << "\n\nRotating right because " << node->data << "'s left is heavy and node's left child's " << leftChild->data << " left is heavy\n\n";
					rotateRight(node, newNodeFile);  // Left-Left Case
				}
				else {
					cout << "\n\nRotating left then right because " << node->data << "'s left is heavy and node's left child's " << leftChild->data << "'s right is heavy\n\n";
					rotateLeftRight(node, newNodeFile);  // Left-Right Case
				}
			}
			else if (balanceFactor < -1) { // Right-heavy
				AVLNode<T>* rightChild = readNodeFromFile(node->rightPath);
				if ((getBalance(*rightChild)) <= 0) {
					cout << "\n\nRotating left because " << node->data << "'s right is heavy and node's right child's " << rightChild->data << "'s right is heavy\n\n";
					rotateLeft(node, newNodeFile);  // Right-Right Case
				}
				else {
					cout << "\n\nRotating right then left because " << node->data << "'s left is heavy and node's right child's " << rightChild->data << "'s left is heavy\n\n";
					rotateRightLeft(node, newNodeFile);  // Right-Left Case
				}
			}

			newNodeFile = node->parentPath;  // Move up the tree
		}


		// Ensure the root file pointer is correct after all rotations
		if (rootFile != "NULL") {
			AVLNode<T>* rootNode = readNodeFromFile(rootFile);
			rootNode->height = 1 + max(getHeight(rootNode->leftPath), getHeight(rootNode->rightPath));
			updateNodeFile(rootNode); // Save the updated root
		}
	}



	// Helper function to get the balance factor of a node
	int getBalance(AVLNode<T>& node) {
		return getHeight(node.leftPath) - getHeight(node.rightPath);
	}

	// Helper function to get the height of a node
	int getHeight(string nodeFile) {
		if (nodeFile == "NULL") return -1;  // Null node has height -1
		AVLNode<T>* node = readNodeFromFile(nodeFile);

		updateNodeHeight(*node);  // Ensure the height is updated
		return node->height;
	}


	// Helper function to update the height of a node
	void updateNodeHeight(AVLNode<T>& node) {
		cout << "updting node height ofr " << node.data << endl;
		cout << node.data << "'s right is " << node.rightPath << endl;
		int leftHeight = getHeight(node.leftPath);
		int rightHeight = getHeight(node.rightPath);
		node.height = max(leftHeight, rightHeight) + 1;
	}

	string rotateLeft(AVLNode<T>*& k1, string k1File) {
		AVLNode<T>* k2 = readNodeFromFile(k1->rightPath);  // k2 becomes the new root of this subtree
		k1->rightPath = k2->leftPath;  // k2's left subtree becomes k1's right subtree

		if (k2->leftPath != "NULL") {
			AVLNode<T>* leftChild = readNodeFromFile(k2->leftPath);
			leftChild->parentPath = k1File;
			updateNodeFile(leftChild);  // Update file of the left child
		}

		k2->parentPath = k1->parentPath;  // k2's parent becomes k1's parent

		if (k1->parentPath == "NULL") {
			rootFile = k2->fileName;  // Update root if k1 was the root
		}
		else {
			AVLNode<T>* parent = readNodeFromFile(k1->parentPath);
			if (parent->leftPath == k1File) {
				parent->leftPath = k2->fileName;
			}
			else {
				parent->rightPath = k2->fileName;
			}
			updateNodeFile(parent);  // Update the parent's file
		}

		k2->leftPath = k1File;
		k1->parentPath = k2->fileName;

		updateNodeHeight(*k1);
		updateNodeHeight(*k2);
		updateNodeFile(k1);
		updateNodeFile(k2);

		return k2->fileName;
	}


	//Saves a couple file opening operations
	string rotateRight(AVLNode<T>*& k1, string k1File) {
		// Read the left child (k2) of the node (k1)
		AVLNode<T>* k2 = readNodeFromFile(k1->leftPath);

		// Update the left child of k1 to k2's right subtree
		cout << "k2->rightpATH: " << k2->rightPath << endl;
		k1->leftPath = k2->rightPath;
		if (k2->rightPath != "NULL") {
			AVLNode<T>* k2RightChild = readNodeFromFile(k2->rightPath);
			k2RightChild->parentPath = k1File;
			updateNodeFile(k2RightChild); // Save changes
		}

		// Update k2's parent pointer to k1's parent
		k2->parentPath = k1->parentPath;
		if (k1->parentPath == "NULL") {
			// k1 is the root, so update the root of the tree
			rootFile = k2->fileName;
		}
		else {
			// Update the parent's left or right pointer to point to k2
			AVLNode<T>* parent = readNodeFromFile(k1->parentPath);
			if (parent->leftPath == k1File) {
				parent->leftPath = k2->fileName;
			}
			else {
				parent->rightPath = k2->fileName;
			}
			updateNodeFile(parent); // Save changes
		}

		// Update k2's right child to be k1
		k2->rightPath = k1File;
		k1->parentPath = k2->fileName;

		updateNodeHeight(*k1);
		updateNodeHeight(*k2);

		updateNodeFile(k1);
		updateNodeFile(k2);

		return k2->fileName;
	}

	string rotateLeftRight(AVLNode<T>*& node, string nodeFile) {
		string leftPath = node->leftPath;
		AVLNode<T>* leftChild = readNodeFromFile(leftPath);
		node->leftPath = rotateLeft(leftChild, leftPath);

		rotateRight(node, nodeFile);

		updateNodeFile(node);
		return node->fileName;
	}


	string rotateRightLeft(AVLNode<T>*& node, string nodeFile) {
		string rightPath = node->rightPath;
		AVLNode<T>* rightChild = readNodeFromFile(rightPath);
		node->rightPath = rotateRight(rightChild, rightPath);

		rotateLeft(node, nodeFile);

		updateNodeFile(node);
		return node->fileName;
	}




	void printTree(string nodeFile, int space = 0, int indent = 4) {
		if (nodeFile == "NULL") {
			return;
		}

		AVLNode<T>* node = readNodeFromFile(nodeFile);

		space += indent;

		printTree(node->rightPath, space);

		std::cout << std::endl;
		for (int i = indent; i < space; ++i) {
			std::cout << " ";
		}
		std::cout << node->data << "\n";

		// Process the left child
		printTree(node->leftPath, space);
	}


	//AVLNode<T>* search(AVLNode<T>* tRoot, T k) {
	//	if (tRoot == nullptr) {
	//		return nullptr;
	//	}
	//	if (isEqual(tRoot->data, k) == 0) {
	//		return root;
	//	}
	//	else if (isEqual(tRoot->data, k) == 1) {
	//		search(tRoot->left, k);
	//	}
	//	else if (isEqual(tRoot->data, k) == -1) {
	//		search(tRoot->right, k);
	//	}

	//}

	//T minimum() {
	//	AVLNode<T>* curr = root;
	//	while (curr->left) {
	//		curr = curr->left;
	//	}
	//	return curr->data;
	//}

	//T maximum() {
	//	AVLNode<T>* curr = root;
	//	while (curr->right) {
	//		curr = curr->right;
	//	}
	//	return curr->data;
	//}

	void deleteNode(string currFile, T key) {
		if (currFile == "NULL") return;

		AVLNode<T>* currNode = readNodeFromFile(currFile);

		// Case 1: Node with key found
		if (isEqual(key, currNode->data) == 0) {
			// Case where node has one or no child
			if (currNode->leftPath == "NULL" || currNode->rightPath == "NULL") {
				string tempFile = (currNode->leftPath != "NULL") ? currNode->leftPath : currNode->rightPath;

				if (tempFile == "NULL") {
					tempFile = currFile;
					currFile = "NULL";
					AVLNode<T>* temp = readNodeFromFile(tempFile);
					AVLNode<T>* parent = readNodeFromFile(temp->parentPath);
					if (parent->leftPath == tempFile) {
						parent->leftPath = "NULL";
					}
					else {
						parent->rightPath = "NULL";
					}
					updateNodeFile(parent);
					temp->parentPath = "NULL";
					updateNodeFile(temp);
					deleteFile(tempFile);
				}
				else {
					// Update the parent of the child node
					AVLNode<T>* tempNode = readNodeFromFile(tempFile);
					string parentFile = currNode->parentPath;

					if (parentFile != "NULL") {
						AVLNode<T>* parentNode = readNodeFromFile(parentFile);
						if (currFile == parentNode->leftPath) {
							parentNode->leftPath = tempFile;
						}
						else {
							parentNode->rightPath = tempFile;
						}
						updateNodeFile(parentNode); // Update the parent's file
					}
					tempNode->parentPath = parentFile;
					updateNodeFile(tempNode);

					*currNode = *tempNode;
					updateNodeFile(currNode);

					deleteFile(currFile);
				}
			}
			else {
				// Node with two children
				string minValueFile = getMinValueFile(currNode->rightPath);
				AVLNode<T>* tempNode = readNodeFromFile(minValueFile);
				T oldData = currNode->data;
				currNode->data = tempNode->data;
				updateNodeFile(currNode);

				// Delete the successor node
				deleteNode(minValueFile, tempNode->data);
				deleteFile(currFile);

				currNode->fileName = to_string_generic(currNode->data) + ".txt";
				createFile(currNode);
				if (currFile == rootFile) {
					rootFile = currNode->fileName;
				}
				if (currNode->leftPath != "NULL") {
					AVLNode<T>* left = readNodeFromFile(currNode->leftPath);
					left->parentPath = currNode->fileName;
					updateNodeFile(left);
				}
				if (currNode->rightPath != "NULL") {
					AVLNode<T>* right = readNodeFromFile(currNode->rightPath);
					right->parentPath = currNode->fileName;
					updateNodeFile(right);
				}

			}
		}
		else if (isEqual(key, currNode->data) == -1) {
			deleteNode(currNode->leftPath, key);
			string fileName = to_string_generic(key) + ".txt";
			if (fileName == currNode->leftPath) {
				currNode->leftPath = "NULL";
			}
		}
		else {
			deleteNode(currNode->rightPath, key);
			string fileName = to_string_generic(key) + ".txt";
			if (fileName == currNode->rightPath) {
				currNode->rightPath = "NULL";
			}
		}

		if (currFile == "NULL") {
			cout << key << " has been deleted\n";
			return;
		}

		cout << "Balacing for " << currNode->data << "\n\n\n\n\n";
		updateNodeHeight(*currNode);
		int balance = getBalance(*currNode);

		// Rebalance the tree
		if (balance > 1 && getBalance(*readNodeFromFile(currNode->leftPath)) >= 0) {
			rotateRight(currNode, currFile);
		}
		if (balance > 1 && getBalance(*readNodeFromFile(currNode->leftPath)) < 0) {
			AVLNode<T>* left = readNodeFromFile(currNode->leftPath);
			rotateLeft(left, currNode->leftPath);
			rotateRight(currNode, currFile);
		}
		if (balance < -1 && getBalance(*readNodeFromFile(currNode->rightPath)) <= 0) {
			rotateLeft(currNode, currFile);
		}
		if (balance < -1 && getBalance(*readNodeFromFile(currNode->rightPath)) > 0) {
			AVLNode<T>* right = readNodeFromFile(currNode->rightPath);
			rotateRight(right, currNode->rightPath);
			rotateLeft(currNode, currFile);
		}

		// Update the current node's file after all changes
		updateNodeFile(currNode);
	}



	string getMinValueFile(string currFile) {
		AVLNode<T>* node = readNodeFromFile(currFile);
		while (node->leftPath != "NULL") {
			currFile = node->leftPath;
			node = readNodeFromFile(currFile);
		}
		return currFile;
	}



	////Virtual function update
	//void update(T oldData, T newData) {
	//	updateNode(root, oldData, newData);
	//}

	////Updates value of a node
	//void updateNode(AVLNode<T>*& tRoot, T oldData, T newData) {
	//	deleteByVal(oldData);
	//	insert(newData);
	//	cout << "Updated Node with value " << oldData << " with " << newData << endl;
	//}

	//bool isAVL() {
	//	if (abs(Height(root->right) - Height(root->left)) <= 1) {
	//		return true;
	//	}
	//	return false;
	//}

	//int balanceFactor() {
	//	return abs(Height(root->right) - Height(root->left));
	//}

	////ConstructTree
	//void constructTree() {

	//}



	////Hashing 



	////Computes Hash for the whole tree
	//void computeHash() {
	//	computeHashHelper(root);
	//	cout << root->hash << endl;
	//}

	////Recursive function that calculates hashes
	//string computeHashHelper(AVLNode<T>* node) {
	//	if (node == nullptr)
	//		return "";
	//	//Currently a placeholder for computing actual has, implement later
	//	node->hash = Tree<T>::instructorHash(node->data) + computeHashHelper(node->left) + computeHashHelper(node->right);
	//	return node->hash;
	//}

};
