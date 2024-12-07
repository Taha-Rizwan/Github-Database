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



template<class T>
class AVLNode {
public:
	string leftPath;
	string rightPath;
	string parentPath;
	string fileName;
	bool dirtyNode;
	vector<int> lineNumbers;
	T data;
	int height;
	string hash;
	bool dups;
	AVLNode(T c) :data(c), leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), hash(""), dups(false),dirtyNode(0){
		fileName = to_string_generic(data) + ".txt";
	}
	AVLNode() :leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), hash(""), dups(false),dirtyNode(0){
	}

	void dirty() {
		dirtyNode = true;
	}

};


template<class T>
class AVL :public Tree<T> {
public:
	int nNodes;
	Repository<T> repo;
	AVL(string path = "") :repo(this, "AVL"), nNodes(0), ht(this, 151) {
		//computeHash();
		Tree<T>::order = 2;
		Tree<T>::rootFile = "NULL";
		if (path == "")
			repo.create();
		else
			repo.readFromFile(path);
		ht.emptyTable();
		repo.main();
	}

	//Hashing stuff





	void deleteByVal(T val) {
		deleteNode(Tree<T>::rootFile, val);
		string fileName = to_string_generic(val) + ".txt";
		deleteFile(fileName);
	}

	void display() {
		printTree(Tree<T>::rootFile);
	}


	//KFKDJ
	 //for cache
	struct Node {
	public:
		string key;
		AVLNode<T>* value;

		Node* prev;
		Node* next;
		int index;
		Node(string k, AVLNode<T>* v) : key(k), value(v), next(nullptr), prev(nullptr), index(-1) {}
		~Node() {
			/* delete value;
			 value = nullptr;*/
		}
	};

	string computeHashHelper(string path) {
		if (path == "NULL" || path == "nil")
			return "";

		AVLNode<T>* node = readNodeFromFile(path);

		//Currently a placeholder for computing actual has, implement later
		node->hash = Tree<T>::instructorHash(node->data) + computeHashHelper(node->leftPath) + computeHashHelper(node->rightPath);
		node->dirty();

		ht.insert(path, node);
		return node->hash;
	}



	struct HashTable {
	private:
		int capacity;
		int currSize;
		int hits, misses;
		pair<string, Node*>* arr;
		Node* head, * tail;
		AVL<T>* parentTree;
		vector<string> toBeDeleted;
		int Hash_Function(string& key) {

			unsigned long hash = 0;
			for (char c : key) {
				hash = (hash * 31) + c;  // Using 31 as a simple multiplier
			}
			return hash % capacity;  // Modulo the capacity (11 in this case)
		}
		int Hash_Function2(string& key) {
			unsigned long hash = 0;
			for (char c : key) {
				hash = hash * 39 + c;  // Another prime multiplier for second hash
			}
			return hash % capacity;
		}
		// Slot Finder with Quadratic Probing
		int findSlot(string& key, bool forInsert = false) {
			if (currSize >= capacity) {
				// Table is full, trigger resize or eviction strategy
				emptyHalf();  // or rehash, depending on your strategy
				return findSlot(key, forInsert); // Retry finding slot after resizing
			}
			int index = Hash_Function(key);
			int step = Hash_Function2(key);  // Second hash function for step size
			int i = 0;

			while (true) {
				int newIndex = (index + i * step) % capacity;

				// Check if the slot is empty or matches the key
				if (arr[newIndex].first == "" || arr[newIndex].first == key) {
					hits++;
					return newIndex;
				}

				// Reuse deleted slots during insertion
				if (forInsert && arr[newIndex].first == "DELETED") {
					hits++;
					arr[newIndex].first = "";  // Mark as usable
					return newIndex;
				}

				misses++;
				i++;
				if (i == capacity) {
					return -1;  // Table is full
				}
			}
		}

	public:
		HashTable(AVL<T>* parentTree, int capacity = 10) : capacity(capacity), parentTree(parentTree) {
			head = nullptr;
			tail = nullptr;
			currSize = 0;
			arr = new pair<string, Node*>[capacity];
			for (int i = 0; i < capacity; i++) {
				arr[i] = { "", nullptr };
			}
			hits = 0;
			misses = 0;
		}

		// Insert Method
		void insert(string key, AVLNode<T>* value) {
			int slot = findSlot(key, true);
			if (slot == -1) {
				emptyHalf(); // Evict half if the table is full
				insert(key, value);
				return;
			}
			if (arr[slot].first == key) {
				if (head->key != key) // Only remove if not already at the head
					remove(slot, true);
				else {
					return;
				}
			}
			/* int arneSlot = searchPos(key);
			 if (arneSlot!=-1) {

			 }*/

			Node* newNode = new Node(key, value);
			newNode->index = slot;
			arr[slot] = { key, newNode };

			if (!head) {
				head = tail = newNode;
			}
			else {
				newNode->next = head;
				head->prev = newNode;
				head = newNode;
			}
			currSize++;
			// Increment only when inserting a new key
					//cout << currSize<<" ";

		}

		// Remove Method
		void remove(string key, bool moveToFront = false) {
			if (key == "NULL" || key == "nil")
				return;

			int slot = findSlot(key);
			if (slot == -1 || arr[slot].first != key)
				return;

			Node* nodeToRemove = arr[slot].second;
			arr[slot] = { "DELETED", nullptr };

			// Correct the currSize after successful removal
			currSize--;

			if (nodeToRemove == head) {
				head = head->next;
				if (head)
					head->prev = nullptr;
			}
			else if (nodeToRemove == tail) {
				tail = tail->prev;
				if (tail)
					tail->next = nullptr;
			}
			else {
				nodeToRemove->prev->next = nodeToRemove->next;
				nodeToRemove->next->prev = nodeToRemove->prev;
			}

			if (!moveToFront && nodeToRemove->value->dirtyNode)
				parentTree->updateNodeFile(nodeToRemove->value);

			delete nodeToRemove;
		}

		void remove(int slot, bool moveToFront = false) {
			if (!arr[slot].second)
				return;
			Node* nodeToRemove = arr[slot].second;
			arr[slot] = { "DELETED", nullptr };

			// Correct the currSize after successful removal
			currSize--;

			if (nodeToRemove == head) {
				head = head->next;
				if (head)
					head->prev = nullptr;
			}
			else if (nodeToRemove == tail) {
				tail = tail->prev;
				if (tail)
					tail->next = nullptr;
			}
			else {
				nodeToRemove->prev->next = nodeToRemove->next;
				nodeToRemove->next->prev = nodeToRemove->prev;
			}

			if (!moveToFront && nodeToRemove->value->dirtyNode)
				parentTree->updateNodeFile(nodeToRemove->value);

			delete nodeToRemove;

		}



		// Search Method
		AVLNode<T>* search(string& key) {
			if (head->key == key)
				return head->value;

			int slot = findSlot(key);
			if (slot == -1 || arr[slot].first != key) {
				return nullptr;
			}
			return arr[slot].second->value;
		}
		int searchPos(string& key) {
			if (head->key == key)
				return head->index;

			int slot = findSlot(key);
			if (slot == -1 || arr[slot].first != key) {
				return -1;
			}
			return slot;
		}
		// Evict Half the Table if Full
		void emptyHalf() {
			for (int i = capacity; i > capacity / 2 && currSize > 0; i--) {
				remove(tail->index);
			}
		}

		void deleteFile(string x) {
			toBeDeleted.push_back(x);
		}
	
		void emptyTable() {
		
			cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->updateNodeFile(current->value);
				//  current->value->print();
				current = current->next;
			}
			cout << "Hits: " << hits << endl;
			cout << "Misses: " << misses << endl;
			head = nullptr;
			for (int i = 0; i < capacity; i++) {
				arr[i] = { "", nullptr };
			}
			for (int i = 0; i < toBeDeleted.size(); i++) {
				std::filesystem::remove(toBeDeleted[i]);
			}

		}
		~HashTable() {
			cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->updateNodeFile(current->value);
				//  current->value->print();
				current = current->next;
			}
			delete[] arr;
			cout << "Hits: " << hits << endl;
			cout << "Misses: " << misses << endl;

			for (int i = 0; i < toBeDeleted.size(); i++) {
				std::filesystem::remove(toBeDeleted[i]);
			}

		}
	};


	HashTable ht;


	//Creates a file in that directory(basically an AVLNode


	string createFile(AVLNode<T>* node) {

		// Open the file for the node
		ht.insert(to_string_generic(node->data), node);
		return node->fileName;
	}


	AVLNode<T>* readNodeFromFile(string filePath) {
		if (filePath == "NULL")
			return nullptr;
		cout << "filepath not null";
		string dataStr = filePath.substr(0, filePath.find(".txt"));
		// Check hash table first
		cout << "datastr: " << dataStr << endl;
		AVLNode<T>* cachedNode = ht.search(dataStr);
		if (cachedNode!=nullptr) {
			cout << cachedNode->data << endl;
			return cachedNode;
		}

		fstream file;
		cout << repo.name + "/" + repo.currBranch + "/" + filePath << endl;
		file.open(repo.name + "/" + repo.currBranch + "/" + filePath);
		if (!file.is_open()) {
			cerr << "Cannot open file: " << filePath << endl;
			throw runtime_error("Unable to open file: " + filePath);
		}

		AVLNode<T>* node = new AVLNode<T>();
		string line;

		getline(file, line);
		node->data = line; // Assuming T is string; use conversion for other types if needed
		node->fileName = filePath;
		getline(file, node->parentPath);
		getline(file, node->leftPath);
		getline(file, node->rightPath);
		getline(file, line);
		node->height = stoi(line);

		string lineNumbers;
		getline(file, lineNumbers);
		stringstream ss(lineNumbers);
		string number;
		while (getline(ss, number, ',')) {
			node->lineNumbers.push_back(stoi(number));
		}

		file.close();

		// Cache the node
		ht.insert(to_string_generic(node->data), node);
		return node;
	}

	void updateNodeFile(AVLNode<T>* node) {
		if (!node || !node->dirtyNode) { return; }
		fstream file;
		
		file.open(repo.name + "/" + repo.currBranch + "/" + node->fileName, ios::out);
		if (!file.is_open()) {
			return;
			throw runtime_error("Unable to create file: " + node->fileName);
		}
		node->fileName = to_string_generic(node->data) + ".txt";
		// Write node data to the file
		file << node->data << "\n";                       // Node data
		file << node->parentPath << "\n"                   // Parent path
			<< node->leftPath << "\n"                     // Left child path
			<< node->rightPath << "\n"                    // Right child path
			<< node->height << "\n";                      // Node height (now included)
		for (int i = 0; i < node->lineNumbers.size(); i++) {
			file << node->lineNumbers[i];
			if (i + 1 < node->lineNumbers.size())
				file << ',';
		}
		file << '\n';
		file.close();
		node->dirtyNode = false;
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

	void insert(T data,int ln) {
		Tree<T>::toLower(data);
		if (Tree<T>::rootFile == "NULL") {
			// Create root node
			AVLNode<T>* rootNode = new AVLNode<T>(data);
			rootNode->lineNumbers.push_back(ln);
			rootNode->dirty();
			Tree<T>::rootFile = createFile(rootNode);
			return;
		}

		string currFile = Tree<T>::rootFile;
		string parentFile = "NULL";

		// Traverse the tree to find the correct position for the new node
		while (currFile != "NULL") {
			AVLNode<T>* currentNode = readNodeFromFile(currFile);
			parentFile = currFile;

			//Deals with duplicate nodes
			if (Tree<T>::isEqual(data, currentNode->data) == 0) {
				currentNode->lineNumbers.push_back(ln);
				currentNode->dirty();
				return;
			}
			else if(Tree<T>::isEqual(data, currentNode->data) == -1) {
				currFile = currentNode->leftPath;
			}
			else {
				currFile = currentNode->rightPath;
			}
		}

		// Create new node
		AVLNode<T>* newNode = new AVLNode<T>(data);
		newNode->parentPath = parentFile;
		newNode->lineNumbers.push_back(ln);
		newNode->dirty();
		string newNodeFile = createFile(newNode);

		// Attach the new node to the parent
		AVLNode<T>* parent = readNodeFromFile(parentFile);
		if (data < parent->data)
			parent->leftPath = newNodeFile;
		else
			parent->rightPath = newNodeFile;

		parent->dirty();
		ht.insert(to_string_generic(parent->data),parent);

		// Start balancing the tree from the new node up to the root
		while (newNodeFile != "NULL") {
			AVLNode<T>* node = readNodeFromFile(newNodeFile);

			updateNodeHeight(*node);  // Update the height of the node
			node->dirty();
			ht.insert(to_string_generic(node->data),node);

			int balanceFactor = getBalance(*node);

			if (balanceFactor > 1) { // Left-heavy
				AVLNode<T>* leftChild = readNodeFromFile(node->leftPath);
				if (getBalance(*leftChild) >= 0) {
					rotateRight(node, newNodeFile);  // Left-Left Case
				}
				else {
					rotateLeftRight(node, newNodeFile);  // Left-Right Case
				}
			}
			else if (balanceFactor < -1) { // Right-heavy
				AVLNode<T>* rightChild = readNodeFromFile(node->rightPath);
				if ((getBalance(*rightChild)) <= 0) {
					rotateLeft(node, newNodeFile);  // Right-Right Case
				}
				else {
					rotateRightLeft(node, newNodeFile);  // Right-Left Case
				}
			}

			newNodeFile = node->parentPath;  // Move up the tree
		}


		// Ensure the root file pointer is correct after all rotations
		if (Tree<T>::rootFile != "NULL") {
			AVLNode<T>* rootNode = readNodeFromFile(Tree<T>::rootFile);
			rootNode->height = 1 + max(getHeight(rootNode->leftPath), getHeight(rootNode->rightPath));
			ht.insert(to_string_generic(rootNode->data), rootNode);
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

		//updateNodeHeight(*node);  // Ensure the height is updated
		
		return node->height;
	}


	// Helper function to update the height of a node
	void updateNodeHeight(AVLNode<T>& node) {
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
			leftChild->dirty();
			ht.insert(to_string_generic(leftChild->data), leftChild);
		}

		k2->parentPath = k1->parentPath;  // k2's parent becomes k1's parent

		if (k1->parentPath == "NULL") {
			Tree<T>::rootFile = k2->fileName;  // Update root if k1 was the root
		}
		else {
			AVLNode<T>* parent = readNodeFromFile(k1->parentPath);
			if (parent->leftPath == k1File) {
				parent->leftPath = k2->fileName;
			}
			else {
				parent->rightPath = k2->fileName;
			}
			parent->dirty();
			ht.insert(to_string_generic(parent->data), parent);
		}

		k2->leftPath = k1File;
		k1->parentPath = k2->fileName;

		updateNodeHeight(*k1);
		updateNodeHeight(*k2);
		k1->dirty();
		k2->dirty();
		ht.insert(to_string_generic(k1->data), k1);
		ht.insert(to_string_generic(k2->data), k2);
		return k2->fileName;
	}


	//Saves a couple file opening operations
	string rotateRight(AVLNode<T>*& k1, string k1File) {
		// Read the left child (k2) of the node (k1)
		AVLNode<T>* k2 = readNodeFromFile(k1->leftPath);

		// Update the left child of k1 to k2's right subtree
		k1->leftPath = k2->rightPath;
		if (k2->rightPath != "NULL") {
			AVLNode<T>* k2RightChild = readNodeFromFile(k2->rightPath);
			k2RightChild->parentPath = k1File;
			k2RightChild->dirty();
			ht.insert(to_string_generic(k2RightChild->data), k2RightChild);
		}

		// Update k2's parent pointer to k1's parent
		k2->parentPath = k1->parentPath;
		if (k1->parentPath == "NULL") {
			// k1 is the root, so update the root of the tree
			Tree<T>::rootFile = k2->fileName;
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
			parent->dirty();
			ht.insert(to_string_generic(parent->data), parent);
		}

		// Update k2's right child to be k1
		k2->rightPath = k1File;
		k1->parentPath = k2->fileName;

		updateNodeHeight(*k1);
		updateNodeHeight(*k2);

		k1->dirty();
		k2->dirty();
		ht.insert(to_string_generic(k1->data), k1);
		ht.insert(to_string_generic(k2->data), k2);

		return k2->fileName;
	}

	string rotateLeftRight(AVLNode<T>*& node, string nodeFile) {
		string leftPath = node->leftPath;
		AVLNode<T>* leftChild = readNodeFromFile(leftPath);
		node->leftPath = rotateLeft(leftChild, leftPath);
		//node->dirty();
		//ht.insert(to_string_generic(node->data), node);
		rotateRight(node, nodeFile);
		//ht.insert(to_string_generic(node->data), node);
		return node->fileName;
	}


	string rotateRightLeft(AVLNode<T>*& node, string nodeFile) {
		string rightPath = node->rightPath;
		AVLNode<T>* rightChild = readNodeFromFile(rightPath);
		node->rightPath = rotateRight(rightChild, rightPath);
		//node->dirty();
		//ht.insert(to_string_generic(node->data), node);
		rotateLeft(node, nodeFile);

		//ht.insert(to_string_generic(node->data), node);
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

	string searchHelper(string path, T val) {
		if (path == "NULL")
			return "NULL";
		cout << "Path: " << path << endl;
		AVLNode<T>* node = readNodeFromFile(path);
		cout << "READ IT\n";
		if (Tree<T>::isEqual(val, node->data) == 1)
			return searchHelper(node->rightPath, val);
		else if (Tree<T>::isEqual(val, node->data) == -1) {
			return searchHelper(node->leftPath, val);
		}
		else {
			return path;
		}
	}

	string search(T val) {
		cout << "Search helper called\n";
		return searchHelper(Tree<T>::rootFile, val);
	}

	vector<int> searchData(T data) {
		string path = search(data);
		cout << "Searching\n";
		if (path == "NULL")
			return {};

		cout << "path: " << path << endl;
		if (path.find(".txt") != path.size() - 4) {
			path += ".txt"; 
		}
		AVLNode<T>* node = readNodeFromFile(path);
		cout << "read node from fiel done\n";

		return node->lineNumbers;
	}

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


	int deleteByVal(T val, bool updation = false) {
		string x = search(val);
		AVLNode<T>* node = readNodeFromFile(x);
		if (x == "NULL")
			return -1;
		else if (node->lineNumbers.size() > 1) {
			cout << "From which line number do you want to delete this from: ";
			for (int i = 0; i < node->lineNumbers.size(); i++) {
				cout << "Line Number: " << node->lineNumbers[i] << endl;
			}
			if (!updation)
				cout << "Delete for Line Number: ";
			else
				cout << "Update Line Number: ";
			int opt;
			cin >> opt;
			bool exists = false;
			for (int i = 0; i < node->lineNumbers.size(); i++) {
				if (node->lineNumbers[i] == opt) {
					exists = true;
					break;
				}
			}
			if (!exists) {
				cout << "Invalid Option!" << endl;
				return -1;
			}
			else {
				remove(node->lineNumbers.begin(), node->lineNumbers.end(), opt);

				node->lineNumbers.pop_back();

				ht.insert(to_string_generic(node->data), node);
				return opt;
			}

		}
		else {
			cout << "Deleting";
			int l = node->lineNumbers[0];
			deleteNode(x);
			deleteFile(x);
			return l;
		}
	}
	int deleteByVal(T val, int ln) {
		string x = search(val);
		AVLNode<T>* node = readNodeFromFile(x);
		if (x == "NULL")
			return -1;
		else if (node->lineNumbers.size() >= 1) {

			remove(node->lineNumbers.begin(), node->lineNumbers.end(), ln);

			node->lineNumbers.pop_back();

			ht.insert(to_string_generic(node->data), node);
			return ln;

		}

	}


	void deleteNode(string currFile) {
		if (currFile == "NULL") return;

		AVLNode<T>* currNode = readNodeFromFile(currFile);

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
					//node->dirty();
					//ht.insert(to_string_generic(node->data), node);
					updateNodeFile(parent);
					temp->parentPath = "NULL";
					//temp->dirty();
					//ht.insert(to_string_generic(temp->data), temp);
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
				deleteNode(minValueFile);
				deleteFile(currFile);

				currNode->fileName = to_string_generic(currNode->data) + ".txt";
				createFile(currNode);
				if (currFile == Tree<T>::rootFile) {
					Tree<T>::rootFile = currNode->fileName;
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

		if (currFile == "NULL") {
			return;
		}

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
	void changeBranch(const string &path) {

		ht.emptyTable();
		Tree<T>::rootFile = path;
	}
	string getRootFile() {
		return Tree<T>::rootFile;
	}
	void emptyTable() {
		ht.emptyTable();
	}
};
