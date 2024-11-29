#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Functions.h"
#include<string>
#include "Repository.h"
using namespace std;
using namespace std::filesystem;

bool RED = 1;
bool BLACK = 0;

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
private:
    string rootFile;
    string nil;
    Repository<T> repo;

    string pathify(string data) {
        string path = repo.name + "/" + repo.currBranch + "/" + to_string_generic(data);
        if (path.find(".txt") == std::string::npos) {  // If ".txt" is not found
            path += ".txt";  // Append ".txt" to the string
        }
        return path;
    }

    string createFile(const RedBlackNode<T>& node) {
        static int nodeCounter = 0;
        string fileName = to_string_generic(node.data);

        ofstream file(pathify(fileName));
        if (!file.is_open()) {
            throw runtime_error("Unable to create file: " + fileName);
        }

        // Write node data to the file
        file << node.data << "\n";                       // Node data
        file << node.parentPath << " "                   // Parent path
            << node.leftPath << " "                     // Left child path
            << node.rightPath << "\n";                  // Right child path
        file << node.color << "\n";                      // Node color (0 for BLACK, 1 for RED)

        file.close();
        return fileName;
    }

    RedBlackNode<T> readNodeFromFile(const string& filePath) {
        ifstream file(pathify(filePath));
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filePath);
        }

        T data;
        string parentPath, leftPath, rightPath;
        bool color;
        RedBlackNode<T> r;
        file >> r.data;

        file >> r.parentPath >> r.leftPath >> r.rightPath;
        file >> r.color;

        file.close();
        return r;
    }

    void writeNodeToFile(const RedBlackNode<T>& node) {
        std::ofstream file(pathify(to_string_generic(node.data)));

        file << node.data << "\n";                       // Node data
        file << node.parentPath << " "                   // Parent path
            << node.leftPath << " "                     // Left child path
            << node.rightPath << "\n";                  // Right child path
        file << node.color << "\n";                      // Node color (0 for BLACK, 1 for RED)

        file.close();

    }

    void createNil() {
        string niller = "nil.txt";
        ofstream file(pathify(niller));
        file << -1 << "\nNULL NULL NULL\n0\n";
        file.close();
    }

    void rotateLeft(const string& k1File) {
        cout << "rotateleft" << endl;
       
        RedBlackNode<T> k1 = readNodeFromFile(k1File);
        RedBlackNode<T> temp = readNodeFromFile(k1.rightPath);
        cout << k1.data<<endl;
        cout << temp.data << endl;
        k1.rightPath = temp.leftPath;
        if (temp.leftPath != "nil") {
            RedBlackNode<T> tempLeft = readNodeFromFile(temp.leftPath);
            tempLeft.parentPath = k1File;
            writeNodeToFile(tempLeft);
        }

        temp.parentPath = k1.parentPath;
        if (k1.parentPath == "NULL") {
            cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp.data);
        }
        else {
            RedBlackNode<T> parent = readNodeFromFile(k1.parentPath);
            if (k1File == parent.leftPath) {
                parent.leftPath = to_string_generic(temp.data);
            }
            else {
                parent.rightPath = to_string_generic(temp.data);
            }
            writeNodeToFile(parent);
        }

        temp.leftPath = k1File;
        writeNodeToFile(temp);
        k1.parentPath = to_string_generic(temp.data);
        writeNodeToFile(k1);
        k1.print();
        temp.print();
    }

    void rotateRight(const string& k1File) {
        cout << "rotateright" << endl;
        RedBlackNode<T> k1 = readNodeFromFile(k1File);
        RedBlackNode<T> temp = readNodeFromFile(k1.leftPath);

        k1.leftPath = temp.rightPath;
        if (temp.rightPath != "nil") {

            RedBlackNode<T> tempRight = readNodeFromFile(temp.rightPath);
            tempRight.parentPath = k1File;
            writeNodeToFile(tempRight);
        }

        temp.parentPath = k1.parentPath;
        if (k1.parentPath == "NULL") {
            cout << "Attempted Change" << endl;
            rootFile = to_string_generic(temp.data);
        }
        else {
            RedBlackNode<T> parent = readNodeFromFile(k1.parentPath);
            if (k1File == parent.rightPath) {
                parent.rightPath = to_string_generic(temp.data);
            }
            else {
                parent.leftPath = to_string_generic(temp.data);
            }
            writeNodeToFile(parent);
        }

        temp.rightPath = k1File;
        writeNodeToFile(temp);
        k1.parentPath = to_string_generic(temp.data);
        writeNodeToFile(k1);
    }

    void change(const string& nodeFile) {
        string currFile = nodeFile;
           while (currFile != rootFile) {
            RedBlackNode<T> node = readNodeFromFile(currFile);
            RedBlackNode<T> parent = readNodeFromFile(node.parentPath);
            cout << "changing" << endl;
             node = readNodeFromFile(currFile);
            parent = readNodeFromFile(node.parentPath);

            if (!parent.color)
                break;

            RedBlackNode<T> grandParent = readNodeFromFile(parent.parentPath);
        
                
            if (node.parentPath == grandParent.leftPath) {
                string uncFile = grandParent.rightPath;
                RedBlackNode<T> uncle = readNodeFromFile(uncFile);

                if (uncFile != "nil" && uncle.color) {
                    cout << "Recolor" << endl;
                    parent.color = BLACK;
                    uncle.color = BLACK;
                    grandParent.color = RED;
                    writeNodeToFile(parent);
                    writeNodeToFile(uncle);
                    writeNodeToFile(grandParent);
                    currFile = parent.parentPath;
                    node = grandParent;
                }
                else {
                    if (currFile == parent.rightPath) {
                        currFile = node.parentPath;
                        node = parent;
                        node.print();
                        rotateLeft(currFile);
                        node = readNodeFromFile(currFile);
                        parent = readNodeFromFile(node.parentPath);
                        grandParent =readNodeFromFile(parent.parentPath);
                       
                    }
                    parent.color = BLACK;
                    grandParent.color = RED;
                    writeNodeToFile(parent);
                    writeNodeToFile(grandParent);
                    rotateRight(parent.parentPath);
                }
            }
            else {
                string uncFile = grandParent.leftPath;
                RedBlackNode<T> uncle = readNodeFromFile(uncFile);

                if (uncFile != "nil" && uncle.color) {
                    cout << "Recolor" << endl;
                    parent.color = BLACK;
                    uncle.color = BLACK;
                    grandParent.color = RED;
                    writeNodeToFile(parent);
                    writeNodeToFile(uncle);
                    writeNodeToFile(grandParent);
                    currFile = parent.parentPath;
                    node = grandParent;
                }
                else {
                    if
                        (currFile == parent.leftPath) {
                        currFile = node.parentPath;
                        node = parent;
                        node.print();
                        rotateRight(currFile);
                        node = readNodeFromFile(currFile);
                        parent = readNodeFromFile(node.parentPath);
                        grandParent = readNodeFromFile(parent.parentPath);
                    }
                    parent.color = BLACK;
                    grandParent.color = RED;
                    writeNodeToFile(parent);
                    writeNodeToFile(grandParent);
                    rotateLeft(parent.parentPath);
                }
            }

        }

        RedBlackNode<T> rootNode = readNodeFromFile(rootFile);
        rootNode.color = BLACK;
        writeNodeToFile(rootNode);
    }

public:
    RedBlackTree() : repo(this) {
        rootFile = "NULL";
        nil = "nil";

        createNil();
        repo.create();
    }

    void insert(T data) {
        if (rootFile == "NULL") {
            RedBlackNode<T> rootNode(data);
            rootNode.color = BLACK;
            rootFile = createFile(rootNode);
            return;
        }
        else {
            string currFile = rootFile;
            string parFile = "NULL";

            while (currFile != "nil" && currFile != "NULL") {

                RedBlackNode<T> currNode = readNodeFromFile(currFile);

                RedBlackNode<T> node = currNode;
              
                parFile = currFile;

                if (Tree<T>::isEqual(data, currNode.data) == 0) {
                    return;
                }
                else if (Tree<T>::isEqual(data, currNode.data) == 1) {
                    currFile = currNode.rightPath;
                }
                else {
                    currFile = currNode.leftPath;
                }
            }

            RedBlackNode<T> newNode(data);
            newNode.parentPath = parFile;

            string newNodeFile = createFile(newNode);

            RedBlackNode<T> parNode = readNodeFromFile(parFile);
            if (Tree<T>::isEqual(data, parNode.data) == 1) {
                parNode.rightPath = newNodeFile;
            }
            else {
                parNode.leftPath = newNodeFile;
            }
            writeNodeToFile(parNode);

            if (newNode.parentPath == "NULL") {
                newNode.color = BLACK;
                writeNodeToFile(newNode);
                return;
            }

            if (parNode.parentPath == "NULL")
                return;

            change(newNodeFile);
           
        }
    }
};
