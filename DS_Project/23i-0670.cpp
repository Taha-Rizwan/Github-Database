//23i-0825 Basit Junaid urf B-tree
//23i-0664 Taha Rizwan Red-Black
//23i-0670 Aqib Buzi AVL
//Section-G


///////////////////////////////////////////////////////////AVL//////////////////////////////////////////////////////////////////
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

//function that converts stringToInt (obvious from name)
int stringToInt(const std::string& str) {
	int result = 0;
	int sign = 1;
	size_t i = 0;

	// Check for empty string
	if (str.empty()) {
		std::cerr << "Error: Empty string" << std::endl;
		return 0;
	}

	// Handles optional '+' or '-' signs
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
			cerr << "Error: Invalid character '" << c << "' in input string" << std::endl;
			return 0;
		}

		result = result * 10 + (c - '0');

		//in case there is an overflow
		if (result < 0) {
			cerr << "Error: Integer overflow occurred" << std::endl;
			return 0;
		}
	}

	return result * sign;
}


//templatized class for AVL Node
template<class T>
class AVLNode {
public:
	T data;
	//node links for traversal
	string leftPath;
	string rightPath;
	string parentPath;
	string fileName;
	//for cache(indicates if the node has been changed or not)
	bool dirtyNode;
	//for duplicates
	vector<int> lineNumbers;
	int height;

	AVLNode(T c) :data(c), leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), dirtyNode(0) {
		fileName = to_string_generic(data) + ".txt";
	}
	AVLNode() :leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0), dirtyNode(0) {
	}

	void dirty() {
		dirtyNode = true;
	}

};

//templatized class for AVL Tree
template<class T>
class AVL :public Tree<T> {
public:
	int nNodes;
	Repository<T> repo;
	AVL(string path = "") :repo(this, "AVL"), nNodes(0), ht(this, 151) {
		Tree<T>::order = 2;
		Tree<T>::rootFile = "NULL";
		if (path == "")
			repo.create();//obvious from the name
		else
			repo.readFromFile(path);
		//hashtable is emptied after inserting all the nodes from the data
		ht.emptyTable();
		//interface for user
		repo.main();
	}

	//Hashing stuff
	void deleteByVal(T val) {
		deleteNode(Tree<T>::rootFile, val);
		string fileName = to_string_generic(val) + ".txt";
		deleteFile(fileName);
	}

	void display() {
		visualizeTree(Tree<T>::rootFile);
	}

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



	//for efficient handling of large data (stores already accessed nodes and makes their retrieval faster)
	struct HashTable {
	private:
		int capacity;
		int currSize;
		int hits, misses;
		pair<string, Node*>* arr;
		Node* head, * tail;
		AVL<T>* parentTree;
		//all nodes to be deleted are stored until committed
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
			if (head && head->key == key)
				return head->value;

			int slot = findSlot(key);
			if (slot == -1 || arr[slot].first != key) {
				return nullptr;
			}
			return arr[slot].second->value;
		}
		//search the index of a given key in the hashtable
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

		//deletes a file from the hashtable
		void deleteFile(string x) {
			toBeDeleted.push_back(x);
		}

		//empties the table when half is full (hahahaha)
		void emptyTable() {

			cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->updateNodeFile(current->value);
				current = current->next;
			}
			cout << "Hits: " << hits << endl;
			cout << "Misses: " << misses << endl;
			head = nullptr;
			for (int i = 0; i < capacity; i++) {
				arr[i] = { "", nullptr };
			}
			for (int i = 0; i < toBeDeleted.size(); i++) {
				filesystem::remove(toBeDeleted[i]);
			}
			toBeDeleted.clear();

		}

		//Destructor for class Hashtable (empties the hashtable) and frees memory
		~HashTable() {
			cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->updateNodeFile(current->value);
				current = current->next;
			}
			delete[] arr;
			//Hits(number of times a node was asked from hashtable and was found)
			//Misses(number of times a node was asked from hashtable and was not found)
			cout << "Hits: " << hits << endl;
			cout << "Misses: " << misses << endl;

			//removes all the files
			for (int i = 0; i < toBeDeleted.size(); i++) {
				filesystem::remove(toBeDeleted[i]);
			}

		}
	};

	//instance of hashtable
	HashTable ht;


	//Creates a file in that directory(basically an AVLNode) through hashtable
	string createFile(AVLNode<T>* node) {
		//Inserts the file in the hashtable
		ht.insert(to_string_generic(node->data), node);
		return node->fileName;
	}

	//gets path of a nodefile and reads node from file according to a decided format
	AVLNode<T>* readNodeFromFile(string filePath) {
		if (filePath == "NULL")
			return nullptr;
		string dataStr = filePath.substr(0, filePath.find(".txt"));
		// Check hash table first (If found return it so we dont have to look for it in the files)
		AVLNode<T>* cachedNode = ht.search(dataStr);
		if (cachedNode != nullptr) {
			return cachedNode;
		}

		fstream file;
		file.open(repo.name + "/" + repo.currBranch + "/" + filePath);
		if (!file.is_open()) {
			cerr << "Cannot open file: " << filePath << endl;
			throw runtime_error("Unable to open file: " + filePath);
		}

		AVLNode<T>* node = new AVLNode<T>();
		string line;

		//Reading from file according to a decided format
		getline(file, line);
		node->data = line;
		node->fileName = filePath;
		getline(file, node->parentPath);
		getline(file, node->leftPath);
		getline(file, node->rightPath);
		getline(file, line);
		//height of a node was saved as it was required in rotations (for height calculations)
		node->height = stoi(line);

		string lineNumbers;
		getline(file, lineNumbers);
		stringstream ss(lineNumbers);
		string number;

		while (getline(ss, number, ',')) {
			node->lineNumbers.push_back(stoi(number));
		}

		file.close();

		//Insert the node in the hashtable to make it's retrieval fast
		ht.insert(to_string_generic(node->data), node);
		return node;
	}

	//updates the file content for a node in case there has been some change in the node
	void updateNodeFile(AVLNode<T>* node) {
		//dont do anything if node is null or if it has not been changed 
		if (!node || !node->dirtyNode) { return; }

		fstream file;
		//open the file for writing
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
		string path = repo.name + "/" + repo.currBranch + "/" + fileName + ".txt";
		//deletes the file from hashtable
		ht.deleteFile(path);
	}

	//member function that returns the height of the node
	int Height(AVLNode<T>* k1) {
		if (k1 == nullptr) {
			return -1;
		}
		return k1->height;
	}

	//insert method that gets the data at line number and inserts it in the tree
	void insert(T data, int ln) {
		//converts to lower to remain consistent in approach
		Tree<T>::toLower(data);
		//if the tree is empty, just insert at root
		if (Tree<T>::rootFile == "NULL") {
			// Create root node
			AVLNode<T>* rootNode = new AVLNode<T>(data);
			rootNode->lineNumbers.push_back(ln);
			rootNode->dirty();
			//create file for the root node
			Tree<T>::rootFile = createFile(rootNode);
			return;
		}
		//start traversal from the rootnode
		string currFile = Tree<T>::rootFile;
		string parentFile = "NULL";

		// Traverse the tree to find the correct position for the new node
		while (currFile != "NULL") {
			//reads node from file
			AVLNode<T>* currentNode = readNodeFromFile(currFile);
			parentFile = currFile;

			//Deals with duplicate nodes
			if (Tree<T>::isEqual(data, currentNode->data) == 0) {
				//pushes the line number to indicate another instance of the same value
				currentNode->lineNumbers.push_back(ln);
				currentNode->dirty();
				return;
			}
			else if (Tree<T>::isEqual(data, currentNode->data) == -1) {//if data to be inserted is smaller than currentNode's data, go to left
				currFile = currentNode->leftPath;
			}
			else {
				currFile = currentNode->rightPath; //if data to be inserted is larger than currentNode's data, go to right
			}
		}

		// Creates a new node and file
		AVLNode<T>* newNode = new AVLNode<T>(data);
		newNode->parentPath = parentFile;
		newNode->lineNumbers.push_back(ln);
		newNode->dirty();
		string newNodeFile = createFile(newNode);

		// Attachs the new node to the parent
		AVLNode<T>* parent = readNodeFromFile(parentFile);
		if (data < parent->data)
			parent->leftPath = newNodeFile;
		else
			parent->rightPath = newNodeFile;

		parent->dirty();
		//inserts in hashtable to make it's retrieval faster if needed in future
		ht.insert(to_string_generic(parent->data), parent);

		// Start balancing the tree from the new node up to the root
		while (newNodeFile != "NULL") {
			AVLNode<T>* node = readNodeFromFile(newNodeFile);

			updateNodeHeight(*node);  // Update the height of the node
			node->dirty();
			ht.insert(to_string_generic(node->data), node);

			//calculates balance factor of the node (height(node's left)-height(node's right))
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



	//get the balance factor of a node
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


	//update the height of a node
	void updateNodeHeight(AVLNode<T>& node) {
		int leftHeight = getHeight(node.leftPath);
		int rightHeight = getHeight(node.rightPath);
		node.height = max(leftHeight, rightHeight) + 1;
	}

	//function to rotate the subtree to left
	string rotateLeft(AVLNode<T>*& k1, string k1File) {
		AVLNode<T>* k2 = readNodeFromFile(k1->rightPath);  // k2 becomes the new root of this subtree
		k1->rightPath = k2->leftPath;  // k2's left subtree becomes k1's right subtree

		//updates the new right child of the affected node
		if (k2->leftPath != "NULL") {
			AVLNode<T>* leftChild = readNodeFromFile(k2->leftPath);
			leftChild->parentPath = k1File;
			leftChild->dirty();
			ht.insert(to_string_generic(leftChild->data), leftChild);
		}

		k2->parentPath = k1->parentPath;  //k2's parent becomes k1's parent 

		//if k1 was previously the root of the tree (then change the root of the tree)
		if (k1->parentPath == "NULL") {
			Tree<T>::rootFile = k2->fileName;  // Update root if k1 was the root
		}
		else {
			//update the parent if it exists  
			AVLNode<T>* parent = readNodeFromFile(k1->parentPath);
			//update the children of the parent node
			if (parent->leftPath == k1File) {
				parent->leftPath = k2->fileName;
			}
			else {
				parent->rightPath = k2->fileName;
			}

			parent->dirty();
			//insert in hashtable to make it's retrieval faster
			ht.insert(to_string_generic(parent->data), parent);
		}
		//update the new parent of the affected node
		k2->leftPath = k1File;
		//update k1's parent to be the node which was previously it's right child (just how rotation works)
		k1->parentPath = k2->fileName;

		//update the heights of the nodes involved
		updateNodeHeight(*k1);
		updateNodeHeight(*k2);
		//nodes have been modified
		k1->dirty();
		k2->dirty();
		ht.insert(to_string_generic(k1->data), k1);
		ht.insert(to_string_generic(k2->data), k2);
		return k2->fileName;
	}

	//rotates the subtree to right
	string rotateRight(AVLNode<T>*& k1, string k1File) {
		// Read the left child (k2) of the node (k1)
		AVLNode<T>* k2 = readNodeFromFile(k1->leftPath);

		// Update the left child of k1 to k2's right subtree
		k1->leftPath = k2->rightPath;
		//if k2's right subtree is not null then update it
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
			//If k1 exists,update the parent's left or right pointer to point to k2
			AVLNode<T>* parent = readNodeFromFile(k1->parentPath);
			if (parent->leftPath == k1File) {
				parent->leftPath = k2->fileName;
			}
			else {
				parent->rightPath = k2->fileName;
			}
			parent->dirty();
			//insert in hashtable to make it's retrieval faster
			ht.insert(to_string_generic(parent->data), parent);
		}

		// Update k2's right child to be k1
		k2->rightPath = k1File;
		//updates the parent of k1 to be it's left child (the way we do it in a normal AVL Tree)
		k1->parentPath = k2->fileName;

		//updates the heights of the nodes involved in rotation
		updateNodeHeight(*k1);
		updateNodeHeight(*k2);
		//to indicate nodes have been modified
		k1->dirty();
		k2->dirty();
		//insert in hashtable to make retreival faster
		ht.insert(to_string_generic(k1->data), k1);
		ht.insert(to_string_generic(k2->data), k2);

		return k2->fileName;
	}

	//rotates left then right (basically RL rotation)
	string rotateLeftRight(AVLNode<T>*& node, string nodeFile) {
		string leftPath = node->leftPath;
		AVLNode<T>* leftChild = readNodeFromFile(leftPath);
		//rotates the left subtree of the affected node to left
		node->leftPath = rotateLeft(leftChild, leftPath);
		//node->dirty();
		//ht.insert(to_string_generic(node->data), node);

		rotateRight(node, nodeFile);
		//ht.insert(to_string_generic(node->data), node);
		return node->fileName;
	}

	//rotates to right then left (basically LR rotation)
	string rotateRightLeft(AVLNode<T>*& node, string nodeFile) {
		string rightPath = node->rightPath;
		AVLNode<T>* rightChild = readNodeFromFile(rightPath);
		//rotates the right child of affected node to right
		node->rightPath = rotateRight(rightChild, rightPath);
		//node->dirty();
		//ht.insert(to_string_generic(node->data), node);
		rotateLeft(node, nodeFile);

		//ht.insert(to_string_generic(node->data), node);
		return node->fileName;
	}


	//AVL visualization
	void printTree(string nodeFile, int space = 0, int indent = 4) {
		if (nodeFile == "NULL") {
			return;
		}

		AVLNode<T>* node = readNodeFromFile(nodeFile);

		space += indent;
		//goes deep down in the right subtree
		printTree(node->rightPath, space);

		std::cout << std::endl;
		for (int i = indent; i < space; ++i) {
			std::cout << " ";
		}
		std::cout << node->data << "\n";

		// Process the left child
		printTree(node->leftPath, space);
	}

	//recursive helper function to search for a node by traversal in files
	string searchHelper(string path, T val) {
		if (path == "NULL")
			return "NULL";

		AVLNode<T>* node = readNodeFromFile(path);
		//if value to be searched is greater than current node's data then search in the right subtree
		if (Tree<T>::isEqual(val, node->data) == 1)
			return searchHelper(node->rightPath, val);
		else if (Tree<T>::isEqual(val, node->data) == -1) {//if value to be searched is smaller than current node's data then search in the left subtree
			return searchHelper(node->leftPath, val);
		}
		else {
			//if equal returns filepath of the node
			return path;
		}
	}

	//search function 
	string search(T val) {
		return searchHelper(Tree<T>::rootFile, val);
	}

	//searches for data and returns all the line number in the csv file where that data exists
	vector<int> searchData(T data) {
		string path = search(data);
		if (path == "NULL")
			return {};
		//if path does not have .txt in it , then append it
		if (path.find(".txt") != path.size() - 4) {
			path += ".txt";
		}

		AVLNode<T>* node = readNodeFromFile(path);
		return node->lineNumbers;
	}

	//function to delete a val in the tree
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
			//if there is only one line number where the data exists then delete it from the tree and it's file
			int l = node->lineNumbers[0];
			deleteNode(x);
			deleteFile(x);
			return l;
		}
	}

	//function that deletes a data on a specific line number
	int deleteByVal(T data, int ln) {
		string x = search(data);
		AVLNode<T>* node = readNodeFromFile(x);
		if (x == "NULL") {
			return -1;
		}
		else if (node->lineNumbers.size() > 1) {//if there are more than one line numbers in the csv file where that data exists then delete the line number 
			remove(node->lineNumbers.begin(), node->lineNumbers.end(), ln);

			node->lineNumbers.pop_back();
			node->dirty();
			ht.insert(to_string_generic(node->data), node);
			ht.emptyTable();
			return ln;

		}
		else {
			//if there is only one line number where that data exists then delete the node from tree and it's file
			deleteNode(x);
			string lineNumber = x.substr(0, x.find(".txt"));
			deleteFile(lineNumber);
			return ln;

		}

	}

	//delete a node from the file based tree
	void deleteNode(string currFile) {
		//if node is null then do nothing
		if (currFile == "NULL") return;

		AVLNode<T>* currNode = readNodeFromFile(currFile);

		//Node has 0 or 1 child
		if (currNode->leftPath == "NULL" || currNode->rightPath == "NULL") {
			string tempFile = (currNode->leftPath != "NULL") ? currNode->leftPath : currNode->rightPath;

			//node has 0 children
			if (tempFile == "NULL") {
				string parentFile = currNode->parentPath;
				//update parent
				if (parentFile != "NULL") {
					AVLNode<T>* parent = readNodeFromFile(parentFile);
					if (parent->leftPath == currFile) {
						parent->leftPath = "NULL";
					}
					else {
						parent->rightPath = "NULL";
					}
					parent->dirty();
					ht.insert(to_string_generic(parent->data), parent);
					updateNodeFile(parent);
				}
				ht.remove(to_string_generic(currNode->data));
				//delete the file
				deleteFile(currFile);
			}
			else {
				//node has 1 child
				AVLNode<T>* tempNode = readNodeFromFile(tempFile);
				string parentFile = currNode->parentPath;
				//update parent node
				if (parentFile != "NULL") {
					AVLNode<T>* parentNode = readNodeFromFile(parentFile);
					if (currFile == parentNode->leftPath) {
						parentNode->leftPath = tempFile;
					}
					else {
						parentNode->rightPath = tempFile;
					}
					parentNode->dirty();
					ht.insert(to_string_generic(parentNode->data), parentNode);
					//update the parent file to remain consistent with tree structure
					updateNodeFile(parentNode);
				}
				//set tempNode's parent to parentFile
				tempNode->parentPath = parentFile;
				tempNode->dirty();
				//insert in hashtable to make its retrieval faster
				ht.insert(to_string_generic(tempNode->data), tempNode);
				updateNodeFile(tempNode);
				//remove the node from hashtable 
				ht.remove(to_string_generic(currNode->data));
				//remove node file
				deleteFile(currFile);
			}
		}
		else {
			// Node with two children

			//finds successor of the node
			string minValueFile = getMinValueFile(currNode->rightPath);
			AVLNode<T>* tempNode = readNodeFromFile(minValueFile);

			// Swap the data and lineNumbers with the in-order successor
			currNode->data = tempNode->data;
			currNode->lineNumbers = tempNode->lineNumbers;
			currNode->dirty();
			ht.insert(to_string_generic(currNode->data), currNode);
			updateNodeFile(currNode);

			// Deletes the in-order successor --->(goes for the 0 child case)
			deleteNode(minValueFile);
		}

		if (currFile == "NULL") {
			return;
		}

		// Update height and balance the tree
		updateNodeHeight(*currNode);
		int balance = getBalance(*currNode);

		//if node's left is heavy 
		if (balance > 1 && getBalance(*readNodeFromFile(currNode->leftPath)) >= 0) {
			rotateRight(currNode, currFile);
		}
		else if (balance > 1 && getBalance(*readNodeFromFile(currNode->leftPath)) < 0) {//if node's left subtree is heavy and left child's right subtree is heavy
			AVLNode<T>* left = readNodeFromFile(currNode->leftPath);
			rotateLeft(left, currNode->leftPath);
			rotateRight(currNode, currFile);
		}
		else if (balance < -1 && getBalance(*readNodeFromFile(currNode->rightPath)) <= 0) {//if node's right is heavy
			rotateLeft(currNode, currFile);
		}
		else if (balance < -1 && getBalance(*readNodeFromFile(currNode->rightPath)) > 0) {//if node's right subtree is heavy and right child's left subtree is heavy
			AVLNode<T>* right = readNodeFromFile(currNode->rightPath);
			rotateRight(right, currNode->rightPath);
			rotateLeft(currNode, currFile);
		}

		//update the node after rotation
		updateNodeFile(currNode);
	}

	//function to get path of the inorder successor
	string getMinValueFile(string currFile) {
		AVLNode<T>* node = readNodeFromFile(currFile);
		while (node->leftPath != "NULL") {
			currFile = node->leftPath;
			node = readNodeFromFile(currFile);
		}
		return currFile;
	}

	//function that changes branch (root's path)
	void changeBranch(const string& path) {

		ht.emptyTable();
		Tree<T>::rootFile = path;
	}

	//function that returns root's filepath
	string getRootFile() {
		ht.emptyTable();
		return Tree<T>::rootFile;
	}

	//function to empty the hashtable
	void emptyTable() {
		ht.emptyTable();
	}

	void printSpaces(int count) {
		for (int i = 0; i < count; ++i)
			cout << " ";
	}

	void visualizeTree(string path) {
		if (path == "NULL" || path == "nil") {
			cout << "The tree is empty!" << endl;
			return;
		}

		int height = getHeight(path);
		int maxWidth = (1 << height) - 1; // Maximum width of the tree at its bottom level

		queue<string> q;
		q.push(path);

		for (int level = 0; level < height; ++level) {
			int levelWidth = (1 << level); // Number of nodes at the current level
			int spaces = maxWidth / levelWidth; // Spaces between nodes

			// Print the current level
			for (int i = 0; i < levelWidth; ++i) {
				if (i == 0) printSpaces(spaces / 2);
				string current = q.front();
				q.pop();

				if (current != "NULL" && current != "nil") {
					AVLNode<T>* node = readNodeFromFile(current);
					cout << setw(2) << node->data;
					q.push(node->leftPath);
					q.push(node->rightPath);
				}
				else {
					cout << "  ";
					q.push("nil");
					q.push("nil");
				}
				printSpaces(spaces);
			}
			cout << endl;

			// Print connecting lines (for visual clarity)
			if (level < height - 1) {
				for (int i = 0; i < levelWidth; ++i) {
					if (i == 0) printSpaces(spaces / 2 - 1);
					printSpaces(spaces - 1);
				}
				cout << endl;
			}
		}
	}
};

///////////////////////////////////////////////////////////BTree2.h//////////////////////////////////////////////////////////////////
#pragma once                    //Btree format
#include <algorithm>            //leaf (yes, no)
#include <iostream>             //keys
#include <string>               //children path
#include <queue>
#include "Tree.h"
#include <vector>
#include <fstream>
#include "Repository.h"
using namespace std;

template <typename T>
class BTreeNode {
public:
	vector<string> keys;
	vector<vector<int>> lineNumbers;
	vector<string> childrenPaths;
	bool leaf;
	string parentPath;
	string nodePath;
	static int i;

	BTreeNode(bool l = true) {
		nodePath = to_string(i);
		parentPath = "null";
		i++;
	}
};
template <typename T>
int BTreeNode<T>::i = 0;

template <typename T>
class BTree :public Tree<T> {
private:
	BTreeNode<T>* root;
	string rootPath;
	int m;
	Repository<T> repo;
	int counter;

public:
	BTree(int degree = 0) :repo(this, "BTree"), m(degree) {
		rootPath = "NULL";
		repo.create();
		counter = 0;
		repo.main();
	}

	string to_bString(vector<T> data) {
		std::stringstream ss;
		ss << data[0];
		return ss.str();
	}

	string to_string_generic(const T& data) {
		std::stringstream ss;
		ss << data;

		return ss.str();
	}

	string pathify(string data) {
		Tree<T>::toLower(data);
		string path = repo.name + "/" + repo.currBranch + "/" + to_string_generic(data) + ".txt";

		return path;
	}

	void make() {

	}

	BTreeNode<T>* readNodeFromFile(string path, bool check = true, bool check1 = true) {
		if (path == "null" || path == "")
			return nullptr;

		ifstream f(path);
		string line;
		BTreeNode<T>* node = new BTreeNode<T>();
		getline(f, line);
		node->nodePath = pathify(line);
		getline(f, line);
		int keys = stoi(line);

		for (int i = 0; i < keys; i++) {
			getline(f, line);
			node->keys.push_back(line);
			getline(f, line);
			int freq = stoi(line);
			vector<int> lines;
			for (int j = 0; j < freq; j++) {
				getline(f, line);
				lines.push_back(stoi(line));
			}
			node->lineNumbers.push_back(lines);
		}

		getline(f, line);
		node->parentPath = line;
		getline(f, line);
		int kiddies = stoi(line);
		if (kiddies != 0) {
			node->leaf = false;
			for (int i = 0;i < kiddies;i++)
			{
				getline(f, line);
				node->childrenPaths.push_back(line);
			}
		}
		else {
			node->leaf = true;
		}


		f.close();

		return node;
	}

	string writeNodeToFile(BTreeNode<T>* node, int path = -1) {
		ofstream file;
		if (path == -1) {
			file.open(pathify(to_string(counter)));
			path = counter;
			counter++;

		}
		else {
			file.open(pathify(to_string(path)));
		}

		file << path << endl;

		file << node->keys.size() << endl;
		for (int i = 0; i < node->keys.size(); i++) {
			file << node->keys[i] << endl;
			if (i < node->lineNumbers.size()) {
				file << node->lineNumbers[i].size() << endl;
				for (int j = 0; j < node->lineNumbers[i].size(); j++) {
					file << node->lineNumbers[i][j] << endl;
				}
			}
			else {
				file << "1" << endl;
				file << "0" << endl;
			}
		}
		file << node->parentPath << endl;
		file << node->childrenPaths.size() << endl;
		for (int i = 0; i < node->childrenPaths.size(); i++)
			file << node->childrenPaths[i] << endl;

		file.close();
		return pathify(node->nodePath);
	}

	BTreeNode<T>* search(T key, bool insert, BTreeNode<T>* node = nullptr, int childIndex = 0) { //searches for the key in BTree
		if (!insert)
			if (!root)
				return root;

		if (!node)
			node = root;

		int i = 0;
		for (i = 0; i < node->keys.size(); i++) {
			int f = Tree<T>::isEqual((key), (node->keys[i]));
			if (f == 1)
				i = i;
			else
				break;
		}

		if (i < node->keys.size() && key == node->keys[i]) //if the key is found
			return node;

		else if (node->leaf) //if key not found, depending on if function used for insertion, return node or null
			return (insert) ? node : nullptr;

		else { //recursive search

			return search(key, insert, readNodeFromFile(node->childrenPaths[i]), i);
		}
	}

	void mySort(vector<T>& arr) {
		for (int i = 0; i < arr.size(); i++)
			for (int j = 0; j < arr.size() - 1; j++)
				if (Tree<T>::isEqual(arr[j], arr[j + 1]) == 1)
					swap(arr[j], arr[j + 1]);
	}
	void sortChildren(vector<T>& arr) {
		vector<BTreeNode<T>*> temp;
		for (int i = 0; i < arr.size(); i++)
			temp.push_back(readNodeFromFile(arr[i]));
		for (int i = 0; i < arr.size(); i++)
			for (int j = 0; j < temp.size() - 1; j++)
				if (temp[j]->keys[0] > temp[j + 1]->keys[0]) {
					swap(temp[j], temp[j + 1]);
					swap(arr[j], arr[j + 1]);
				}
	}


	int pathToInt(string str) {
		int res = 0;
		int multiple = 1;
		int i = repo.name.size();
		i += 6;
		for (i; i < str.length(); i++) {
			if (str[i] == '.')
				break;
			res = 10 * res + str[i] - 48;
		}
		return res;
	}

	void insert(T k, int ln) { //inserting a key in a BTree
		if (rootPath == "NULL") {
			BTreeNode<T>* curr = new BTreeNode<T>;
			curr->keys.push_back(k);
			curr->nodePath = pathify(curr->nodePath);
			//vector<int> line;
			//line.push_back(ln);
			//curr->lineNumbers.push_back(line);
			rootPath = writeNodeToFile(curr);
			rootPath = curr->nodePath;
			return;
		}
		//setLeafNodes();
		BTreeNode<T>* temp = readNodeFromFile(rootPath);
		root = temp;
		BTreeNode<T>* leafNodeForInsert = search(k, true);
		BTreeNode<T>* node = leafNodeForInsert;
		//BTreeNode<T>* node = temp;
		node->keys.push_back(k);
		//node->lineNumbers.push_back(ln);
		sort(node->keys.begin(), node->keys.end());

		while (node && node->keys.size() == m) {
			int splitFrom = node->keys.size() / 2; // the index of the splitting node
			BTreeNode<T>* left = new BTreeNode<T>;
			BTreeNode<T>* right = new BTreeNode<T>;
			right->nodePath = pathify(right->nodePath);
			left->nodePath = pathify(left->nodePath);
			for (int i = 0; i < splitFrom; i++)
				left->keys.push_back(node->keys[i]); // insertion in left childs of the parent which is split
			for (int i = splitFrom + 1; i < m; i++)
				right->keys.push_back(node->keys[i]);

			if (node->nodePath == rootPath) {
				T splitKey = node->keys[splitFrom]; // the key which is to be moved to parent
				mySort(node->keys);
				sortChildren(node->childrenPaths);
				if (node->keys.size() == m)
					setChildren(node, left, right, splitFrom);
				node->keys.clear();
				node->keys.push_back(splitKey);
				node->childrenPaths.clear();
				node->childrenPaths.push_back(left->nodePath);
				node->childrenPaths.push_back(right->nodePath);
				mySort(node->childrenPaths);
				left->parentPath = node->nodePath;
				right->parentPath = node->nodePath;
				writeNodeToFile(left, pathToInt(left->nodePath));
				writeNodeToFile(right, pathToInt(right->nodePath));
				writeNodeToFile(node, pathToInt(node->nodePath));
			}
			else {
				BTreeNode<T>* nodeParent = readNodeFromFile(node->parentPath);
				remove(nodeParent->nodePath);
				remove(node->nodePath);
				nodeParent->keys.push_back(node->keys[splitFrom]);
				mySort(nodeParent->keys);
				int currentChildIndex = 0;
				for (int i = 0; i < nodeParent->childrenPaths.size(); i++) {
					if (node->nodePath == nodeParent->childrenPaths[i])
						break;
					currentChildIndex++;
				}
				nodeParent->childrenPaths[currentChildIndex] = left->nodePath; //updating the children with left and right nodes
				nodeParent->childrenPaths.push_back(right->nodePath);
				left->parentPath = node->parentPath;
				right->parentPath = node->parentPath;
				if (node->keys.size() == m)
					setChildren(node, left, right, splitFrom);
				node->childrenPaths.clear();
				if (left->keys[0] < right->keys[0]) {
					node->childrenPaths.push_back(left->nodePath);
					node->childrenPaths.push_back(right->nodePath);
				}
				else {
					node->childrenPaths.push_back(right->nodePath);
					node->childrenPaths.push_back(left->nodePath);
				}
				//mySort(node->childrenPaths);
				writeNodeToFile(nodeParent, pathToInt(nodeParent->nodePath));
				//writeNodeToFile(node);
				writeNodeToFile(left, pathToInt(left->nodePath));
				writeNodeToFile(right, pathToInt(right->nodePath));
				sortChildren(nodeParent->childrenPaths);
				writeNodeToFile(nodeParent, pathToInt(nodeParent->nodePath));
			}
			BTreeNode<T>* next = readNodeFromFile(node->parentPath);
			if (next != nullptr) delete node;
			else rootPath = node->nodePath;
			node = next;
			//setLeafNodes();
		}
		if (node) {
			string checke = node->nodePath;
			int path = pathToInt(node->nodePath);
			writeNodeToFile(node, path);
		}
	}

	void setChildren(BTreeNode<T>* node, BTreeNode<T>* left, BTreeNode<T>* right, int mid) {
		for (int i = 0, j = 0; i <= mid; i++, j++) { //moving children to the left child node
			if (i < node->childrenPaths.size()) {
				left->childrenPaths.push_back(node->childrenPaths[i]);
				if (j < left->childrenPaths.size()) {
					BTreeNode<T>* temp = readNodeFromFile(left->childrenPaths[i]);
					temp->parentPath = (left->nodePath);
					writeNodeToFile(temp, pathToInt(temp->nodePath));
				}
				left->leaf = false;
			}
		}
		for (int i = mid + 1, j = 0; i <= m; i++, j++) { //moving children to the right child node
			if (i < node->childrenPaths.size()) {
				right->childrenPaths.push_back(node->childrenPaths[i]);
				if (j < right->childrenPaths.size()) {
					BTreeNode<T>* temp = readNodeFromFile(right->childrenPaths[j]);
					temp->parentPath = (right->nodePath);
					writeNodeToFile(temp, pathToInt(temp->nodePath));
				}
				right->leaf = false;
			}
		}
	}

};

//////////////////////////////////////////////////RedBlack.h////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Functions.h"
#include<string>
#include "Repository.h"
#include "LRUCache.h"
#include "queue"
using namespace std;
using namespace std::filesystem;

bool RED = 1;
bool BLACK = 0;


string toLowerString(string data) {
	for (int i = 0; i < data.length(); i++) {
		if (data[i] >= 'A' && data[i] <= 'Z')
			data[i] += 32;
	}
	return data;
}


// Node structure for the Red-Black Tree
template<class T>
struct RedBlackNode {
	T data;                     // Data stored in the node
	bool color;                 // 0 for BLACK, 1 for RED
	string leftPath;            // File path to the left child
	string rightPath;           // File path to the right child
	string parentPath;          // File path to the parent
	bool dirtyNode;
	vector<int> lineNumbers;
	RedBlackNode(T data) : data(data), color(RED), leftPath("nil"), rightPath("nil"), parentPath("NULL"), dirtyNode(false) {}

	RedBlackNode() : color(BLACK), leftPath("nil"), rightPath("nil"), parentPath("NULL"), dirtyNode(false) {}
	void print() {
		/*   cout << "Node Data: " << data << ", Parent: " << parentPath
			   << ", Left: " << leftPath << ", Right: " <<rightPath << endl;*/
	}
	void dirty() {
		dirtyNode = true;
	}
};



// Red-Black Tree class
template<class T>
class RedBlackTree : public Tree<T> {
	//For Cache
	//Double Linked List
private:
	RedBlackNode<T>* nil;
	Repository<T> repo;

	//for cache
	struct Node {
	public:
		string key;
		RedBlackNode<T>* value;

		Node* prev;
		Node* next;
		int index;
		Node(string k, RedBlackNode<T>* v) : key(k), value(v), next(nullptr), prev(nullptr), index(-1) {}
		~Node() {
			/* delete value;
			 value = nullptr;*/
		}
	};

	//string computeHashHelper(string path) {
	//    if (path == "NULL" || path == "nil")
	//        return "";

	//    RedBlackNode<T>* node = readNodeFromFile(path);

	//    //Currently a placeholder for computing actual has, implement later
	//    //node->hash = Tree<T>::instructorHash(node->data) + computeHashHelper(node->leftPath) + computeHashHelper(node->rightPath);
	//    node->dirty();
	//    ht.insert(path, node);

	//    return "";
	//}



	struct HashTable {
	private:
		int capacity;
		int currSize;
		int hits, misses;
		pair<string, Node*>* arr;
		Node* head, * tail;
		RedBlackTree<T>* parentTree;
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
		HashTable(RedBlackTree<T>* parentTree, int capacity = 10) : capacity(capacity), parentTree(parentTree) {
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
		void insert(string key, RedBlackNode<T>* value) {




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
				parentTree->writeNodeToFile(nodeToRemove->value);

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
				parentTree->writeNodeToFile(nodeToRemove->value);

			delete nodeToRemove;

		}



		// Search Method
		RedBlackNode<T>* search(string& key) {
			if (head && head->key == key)
				return head->value;

			int slot = findSlot(key);
			if (slot == -1 || arr[slot].first != key) {
				return nullptr;
			}

			//cout << "Found" << endl;

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
			//cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->writeNodeToFile(current->value);
				//  current->value->print();
				current = current->next;
			}/*
			cout << "Hits: " << hits << endl;
			cout << "Misses: " << misses << endl;*/
			head = nullptr;
			for (int i = 0; i < capacity; i++) {
				arr[i] = { "", nullptr };
			}
			for (int i = 0; i < toBeDeleted.size(); i++) {
				std::filesystem::remove(toBeDeleted[i]);
			}
			toBeDeleted.clear();

		}
		~HashTable() {
			//cout << "Destructor" << endl;
			Node* current = head;
			while (current) {
				parentTree->writeNodeToFile(current->value);
				//  current->value->print();
				current = current->next;
			}
			delete[] arr;
			/*  cout << "Hits: " << hits << endl;
			  cout << "Misses: " << misses << endl;*/

			for (int i = 0; i < toBeDeleted.size(); i++) {
				std::filesystem::remove(toBeDeleted[i]);
			}

		}
	};


	HashTable ht;

	string pathify(string data) {
		Tree<T>::toLower(data);
		string path = repo.name + "/" + repo.currBranch + "/" + to_string_generic(data) + ".txt";
		// cout << path << endl;

		return path;
	}

	string createFile(RedBlackNode<T>* node) {
		static int nodeCounter = 0;
		string fileName = to_string_generic(node->data);
		ht.insert(to_string_generic(node->data), node);

		return fileName;
	}

	RedBlackNode<T>* readNodeFromFile(string filePath) {
		if (filePath == "nil")
			return nil;

		RedBlackNode<T>* r = ht.search(filePath);
		if (r != nullptr)
			return r;

		ifstream file(pathify(filePath));
		if (!file.is_open()) {
			throw runtime_error("Unable to open file: " + filePath);
		}

		r = new RedBlackNode<T>();

		// Read data
		getline(file, r->data);

		// Read parent path
		getline(file, r->parentPath);

		// Read left child path
		getline(file, r->leftPath);

		// Read right child path
		getline(file, r->rightPath);

		// Read color
		file >> r->color;
		file.ignore(); // Ignore the newline character after the color


		// Read line numbers
		string lineNumbers;
		getline(file, lineNumbers);
		stringstream ss(lineNumbers);
		string number;
		while (getline(ss, number, ',')) {
			r->lineNumbers.push_back(stoi(number));
		}

		file.close();

		ht.insert(to_string_generic(r->data), r);
		return r;
	}
	void writeNodeToFile(RedBlackNode<T>* node) {
		if (!node->dirtyNode)
			return;
		std::ofstream file(pathify(to_string_generic(node->data)));

		file << node->data << "\n";
		file << node->parentPath << "\n"
			<< node->leftPath << "\n"
			<< node->rightPath << "\n";
		file << node->color << "\n";
		for (int i = 0; i < node->lineNumbers.size(); i++) {
			file << node->lineNumbers[i];
			if (i + 1 < node->lineNumbers.size())
				file << ',';
		}
		file << '\n';
		node->dirtyNode = false;
		file.close();

	}

	void createNil() {
		string niller = "nil";
		ofstream file(pathify(niller));
		file << -1 << "\nNULL\nNULL\nNULL\n0\n";
		file.close();
	}
	void rotateLeft(RedBlackNode<T>* k1, string k1File) {

		RedBlackNode<T>* temp = readNodeFromFile(k1->rightPath);
		k1->rightPath = temp->leftPath;
		//Change Parent
		if (temp->leftPath != "nil") {
			RedBlackNode<T>* tempLeft = readNodeFromFile(temp->leftPath);
			tempLeft->parentPath = k1File;
			tempLeft->dirty();
			ht.insert(to_string_generic(tempLeft->data), tempLeft);
		}

		temp->parentPath = k1->parentPath;
		//Rotation involving root
		if (k1->parentPath == "NULL") {
			Tree<T>::rootFile = to_string_generic(temp->data);
		}
		else {

			//in case node is at left of parent
			RedBlackNode<T>* parent = readNodeFromFile(k1->parentPath);
			if (k1File == parent->leftPath) {
				parent->leftPath = to_string_generic(temp->data);
			}

			//in case node is at right of parent
			else {
				parent->rightPath = to_string_generic(temp->data);
			}
			parent->dirty();
			ht.insert(to_string_generic(parent->data), parent);
		}

		temp->leftPath = k1File;
		temp->dirty();
		ht.insert(to_string_generic(temp->data), temp);
		k1->parentPath = to_string_generic(temp->data);
		k1->dirty();
		ht.insert(to_string_generic(k1->data), k1);

	}

	void rotateRight(RedBlackNode<T>* k1, string k1File) {
		RedBlackNode<T>* temp = readNodeFromFile(k1->leftPath);

		k1->leftPath = temp->rightPath;
		//Change Parent
		if (temp->rightPath != "nil") {

			RedBlackNode<T>* tempRight = readNodeFromFile(temp->rightPath);
			tempRight->parentPath = k1File;
			tempRight->dirty();
			ht.insert(to_string_generic(tempRight->data), tempRight);
		}

		temp->parentPath = k1->parentPath;
		//Rotation involving root
		if (k1->parentPath == "NULL") {
			Tree<T>::rootFile = to_string_generic(temp->data);
		}
		else {
			RedBlackNode<T>* parent = readNodeFromFile(k1->parentPath);
			//in case node is at right of parent
			if (k1File == parent->rightPath) {
				parent->rightPath = to_string_generic(temp->data);
			}
			//in case node is at left of parent
			else {
				parent->leftPath = to_string_generic(temp->data);
			}
			parent->dirty();
			ht.insert(to_string_generic(parent->data), parent);
		}

		temp->rightPath = k1File;
		temp->dirty();
		ht.insert(to_string_generic(temp->data), temp);
		k1->parentPath = to_string_generic(temp->data);
		k1->dirty();
		ht.insert(to_string_generic(k1->data), k1);
	}

	void change(string nodeFile) {
		string currFile = nodeFile;
		while (currFile != Tree<T>::rootFile) {
			RedBlackNode<T>* node = readNodeFromFile(currFile);
			RedBlackNode<T>* parent = readNodeFromFile(node->parentPath);
			node->print();
			parent->print();
			if (!parent->color)
				break;

			RedBlackNode<T>* grandParent = readNodeFromFile(parent->parentPath);
			grandParent->print();
			//node's parent is on left of grandfather
			if (node->parentPath == grandParent->leftPath) {
				// since parent is on left, uncle would be on right
				string uncFile = grandParent->rightPath;
				RedBlackNode<T>* uncle = readNodeFromFile(uncFile);
				uncle->print();

				if (uncFile != "nil" && uncle->color) {

					//since uncle is red, recolor
					parent->color = BLACK;
					uncle->color = BLACK;
					grandParent->color = RED;
					parent->dirty();
					uncle->dirty();
					grandParent->dirty();
					ht.insert(to_string_generic(parent->data), parent);
					ht.insert(to_string_generic(uncle->data), uncle);
					ht.insert(to_string_generic(grandParent->data), grandParent);

					//Since upto grandfather now satisfies properties, move to grandfather
					currFile = parent->parentPath;
					node = grandParent;
				}
				else {
					if (currFile == parent->rightPath) {

						//Additional Case for double rotation LR
						currFile = node->parentPath;
						node = parent;

						rotateLeft(node, currFile);
						node->dirty();

						parent = readNodeFromFile(node->parentPath);
						grandParent = readNodeFromFile(parent->parentPath);

					}

					//Normal Single Right Rotation'
					parent->color = BLACK;
					grandParent->color = RED;
					parent->dirty();

					grandParent->dirty();
					ht.insert(to_string_generic(parent->data), parent);
					ht.insert(to_string_generic(grandParent->data), grandParent);

					rotateRight(grandParent, parent->parentPath);
				}
			}

			//node's parent is on right of grandfather
			else {
				// since parent is on right, uncle would be on ,left
				string uncFile = grandParent->leftPath;
				RedBlackNode<T>* uncle = readNodeFromFile(uncFile);

				if (uncFile != "nil" && uncle->color) {

					//since uncle is red, recolor
					parent->color = BLACK;
					uncle->color = BLACK;
					grandParent->color = RED;
					parent->dirty();
					uncle->dirty();
					grandParent->dirty();
					ht.insert(to_string_generic(parent->data), parent);
					ht.insert(to_string_generic(uncle->data), uncle);
					ht.insert(to_string_generic(grandParent->data), grandParent);

					//Since upto grandfather now satisfies properties, move to grandfather
					currFile = parent->parentPath;
					node = grandParent;
				}
				else {

					//Additional Case for double rotation RL
					if (currFile == parent->leftPath) {
						currFile = node->parentPath;
						node = parent;
						node->dirty();
						rotateRight(node, currFile);
						parent = readNodeFromFile(node->parentPath);
						grandParent = readNodeFromFile(parent->parentPath);
					}
					parent->color = BLACK;
					grandParent->color = RED;
					parent->dirty();

					//Normal Single Right Rotation'
					grandParent->dirty();
					ht.insert(to_string_generic(parent->data), parent);
					ht.insert(to_string_generic(grandParent->data), grandParent);
					rotateLeft(grandParent, parent->parentPath);
				}
			}

		}

		RedBlackNode<T>* rootNode = readNodeFromFile(Tree<T>::rootFile);
		rootNode->color = BLACK;
		rootNode->dirty();
		ht.insert(to_string_generic(rootNode->data), rootNode);

	}

	string searchHelper(string path, T val) {
		if (path == "nil" || path == "NULL")
			return "NULL";

		RedBlackNode<T>* node = readNodeFromFile(path);
		if (Tree<T>::isEqual(val, node->data) == 1)
			return searchHelper(node->rightPath, val);
		else if (Tree<T>::isEqual(val, node->data) == -1) {
			return searchHelper(node->leftPath, val);
		}
		else {
			return path;
		}
	}

	void transplant(string path1, string path2) {
		RedBlackNode<T>* x = readNodeFromFile(path1);
		RedBlackNode<T>* y = readNodeFromFile(path2);
		if (x->parentPath == "NULL") {
			//x is root
			Tree<T>::rootFile = path2;
		}
		else {
			RedBlackNode<T>* parent = readNodeFromFile(x->parentPath);
			if (path1 == parent->leftPath) {
				//x is on left of parent
				parent->leftPath = path2;
			}
			else {
				//x is ojn right of parent
				parent->rightPath = path2;
			}
			ht.insert(x->parentPath, parent);
		}



		y->parentPath = x->parentPath;
		ht.insert(path2, y);


	}
	RedBlackNode<T>* findMin(string path) {
		RedBlackNode<T>* x = readNodeFromFile(path);
		while (x->leftPath != "nil") {
			x = readNodeFromFile(x->leftPath);
		}
		return x;
	}

	void fixDelete(RedBlackNode<T>* node) {
		RedBlackNode<T>* sibling;

		//While node is not root and node is black
		while (to_string_generic(node->data) != Tree<T>::rootFile && !node->color) {
			RedBlackNode<T>* parent = readNodeFromFile(node->parentPath);

			//if node is at left of parent
			if (to_string_generic(node->data) == parent->leftPath) {
				sibling = readNodeFromFile(parent->rightPath);

				//if node is at left of parent
				if (!sibling->color) {

					//Sibling is red(Case 1)
					sibling->color = BLACK;
					parent->color = RED;
					rotateLeft(parent, node->parentPath);
					ht.insert(to_string_generic(sibling->data), sibling);
					ht.insert(to_string_generic(parent->data), parent);

					sibling = readNodeFromFile(parent->rightPath);
				}

				RedBlackNode<T>* sibLeft = readNodeFromFile(sibling->leftPath);
				RedBlackNode<T>* sibRight = readNodeFromFile(sibling->rightPath);

				//Sibling is black and has two black children (Case 2)
				if (!sibLeft->color && !sibRight->color) {
					sibling->color = RED;
					node = parent;

					parent = readNodeFromFile(node->parentPath);
					ht.insert(to_string_generic(sibling->data), sibling);
				}
				//Sibling is black and left child is red right child is black (Case 3)
				else if (!sibRight->color) {
					sibLeft->color = BLACK;
					sibling->color = RED;

					ht.insert(sibling->leftPath, sibLeft);
					ht.insert(sibling->rightPath, sibRight);
					rotateRight(sibling, to_string_generic(sibling->data));
					RedBlackNode<T>* gp = readNodeFromFile(parent->parentPath);
					node = gp;
					parent = readNodeFromFile(node->parentPath);
					sibling = readNodeFromFile(parent->rightPath);

				}

				//Sibling is black and right child is red (Case 4)
				sibling->color = parent->color;
				parent->color = BLACK;

				sibRight = readNodeFromFile(sibling->rightPath);
				sibRight->color = BLACK;



				rotateLeft(parent, node->parentPath);
				ht.insert(sibling->rightPath, sibRight);

				ht.insert(node->parentPath, parent);
				ht.insert(to_string_generic(sibling->data), sibling);


				node = readNodeFromFile(Tree<T>::rootFile);

			}
			else {
				{
					//if node is at right of parent
					sibling = readNodeFromFile(parent->leftPath);



					//Sibling is red(Case 1)
					if (!sibling->color) {
						sibling->color = BLACK;
						parent->color = RED;
						rotateLeft(parent, node->parentPath);
						ht.insert(to_string_generic(sibling->data), sibling);
						ht.insert(to_string_generic(parent->data), parent);

						sibling = readNodeFromFile(parent->leftPath);
					}

					RedBlackNode<T>* sibLeft = readNodeFromFile(sibling->leftPath);
					RedBlackNode<T>* sibRight = readNodeFromFile(sibling->rightPath);

					//Sibling is black and has two black children (Case 2)
					if (!sibLeft->color && !sibRight->color) {
						sibling->color = RED;
						node = parent;

						parent = readNodeFromFile(node->parentPath);
						ht.insert(to_string_generic(sibling->data), sibling);
					}

					//Sibling is black and left child is red right child is black (Case 3)
					else if (!sibLeft->color) {
						sibRight->color = BLACK;
						sibLeft->color = RED;

						ht.insert(sibling->leftPath, sibLeft);
						ht.insert(sibling->rightPath, sibRight);
						rotateLeft(sibling, to_string_generic(sibling->data));
						RedBlackNode<T>* gp = readNodeFromFile(parent->parentPath);
						node = gp;

						//Going to grandpapi
						parent = readNodeFromFile(node->parentPath);
						sibling = readNodeFromFile(parent->leftPath);

					}
					sibling->color = parent->color;
					parent->color = BLACK;

					sibLeft = readNodeFromFile(sibling->leftPath);
					sibLeft->color = BLACK;



					rotateRight(parent, node->parentPath);
					ht.insert(sibling->leftPath, sibLeft);

					ht.insert(node->parentPath, parent);
					ht.insert(to_string_generic(sibling->data), sibling);


					node = readNodeFromFile(Tree<T>::rootFile);

				}
			}
		}
		if (node->data != "NULL" && node->data != "nil")
		{
			node->color = BLACK;
			ht.insert(to_string_generic(node->data), node);
		}

	}

	void deleteNode(string path) {
		RedBlackNode<T>* y = readNodeFromFile(path);
		bool origColor = y->color;
		RedBlackNode<T>* node = y;
		RedBlackNode<T>* x = nullptr;

		//One child only case(right) + no child case
		if (node->leftPath == "NULL" || node->leftPath == "nil") {
			x = readNodeFromFile(y->rightPath);
			transplant(path, node->rightPath);
		}

		//One child only case(left)
		else if (node->rightPath == "NULL" || node->rightPath == "nil") {
			x = readNodeFromFile(y->leftPath);
			transplant(path, node->leftPath);
		}

		//two child case
		else {
			y = findMin(node->rightPath);
			origColor = y->color;
			x = readNodeFromFile(y->rightPath);

			if (y->parentPath == path) {
				x->parentPath = to_string_generic(y->data);
			}
			else {
				transplant(to_string_generic(y->data), y->rightPath);
				y->rightPath = node->rightPath;
				RedBlackNode<T>* right = readNodeFromFile(y->rightPath);
				right->parentPath = to_string_generic(y->data);
				ht.insert(y->rightPath, right);
			}
			transplant(to_string_generic(node->data), to_string_generic(y->data));
			y->leftPath = node->leftPath;
			RedBlackNode<T>* left = readNodeFromFile(y->leftPath);
			left->parentPath = to_string_generic(y->data);
			y->color = node->color;
			ht.insert(y->leftPath, left);
			ht.insert(to_string_generic(y->data), y);
		}

		//In case original color is black there will be violations
		if (!origColor)
			fixDelete(x);
	}


	int getHeight(string path) {
		if (path == "nil" || path == "NULL") return 0;
		RedBlackNode<T>* root = readNodeFromFile(path);
		return 1 + max(getHeight(root->leftPath), getHeight(root->rightPath));
	}
	void printSpaces(int count) {
		for (int i = 0; i < count; ++i)
			cout << " ";
	}

	void visualizeTree(string path) {
		if (path == "NULL" || path == "nil") {
			cout << "The tree is empty!" << endl;
			return;
		}

		int height = getHeight(path);
		int maxWidth = (1 << height) - 1; // Maximum width of the tree at its bottom level

		queue<string> q;
		q.push(path);

		for (int level = 0; level < height; ++level) {
			int levelWidth = (1 << level); // Number of nodes at the current level
			int spaces = maxWidth / levelWidth; // Spaces between nodes

			// Print the current level
			for (int i = 0; i < levelWidth; ++i) {
				if (i == 0) printSpaces(spaces / 2);
				string current = q.front();
				q.pop();

				if (current != "NULL" && current != "nil") {
					RedBlackNode<T>* node = readNodeFromFile(current);
					cout << setw(2) << node->data;
					q.push(node->leftPath);
					q.push(node->rightPath);
				}
				else {
					cout << "  ";
					q.push("nil");
					q.push("nil");
				}
				printSpaces(spaces);
			}
			cout << endl;

			// Print connecting lines (for visual clarity)
			if (level < height - 1) {
				for (int i = 0; i < levelWidth; ++i) {
					if (i == 0) printSpaces(spaces / 2 - 1);
					printSpaces(spaces - 1);
				}
				cout << endl;
			}
		}
	}


public:
	RedBlackTree(string path = "") : repo(this, "RedBlack"), ht(this, 151) {
		Tree<T>::order = 2;
		Tree<T>::rootFile = "NULL";
		nil = new RedBlackNode<T>();

		createNil();
		if (path == "")
			repo.create();
		else
			repo.readFromFile(path);
		ht.emptyTable();


		//  cout << Tree<T>::rootFile << endl;
		repo.main();


	}

	string search(T val) {
		// cout << "Roofile: " << Tree<T>::rootFile << endl;
		return searchHelper(Tree<T>::rootFile, val);
	}

	void insert(T data, int ln) {

		Tree<T>::toLower(data);

		if (Tree<T>::rootFile == "NULL") {
			RedBlackNode<T>* rootNode = new RedBlackNode<T>(data);
			rootNode->color = BLACK;
			rootNode->dirty();
			rootNode->lineNumbers.push_back(ln);
			Tree<T>::rootFile = createFile(rootNode);
			return;
		}
		else {
			string currFile = Tree<T>::rootFile;
			string parFile = "NULL";

			while (currFile != "nil" && currFile != "NULL") {

				RedBlackNode<T>* currNode = readNodeFromFile(currFile);
				//cout << currFile << endl;
				parFile = currFile;

				if (Tree<T>::isEqual(data, currNode->data) == 0) {
					currNode->lineNumbers.push_back(ln);
					currNode->dirty();
					return;
				}
				else if (Tree<T>::isEqual(data, currNode->data) == 1) {
					currFile = currNode->rightPath;
				}
				else {
					currFile = currNode->leftPath;
				}
			}

			RedBlackNode<T>* newNode = new RedBlackNode<T>(data);
			newNode->parentPath = parFile;
			newNode->lineNumbers.push_back(ln);
			newNode->dirty();
			string newNodeFile = createFile(newNode);

			RedBlackNode<T>* parNode = readNodeFromFile(parFile);
			if (Tree<T>::isEqual(data, parNode->data) == 1) {
				parNode->rightPath = newNodeFile;
			}
			else {
				parNode->leftPath = newNodeFile;
			}
			parNode->dirty();
			ht.insert(to_string_generic(parNode->data), parNode);
			if (newNode->parentPath == "NULL") {
				newNode->color = BLACK;
				ht.insert(to_string_generic(newNode->data), newNode);

				return;
			}

			if (parNode->parentPath == "NULL") {

				return;
			}
			change(newNodeFile);

		}
	}
	void deleteFile(string x) {
		//   cout << "file ot be deleted: " << x << endl;
		ht.deleteFile(pathify(x));
	}
	int deleteByVal(T val, bool updation = false) {
		string x = search(val);
		RedBlackNode<T>* node = readNodeFromFile(x);
		if (x == "NULL" || x == "nil")
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

	int deleteByVal(T data, int ln) {
		string x = to_string_generic(data);
		RedBlackNode<T>* node = readNodeFromFile(x);
		//  cout << node->lineNumbers.size()<<endl;
	   //   cout << node->data << endl;
		if (node->lineNumbers.size() > 1) {
			// cout << "hello" << endl;
			remove(node->lineNumbers.begin(), node->lineNumbers.end(), ln);

			node->lineNumbers.pop_back();
			node->dirty();
			ht.insert(to_string_generic(node->data), node);
			ht.emptyTable();
			return ln;

		}
		else {
			//   cout << "Hello" << endl;
			deleteNode(x);
			deleteFile(x);


			return ln;

		}

	}
	//void computeHash() {
	//    cout<<computeHashHelper(Tree<T>::rootFile);
	//}
	vector<int> searchData(T data) {
		string path = search(data);
		if (path == "NULL" || path == "nil")
			return {};
		RedBlackNode<T>* node = readNodeFromFile(path);

		return node->lineNumbers;

	}

	void changeBranch(const string& path) {

		ht.emptyTable();
		Tree<T>::rootFile = path;
	}
	string getRootFile() {
		ht.emptyTable();
		return Tree<T>::rootFile;
	}
	void emptyTable() {
		ht.emptyTable();
	}

	void display() {
		visualizeTree(Tree<T>::rootFile);
	}
};


/////////////////////////////////////////////////Tree.h////////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include"Merkle.h"
using namespace std;



template<class T>
class Tree {
public:
	virtual void make() {}
	MerkleTree<T>* merkle;
	string rootFile;
	int order;
	//Comparison fUNCTIONS
	//CHAR
	void toLower(string& data) {
		for (int i = 0; i < data.length(); i++) {
			if (data[i] >= 'A' && data[i] <= 'Z')
				data[i] += 32;
		}
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
		toLower(c);
		toLower(d);
		int len1 = c.length();
		int len2 = d.length();
		for (int i = 0; i < min(len1, len2); i++) {
			if (c[i] > d[i] && len1 >= len2) {
				return 1;
			}
			else if (c[i] < d[i] && len2 >= len1) {
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


	//instructor's hash

	string instructorHash(int number) {
		int hash = 1;

		while (number > 0) {
			int digit = number % 10; // Extract the last digit
			hash *= digit;           // Multiply the digit
			number /= 10;            // Remove the last digit
		}

		return to_string_generic(hash); // Take the result modulo 29
	}

	// Function to calculate hash for a string
	string instructorHash(string text) {
		int hash = 1;

		for (int i = 0; i < text.length();i++) {
			hash *= text[i]; // Multiply ASCII values of characters

			hash %= 29;     // To avoid integer overflow, take modulo 29 after each step
		}

		return to_string_generic(hash);
	}
	/*virtual string computeHashHelper(Tree<T>* node) { return " "; }
	virtual void computeHash() {}*/
	virtual void insert(T data, int ln) {}
	virtual void display() {}
	//Return Line Number of where data is deleted from
	virtual int deleteByVal(T data, bool updation) { return 1; }
	virtual int deleteByVal(T data, int ln) { return 1; }
	virtual void update(T oldData, T newData) {}
	virtual void createNil() {

	}
	virtual void emptyTable() {

	}
	virtual void changeBranch(const string& path) {}
	virtual string getRootFile() {
		return "nil";
	}
	virtual vector<int> searchData(T data) {
		return {};
	}

};


///////////////////////////////////////////////////Merkle.h//////////////////////////////////////////////////////////////////
//Now calculates hash of the entire row of the node
//MerkleTree is completely based on the data of the branch....if there is no change in the data file there wont be any change in the hash

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <openssl/evp.h>//for sha256
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

//function to convert the data to string in case the data type is not string
template<class T>
string to_string_generic(const T& data) {
	std::stringstream ss;
	ss << data;
	return ss.str();
}

//function that calculates hash using sha256 hashing algorithm
string calculateSHA256(const string& data) {
	EVP_MD_CTX* context = EVP_MD_CTX_new();
	if (context == nullptr) {
		cerr << "Error creating EVP_MD_CTX" << endl;
		return "";
	}

	if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
		cerr << "Error initializing SHA256" << endl;
		EVP_MD_CTX_free(context);
		return "";
	}

	if (EVP_DigestUpdate(context, data.c_str(), data.size()) != 1) {
		cerr << "Error updating SHA256" << endl;
		EVP_MD_CTX_free(context);
		return "";
	}

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int lengthOfHash = 0;
	if (EVP_DigestFinal_ex(context, hash, &lengthOfHash) != 1) {
		cerr << "Error finalizing SHA256" << endl;
		EVP_MD_CTX_free(context);
		return "";
	}

	stringstream hexStream;
	for (unsigned int i = 0; i < lengthOfHash; i++) {
		hexStream << hex << setw(2) << setfill('0') << (int)hash[i];
	}

	EVP_MD_CTX_free(context);

	return hexStream.str();
}


// Instructor's hash function
string instructorHash(string text) {
	int hash = 1;
	for (int i = 0; i < text.length(); i++) {
		hash *= text[i];  // Multiply ASCII values of characters
		hash %= 29499439;       // To avoid integer overflow, take modulo 29 after each step
	}
	stringstream ss;
	//converts the hash value to hex
	ss << std::hex << std::setw(16) << std::setfill('0') << hash;
	return ss.str();
}


//templatized class for merkle node
template<class T>
class MerkleNode {
public:
	// Data field only for leaf nodes
	T data;
	string lineNumber;      //line number on which the data exists
	string hash;            //hash of the data
	MerkleNode* left;       //left child of node
	MerkleNode* right;      //right child of node
	bool leaf;              //if node is leaf--->In merkle tree, only leaf nodes have data
	bool useSha;            //bool which indicates which hashing algorithm to use

	// Constructor for leaf nodes
	MerkleNode(T data, bool leaf, bool useSha) : data(data), hash(" "), useSha(useSha), left(nullptr), right(nullptr), leaf(leaf) {
		computeHash(data);
	}

	// Constructor for internal nodes
	MerkleNode(MerkleNode* left, MerkleNode* right, bool useSha) :useSha(useSha), left(left), right(right), leaf(false) {
		computeHash(left->hash + right->hash);
	}

	//function that computes hash of a given string using hashing algorithm selected by user
	void computeHash(string text) {
		if (useSha) {
			hash = calculateSHA256(text);
		}
		else {
			hash = instructorHash(text);
		}
	}
};

//templatized class for Merkle Tree
template<class T>
class MerkleTree {
public:
	MerkleNode<T>* root;
	string repoName;
	string currBranch;
	int order;
	bool useSha;
	MerkleTree(int order, bool useSha) : order(order), useSha(useSha), root(nullptr) {}

	//delete the tree
	void deleteTree(MerkleNode<T>* node) {
		if (node == nullptr) {
			return;
		}
		if (node && node->left) {
			deleteTree(node->left);
			node->left = nullptr;
		}
		if (node && node->right) {
			deleteTree(node->right);
			node->right = nullptr;
		}
		delete node;
	}


	//gets the path of the data folder and create a merkle tree from the data files in the folder
	vector<MerkleNode<T>*> createLeafNodes(const string& dataFolder) {

		vector<MerkleNode<T>*> leafNodes;

		vector<string> filePaths;

		//gathers path of all the data files
		for (const auto& entry : fs::directory_iterator(dataFolder)) {
			if (entry.is_regular_file() && entry.path().extension() == ".txt") {
				filePaths.push_back(entry.path().string());
			}
		}

		//tries sorting the file paths (address the issue of sorting lexicographcially e.g after 89.txt the next file would be 9.txt and after that 90.txt)
		sort(filePaths.begin(), filePaths.end(), [](const string& a, const string& b) {
			auto extractLineNumber = [](const string& path) -> int {
				std::string filename = fs::path(path).stem().string();
				try {
					return std::stoi(filename);
				}
				catch (const std::invalid_argument&) {
					return 0;
				}
				};

			return extractLineNumber(a) < extractLineNumber(b);
			});

		//iterates in the filepath vector and creates a merkle leaf node on that data
		for (const string& filePath : filePaths) {
			string lineNumber = filePath.substr(0, filePath.find(".txt"));
			ifstream file(filePath);
			if (file.is_open()) {
				//Reads the entire data related to the node
				stringstream buffer;
				buffer << file.rdbuf();
				string data = buffer.str();
				file.close();
				MerkleNode<T>* leafNode = new MerkleNode<T>(data, true, useSha);
				leafNode->lineNumber = lineNumber;
				leafNodes.push_back(leafNode);
			}
			else {
				cerr << "Could not open file: " << filePath << endl;
			}
		}

		return leafNodes;
	}



	// Build the Merkle Tree after creating the leaf nodes
	MerkleNode<T>* buildMerkleTree(const string& dataFolder) {
		//issues with deletion--->we have to construct merkle tree eveyr time we are looking for any changes so we must delete the old one first
		if (root != nullptr) {
			//deleteTree(root);
			root = nullptr;
		}

		//creates leaf nodes from the data folder path 
		vector<MerkleNode<T>*> leafNodes = createLeafNodes(dataFolder);

		if (leafNodes.empty()) return nullptr;
		vector<MerkleNode<T>*> currentLevel = leafNodes;

		//builds the tree level by level...starting from leaf nodes(containing data) and constructs the tree using bottom up approach
		while (currentLevel.size() > 1) {
			vector<MerkleNode<T>*> nextLevel;

			for (size_t i = 0; i < currentLevel.size(); i += 2) {
				MerkleNode<T>* left = currentLevel[i];
				//if there is no node left for pairing then pair it with itself and then calculate hash
				MerkleNode<T>* right = (i + 1 < currentLevel.size()) ? currentLevel[i + 1] : left;
				//calculates hash of the two nodes and creates a parent by combining the hashes of those nodes
				MerkleNode<T>* parentNode = new MerkleNode<T>(left, right, useSha);
				//adds that node to the upper level
				nextLevel.push_back(parentNode);
			}

			currentLevel = nextLevel;
		}

		root = currentLevel[0];
		return root;
	}


	//Looks where data has been changed and only modifies that data when merging
	void lookForChange(MerkleNode<T>* mRoot, MerkleNode<T>* tRoot, string path) {
		if (mRoot == nullptr && tRoot == nullptr) {
			return;
		}


		if (!mRoot || !tRoot) {

			return;
		}


		if (mRoot->hash != tRoot->hash) {
			if (mRoot->leaf && tRoot->leaf) {
				//Updation Case
				if (mRoot->data != tRoot->data) {
					string fileName = mRoot->lineNumber + ".txt";
					//updates the file
					ofstream file(fileName);
					if (file.is_open()) {
						file << tRoot->data;
						file.close();
					}
					else {
						cerr << "Error opening file: " << fileName << endl;
					}
					mRoot->data = tRoot->data;

					mRoot->computeHash(mRoot->data);
				}
			}
			else {
				//goes parallel in both trees
			   //look for change in the left of both trees
				if (mRoot->left && tRoot->left) {
					lookForChange(mRoot->left, tRoot->left, path + "/left");
				}
				//look for change in the right of both trees
				if (mRoot->right && tRoot->right) {
					lookForChange(mRoot->right, tRoot->right, path + "/right");
				}

				mRoot->computeHash(mRoot->left->hash + mRoot->right->hash);
			}
		}

	}



	// Get the root hash of the Merkle Tree
	string getRootHash() const {
		return root ? root->hash : "";
	}
};


/////////////////////////////////////////////////////Menu.h////////////////////////////////////////////////////////
#pragma once
#include "RedBlack.h"
#include "AVL.h"
#include "BTree2.h"

struct Menu {

	void main() {
		cout << "1: Create a repository\n";
		cout << "2: Load a repository\n";
		int op;
		cin >> op;
		if (op == 1)
			createRepo();
		else if (op == 2)
			readRepo();
	}

	void createRepo() {
		cout << "1: RedBlack Tree" << endl;
		cout << "2: AVL Tree" << endl;
		cout << "3: B Tree" << endl;
		cout << "Enter Option: " << endl;
		int opt;
		cin >> opt;
		if (opt == 1) {
			RedBlackTree<string> r;
		}
		else if (opt == 2) {
			AVL<string> r;
		}
		else if (opt == 3) {
			int deg;
			cout << "Select Order: ";
			cin >> deg;
			BTree<string> r(deg);
		}

	}

	void readRepo() {
		cout << "Enter Repo Path: " << endl;
		string path;
		string data;
		cin.ignore();
		getline(cin, path);

		path += ".txt";
		ifstream file(path);
		file >> data;
		file.close();


		if (data == "RedBlack") {
			RedBlackTree<string> r(path);
		}
		else
			AVL<string> r(path);
	}




};


//////////////////////////////////////////////////////Repository.h//////////////////////////////////////////////////////////
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tree.h"
#include <vector>
using namespace std;
using namespace std::filesystem;

//Addtiion in current data
struct Addition {
	vector<string> rowData;
	int lineNumber;
	Addition(vector<string> rowData, int lineNumber) : rowData(rowData), lineNumber(lineNumber) {}
	string metaData() {
		string data = "Addition\n";
		for (int i = 0; i < rowData.size(); i++) {
			data += rowData[i] + "\n";
		}

		data += to_string(lineNumber) + "\n";
		return data;
	}
};


//Deletion in current data
struct Deletion {
	string data;
	int lineNumber;
	vector<string>rowData;
	Deletion(string data, int lineNumber, vector<string>& rowData) : data(data), lineNumber(lineNumber), rowData(rowData) {}
	string metaData() {
		string data = "Deletion\n";
		data += this->data + "\n";
		data += to_string(lineNumber) + "\n";
		for (int i = 0; i < rowData.size(); i++)
			data += rowData[i] + '\n';
		return data;
	}
};


//Updation in current data
struct Updation {
	vector<string> rowData;
	string old;
	int lineNumber;
	int column;
	Updation(vector<string> rowData, int lineNumber, int column, string old) : rowData(rowData), lineNumber(lineNumber), column(column), old(old) {}
	string metaData() {
		string data = "Updation\n";
		data += old + "\n" + rowData[column] + "\n" + to_string(column) + "\n" + to_string(lineNumber) + "\n";
		for (int i = 0; i < rowData.size(); i++) {
			data += rowData[i] + "\n";
		}
		return data;
	}
};


template<class T>
class Repository {
private:
	void printCommands() {
		cout << "\t1: Add a Node" << endl;
		cout << "\t2: Update a Node" << endl;
		cout << "\t3: Delete a Node" << endl;
		cout << "\t4: Switch Branch" << endl;
		cout << "\t5: Add A Branch" << endl;
		cout << "\t6: Visualize" << endl;
		cout << "\t7: Commit" << endl;
		cout << "\t8: View Data" << endl;
		cout << "\t9: Merge Branch " << endl;
		cout << "\t10: Roll Back to Version " << endl;
		cout << "\t11: Update All " << endl;
		cout << "\t12: Delete All " << endl;
		cout << "\t0: Save and Exit " << endl;
		cout << "\tChoose: ";
	}

public:
	string treeType;
	string name;
	string csv_path;
	int column, ln;
	//Create vector class later
	vector<string> branches;
	vector<string> roots;
	vector<string> header;
	vector<Addition> additions;
	vector<Deletion> deletions;
	vector<Updation> updations;
	string currBranch;
	Tree<T>* tree;
	bool useSha;
	float currVersion;

	Repository(Tree<T>* tree, string treeType) :tree(tree), currVersion(0.1), treeType(treeType) {
	}


	void create() {
		cout << "Enter Repo Name: ";
		cin >> name;
		cout << "Enter csv path: ";
		cin >> csv_path;

		cout << "Select Hashing Algorithm: \n0.Instructor's Hash \n1.SHA256\nSelect: ";
		cin >> useSha;
		create_directory(name);
		currBranch = "main";
		create_directory(name + "/" + currBranch);
		create_directory(name + "/" + currBranch + "/data");
		branches.push_back("main");

		tree->createNil();
		ifstream file(csv_path);
		if (!file.is_open()) {
			cerr << "Error: Could not open file!" << endl;
			return;
		}


		string line;
		getline(file, line);
		stringstream ss(line);
		string cell;
		int currentColumnIndex = 0;
		// Parse the line using ',' as a delimiter
		while (getline(ss, cell, ',')) {
			// Check if the cell starts with a quotation mark
			if (!cell.empty() && cell.front() == '"') {
				string quotedCell = cell;
				// Continue accumulating until we find the closing quote
				while (!quotedCell.empty() && quotedCell.back() != '"') {
					string nextPart;
					if (getline(ss, nextPart, ',')) {
						quotedCell += "," + nextPart; // Add delimiter and next part
					}
					else {
						break; // Exit if no more parts
					}
				}
				// Remove enclosing quotes
				if (!quotedCell.empty() && quotedCell.front() == '"' && quotedCell.back() == '"') {
					quotedCell = quotedCell.substr(1, quotedCell.size() - 2);
				}
				cell = quotedCell; // Update the cell to the accumulated quoted content
			}
			currentColumnIndex++;
			header.push_back(cell);
		}
		cout << "Columns: " << endl;
		for (int i = 0; i < header.size(); i++) {
			cout << i << ": " << header[i] << endl;
		}

		cout << "Enter column Number(0-indexed): ";
		cin >> column;

		cout << "Reading CSV to main branch(default): " << endl;
		ln = 2;

		while (getline(file, line)) {
			stringstream ss(line);
			string cell;
			int currentColumnIndex = 0;
			vector<string> rowData;
			// Parse the line using ',' as a delimiter
			while (getline(ss, cell, ',')) {
				// Check if the cell starts with a quotation mark
				if (!cell.empty() && cell.front() == '"') {
					string quotedCell = cell;
					// Continue accumulating until we find the closing quote
					while (!quotedCell.empty() && quotedCell.back() != '"') {
						string nextPart;
						if (getline(ss, nextPart, ',')) {
							quotedCell += "," + nextPart; // Add delimiter and next part
						}
						else {
							break; // Exit if no more parts
						}
					}
					// Remove enclosing quotes
					if (!quotedCell.empty() && quotedCell.front() == '"' && quotedCell.back() == '"') {
						quotedCell = quotedCell.substr(1, quotedCell.size() - 2);
					}
					cell = quotedCell; // Update the cell to the accumulated quoted content
				}

				if (currentColumnIndex == column) {
					tree->insert(cell, ln);

				}

				currentColumnIndex++;
				rowData.push_back(cell);
			}

			ofstream dataFile;
			dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
			for (int i = 0; i < rowData.size(); i++) {
				dataFile << rowData[i] << '\n';
			}
			dataFile.close();
			ln++;
		}
		cout << "reading done\n";
		file.close();
		tree->make();
		//tree->computeHash();
		tree->merkle = new MerkleTree<T>(tree->order, useSha);
		tree->changeBranch(tree->getRootFile());
		//cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
		string dataFolder = name + "\\" + currBranch + "\\" + "data";
		cout << "AFTER READING ...DATA FOLDER: " << dataFolder << endl << endl;
		cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
		roots.push_back(tree->getRootFile());
		cout << endl;

		ofstream log(name + "/" + currBranch + "/log.txt");
		log.close();


	}

	void main() {


		bool logic = true;
		while (logic) {
			printCommands();
			int opt;
			cin >> opt;

			switch (opt) {
			case 1:
				addNode();
				break;
			case 3:
				deleteNode();
				break;
			case 2:
				updateNode();
				break;
			case 4:
				switchBranch();
				break;
			case 5:
				addBranch();
				break;
			case 6:
				visualizeTree();
				break;
			case 7:
				commit();
				break;
			case 8:
				viewNodeData();
				break;
			case 9:
				mergeBranch();
				break;
			case 10:
				rollBackToVersion();
				break;
			case 11:
				updateAll();
				break;
			case 12:
				deleteAll();
				break;
			default:
				logic = false;
				break;
			}
		}
	}
	//Read Data from a line Number
	vector<string> readFileByLineNumber(int lineNumber) {
		vector<string> rowData;
		string filePath = name + "/" + currBranch + "/data/" + to_string(lineNumber) + ".txt";
		ifstream file(filePath);

		if (!file.is_open()) {
			cerr << "Error: Could not open file " << filePath << endl;
			return rowData;
		}

		string line;
		while (getline(file, line)) {
			rowData.push_back(line);
		}

		file.close();
		return rowData;
	}
	//Write Data to a Line Number
	void writeFileByLineNumber(int ln, vector<string>& rowData) {
		ofstream dataFile;
		dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
		for (int i = 0; i < rowData.size(); i++) {
			dataFile << rowData[i] << '\n';
		}
		dataFile.close();
	}


	//Add node to staging(Won't actually work unless commited)
	void addNode() {
		// T val;
		// cout << "Value to add: ";
		// cin >> val;
		vector<string> rowData;
		string data;
		cin.ignore();
		for (int i = 0; i < header.size(); i++) {
			cout << header[i] << ": ";

			getline(cin, data);
			//getline(data);
			rowData.push_back(data);
		}
		additions.emplace_back(Addition(rowData, ln));
	}

	//Permanently commit data
	void addDataFr(Addition& addition) {

		writeFileByLineNumber(ln, addition.rowData);
		tree->insert(addition.rowData[column], ln++);
		for (int i = 0; i < branches.size(); i++) {
			if (branches[i] == currBranch) {
				roots[i] = tree->getRootFile();
				return;
			}
		}
		//tree->computeHash();
	}

	//Delete node from staging(Won't actually work unless commited)
	void deleteNode() {
		T val;
		cout << "Value to delete: ";
		cin >> val;
		vector<int> l = tree->searchData(val);
		if (!l.empty()) {

			int opt;
			cout << "From which line number do you want to see data: ";
			for (int i = 0; i < l.size(); i++) {
				bool alreadyGone = false;
				for (int j = 0; j < deletions.size(); j++) {
					cout << deletions[j].lineNumber << " " << l[i] << endl;
					if (deletions[j].lineNumber == l[i]) {
						alreadyGone = true;
						break;
					}
				}
				if (!alreadyGone)
					cout << "Line Number: " << l[i] << endl;
			}
			cin >> opt;
			bool find = false;
			for (int i = 0; i < l.size(); i++) {
				if (l[i] == opt) {
					find = true;
					break;
				}
			}
			if (find) {
				vector<string> rowData = readFileByLineNumber(opt);
				deletions.push_back(Deletion(val, opt, rowData));
			}
		}
		else {
			for (int i = 0; i < additions.size(); i++) {
				if (additions[i].rowData[column] == val) {
					additions.erase(additions.begin() + i);
					//cout << "Deleted from line number: " << val << endl;
					return;
				}
			}

			// cout << "Not Found: " << endl;
		}
	}
	void deleteAll() {
		T val;
		cout << "Value to delete: ";
		cin >> val;
		vector<int> l = tree->searchData(val);
		if (!l.empty()) {

			for (int i = 0; i < l.size(); i++) {
				bool alrGone = false;
				for (int j = 0; j < deletions.size(); j++) {
					if (l[i] == deletions[j].lineNumber) {
						alrGone = true;
						break;
					}
				}
				if (!alrGone) {

					vector<string> rowData = readFileByLineNumber(l[i]);
					deletions.push_back(Deletion(val, l[i], rowData));
				}
			}

		}
		else {
			for (int i = 0; i < additions.size(); i++) {
				if (additions[i].rowData[column] == val) {
					additions.erase(additions.begin() + i);
					//cout << "Deleted from line number: " << val << endl;
					return;
				}
			}

			// cout << "Not Found: " << endl;
		}
	}
	//Permanently delete data
	void deleteDataFr(Deletion& deletion) {

		cout << deletion.data << " " << deletion.lineNumber << endl;

		tree->deleteByVal(deletion.data, deletion.lineNumber);

		cout << "Deleted from line number: " << deletion.lineNumber << endl;
		remove((name + "/" + currBranch + "/data/" + to_string_generic(deletion.lineNumber) + ".txt").c_str());
		for (int i = 0; i < branches.size(); i++) {
			if (branches[i] == currBranch) {
				roots[i] = tree->getRootFile();
				return;
			}
		}
	}

	//Update data to staging
	void updateNode() {
		T val, newVal;
		cout << "Value to update: ";
		cin >> val;
		//cout << "searching for data\n";
		vector<int> l = tree->searchData(val);


		if (!l.empty()) {
			//for (int i = 0; i < deletions.size(); i++) {
			//    if (deletions[i].lineNumber == ln) {
			//       // cout << "Not Found!" << endl;
			//        return;
			//    }
			//}

			int ln;
			cout << "From which line number do you want to see data: ";
			for (int i = 0; i < l.size(); i++) {
				bool alreadyGone = false;
				for (int j = 0; j < deletions.size(); j++) {
					if (deletions[j].lineNumber == l[i]) {
						alreadyGone = true;
						if (l.size() == 1)
							return;
						break;
					}
				}
				if (!alreadyGone)
					cout << "Line Number: " << l[i] << endl;
			}
			cin >> ln;
			bool find = false;
			for (int i = 0; i < l.size(); i++) {
				if (l[i] == ln) {
					find = true;
					break;
				}
			}


			if (find) {
				cout << "What do you want to change: " << endl;
				for (int i = 0; i < header.size(); i++) {
					cout << i << ": " << header[i] << endl;
				}
				int opt;
				cin >> opt;
				if (opt >= 0 && opt < header.size()) {
					vector<string> rowData = readFileByLineNumber(ln);
					cout << "Current " << header[opt] << ": " << rowData[opt] << endl;
					cout << "Updated " << header[opt] << ": ";
					string data;
					cin.ignore();
					getline(cin, data);
					string old = rowData[opt];
					rowData[opt] = data;
					updations.emplace_back(Updation(rowData, ln, opt, old));

					//writeFileByLineNumber(ln, rowData);
					// if (opt==column) {
					//     tree->deleteByVal(old,ln);
					//     tree->insert(data,ln);
					// }
				}
			}
		}
		else {
			for (int i = 0; i < additions.size(); i++) {
				if (additions[i].rowData[column] == val) {
					cout << "What do you want to change: " << endl;
					for (int i = 0; i < header.size(); i++) {
						cout << i << ": " << header[i] << endl;
					}
					int opt;
					cin >> opt;
					if (opt >= 0 && opt < header.size()) {
						cout << "Current " << header[opt] << ": " << additions[i].rowData[opt] << endl;
						cout << "Updated " << header[opt] << ": ";
						string data;
						cin.ignore();
						getline(cin, data);
						string old = additions[i].rowData[opt];
						additions[i].rowData[opt] = data;
						return;
					}
					else
						break;

				}
			}
		}
		cout << "Not Found: " << endl;
	}

	void updateAll() {
		T val, newVal;
		cout << "Value to update: ";
		cin >> val;
		cout << "searchind for dta\n";
		vector<int> l = tree->searchData(val);


		if (!l.empty()) {

			cout << "What do you want to change: " << endl;
			for (int i = 0; i < header.size(); i++) {
				cout << i << ": " << header[i] << endl;
			}
			int opt;
			cin >> opt;
			cout << "New " << header[opt] << ": ";
			string data;
			cin.ignore();
			getline(cin, data);
			for (int i = 0; i < l.size(); i++) {
				int ln = l[i];
				if (opt >= 0 && opt < header.size()) {
					vector<string> rowData = readFileByLineNumber(ln);
					string old = rowData[opt];
					rowData[opt] = data;
					updations.emplace_back(Updation(rowData, ln, opt, old));

					//writeFileByLineNumber(ln, rowData);
					// if (opt==column) {
					//     tree->deleteByVal(old,ln);
					//     tree->insert(data,ln);
					// }
				}
			}

		}
	}

	//Update data permanently
	void updateDataFr(Updation& update) {
		writeFileByLineNumber(update.lineNumber, update.rowData);
		if (update.column == column) {
			cout << "Updating\n";
			tree->deleteByVal(update.old, update.lineNumber);
			tree->insert(update.rowData[column], update.lineNumber);

			for (int i = 0; i < branches.size(); i++) {
				if (branches[i] == currBranch) {
					roots[i] = tree->getRootFile();
					return;
				}
			}


		}

	}


	void commit() {
		if (additions.empty() && deletions.empty() && updations.empty()) {
			cout << "Nothing to update" << endl;
			return;
		}

		//Write commit to commit file whilst also commiting everything
		cout << "Commit Message: ";
		string message;
		cin.ignore();
		getline(cin, message);
		ofstream file;
		std::ostringstream versionStream;
		versionStream << fixed << setprecision(1) << currVersion;
		string versionStr = versionStream.str();
		string path = name + "/" + currBranch + "/commit" + currBranch
			+ (versionStr)+".txt";
		file.open(path, ios::trunc);
		file << "Version: " << currVersion << endl;
		for (int i = 0; i < additions.size(); i++) {
			addDataFr(additions[i]);
			file << additions[i].metaData();
		}
		for (int i = 0; i < deletions.size(); i++) {
			deleteDataFr(deletions[i]);
			//cout<<"Deleting: "<<deletions[i].data<<endl;
			file << deletions[i].metaData();
		}
		for (int i = 0; i < updations.size(); i++) {
			updateDataFr(updations[i]);

			file << updations[i].metaData();
		}
		file << message << endl;
		file.close();
		currVersion += 0.1;
		ofstream log(name + "/" + currBranch + "/log.txt", ios::app);
		log << path << endl;
		log.close();
		tree->emptyTable();

		string dataFolder = name + "\\" + currBranch + "\\" + "data";
		cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;

		//Clearing up everything for future
		updations.clear();
		deletions.clear();
		additions.clear();
		cout << updations.size() << endl;
		cout << deletions.size() << endl;
		cout << additions.size() << endl;
	}

	void viewNodeData() {

		T data;
		cout << "Enter data to view";
		cin >> data;
		vector<int> toBeViewed = tree->searchData(data);

		if (toBeViewed.empty()) {
			cout << "Data not found!" << endl;
			return;
		}
		else {
			for (int i = 0; i < toBeViewed.size(); i++) {
				vector<string> rowData = readFileByLineNumber(toBeViewed[i]);

				for (int j = 0; j < rowData.size(); j++) {
					cout << header[j] << ": " << rowData[j] << endl;
				}
			}


		}
	}

	void visualizeTree() {
		tree->display();
	}
	//Switching branches
	void switchBranch() {

		if (!additions.empty() && !deletions.empty() && !updations.empty()) {
			cout << "Current Branch has uncommited changes!" << endl;
			return;
		}

		cout << "Select which branch you want: ";
		int n;
		for (int i = 0; i < branches.size(); i++) {
			cout << i + 1 << ": " << branches[i] << endl;
		}
		cin >> n;
		if (n - 1 >= 0 && n <= branches.size()) {
			tree->emptyTable();

			tree->changeBranch(roots[n - 1]);
			currBranch = branches[n - 1];
			cout << "Current Branch is set to: " << currBranch << endl;
		}
		else {
			cout << "Branch not found!" << endl;
		}


	}


	// Adding branch by copying current branch
	void addBranch() {
		if (!additions.empty() && !deletions.empty() && !updations.empty()) {
			cout << "Current Branch has uncommited changes!" << endl;
			return;
		}
		string newBranch;
		cout << "Enter the name for your new branch: ";
		cin >> newBranch;

		tree->emptyTable();
		branches.push_back(newBranch);

		tree->changeBranch(tree->getRootFile());
		currBranch = newBranch;
		roots.push_back(tree->getRootFile());

		cout << "New branch has been created and cloned by current version of main" << endl;
		cout << "Current Branch is set to: " << currBranch << endl;
		tree->merkle->currBranch = newBranch;

		path sourceDir = name + "/" + "main";
		path destinationDir = name + "/" + newBranch;

		create_directory(destinationDir);

		for (const auto& entry : recursive_directory_iterator(sourceDir)) {
			const auto& sourcePath = entry.path();
			auto destinationPath = destinationDir / relative(sourcePath, sourceDir);

			if (is_directory(sourcePath)) {
				create_directory(destinationPath);
			}
			else {
				copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);
			}
		}
		string dataFolder = name + "\\" + currBranch + "\\" + "data";
		cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
	}


	//Saving current state of repository to the file
	void saveRepoToFile() {
		ofstream repoFile(name + ".txt");
		repoFile << treeType << endl;
		repoFile << name << endl;
		repoFile << column << endl;
		repoFile << ln << endl;
		repoFile << branches.size() << endl;
		for (int i = 0; i < branches.size(); i++) {
			repoFile << branches[i] << endl;
		}
		for (int i = 0; i < roots.size(); i++) {
			repoFile << roots[i] << endl;
		}
		repoFile << header.size() << endl;
		for (int i = 0; i < header.size(); i++) {
			repoFile << header[i] << endl;
		}
		repoFile << currBranch << endl;
		repoFile << currVersion << endl;
		repoFile.close();
	}
	//Reading repository state from file
	void readFromFile(const string& path) {
		ifstream repoFile(path);
		if (!repoFile.is_open()) {
			cerr << "Error: Could not open file " << path << endl;
			return;
		}

		getline(repoFile, treeType);
		getline(repoFile, name);
		repoFile >> column;
		repoFile >> ln;
		cout << treeType << endl;
		cout << name << endl;
		cout << column << endl;
		int branchesSize;
		repoFile >> branchesSize;
		branches.clear();
		repoFile.ignore(); // Ignore the newline character after branchesSize
		for (int i = 0; i < branchesSize; i++) {
			string branch;
			getline(repoFile, branch);
			cout << branch << endl;
			branches.push_back(branch);
		}
		roots.clear();
		for (int i = 0; i < branchesSize; i++) {
			string branch;
			getline(repoFile, branch);
			cout << branch << endl;
			roots.push_back(branch);
		}
		int headerSize;
		repoFile >> headerSize;
		header.clear();
		repoFile.ignore(); // Ignore the newline character after headerSize
		for (int i = 0; i < headerSize; i++) {
			string headerItem;
			getline(repoFile, headerItem);
			header.push_back(headerItem);
		}

		getline(repoFile, currBranch);
		repoFile >> currVersion;

		repoFile.close();


		tree->changeBranch(roots[0]);
		currBranch = branches[0];
		tree->merkle = new MerkleTree<T>(tree->order, useSha);
		string dataFolder = name + "\\" + currBranch + "\\" + "data";
		cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;



	}

	//merging of branches 
	void mergeBranch() {
		if (!additions.empty() && !deletions.empty() && !updations.empty()) {
			cout << "Current Branch has uncommited changes!" << endl;
			return;
		}
		tree->emptyTable();
		cout << "Enter the name of the branch to merge with: ";
		string targetBranch;
		cin >> targetBranch;
		//now merge current branch adn target branch
		string currFolder = name + "\\" + currBranch + "\\" + "data";
		string targetFolder = name + "\\" + targetBranch + "\\" + "data";
		cout << currFolder << " and " << targetFolder << endl;

		MerkleTree<T>* targetMerkle = new MerkleTree<T>(tree->order, useSha);
		string targetHash = targetMerkle->buildMerkleTree(targetFolder)->hash;
		string currHash = tree->merkle->buildMerkleTree(currFolder)->hash;
		cout << "Currhash: " << currHash << endl;
		if (targetHash == currHash) {
			cout << "No change in data\n";
			cout << currBranch << " and " << targetBranch << " merged successfully\n";
		}
		else {
			cout << "Data has been changed in the currBranch...We need to merge\n";
			string path = "root";
			tree->merkle->lookForChange(targetMerkle->root, tree->merkle->root, path);
			compareLogs(currBranch, targetBranch);
		}
	}

	void compareLogs(string currBranch, string targetBranch) {
		ifstream curr(name + "\\" + currBranch + "\\" + "log.txt");
		ifstream target(name + "\\" + targetBranch + "\\" + "log.txt");

		if (!curr.is_open() || !target.is_open()) {
			cerr << "Error: Could not open log files for comparison." << endl;
			return;
		}

		vector<string> currLines;
		vector<string> targetLines;
		string line;

		// Read lines from the current branch log file
		while (getline(curr, line)) {
			currLines.push_back(line);
		}

		// Read lines from the target branch log file
		while (getline(target, line)) {
			targetLines.push_back(line);
		}

		curr.close();
		target.close();
		// Output the lines for verification
		int similar = 0;

		for (int i = 0; i < targetLines.size() && i < currLines.size(); i++) {
			if (currLines[i] != targetLines[i])
				break;
			similar++;
		}

		std::filesystem::copy(name + "\\" + targetBranch + "\\" + "log.txt", name + "\\" + currBranch + "\\" + "log.txt", std::filesystem::copy_options::overwrite_existing);
		makeChanges(targetBranch, currLines, similar);
	}

	void makeChanges(string& targetBranch, vector<string>& currLines, int& similar) {

		for (int i = 0; i < branches.size(); i++) {
			if (targetBranch == branches[i]) {
				tree->changeBranch(roots[i]);
				currBranch = targetBranch;
				break;
			}
		}
		//Making changes after reading logs
		for (int i = similar; i < currLines.size(); i++) {
			string str = readCommit(currLines[i]);
			performCommit(targetBranch, str);
		}
	}

	void performCommit(string& targetBranch, string& commit) {
		stringstream ss(commit);
		string line;
		while (getline(ss, line)) {
			if (line == "Addition") {
				//cout << "Wow addition" << endl;
				vector<string> rowData;
				for (int i = 0; i < header.size(); i++) {
					getline(ss, line);
					rowData.push_back(line);
				}

				getline(ss, line);
				int lineNumber = stoi(line);
				//cout << rowData[column] << " " << lineNumber << endl;
				tree->insert(rowData[column], lineNumber);

				writeFileByLineNumber(lineNumber, rowData);
				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}

			}
			else if (line == "Deletion") {
				//cout << "Wow Deletion" << endl;

				getline(ss, line);
				string data = line;
				getline(ss, line);
				int lineNumber = stoi(line);

				//cout << data << " " << lineNumber << endl;
				tree->deleteByVal(data, lineNumber);
				remove(name + "\\" + currBranch + "\\data\\" + to_string(lineNumber) + ".txt");
				for (int i = 0; i < header.size(); i++)
					getline(ss, line);
				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}
			}
			else if (line == "Updation") {
				//cout << "wow Updation" << endl;

				getline(ss, line);
				string old = line;
				getline(ss, line);
				string newD = line;
				getline(ss, line);
				int cn = stoi(line);
				getline(ss, line);
				int lineNumber = stoi(line);
				vector<string> rowData;
				for (int i = 0; i < header.size(); i++) {
					getline(ss, line);
					rowData.push_back(line);
				}
				writeFileByLineNumber(lineNumber, rowData);
				if (cn == column) {
					tree->deleteByVal(old, lineNumber);
					tree->insert(newD, lineNumber);
				}
				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}


				/*
				  string metaData() {
				string data = "Updation\n";
				data += old + "\n" + rowData[column] + "\n" +  to_string(column) + "\n" + to_string(lineNumber) + "\n";
				return data;
				 }
	 */
			}
			else {
				return;
			}
		}

	}

	string readCommit(string& path) {
		ifstream file(path);
		string result;

		if (!file.is_open()) {
			throw runtime_error("Could not open file: " + path);
		}

		string line;
		bool firstLineSkipped = false;

		while (getline(file, line)) {
			if (!firstLineSkipped) {
				firstLineSkipped = true; // Skip the first line
				continue;
			}
			result += line + "\n"; // Append the line with its newline character
		}

		if (!result.empty() && result.back() == '\n') {
			result.pop_back(); // Remove the trailing newline character (if needed)
		}

		return result;
	}


	void rollBackToVersion() {
		ifstream file(name + "\\" + currBranch + "\\" + "log.txt");
		vector<string> targetLines;
		string line;

		// Read lines from the current branch log file
		while (getline(file, line)) {
			targetLines.push_back(line);
		}
		file.close();
		cout << "Select which version you want to rollback to(0 to go back to default): " << endl;
		for (int i = 0; i < targetLines.size(); i++) {
			cout << i + 1 << ": " << targetLines[i] << endl;
		}
		//Select which version to roll back to
		int opt;
		cin >> opt;

		for (int i = targetLines.size() - 1; i >= opt; i--) {

			string str = readCommit(targetLines[i]);
			//cout << str << endl;
			performReverse(str);
			tree->emptyTable();
			currVersion -= 0.1;
		}

		ofstream file2(name + "\\" + currBranch + "\\" + "log.txt", ios::trunc);
		for (int i = 0; i < opt && i < targetLines.size(); i++) {
			file2 << targetLines[i] << endl;
		}
		file2.close();
	}
	void performReverse(string& commit) {
		stringstream ss(commit);
		string line;
		while (getline(ss, line)) {
			if (line == "Addition") {
				//cout << "Wow addition" << endl;
				vector<string> rowData;
				for (int i = 0; i < header.size(); i++) {
					getline(ss, line);
					rowData.push_back(line);
				}

				getline(ss, line);
				int lineNumber = stoi(line);
				cout << rowData[column] << " " << lineNumber << endl;
				tree->deleteByVal(rowData[column], lineNumber);

				remove(name + "\\" + currBranch + "\\data\\" + to_string(lineNumber) + ".txt");
				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}

			}
			else if (line == "Deletion") {
				cout << "Wow Deletion" << endl;

				getline(ss, line);
				string data = line;
				getline(ss, line);
				int lineNumber = stoi(line);

				//cout << data << " " << lineNumber << endl;
				vector<string> rowData;
				for (int i = 0; i < header.size(); i++) {

					getline(ss, line);
					rowData.push_back(line);
				}
				cout << rowData[column] << endl;
				tree->insert(rowData[column], lineNumber);

				tree->emptyTable();
				cout << "Hello" << endl;
				writeFileByLineNumber(lineNumber, rowData);
				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}
			}
			else if (line == "Updation") {
				//cout << "wow Updation" << endl;

				getline(ss, line);
				string old = line;
				getline(ss, line);
				string newD = line;
				getline(ss, line);
				int cn = stoi(line);
				getline(ss, line);
				int lineNumber = stoi(line);
				vector<string> rowData;
				for (int i = 0; i < header.size(); i++) {

					getline(ss, line);
					if (i != column)
						rowData.push_back(line);
					else
						rowData.push_back(old);

				}
				writeFileByLineNumber(lineNumber, rowData);
				if (cn == column) {
					tree->deleteByVal(newD, lineNumber);
					tree->insert(old, lineNumber);
				}

				for (int i = 0; i < branches.size(); i++) {
					if (branches[i] == currBranch) {
						roots[i] = tree->getRootFile();
					}
				}

				/*
				  string metaData() {
				string data = "Updation\n";
				data += old + "\n" + rowData[column] + "\n" +  to_string(column) + "\n" + to_string(lineNumber) + "\n";
				return data;
				 }
	 */
			}
			else {
				cout << "Dwad" << line << endl;
				return;
			}
		}

	}


	~Repository() {
		saveRepoToFile();
	}


};

//////////////////////////////////////////////////////source.cpp//////////////////////////////////////////////////////////////////
#include "Menu.h"
int main() {
	Menu m;

	m.main();


	return 0;
}










