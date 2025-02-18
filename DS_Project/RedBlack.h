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
                    if(currFile == parent->leftPath) {
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
        if (path =="nil"|| path =="NULL") return 0;
        RedBlackNode<T>* root = readNodeFromFile(path);
        return 1 + max(getHeight(root->leftPath), getHeight(root->rightPath));
    }
    void printSpaces(int count) {
        for (int i = 0; i < count; ++i)
            cout << " ";
    }

    void visualizeTree(string path) {
        if (path=="NULL"||path=="nil") {
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

                if (current!="NULL" && current!="nil") {
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
    int deleteByVal(T val,bool updation =false) {
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
                cout<<"Update Line Number: ";
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
        if (path == "NULL"||path=="nil")
            return {};
        RedBlackNode<T> * node = readNodeFromFile(path);
        
        return node->lineNumbers;

    }

    void changeBranch(const string &path) {

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