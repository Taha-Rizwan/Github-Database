#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Functions.h"
#include<string>
#include "Repository.h"
#include "LRUCache.h"
using namespace std;
using namespace std::filesystem;

bool RED = 1;
bool BLACK = 0;


void toLower(string& data) {
    for (int i = 0; i < data.length(); i++) {
        if (data[i] >= 'A' && data[i] <= 'Z')
            data[i] += 32;
    }
}

string toLowerString(string data) {
    for (int i = 0; i < data.length(); i++) {
        if (data[i] >= 'A' && data[i] <= 'Z')
            data[i] += 32;
    }
    return data;
}


// Node structure for the Red-Black Tree
// Node structure for the Red-Black Tree
template<class T>
struct RedBlackNode {
    T data;                     // Data stored in the node
    bool color;                 // 0 for BLACK, 1 for RED
    string leftPath;            // File path to the left child
    string rightPath;           // File path to the right child
    string parentPath;          // File path to the parent
    string hash;                // Hash of the node (empty for now)

    RedBlackNode(T data) : data(data), color(RED), leftPath("nil"), rightPath("nil"), parentPath("NULL"), hash("") {}

    RedBlackNode() : color(BLACK), leftPath("nil"), rightPath("nil"), parentPath("NULL"), hash("") {}
    void print() {
        cout << "Node Data: " << data << ", Parent: " << parentPath
            << ", Left: " << leftPath << ", Right: " <<rightPath << endl;
    }
};



// Red-Black Tree class
template<class T>
class RedBlackTree : public Tree<T> {
    //For Cache
    //Double Linked List
    

private:
    string rootFile;
    string nil;
    Repository<T> repo;

    //for cache
    struct Node {
    public:
        string key;
        RedBlackNode<T>* value;
        Node* chainNext;
        Node* prev;
        Node* next;

        Node(string k, RedBlackNode<T>* v) : key(k), value(v), next(nullptr), chainNext(nullptr), prev(nullptr) {}
        ~Node() {
           /* delete value;
            value = nullptr;*/
        }
    };



    struct HashTable {
    private:
        int capacity;
        int currSize;
        Node** arr;
        Node* head, * tail;
        RedBlackTree<T>* parentTree;
        int Hash_Function(string& key) {
            int j = 0;
            for (int i = 0; i < key.length(); i++)
                j += key[i];
            return j % capacity;
        }

    public:
        HashTable(RedBlackTree<T>* parentTree, int capacity = 10) : capacity(capacity),parentTree(parentTree) {
            head = nullptr;
            tail = nullptr;
            currSize = 0;
            arr = new Node * [capacity];
            for (int i = 0; i < capacity; i++) {
                arr[i] = nullptr;
            }
        }

        void insert(string key, RedBlackNode<T>* value) {
            cout << "Inserting: " << key << endl;
            int index = Hash_Function(key);
            //cout << "Inserting key: " << key << endl;
            RedBlackNode<T>* dup = search(key);
            if (dup) {
               /* remove(key);
                insert(key,dup);*/
                return;
            }
             Node* newNode = new Node(key, value);

            if (!arr[index]) {
               
                arr[index] = newNode;
            }
            else {
                //Inserting at the end, higher time complexity + Stupid
               /* Node* ptr = arr[index];
                while (ptr->next)
                    ptr = ptr->next;
                ptr->next = newNode;*/
                Node* temp = arr[index];
               // cout << "MEOW MEOW" << endl;
  

                newNode->chainNext = arr[index];

                arr[index] = newNode;

            }

            if (head == nullptr) {

                head = newNode;
                tail = head;
            }
            else {
                newNode->next = head;
                head->prev = newNode;
                head = newNode;

            }
           // cout << "Adding Node to Cache: " << endl;
            //newNode->value->print();
            if (capacity == currSize) {
      
                remove(tail->key);
           
            }
            currSize++;
            cout << currSize << endl;
           
        }

        void remove(string key) {
            if (key == "NULL" || key == "nil")
                return;
            currSize--;
            cout << "Removing key: " << key << endl;
            int index = Hash_Function(key);
            Node* current = arr[index];
            Node* prev = nullptr;
            cout << "WRITING TO FILE!" << endl;
            while (current != nullptr) {
                if (current->key == key) {
                    if (prev == nullptr) {
                        arr[index] = current->chainNext;
                    }
                    else {

                        prev->chainNext = current->chainNext;
                    }
                    //cout << current->value;
                    if (current == head) {
                        head->next->prev = nullptr;
                        head = head->next;
                    }
                    else if (current == tail) {
                        tail->prev->next = nullptr;
                        tail = tail->prev;
                    }
                    else {
                        current->prev->next = current->next;
                        current->next->prev = current->prev;
                    }
                    parentTree->writeNodeToFile(current->value);
                    delete current;
                    current = nullptr;
                    return;
                }
                prev = current;
                current = current->chainNext;
            }
        }

        RedBlackNode<T>* search(string key) {
            int index = Hash_Function(key);
            cout << "Searching key: " << key << endl;
            Node* current = arr[index];

            while (current != nullptr) {
                if (current->key == key) {
                   // current->value->print();
                    return current->value;
                }
                current = current->chainNext;
            }
           // cout << "Not found." << endl;
            return nullptr;
        }



        ~HashTable() {
            cout << "Destructor" << endl;
            for (int i = 0; i < 10; i++) {
                if (arr[i]) {
                    while (arr[i]) {
                        parentTree->writeNodeToFile(arr[i]->value);
                        arr[i] = arr[i]->chainNext;
                    }
                }
            }
        }

    };

    HashTable ht;

     string pathify(string data) {
         cout << "Pathifying" << endl;
        toLower(data);
        string path = repo.name + "/" + repo.currBranch + "/" + to_string_generic(data);
        if (path.find(".txt") == std::string::npos) {  // If ".txt" is not found
            path += ".txt";  // Append ".txt" to the string
        }
        return path;
    }

    string createFile( RedBlackNode<T>* node) {
        static int nodeCounter = 0;
        string fileName = to_string_generic(node->data);
        ht.insert(to_string_generic(node->data), node);
  
        return fileName;
    }

    RedBlackNode<T>* readNodeFromFile( string filePath) {
        cout << "reading" << endl;
        
        cout << filePath << endl;
        RedBlackNode<T>* r = ht.search(filePath);
        if (r!=nullptr)
            return r;
        ifstream file(pathify(filePath));
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filePath);
        }
        r = new RedBlackNode<T>();

        r->leftPath = "";
        r->rightPath = "";
        r->parentPath = "";
        string pathsLine;
        getline(file, pathsLine);
        r->data = pathsLine;

  
        getline(file, pathsLine);
        r->parentPath = pathsLine;
        getline(file, pathsLine);
        r->leftPath = pathsLine;
        getline(file, pathsLine);
        r->rightPath = pathsLine;
       
        file >> r->color;
       
        file.close();

        ht.insert(to_string_generic(r->data),r);
        cout << "Done reading" << endl;
        cout << r->data<<endl;
        return r;
    }

   void writeNodeToFile(RedBlackNode<T>* node) {
       cout << "Writing" << endl;
        std::ofstream file(pathify(to_string_generic(node->data)));

        file << node->data << "\n";                       // Node data
        file << node->parentPath << "\n"                   // Parent path
            << node->leftPath << "\n"                     // Left child path
            << node->rightPath << "\n";                  // Right child path
        file << node->color << "\n";                      // Node color (0 for BLACK, 1 for RED)

        file.close();

    }

    void createNil() {
        string niller = "nil.txt";
        ofstream file(pathify(niller));
        file << -1 << "\nNULL\nNULL\nNULL\n0\n";
        file.close();
    }
    //Saves a couple file opening operations
    void rotateLeft(RedBlackNode<T>* k1,string k1File) {
        cout << "rotateleft" << endl;

        RedBlackNode<T>* temp = readNodeFromFile(k1->rightPath);
        //cout << k1->data << endl;
       // cout << temp->data << endl;
        k1->rightPath = temp->leftPath;
        if (temp->leftPath != "nil") {
            RedBlackNode<T>* tempLeft = readNodeFromFile(temp->leftPath);
            tempLeft->parentPath = k1File;
            ht.insert(to_string_generic(tempLeft->data),tempLeft);
        }

        temp->parentPath = k1->parentPath;
        if (k1->parentPath == "NULL") {
           // cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp->data);
        }
        else {
            RedBlackNode<T>* parent = readNodeFromFile(k1->parentPath);
            if (k1File == parent->leftPath) {
                parent->leftPath = to_string_generic(temp->data);
            }
            else {
                parent->rightPath = to_string_generic(temp->data);
            }
            ht.insert(to_string_generic(parent->data),parent);
        }

        temp->leftPath = k1File;
        ht.insert(to_string_generic(temp->data), temp);
        k1->parentPath = to_string_generic(temp->data);
        ht.insert(to_string_generic(k1->data), k1);
      //  k1->print();
      //  temp->print();
    }
    void rotateLeft( string k1File) {
        cout << "rotateleft" << endl;
       
        RedBlackNode<T>* k1 = readNodeFromFile(k1File);
        RedBlackNode<T>* temp = readNodeFromFile(k1->rightPath);
      // cout << k1->data<<endl;
      // cout << temp->data << endl;
        k1->rightPath = temp->leftPath;
        if (temp->leftPath != "nil") {
            RedBlackNode<T>* tempLeft = readNodeFromFile(temp->leftPath);
            tempLeft->parentPath = k1File;
            insert(to_string_generic(tempLeft->data), tempLeft);
        }

        temp->parentPath = k1->parentPath;
        if (k1->parentPath == "NULL") {
         //  cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp->data);
        }
        else {
            RedBlackNode<T> parent = readNodeFromFile(k1->parentPath);
            if (k1File == parent->leftPath) {
                parent->leftPath = to_string_generic(temp->data);
            }
            else {
                parent->rightPath = to_string_generic(temp->data);
            }
            ht.insert(to_string_generic(parent->data), parent);
        }

        temp->leftPath = k1File;
        ht.insert(to_string_generic(temp->data), temp);
        k1->parentPath = to_string_generic(temp.data);
        ht.insert(to_string_generic(k1->data), k1);
      //  k1->print();
      //  temp->print();
    }
    //Saves a couple file opening operations
    void rotateRight(RedBlackNode<T>* k1,  string k1File) {
       cout << "rotateright" << endl;
        RedBlackNode<T>* temp = readNodeFromFile(k1->leftPath);

        k1->leftPath = temp->rightPath;
        if (temp->rightPath != "nil") {

            RedBlackNode<T>* tempRight = readNodeFromFile(temp->rightPath);
            tempRight->parentPath = k1File;
            ht.insert(to_string_generic(tempRight->data), tempRight);
        }

        temp->parentPath = k1->parentPath;
        if (k1->parentPath == "NULL") {
        //    cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp->data);
        }
        else {
            RedBlackNode<T>* parent = readNodeFromFile(k1->parentPath);
            if (k1File == parent->rightPath) {
                parent->rightPath = to_string_generic(temp->data);
            }
            else {
                parent->leftPath = to_string_generic(temp->data);
            }
            //writeNodeToFile(parent);
            ht.insert(to_string_generic(parent->data), parent);
        }

        temp->rightPath = k1File;
       // writeNodeToFile(temp);
        ht.insert(to_string_generic(temp->data), temp);
        k1->parentPath = to_string_generic(temp->data);
        //writeNodeToFile(k1);
        ht.insert(to_string_generic(k1->data), k1);
    }

    void rotateRight(string k1File) {
        cout << "rotateright" << endl;
        RedBlackNode<T>* k1 = readNodeFromFile(k1File);
        RedBlackNode<T>* temp = readNodeFromFile(k1->leftPath);

        k1->leftPath = temp->rightPath;
        if (temp->rightPath != "nil") {

            RedBlackNode<T>* tempRight = readNodeFromFile(temp->rightPath);
            tempRight->parentPath = k1File;
            //writeNodeToFile(tempRight);
            ht.insert(to_string_generic(tempRight->data), tempRight);
        }

        temp->parentPath = k1->parentPath;
        if (k1->parentPath == "NULL") {
       //      cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp.data);
        }
        else {
            RedBlackNode<T>* parent = readNodeFromFile(k1->parentPath);
            if (k1File == parent->rightPath) {
                parent->rightPath = to_string_generic(temp->data);
            }
            else {
                parent->leftPath = to_string_generic(temp->data);
            }
            //writeNodeToFile(parent);
            ht.insert(to_string_generic(parent->data), parent);
        }

        temp->rightPath = k1File;
        //writeNodeToFile(temp);
        ht.insert(to_string_generic(temp->data), temp);
        k1->parentPath = to_string_generic(temp->data);
        ht.insert(to_string_generic(k1->data), k1);
       // writeNodeToFile(k1);
    }

    void change( string nodeFile) {
        string currFile = nodeFile;
           while (currFile != rootFile) {
            RedBlackNode<T>* node = readNodeFromFile(currFile);
            RedBlackNode<T>* parent = readNodeFromFile(node->parentPath);
            node->print();
            parent->print();
          cout << "changing" << endl;
            if (!parent->color)
                break;

            RedBlackNode<T>* grandParent = readNodeFromFile(parent->parentPath);
            //node->print();
            grandParent->print();
                
            if (node->parentPath == grandParent->leftPath) {
                string uncFile = grandParent->rightPath;
                RedBlackNode<T>* uncle = readNodeFromFile(uncFile);
                uncle->print();
                if (uncFile != "nil" && uncle->color) {
                   cout << "Recolor" << endl;
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandParent->color = RED;
                    ht.insert(to_string_generic(parent->data), parent);
                    ht.insert(to_string_generic(uncle->data), uncle);
                    ht.insert(to_string_generic(grandParent->data), grandParent);
                   /* writeNodeToFile(parent);
                    writeNodeToFile(uncle);
                    writeNodeToFile(grandParent);*/
                    currFile = parent->parentPath;
                    node = grandParent;
                }
                else {
                    if (currFile == parent->rightPath) {
                        currFile = node->parentPath;
                        node = parent;
                      //  node->print();
                        rotateLeft(node,currFile);
                        parent = readNodeFromFile(node->parentPath);
                        grandParent =readNodeFromFile(parent->parentPath);
                       
                    }
                    parent->color = BLACK;
                    grandParent->color = RED;
                    ht.insert(to_string_generic(parent->data), parent);
                    ht.insert(to_string_generic(grandParent->data), grandParent);
                  /*  writeNodeToFile(parent);
                    writeNodeToFile(grandParent);*/
                    rotateRight(grandParent,parent->parentPath);
                }
            }
            else {
                string uncFile = grandParent->leftPath;
                RedBlackNode<T>* uncle = readNodeFromFile(uncFile);

                if (uncFile != "nil" && uncle->color) {
                   cout << "Recolor" << endl;
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandParent->color = RED;
                    ht.insert(to_string_generic(parent->data), parent);
                    ht.insert(to_string_generic(uncle->data), uncle);
                    ht.insert(to_string_generic(grandParent->data), grandParent);
                    currFile = parent->parentPath;
                    node = grandParent;
                }
                else {
                    if
                        (currFile == parent->leftPath) {
                        currFile = node->parentPath;
                        node = parent;
                     //   node->print();
                        rotateRight(node,currFile);
                        parent = readNodeFromFile(node->parentPath);
                        grandParent = readNodeFromFile(parent->parentPath);
                    }
                    parent->color = BLACK;
                    grandParent->color = RED;
                    ht.insert(to_string_generic(parent->data), parent);
                    ht.insert(to_string_generic(grandParent->data), grandParent);
                    rotateLeft(grandParent,parent->parentPath);
                }
            }

        }

        RedBlackNode<T>* rootNode = readNodeFromFile(rootFile);
        rootNode->color = BLACK;
        ht.insert(to_string_generic(rootNode->data), rootNode);

    }

public:
    RedBlackTree() : repo(this),ht(this) {
        rootFile = "NULL";
        nil = "nil";

        createNil();
        repo.create();



    }

    void insert(T data) {

        toLower(data);


        if (rootFile == "NULL") {
            RedBlackNode<T>* rootNode = new RedBlackNode<T>(data);
            rootNode->color = BLACK;
            rootFile = createFile(rootNode);
            return;
        }
        else {
            string currFile = rootFile;
            string parFile = "NULL";

            while (currFile != "nil" && currFile != "NULL") {

                RedBlackNode<T>* currNode = readNodeFromFile(currFile);

                parFile = currFile;

                if (Tree<T>::isEqual(data, currNode->data) == 0) {
                  //  cout << "Dup" << endl;
                    
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

            string newNodeFile = createFile(newNode);

            RedBlackNode<T>* parNode = readNodeFromFile(parFile);
            if (Tree<T>::isEqual(data, parNode->data) == 1) {
                parNode->rightPath = newNodeFile;
            }
            else {
                parNode->leftPath = newNodeFile;
            }
           // writeNodeToFile(parNode);
            ht.insert(to_string_generic(parNode->data), parNode);
            if (newNode->parentPath == "NULL") {
                newNode->color = BLACK;
                //writeNodeToFile(newNode);
                ht.insert(to_string_generic(newNode->data), newNode);
                return;
            }

            if (parNode->parentPath == "NULL") {

                return;
            }
            cout << parNode->parentPath << endl;
            change(newNodeFile);
           
        }
    }
};
