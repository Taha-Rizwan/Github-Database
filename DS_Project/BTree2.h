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
    vector<T> keys;
    vector<BTreeNode<T>*> children;
    bool leaf;
    BTreeNode<T>* parent;
    string parentPath;
    string leftChildPath;
    string rightChildPath;
    vector<string> childPath;
    BTreeNode(bool l = true) : leaf(l), parent(nullptr), parentPath("null"), leftChildPath("null"), rightChildPath("null") {}
};

template <typename T>
class BTree :public Tree<T> {
private:
    BTreeNode<T>* root;
    string rootPath;
    int m;
    Repository<T> repo;

public:
    BTree(int degree = 0) :repo(this, "BTree"), m(degree), rootPath("null") {
        rootPath = "\0";
        root = nullptr;
        repo.create();
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
        //cout << "Pathifying" << endl;
        Tree<T>::toLower(data);
        string path = repo.name + "/" + repo.currBranch + "/" + to_string_generic(data) + ".txt";
        //if (path.find(".txt") == std::string::npos) {  // If ".txt" is not found
        //    path += ".txt";  // Append ".txt" to the string
        //}
        return path;
    }

    void make() {
        root = readNodeFromFile(rootPath);
        print(root);
        cout << endl;
        //BTreeNode<T>* temp = nullptr;
        //rootPath = createFile(root);
        //Tree<T>::rootFile = rootPath;
        //queue<BTreeNode<T>*> q;
        //q.push(root);
        //while (!q.empty()) {
        //    temp = q.front();
        //    q.pop();
        //    for (int i = 0; i < temp->children.size(); i++)
        //        q.push(temp->children[i]);
        //    //writeNodeToFile(temp);
        //}
        //readNodeFromFile(rootPath);
    }

    BTreeNode<T>* readNodeFromFile(string path, bool check = true, bool check1 = true) {
        if (path == "null" || path == "")
            return nullptr;

        //path = pathify(path);
        ifstream f((path));
        if (!f.is_open())
            throw runtime_error("Unable to open file: " + path);

        BTreeNode<T>* r = new BTreeNode<T>(m);

        string line;
        getline(f, line);
        if (line == "no")
            r->leaf = false;
        else
            r->leaf = true;

        vector<T> keys;
        getline(f, line);
        keys.push_back((line));

        string leftKey, rightKey;
        getline(f, line);
        stringstream ss(line);
        getline(ss, leftKey, ',');
        getline(ss, rightKey, ',');

        string extra;
        while (leftKey != "null") {
            ifstream file(pathify(leftKey));
            getline(file, extra);
            getline(file, extra);
            bool found = false;
            for (int i = 0; i < keys.size(); i++)
                if (keys[i] == extra) {
                    found = true;
                    break;
                }
            if (found)
                break;
            keys.push_back((extra));
            getline(file, extra);
            stringstream yy(extra);
            getline(yy, extra, ',');
        }
        extra = "";
        while (rightKey != "null") {
            ifstream file(pathify(rightKey));
            getline(file, extra);
            getline(file, extra);
            bool found = false;
            for (int i = 0; i < keys.size(); i++)
                if (keys[i] == extra) {
                    found = true;
                    break;
                }
            if (found)
                break;
            keys.push_back((extra));
            getline(file, extra);
            stringstream yy(extra);
            getline(yy, extra, ',');
        }
        for (int i = 0; i < keys.size(); i++)
            r->keys.push_back(keys[i]);

        mySort(r->keys);

        getline(f, line);
        r->parentPath = line;
        if (r->parentPath != "null" && check1) {
            r->parent = readNodeFromFile(r->parentPath, false);
            if (r->parent) {
                for (int i = 0; i < r->parent->childPath.size(); i++)
                    r->parent->children.push_back(readNodeFromFile(r->parent->childPath[i], false, false));
                /*if (r->parent->leftChildPath != "null")
                    r->parent->children.push_back(readNodeFromFile(r->parent->leftChildPath, false, false));
                if (r->parent->rightChildPath != "null")
                    r->parent->children.push_back(readNodeFromFile(r->parent->rightChildPath, false, false));*/
            }
        }
        while (getline(f, line))
            r->childPath.push_back(line);
       /* getline(f, line);
        r->leftChildPath = line;
        getline(f, line);
        r->rightChildPath = line;*/
        if (!check)
            return r;
        for (int i = 0; i < r->childPath.size(); i++)
            if (r->childPath[i]!="null")
            r->children.push_back(readNodeFromFile(r->childPath[i]));
        /*if (r->leftChildPath != "null")
            r->children.push_back(readNodeFromFile(r->leftChildPath));
        if (r->rightChildPath != "null")
            r->children.push_back(readNodeFromFile(r->rightChildPath));*/

        f.close();

        return r;
    }

    string createFile(BTreeNode<T>* node) {
        static int counter = 0;
        string file = to_bString(node->keys);
        return file;
    }

    void writeNodeToFile(BTreeNode<T>* node) {
        ofstream* file = new ofstream[node->keys.size()];
        for (int i = 0; i < node->keys.size(); i++) {
            file[i].open(pathify(node->keys[i]));
            if (!file[i].is_open()) {
                throw runtime_error(pathify(node->keys[i]) + " not opened");
            }

            if (node->leaf)
                file[i] << "yes\n";
            else
                file[i] << "no\n";

            file[i] << to_string_generic(node->keys[i]) << "\n";

            if (i == 0)
                file[i] << "null,";
            else
                file[i] << to_string_generic(node->keys[i - 1]) << ",";

            if (i < node->keys.size() - 1)
                file[i] << to_string_generic(node->keys[i + 1]) << "\n";
            else
                file[i] << "null\n";

            if (node->parent)
                file[i] << node->parentPath << "\n";
            else
                file[i] << "null\n";

            if (node->childPath.size() > 0) {
                for (int j = 0; j < node->childPath.size(); j++)
                    file[i] << node->childPath[j] << '\n';
            }
            else {
                file[i] << "null\nnull\n";
            }
            file[i].close();
        }
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

        else //recursive search
            return search(key, insert, node->children[i], i);
    }

    BTreeNode<T>* searchForDelete(T key, int& i, int& index, BTreeNode<T>* node = nullptr, int childIndex = 0) {
        if (!root) {
            i = index = -1;
            return nullptr;
        }

        if (!node)
            node = root;

        int j = 0;
        while (j < node->keys.size() && key > node->keys[j])
            j++;

        if (j < node->keys.size() && key == node->keys[j]) { //if the key is found, the function gives the information about its location
            i = j;
            index = childIndex;
            return node;
        }
        else if (node->leaf) { //if its a leaf node, then it gives information that the key is not present
            i = index = -1;
            return nullptr;
        }
        else {   //performs recursive search
            return searchForDelete(key, i, index, node->children[j], j);
        }
    }


    void setLeafNodes() { //function to set leaf nodes in the BTree
        queue<BTreeNode<T>*> levelOrderQueue;
        levelOrderQueue.push(root);
        root->parent = nullptr;
        while (!levelOrderQueue.empty()) { //performing BFS to set all leafs with needing recursion
            BTreeNode<T>* curr = levelOrderQueue.front();
            levelOrderQueue.pop();
            if (curr->children.size() == 0)  //checks if the curr node has no children
                curr->leaf = true;
            else
                curr->leaf = false;
            for (size_t i = 0; i < curr->children.size(); ++i)
                levelOrderQueue.push(curr->children[i]);
        }
    }

    void sortForRoot(BTreeNode<T>*& node) {
        if (node->children.size() == 0)
            return;
        for (int j = 0; j < node->children.size(); j++) {
            for (int i = 0; i < node->children.size() - 1; i++) {
                if (node->children[i]->keys[0] > node->children[i + 1]->keys[0])
                    swap(node->children[i], node->children[i + 1]);
            }
        }
    }
    void mySort(vector<T>& arr) {
        for (int i = 0; i < arr.size(); i++)
            for (int j = 0; j < arr.size() - 1; j++)
                if (Tree<T>::isEqual(arr[j], arr[j + 1]) == 1)
                    swap(arr[j], arr[j + 1]);
    }
    bool checkExists(BTreeNode<T>*& node, BTreeNode<T>*& check) {
        for (int i = 0; i < check->keys.size(); i++)
            for (int j = 0; j < node->keys.size(); j++)
                if (node->keys[j] == check->keys[i])
                    return true;
        return false;
    }

    BTreeNode<T>* readSurfaceNodeFromFile(string path, bool check1 = true) {
        if (path == "null" || path == "")
            return nullptr;

        //path = pathify(path);
        ifstream f((path));
        if (!f.is_open())
            throw runtime_error("Unable to open file: " + path);

        BTreeNode<T>* r = new BTreeNode<T>(m);

        string line;
        getline(f, line);
        if (line == "no")
            r->leaf = false;
        else
            r->leaf = true;

        vector<T> keys;
        getline(f, line);
        keys.push_back((line));

        string leftKey, rightKey;
        getline(f, line);
        stringstream ss(line);
        getline(ss, leftKey, ',');
        getline(ss, rightKey, ',');

        string extra;
        while (leftKey != "null") {
            ifstream file(pathify(leftKey));
            getline(file, extra);
            getline(file, extra);
            bool found = false;
            for (int i = 0; i < keys.size(); i++)
                if (keys[i] == extra) {
                    found = true;
                    break;
                }
            if (found)
                break;
            keys.push_back((extra));
            getline(file, extra);
            stringstream yy(extra);
            getline(yy, extra, ',');
        }
        extra = "";
        while (rightKey != "null") {
            ifstream file(pathify(rightKey));
            getline(file, extra);
            getline(file, extra);
            bool found = false;
            for (int i = 0; i < keys.size(); i++)
                if (keys[i] == extra) {
                    found = true;
                    break;
                }
            if (found)
                break;
            keys.push_back((extra));
            getline(file, extra);
            stringstream yy(extra);
            getline(yy, extra, ',');
        }
        for (int i = 0; i < keys.size(); i++)
            r->keys.push_back(keys[i]);

        mySort(r->keys);

        getline(f, line);
        r->parentPath = line;
        if (r->parentPath != "null" && check1) {
            r->parent = readNodeFromFile(r->parentPath, false);
            //if (r->parent) {
            //    for (int i = 0; i < r->parent->childPath.size(); i++)
            //        r->parent->children.push_back(readNodeFromFile(r->parent->childPath[i], false, false));
            //    /*if (r->parent->leftChildPath != "null")
            //        r->parent->children.push_back(readNodeFromFile(r->parent->leftChildPath, false, false));
            //    if (r->parent->rightChildPath != "null")
            //        r->parent->children.push_back(readNodeFromFile(r->parent->rightChildPath, false, false));*/
            //}
        }
        while (getline(f, line))
            r->childPath.push_back(line);
         getline(f, line);
         r->leftChildPath = line;
         getline(f, line);
         r->rightChildPath = line;
        /*if (!check)
            return r;
        for (int i = 0; i < r->childPath.size(); i++)
            if (r->childPath[i] != "null")
                r->children.push_back(readNodeFromFile(r->childPath[i]));*/
        /*if (r->leftChildPath != "null")
            r->children.push_back(readNodeFromFile(r->leftChildPath));
        if (r->rightChildPath != "null")
            r->children.push_back(readNodeFromFile(r->rightChildPath));*/

        f.close();

        return r;
    }
    BTreeNode<T>* readNodeFromFileForInsertion(string root, string path) {
        if (root == "null" || root=="")
            return nullptr;
        BTreeNode<T>* x = readSurfaceNodeFromFile(root);
        string setPath = pathify(to_bString(x->keys));
        while (!x->leaf) {
            for (int i = 0; i < x->childPath.size() - 1; i++) {
                int f = Tree<T>::isEqual(setPath, path);
                if (f == 1) {
                    setPath = x->childPath[i];
                    break;
                }
                else
                    setPath = x->childPath[i + 1];

            }
            //path = setPath;
            x = readSurfaceNodeFromFile(setPath);
            ifstream f(setPath);
            string line;
            getline(f, line);
            getline(f, line);
            getline(f, line);
            vector<T> keys;
            getline(f, line);
            x->parentPath = line;
            for (int i = 0; i < keys.size(); i++)
                x->keys.push_back(keys[i]);

            auto it = unique(x->keys.begin(), x->keys.end());

            // Remove all duplicates
            x->keys.erase(it, x->keys.end());

            if (x->parentPath != "null") {
                x->parent = readSurfaceNodeFromFile(x->parentPath);
                for (int i = 0; i < x->parent->childPath.size(); i++)
                    x->parent->children.push_back(readSurfaceNodeFromFile(x->parent->childPath[i]));
            }
        }
        return x;
        
    }
    void insert(T k, int ln) { //inserting a key in a BTree
        BTreeNode<T>* temp = readNodeFromFileForInsertion(rootPath, pathify(to_string_generic(k)));
        BTreeNode<T>* forRoot = nullptr;
        if (temp == nullptr) {
            temp = new BTreeNode<T>;
            temp->keys.push_back(k);
            writeNodeToFile(temp);
            rootPath = pathify(to_bString(temp->keys));
            return;
        }
        //print(temp);
        //cout << endl;
        root = readSurfaceNodeFromFile(rootPath);
        //setLeafNodes();
        //BTreeNode<T>* leafNodeForInsert = search(k, true);
        BTreeNode<T>* node = temp;
        node->keys.push_back(k);
        mySort(node->keys);
        //forRoot = node;

        while (node && node->keys.size() == m) {
            int splitFrom = node->keys.size() / 2; // the index of the splitting node
            BTreeNode<T>* left = new BTreeNode<T>;
            BTreeNode<T>* right = new BTreeNode<T>;
            for (int i = 0; i < splitFrom; i++)
                left->keys.push_back(node->keys[i]); // insertion in left childs of the parent which is split
            for (int i = splitFrom + 1; i < m; i++)
                right->keys.push_back(node->keys[i]);

            if (node == root) {
                T splitKey = node->keys[splitFrom]; // the key which is to be moved to parent
                sortForRoot(node);
                if (node->keys.size() == m)
                    setChildren(node, left, right, splitFrom);
                node->keys.clear(); node->keys.push_back(splitKey);
                node->children.clear();
                node->children.push_back(left);
                node->children.push_back(right);
                node->leaf = false;
                sortForRoot(node);
                left->parent = node; right->parent = node;
                left->parentPath = pathify(node->keys[node->keys.size() / 2]);
                right->parentPath = pathify(node->keys[node->keys.size() / 2]);

                for (int i = 0; i < node->children.size(); i++) {
                    node->children[i]->parent = node;
                    node->children[i]->parentPath = pathify(to_bString(node->keys));
                }

                int j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(left->keys[left->keys.size() / 2]));
                else
                    node->childPath[j] = pathify(left->keys[left->keys.size() / 2]);

                j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(right->keys[right->keys.size() / 2]));
                else
                    node->childPath[j] = (pathify(right->keys[right->keys.size() / 2]));

                node->leaf = false;
                node->parent = nullptr;
                node->parentPath = "null";

                node->childPath.clear();
                for (int i = 0; i < node->children.size(); i++)
                    node->childPath.push_back(pathify(to_bString(node->children[i]->keys)));

                if (exists(pathify(node->keys[node->keys.size() / 2]))) {
                    remove(pathify(node->keys[node->keys.size() / 2]));
                }
                writeNodeToFile(node);

                if (exists(pathify(left->keys[left->keys.size() / 2]))) {
                    remove(pathify(left->keys[left->keys.size() / 2]));
                }
                writeNodeToFile(left);

                if (exists(pathify(right->keys[right->keys.size() / 2]))) {
                    remove(pathify(right->keys[right->keys.size() / 2]));
                }
                writeNodeToFile(right);
                rootPath = pathify(to_bString(node->keys));
            }
            else if (!node->parent) {
                T splitKey = node->keys[splitFrom]; // the key which is to be moved to parent
                sortForRoot(node);
                if (node->keys.size() == m)
                    setChildren(node, left, right, splitFrom);
                node->keys.clear(); node->keys.push_back(splitKey);
                node->children.clear();
                node->children.push_back(left);
                node->children.push_back(right);
                node->leaf = false;
                sortForRoot(node);
                left->parent = node; right->parent = node;
                left->parentPath = pathify(node->keys[node->keys.size() / 2]);
                right->parentPath = pathify(node->keys[node->keys.size() / 2]);

                for (int i = 0; i < left->children.size(); i++) {
                    left->children[i]->parentPath = pathify(to_bString(left->keys));
                }
                for (int i = 0; i < right->children.size(); i++) {
                    right->children[i]->parentPath = pathify(to_bString(right->keys));
                }

                for (int i = 0; i < left->children.size(); i++)
                    left->childPath.push_back(pathify(to_bString(left->children[i]->keys)));
                for (int i = 0; i < right->children.size(); i++)
                    right->childPath.push_back(pathify(to_bString(right->children[i]->keys)));


                for (int i = 0; i < node->children.size(); i++) {
                    node->children[i]->parent = node;
                    node->children[i]->parentPath = pathify(to_bString(node->keys));
                    /* if (exists(pathify(node->children[i]->keys[node->children[i]->keys.size() / 2]))) {
                         remove(pathify(node->children[i]->keys[node->children[i]->keys.size() / 2])));
                     }*/
                }
                for (int i = 0; i < node->children.size(); i++) {
                    for (int j = 0; j < node->children[i]->children.size(); j++)
                        writeNodeToFile(node->children[i]->children[j]);
                }

                int j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(left->keys[left->keys.size() / 2]));
                else
                    node->childPath[j] = pathify(left->keys[left->keys.size() / 2]);

                j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(right->keys[right->keys.size() / 2]));
                else
                    node->childPath[j] = (pathify(right->keys[right->keys.size() / 2]));

                node->leaf = false;
                node->parent = nullptr;
                node->parentPath = "null";

                node->childPath.clear();
                for (int i = 0; i < node->children.size(); i++)
                    node->childPath.push_back(pathify(to_bString(node->children[i]->keys)));

                if (exists(pathify(node->keys[node->keys.size() / 2]))) {
                    remove(pathify(node->keys[node->keys.size() / 2]));
                }
                writeNodeToFile(node);

                if (exists(pathify(left->keys[left->keys.size() / 2]))) {
                    remove(pathify(left->keys[left->keys.size() / 2]));
                }
                writeNodeToFile(left);

                if (exists(pathify(right->keys[right->keys.size() / 2]))) {
                    remove(pathify(right->keys[right->keys.size() / 2]));
                }
                writeNodeToFile(right);
                rootPath = pathify(to_bString(node->keys));
            }
            else {
                node->parent->keys.push_back(node->keys[splitFrom]);
                node->parent = readSurfaceNodeFromFile(node->parentPath);
                for (int i = 0; i < node->parent->childPath.size(); i++)
                    node->parent->children.push_back(readSurfaceNodeFromFile(node->parent->childPath[i]));
                sort(node->parent->keys.begin(), node->parent->keys.end());
                string nodePath = pathify((node->keys[splitFrom]));
                int currentChildIndex = 0;
                for (int i = 0; i < node->parent->children.size(); i++) {
                    string childPath = pathify(to_bString(node->parent->children[i]->keys));
                    //if (node == node->parent->children[i])
                    if (checkExists(node, node->parent->children[i]))
                        break;
                    currentChildIndex++;
                }
                node->parent->children[currentChildIndex] = left; //updating the children with left and right nodes
                node->parent->children.push_back(right);
                left->parent = node->parent;
                right->parent = node->parent;
                if (node->keys.size() == m)
                    setChildren(node, left, right, splitFrom);
                node->children.clear();
                node->children.push_back(left);
                node->children.push_back(right);

                node->keys.clear();
                for (int i = 0; i < node->parent->keys.size(); i++)
                    node->keys.push_back(node->parent->keys[i]);

                for (int i = 0; i < node->children.size(); i++) {
                    node->children[i]->parent = node;
                    node->children[i]->parentPath = pathify(to_bString(node->keys));
                }

                int j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(left->keys[left->keys.size() / 2]));
                else
                    node->childPath[j] = pathify(left->keys[left->keys.size() / 2]);

                j = 0;
                for (j; j < node->childPath.size(); j++)
                    if (node->childPath[j] == "null")
                        break;
                if (j == node->childPath.size())
                    node->childPath.push_back(pathify(right->keys[right->keys.size() / 2]));
                else
                    node->childPath[j] = (pathify(right->keys[right->keys.size() / 2]));

                j = 0;
                
                for (int i = node->parent->childPath.size() / 2; i < node->parent->childPath.size(); i++) {
                    bool found = false;
                    for (int j = 0; j < node->childPath.size(); j++)
                        if (node->parent->childPath[i] == node->childPath[j]) {
                            found = true;
                            break;
                        }
                    if (!found)
                        node->childPath.push_back(node->parent->childPath[i]);
                }
                   


                node->leaf = false;
                if (node->keys.size() != m)
                    node->parent = node->parent->parent;
                if (node->parent)
                    node->parentPath = pathify(to_bString(node->parent->keys));
                else
                    node->parentPath = "null";

                if (exists(pathify(node->keys[node->keys.size() / 2]))) {
                    remove(pathify(node->keys[node->keys.size() / 2]));
                }
                writeNodeToFile(node);

                if (exists(pathify(left->keys[left->keys.size() / 2]))) {
                    remove(pathify(left->keys[left->keys.size() / 2]));
                }
                writeNodeToFile(left);

                if (exists(pathify(right->keys[right->keys.size() / 2]))) {
                    remove(pathify(right->keys[right->keys.size() / 2]));
                }
                writeNodeToFile(right);
                rootPath = pathify(to_bString(node->keys));
            }

            forRoot = node;
            BTreeNode<T>* next = node->parent;
            if (next != nullptr) {
                forRoot = next;
                delete node;
            }
            node = next;
            setLeafNodes();
        }
        if (node)
            writeNodeToFile(node);
       /* while (forRoot->parent)
            forRoot = forRoot->parent;*/
        
        root = nullptr;
    }

    void setChildren(BTreeNode<T>* node, BTreeNode<T>* left, BTreeNode<T>* right, int mid) {
        for (int i = 0, j = 0; i <= mid; i++, j++) { //moving children to the left child node
            if (i < node->children.size()) {
                left->children.push_back(node->children[i]);
                if (j < left->children.size())
                    left->children[j]->parent = left;
                left->leaf = false;
            }
        }
        for (int i = mid + 1, j = 0; i <= m; i++, j++) { //moving children to the right child node
            if (i < node->children.size()) {
                right->children.push_back(node->children[i]);
                if (j < right->children.size())
                    right->children[j]->parent = right;
                right->leaf = false;
            }
        }
    }


    void deleteNode(T val) {
        int i = 0;
        int index = 0;
        BTreeNode<T>* res = searchForDelete(val, i, index);
        if (res == nullptr)
            return;
        //if node is a leaf and not the root 
        if (!res->parent && res->leaf)
            res->keys.erase(res->keys.begin() + i);
        else if (res->leaf) {   //if the node is a leaf
            deleteFromLeaf(res, i, index);
            root->parent = nullptr;
        }
        else {   //when its an internal node
            deleteInternalNode(res, i, index);
            root->parent = nullptr;
        }
    }

    void deleteFromLeaf(BTreeNode<T>* node, int index, int childIndex) {
        if (node->keys.size() > ceil(m / 2.0) - 1) // case 1a. That we can delete from the leaf without violating the property
            node->keys.erase(node->keys.begin() + index);

        else {  // case 1b. check for borrow from the siblings. 
            if (childIndex != 0 && node->parent->children[childIndex - 1]->keys.size() > ceil(m / 2.0) - 1) { // check if we can borrow from left child
                BTreeNode<T>* leftSibling = node->parent->children[childIndex - 1];
                node->keys.erase(node->keys.begin() + index);
                node->keys.push_back(node->parent->keys[childIndex - 1]);
                sort(node->keys.begin(), node->keys.end());
                node->parent->keys[childIndex - 1] = leftSibling->keys.back();
                leftSibling->keys.pop_back();
            }
            else if (childIndex != node->parent->children.size() - 1 && node->parent->children[childIndex + 1]->keys.size() > ceil(m / 2.0) - 1) {
                BTreeNode<T>* rightSibling = node->parent->children[childIndex + 1];
                node->keys.erase(node->keys.begin() + index);
                node->keys.push_back(node->parent->keys[childIndex]);
                sort(node->keys.begin(), node->keys.end());
                node->parent->keys[childIndex] = rightSibling->keys.front();
                rightSibling->keys.erase(rightSibling->keys.begin());
            }
            else { // here we see that we cannot borrow from any of the siblings then we merge by checking where we can merge the node. left or right
                if (childIndex == 0) { // then only right merging is possible
                    BTreeNode<T>* rightSibling = node->parent->children[childIndex + 1];
                    rightSibling->keys.push_back(node->parent->keys.front());
                    node->parent->keys.erase(node->parent->keys.begin());
                    for (int i = 0; i < node->keys.size(); i++)
                        if (i != index)
                            rightSibling->keys.push_back(node->keys[i]);
                    for (int i = 0; i < node->parent->children.size() - 1; i++)
                        node->parent->children[i] = node->parent->children[i + 1];
                    node->parent->children.pop_back();
                }
                else if (childIndex == node->parent->children.size() - 1) { // then only left merging is possible
                    BTreeNode<T>* leftSibling = node->parent->children[childIndex - 1];
                    leftSibling->keys.push_back(node->parent->keys.back());
                    node->parent->keys.pop_back();
                    for (int i = 0; i < node->keys.size(); i++)
                        if (i != index)
                            leftSibling->keys.push_back(node->keys[i]);
                    node->parent->children.pop_back();
                }
                else { // any merging side is possible we will do left in this case
                    BTreeNode<T>* leftSibling = node->parent->children[childIndex - 1];
                    leftSibling->keys.push_back(node->parent->keys[childIndex - 1]);
                    node->parent->keys.erase(node->parent->keys.begin() + childIndex - 1);
                    for (int i = 0; i < node->keys.size(); i++)
                        leftSibling->keys.push_back(node->keys[i]);
                    for (int i = childIndex; i < node->parent->children.size() - 1; i++)
                        node->parent->children[i] = node->parent->children[i + 1];
                    node->parent->children.pop_back();
                }
                // now we propagate the conditions upwards to check if there is violation in the parents nodes
                BTreeNode<T>* curr = node->parent;
                delete node;
                while (curr != root) {
                    BTreeNode<T>* temp = nullptr; // in case we have to delete this node after merging
                    // first check which child is the curr of its parent
                    if (curr->keys.size() >= ceil(m / 2.0) - 1)
                        break;
                    int j = 0;
                    while (j < curr->parent->children.size()) {
                        if (curr == curr->parent->children[j])
                            break;
                        j++;
                    }
                    // case 1b. check for borrow from the siblings.
                    if (j != 0 && curr->parent->children[j - 1]->keys.size() > ceil(m / 2.0) - 1) { // check if we can borrow from left child
                        BTreeNode<T>* left = curr->parent->children[j - 1];
                        curr->keys.push_back(curr->parent->keys[j - 1]);
                        sort(curr->keys.begin(), curr->keys.end());
                        curr->parent->keys[j - 1] = left->keys.back();
                        left->keys.pop_back();

                        left->children.back()->parent = curr;
                        curr->children.push_back(left->children.back());
                        left->children.pop_back();
                    }
                    else if (j != curr->parent->children.size() - 1 && curr->parent->children[j + 1]->keys.size() > ceil(m / 2.0) - 1) { // here we borrow from right
                        BTreeNode<T>* right = curr->parent->children[j + 1];
                        curr->keys.push_back(curr->parent->keys[j]);
                        sort(curr->keys.begin(), curr->keys.end());
                        curr->parent->keys[j] = right->keys.front();
                        right->keys.erase(right->keys.begin());

                        right->children.front()->parent = curr;
                        curr->children.push_back(right->children.front());
                        right->children.erase(right->children.begin());
                    }
                    else { // here we see that we cannot borrow from any of the siblings then we merge by checking where we can merge the node. left or right
                        if (j == 0) { // then only right merging is possible
                            BTreeNode<T>* right = curr->parent->children[j + 1];
                            right->keys.push_back(curr->parent->keys.front());
                            sort(right->keys.begin(), right->keys.end());
                            curr->parent->keys.erase(curr->parent->keys.begin());;
                            for (int i = 0; i < curr->keys.size(); i++)
                                right->keys.push_back(curr->keys[i]);
                            for (int i = 0; i < curr->parent->children.size() - 1; i++)
                                curr->parent->children[i] = curr->parent->children[i + 1];
                            // here curr also has children to be given to right sibling
                            for (int i = curr->children.size() - 1; i >= 0; i--) {
                                curr->children[i]->parent = right;
                                right->children.push_back(curr->children[i]);
                            }
                            sort(right->children.begin(), right->children.end());
                            curr->parent->children.pop_back();
                        }
                        else if (j == curr->parent->children.size() - 1) { // then only left merging is possible
                            BTreeNode<T>* left = curr->parent->children[j - 1];
                            left->keys.push_back(curr->parent->keys.back());
                            sort(left->keys.begin(), left->keys.end());
                            curr->parent->keys.pop_back();
                            for (int i = 0; i < curr->keys.size(); i++)
                                left->keys.push_back(curr->keys[i]);
                            // here curr also has children to be given to left sibling
                            for (int i = 0; i < curr->children.size(); i++) {
                                curr->children[i]->parent = left;
                                left->children.push_back(curr->children[i]);
                            }
                            sort(left->children.begin(), left->children.end());
                            curr->parent->children.pop_back();
                        }
                        else { // any merging side is possible we will do left in this case
                            BTreeNode<T>* left = curr->parent->children[j - 1];
                            left->keys.push_back(curr->parent->keys[j - 1]);
                            sort(left->keys.begin(), left->keys.end());
                            curr->parent->keys.erase(curr->parent->keys.begin() + j - 1);
                            for (int i = 0; i < curr->keys.size(); i++)
                                left->keys.push_back(curr->keys[i]);
                            for (int i = j; i < curr->parent->children.size() - 1; i++)
                                curr->parent->children[i] = curr->parent->children[i + 1];
                            // here curr also has children
                            for (int i = 0; i < curr->children.size(); i++) {
                                curr->children[i]->parent = left;
                                left->children.push_back(curr->children[i]);
                            }
                            sort(left->children.begin(), left->children.end());
                            curr->parent->children.pop_back();
                        }
                        temp = curr;
                    }
                    curr = curr->parent;
                    if (temp)
                        delete temp;
                }
                if (root->keys.empty()) {
                    BTreeNode<T>* temp1 = root;
                    root = root->children[0];
                    delete temp1;
                }
            }
        }
    }

    void deleteInternalNode(BTreeNode<T>* node, int index, int childIndex) {
        // case 1 : that if we can take predecessor or successor of the curr key being deleted
        BTreeNode<T>* left = node->children[index];
        BTreeNode<T>* right = node->children[index + 1];
        BTreeNode<T>* prevNode = left;
        BTreeNode<T>* nextNode = right;

        T delKey = node->keys[index]; //key to be deleted
        // now getting the predecessor and successor nodes.
        while (!prevNode->children.empty())
            prevNode = prevNode->children.back();
        while (!nextNode->children.empty())
            nextNode = nextNode->children.front();

        // we would also need which child is the predecessor and successor nodes of their specific parent
        int prevIndex = 0, nextIndex = 0;
        for (int i = 0; i < prevNode->parent->children.size(); i++) {
            if (prevNode->parent->children[i] == prevNode)
                break;
            prevIndex++;
        }
        for (int i = 0; i < nextNode->parent->children.size(); i++) {
            if (nextNode->parent->children[i] == nextNode)
                break;
            nextIndex++;
        }

        if (prevNode->keys.size() > ceil(m / 2.0) - 1) { // check to see if prev can be given
            T temp = prevNode->keys.back();  // then we can take prev up to the parent node and delete from leaf the original value
            prevNode->keys.pop_back();
            prevNode->keys.push_back(delKey);
            node->keys[index] = temp;
            deleteFromLeaf(prevNode, prevNode->keys.size() - 1, prevIndex);
        }

        else if (nextNode->keys.size() > ceil(m / 2.0) - 1) { // check to see if next can be brought up
            T temp = nextNode->keys.front(); // then we can take successor up to the parent node and delete from leaf the original value
            nextNode->keys.erase(nextNode->keys.begin());
            nextNode->keys.push_back(node->keys[index]);
            sort(nextNode->keys.begin(), nextNode->keys.end());
            node->keys[index] = temp;
            deleteFromLeaf(nextNode, 0, nextIndex);
        }
        else { // we cannot take the predecessor or successor up. here we consider two scenarios
            if (node->keys.size() > ceil(m / 2.0) - 1) { // we take this key down to the leaf and delete from leaf is called
                T temp = prevNode->keys.back();
                prevNode->keys.pop_back();
                prevNode->keys.push_back(delKey);
                node->keys[index] = temp;
                deleteFromLeaf(prevNode, prevNode->keys.size() - 1, prevIndex);
            }
            else { // exchange with leaf and delete there
                T temp = prevNode->keys.back();
                prevNode->keys.pop_back();
                prevNode->keys.push_back(delKey);
                node->keys[index] = temp;
                deleteFromLeaf(prevNode, prevNode->keys.size() - 1, prevIndex);
            }
        }
    }

    BTreeNode<T>* getRoot() {
        return this->root;
    }

    void print(BTreeNode<T>* root, int depth = 0) {  //function to print the BTree
        cout << "Level " << depth << ": ";
        for (int i = 0; i < root->keys.size(); i++)
            cout << root->keys[i] << " ";
        cout << endl;
        depth++;
        if (!root->children.empty())
            for (int i = 0; i < root->children.size(); i++)
                print(root->children[i], depth);
    }
};