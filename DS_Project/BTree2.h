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
            for(int i = 0;i<kiddies;i++)
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

    void insert(T k,int ln) { //inserting a key in a BTree
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