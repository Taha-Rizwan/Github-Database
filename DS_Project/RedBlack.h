#include <iostream>
using namespace std;

bool RED = 1;
bool BLACK = 0;


// Node structure for the Red-Black Tree
template<class T>
struct RedBlackNode {
    T data;
    bool color;
    //0 for black, 1 for red
    RedBlackNode<T>* left, * right, * parent;

    RedBlackNode(T data)
        : data(data)
        , color(RED)    
        , left(nullptr)
        , right(nullptr)
        , parent(nullptr)
    {
    }
};

// Red-Black Tree class
template<class T>
class RedBlackTree {
private:
    RedBlackNode<T>* root;


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

        temp->left = k1;
        k1->parent = temp;
        
    }

    void rotateRight(RedBlackNode<T>* k1) {
        RedBlackNode<T>* temp = k1->left;
        k1->left = temp->right;
        //Change Parent
        if (temp->right) {
            temp->right->parent = k1;
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

        temp->right = k1;
        k1->parent = temp;

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
                    cout << "RECOLOR" << endl;
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
                    cout << "ROTATION" << endl;
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
                    cout << "RECOLOR" << endl;
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
                    cout << "ROTATION" << endl;
                }
            }
        }
        root ->color = BLACK;
    }

public:
    RedBlackTree() {
        root = nullptr;
    }

   
    void insert(T data) {
        cout << data << endl;
        if (root == nullptr) {
            root = new RedBlackNode<T>(data);
            root->color = BLACK;
            return;
        }
        else {
            RedBlackNode<T>* curr = root;
            RedBlackNode<T>* par = nullptr;
            while (curr) {
                par = curr;
                if (isEqual(data, curr->data) == 1) {
                    curr = curr->right;
                }
                else {
                    curr = curr->left;
                }
            }
            RedBlackNode<T>* yo;
            //if greater or equal add to right
            if (isEqual(data, par->data)==1||isEqual(data,par->data)==0) {
                par->right = new RedBlackNode<T>(data);
                par->right->parent = par;
                yo = par->right;
            }
            else {
                par->left = new RedBlackNode<T>(data);
                par->left->parent = par;
                yo = par->left;
            }

            if (yo->parent == nullptr) {
                yo->color = BLACK;
                return;
            }
              
            if (yo->parent->parent == nullptr)
                return;


            change(yo);

        }
    }

    void inorderHelper(RedBlackNode<T>* node) {
        if (node != nullptr) {
            inorderHelper(node->left);
            cout << node->data << " ";
          
            inorderHelper(node->right);
        }
    }
    void printTree(RedBlackNode <T>* node, int space = 0, int indent = 4) {
        if (node == nullptr) {
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
        std::cout << node->data << "\n";

        // Process left child
        printTree(node->left, space);
    }

    void display() {
        printTree(root);
    }
    void inorder() {
        inorderHelper(root);
    }

};

