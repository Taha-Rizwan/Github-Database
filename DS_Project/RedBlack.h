#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include<string>
#include "Repository.h"
using namespace std;
using namespace std::filesystem;



bool RED = 1;
bool BLACK = 0;

template <class T>
std::string to_string_generic(const T& data) {
    std::stringstream ss;
    ss << data;
   
    return ss.str();
}
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
string instructorHash(const std::string& text) {
    int hash = 1;

    for (char ch : text) {
        hash *= static_cast<int>(ch); // Multiply ASCII values of characters
        hash %= 29;                   // To avoid integer overflow, take modulo 29 after each step
    }

    return to_string_generic(hash);
}

// Node structure for the Red-Black Tree
template<class T>
struct RedBlackNode {
    T data;
    bool color;
    int occurences;
    //0 for black, 1 for red
    RedBlackNode<T>* left, * right, * parent;
    string hash;
    RedBlackNode(T data): data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) , occurences(1)
    {
        hash = "";
        //cout << to_string_generic(data);
    }
    RedBlackNode() :  color(BLACK), left(nullptr), right(nullptr), parent(nullptr), occurences(1)
    {}

};

// Red-Black Tree class
template<class T>
class RedBlackTree:public Tree<T> {
private:
    RedBlackNode<T>* root;
    RedBlackNode<T>* nil;
    Repository<T> repo;
    void createFile(RedBlackNode<T>* node) {
       if (!node || node == nil)
            return;
        fstream file;
        string fileName = to_string_generic(node->data) + ".txt";
        file.open(repo.name + "/" + repo.currBranch + "/" + fileName, ios::out);


        file << node->data << endl;
        if (node->parent && node->parent!=nil) {
            file << node->parent->data << ".txt ";
            updateFile(node->parent);
        }
        else {
            file << "NULL ";
        }
        if (node->left&&node->left!=nil) {
            file << node->left->data << ".txt ";
        }
        else {
            file << "NULL ";
        }
        if (node->right && node->right != nil) {
            file << node->right->data << ".txt" << endl;
        }
        else {
            file << "NULL" << endl;
        }


        file.close();
    }

    void updateFile(RedBlackNode<T>* node){
        if (!node || node == nil)
            return;
        fstream file;
        file.open(repo.name + "/" + repo.currBranch + "/" + to_string_generic(node->data) + ".txt", std::ofstream::out | std::ofstream::trunc);


        file << node->data << endl;
        if (node->parent && node->parent != nil) {
            file << node->parent->data << ".txt ";
        }
        else {
            file << "NULL ";
        }
        if (node->left && node->left != nil) {
            file << node->left->data << ".txt ";
        }
        else {
            file << "NULL ";
        }
        if (node->right && node->right != nil) {
            file << node->right->data << ".txt" << endl;
        }
        else {
            file << "NULL" << endl;
        }

        file.close();
    }

    void deleteFile(T data) {
            
            remove(repo.name + "/" + repo.currBranch + "/" + to_string_generic(data) + ".txt");
        
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
    void rotateLeft(RedBlackNode<T>* k1) {
        RedBlackNode<T>* temp = k1->right;
        k1->right = temp->left;
        //Change Parent
        if (temp->left) {
            temp->left->parent = k1;
            updateFile(temp->left);
        }
        temp->parent = k1->parent;
        //Rotation involving root
        if (k1->parent == nullptr) {
            root = temp;
        }
        //in case node is at left of parent
        else if (k1== k1->parent -> left) {
            k1->parent->left = temp;
           
        }
        //in case node is at right of parent
        else {
            k1->parent->right = temp;
        }
        updateFile(k1->parent);
        temp->left = k1;
        k1->parent = temp;
        updateFile(temp);
        updateFile(k1);

    }

    void rotateRight(RedBlackNode<T>* k1) {
        RedBlackNode<T>* temp = k1->left;
        k1->left = temp->right;
        //Change Parent
        if (temp->right) {
            temp->right->parent = k1;
            updateFile(temp->right);
        }
        temp->parent = k1->parent;
        //Rotation involving root
        if (k1->parent == nullptr) {
            root = temp;
        }
        //in case node is at right of parent
        else if (k1 == k1->parent->right) {
            k1->parent->right = temp;
        }
        //in case node is at right of parent
        else {
            k1->parent->left = temp;
        }
        updateFile(k1->parent);
        temp->right = k1;
        k1->parent = temp;
        updateFile(temp);
        updateFile(k1);
    }

    void change(RedBlackNode<T>* node) {
        while (node != root && node->parent->color) {
            //node's parent is on left of grandfather
            if (node->parent == node->parent->parent->left) {
                RedBlackNode<T>* unc = node->parent->parent->right; // since parent is on left, uncle would be on right
                if (unc&&unc->color) {
                    //since uncle is red, recolor
                    node->parent->color = BLACK;
                    unc->color = BLACK;
                    node->parent->parent->color = RED;
                    //Since upto grandfather now satisfies properties, move to grandfather
                    node = node->parent->parent;
                    //cout << "RECOLOR" << endl;
                }
                else {
                   //Additional Case for double rotation LR
                    if (node == node->parent->right) {
                        node = node->parent;
                       rotateLeft(node);
                    }
                    //Normal Single Right Rotation'
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateRight(node->parent->parent);
                   // cout << "ROTATION" << endl;
                }
            }
            //node's parent is on right of grandfather
            else {
                RedBlackNode<T>* unc = node->parent->parent->left; // since parent is on right, uncle would be on ,left
                if (unc&&unc->color) {
                    //since uncle is red, recolor
                    node->parent->color = BLACK;
                    unc->color = BLACK;
                    node->parent->parent->color = RED;
                    //Since upto grandfather now satisfies properties, move to grandfather
                    node = node->parent->parent;
                   // cout << "RECOLOR" << endl;
                }
                else {
                    //Additional Case for double rotation RL
                    if (node == node->parent->left) {
                        node = node->parent;
                        rotateRight(node);
                    }
                    //Normal Single Right Rotation'
                   
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateLeft(node->parent->parent);
                  //  cout << "ROTATION" << endl;
                }
            }
        }
        root ->color = BLACK;
    }

    void inorderHelper(RedBlackNode<T>* node) {
        if (node != nullptr && node!=nil) {
            inorderHelper(node->left);
            cout << node->data << " ";

            inorderHelper(node->right);
        }
    }
    void printTree(RedBlackNode <T>* node, int space = 0, int indent = 4) {
        if (node == nullptr||node==nil) {
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
        std::cout << node->data<<"!" << node->color << "\n";

        // Process left child
        printTree(node->left, space);
    }



    void transplant(RedBlackNode<T>* x, RedBlackNode<T>* y) {
        if (!x->parent||x->parent==nil) {
            //X is root
            root = y;
        }
        else if (x == x->parent->left) {
            //x is on the left of parent
            x->parent->left = y;
        }
        else {
            //x is on the right of parent
            x->parent->right = y;
        }
         updateFile(y->parent);
         y->parent = x->parent;
         updateFile(y);
         updateFile(x);
         updateFile(y->parent);
    }

    RedBlackNode<T>* searchHelper(RedBlackNode<T>* node, T n) {
        if (!node || node==nil)
            return nullptr;


        if (isEqual(n,node->data)==1)
            return searchHelper(node->right, n);
        else if (isEqual(n, node->data) == -1) {
            return searchHelper(node->left, n);
        }
        else {
            return node;
        }

    }

    void fixDelete(RedBlackNode<T>* node) {
        //While node is not root and node is black
        RedBlackNode<T>* sibling;
        while (node != root && !node->color) {
            //if node is at left of parent
            if (node == node->parent->left) {
                sibling = node->parent->right;
                //Sibling is red(Case 1)
                if (sibling->color) {
                    sibling->color = BLACK;
                    node->parent->color = RED;
                    rotateLeft(node->parent);
                    sibling = node->parent->right;
                }
                //Sibling is black and has two black children (Case 2)
                if (!sibling->left->color && !sibling->right->color) {
                    sibling->color = RED;
                    node = node->parent;
                }
                //Sibling is black and left child is red right child is black (Case 3)
                else if (!sibling->right->color) {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    rotateRight(sibling);
                    //Going to grandpapi
                    node = node->parent->parent;
                    sibling = node->parent->right;
                   
                }
                //Sibling is black and right child is red (Case 4)
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                
                sibling->right->color = BLACK;

               
                rotateLeft(node->parent);
                node = root;
            }
            //if node is at right of parent
            else {
                sibling = node->parent->left;
                //Sibling is red(Case 1)
                if (sibling->color) {
                    sibling->color = BLACK;
                    node->parent->color = RED;
                    rotateRight(node->parent);
                    //display();
                    sibling = node->parent->left;
                }
                //Sibling is black and has two black children (Case 2)
                if (!sibling->right->color && !sibling->left->color) {
                    sibling->color = RED;
                    node = node->parent;
                }
                //Sibling is black and left child is red right child is black (Case 3)
                else if (!sibling->left->color) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    rotateLeft(sibling);
                  
                    //Going to grandpapi
                    node = node->parent->parent;
                    sibling = node->parent->left;
                }
             
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
              
                sibling->left->color = BLACK;
                rotateRight(node->parent);
                node = root;
            }
        }
        if(node)
            node->color = BLACK;
    }

    void deleteNode(RedBlackNode<T>* node) {
        RedBlackNode<T>* y = node;
        bool origColor = y->color;
        RedBlackNode<T>* x;
        //One child only case(right) + no child case
        if (node->left == nullptr||node->left==nil) {
    
            x = node->right;
            transplant(node, node->right);
        }
        //One child only case(left)
        else if (node->right == nullptr || node->right == nil) {
            x = node->left;
            transplant(node, node->left);
        }
        //two child case
        else {
            
            y = findMin(node->right);
            origColor = y->color;
            x = y->right;
            if (y->parent == node) {
                x->parent = y;
                updateFile(x->parent);
            }
            else {
                transplant(y, y->right);
                y->right = node->right;
                y->right->parent = y;
                updateFile(y->right);
                updateFile(y);
            }
            transplant(node, y);
            y->left = node->left;
            y->left->parent = y;
            y->color = node->color;
            updateFile(y->left);
            updateFile(y);
        }
        //In case original color is black there will be violations
        if (!origColor)
            fixDelete(x);
    }

    RedBlackNode<T>* findMin(RedBlackNode<T>* node) {
        while (node->left&& node->left != nil) {
            node = node->left;
        }
        return node;
    }
    string computeHashHelper(RedBlackNode<T>* node) {
        if (node == nullptr || node == nil)
            return "";
        //Currently a placeholder for computing actual has, implement later
        node->hash = instructorHash(node->data) + computeHashHelper(node->left) + computeHashHelper(node->right);
        return node->hash;
        
    }
public:
    RedBlackTree():repo(this) {
        root = nullptr;
        nil = new RedBlackNode<T>();
        repo.create();
        computeHash();
        cout<<root->hash << endl;
        repo.main();

    }

   
    void insert(T data) {
        //cout << data << endl;
        if (root == nullptr) {
            root = new RedBlackNode<T>(data);
            root->color = BLACK;
            root->left = nil;
            root->right = nil;
            createFile(root);
            return;
        }
        else {
            RedBlackNode<T>* curr = root;
            RedBlackNode<T>* par = nullptr;
            while (curr&&curr!=nil) {
                par = curr;
                if (isEqual(data, curr->data) == 0) {
                    curr->occurences++;
                    return;
                }
                else if (isEqual(data, curr->data) == 1) {
                    curr = curr->right;
                }
                else {
                    curr = curr->left;
                }
            }
            RedBlackNode<T>* yo;
            //if greater or equal add to right
            if (isEqual(data, par->data) == 0) {
                par->occurences++;
                return;
            }
            else if (isEqual(data, par->data)==1) {
                par->right = new RedBlackNode<T>(data);
                par->right->parent = par;
                yo = par->right;
            }
            else {
                par->left = new RedBlackNode<T>(data);
                par->left->parent = par;
                yo = par->left;
            }

            createFile(yo);
            yo->left = nil;
            yo->right = nil;
            if (yo->parent == nullptr) {
                yo->color = BLACK;
                return;
            }
              
            if (yo->parent->parent == nullptr)
                return;


            change(yo);

        }
    }
    RedBlackNode<T>* search(T val) {
        return searchHelper(root, val);
    }

    void deleteByVal(T val) {
        RedBlackNode<T>* x = search(val);
        if (!x||x==nil) {
            return;
        }
        else {
            deleteNode(x);
            deleteFile(val);
        }
    }

    void display() {
        printTree(root);
    }
    void inorder() {
        inorderHelper(root);
    }

    void computeHash() {
        computeHashHelper(root);
        cout << root->hash;
    }

};

