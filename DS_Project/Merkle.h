//Now calculates hash of the entire row of the respective node
//MerkleTree is completely based on the data of the branch....if there is no change in the data file there wont be any change in the hash

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>

using namespace std;
namespace fs = std::filesystem;

template<class T>
string to_string_generic(const T& data) {
    std::stringstream ss;
    ss << data;
    return ss.str();
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

template<class T>
class MerkleNode {
    // Data field only for leaf nodes
public:
    T data;
    string hash;
    MerkleNode* left;
    MerkleNode* right;
    bool leaf;

    // Constructor for leaf nodes
    MerkleNode(T data, bool leaf) : data(data), hash(" "), left(nullptr), right(nullptr), leaf(leaf) {
        hash = instructorHash(data);
    }

    // Constructor for internal nodes
    MerkleNode(MerkleNode* left, MerkleNode* right) : left(left), right(right), leaf(false) {
        hash = instructorHash(left->hash + right->hash);
    }
};

template<class T>
class MerkleTree {
    MerkleNode<T>* root;
    string repoName;
    string currBranch;
    int order; 
public:
    MerkleTree(int order) : order(order), root(nullptr) {}
   
    //tried this but it's not working..
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



    vector<MerkleNode<T>*> createLeafNodes(const string& dataFolder) {
        vector<MerkleNode<T>*> leafNodes;

        vector<string> filePaths;
        for (const auto& entry : fs::directory_iterator(dataFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                filePaths.push_back(entry.path().string());
            }
        }

        //tries sorting the file paths (but not working as expeceted e.g after 89.txt the next file would be 9.txt and after that 90.txt)
        sort(filePaths.begin(), filePaths.end());

        for (const string& filePath : filePaths) {
            ifstream file(filePath);
            if (file.is_open()) {

                //Reads the entire data related to the node
                stringstream buffer;
                buffer << file.rdbuf();  
                string data = buffer.str();
                file.close();

                data.erase(remove(data.begin(), data.end(), '\n'), data.end());
                
                MerkleNode<T>* leafNode = new MerkleNode<T>(data, true);
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

        //issues wiht deletion...we have to construct merkle tree eveyr time we are looking for any changes so we must delete the old one first
        if (root!=nullptr) {
            //deleteTree(root);
            root = nullptr;
        }

        vector<MerkleNode<T>*> leafNodes = createLeafNodes(dataFolder);
        if (leafNodes.empty()) return nullptr;

        vector<MerkleNode<T>*> currentLevel = leafNodes;

        //builds the tree level by level...starting from leaf nodes(containing data) and constructs the tree using bottom up approach
        while (currentLevel.size() > 1) {
            vector<MerkleNode<T>*> nextLevel;

            for (size_t i = 0; i < currentLevel.size(); i += 2) {
                MerkleNode<T>* left = currentLevel[i];
                MerkleNode<T>* right = (i + 1 < currentLevel.size()) ? currentLevel[i + 1] : left;
                MerkleNode<T>* parentNode = new MerkleNode<T>(left, right);
                nextLevel.push_back(parentNode);
            }

            currentLevel = nextLevel;
        }

        root = currentLevel[0];
        return root;
    }

    // Get the root hash of the Merkle Tree
    string getRootHash() const {
        cout << "left : " << root->left->hash << " ...right: " << root->right->hash << endl;
        return root ? root->hash : "";
    }
};
