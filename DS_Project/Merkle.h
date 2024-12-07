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
    MerkleNode(T data, bool leaf,bool useSha) : data(data), hash(" "),useSha(useSha), left(nullptr), right(nullptr), leaf(leaf) {
        computeHash(data);
    }

    // Constructor for internal nodes
    MerkleNode(MerkleNode* left, MerkleNode* right,bool useSha) :useSha(useSha), left(left), right(right), leaf(false) {
        computeHash(left->hash + right->hash);
    }

    //function that computes hash of a given string using hashing algorithm selected by user
    void computeHash(string text) {
        if (useSha) {
            hash=calculateSHA256(text);
        }
        else {
            hash=instructorHash(text);
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
    MerkleTree(int order,bool useSha) : order(order),useSha(useSha), root(nullptr) {}
   
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
                MerkleNode<T>* leafNode = new MerkleNode<T>(data, true,useSha);
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
        if (root!=nullptr) {
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
                MerkleNode<T>* parentNode = new MerkleNode<T>(left, right,useSha);
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
