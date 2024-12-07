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

	AVLNode(T c) :data(c), leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0),dirtyNode(0){
		fileName = to_string_generic(data) + ".txt";
	}
	AVLNode() :leftPath("NULL"), rightPath("NULL"), parentPath("NULL"), height(0),dirtyNode(0){
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
		printTree(Tree<T>::rootFile);
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
		if (cachedNode!=nullptr) {
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
	void insert(T data,int ln) {
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
			else if(Tree<T>::isEqual(data, currentNode->data) == -1) {//if data to be inserted is smaller than currentNode's data, go to left
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
		ht.insert(to_string_generic(parent->data),parent);

		// Start balancing the tree from the new node up to the root
		while (newNodeFile != "NULL") {
			AVLNode<T>* node = readNodeFromFile(newNodeFile);

			updateNodeHeight(*node);  // Update the height of the node
			node->dirty();
			ht.insert(to_string_generic(node->data),node);

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
	void changeBranch(const string &path) {

		ht.emptyTable();
		Tree<T>::rootFile = path;
	}

	//function that returns root's filepath
	string getRootFile() {
		return Tree<T>::rootFile;
	}

	//function to empty the hashtable
	void emptyTable() {
		ht.emptyTable();
	}
};
