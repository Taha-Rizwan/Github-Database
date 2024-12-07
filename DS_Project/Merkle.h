//Now calculates hash of the entire row of the respective node
//MerkleTree is completely based on the data of the branch....if there is no change in the data file there wont be any change in the hash

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <openssl/evp.h>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

template<class T>
string to_string_generic(const T& data) {
    std::stringstream ss;
    ss << data;
    return ss.str();
}

string calculateSHA256(const string& data) {
    // Create a context for SHA256 hashing
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    if (context == nullptr) {
        cerr << "Error creating EVP_MD_CTX" << endl;
        return "";
    }

    // Initialize the SHA256 context
    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
        cerr << "Error initializing SHA256" << endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    // Update the context with the data
    if (EVP_DigestUpdate(context, data.c_str(), data.size()) != 1) {
        cerr << "Error updating SHA256" << endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    // Finalize the hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    if (EVP_DigestFinal_ex(context, hash, &lengthOfHash) != 1) {
        cerr << "Error finalizing SHA256" << endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    // Convert the hash into a hex string
    stringstream hexStream;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        hexStream << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    // Free the context
    EVP_MD_CTX_free(context);

    // Return the hash as a string
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

template<class T>
class MerkleNode {
    // Data field only for leaf nodes
public:
    T data;
    string lineNumber;
    string hash;
    MerkleNode* left;
    MerkleNode* right;
    bool leaf;

    // Constructor for leaf nodes
    MerkleNode(T data, bool leaf) : data(data), hash(" "), left(nullptr), right(nullptr), leaf(leaf) {
        hash = instructorHash(data);
        //hash = calculateSHA256(data);
    }

    // Constructor for internal nodes
    MerkleNode(MerkleNode* left, MerkleNode* right) : left(left), right(right), leaf(false) {
        hash = instructorHash(left->hash + right->hash);
        //hash = calculateSHA256(left->hash + right->hash);
    }
};

template<class T>
class MerkleTree {
  
public:
    MerkleNode<T>* root;
    string repoName;
    string currBranch;
    int order;
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
        if (!fs::exists(dataFolder)) {
            cerr << "Directory does not exist: " << dataFolder << endl;
            return {};
        }

        if (!fs::is_directory(dataFolder)) {
            cerr << "Path is not a directory: " << dataFolder << endl;
            return {};
        }

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

        for (const string& filePath : filePaths) {
            string lineNumber = filePath.substr(0, filePath.find(".txt"));
            ifstream file(filePath);
            if (file.is_open()) {
                cout << "File " << filePath << " is open\n";
                //Reads the entire data related to the node
                stringstream buffer;
                buffer << file.rdbuf();  
                string data = buffer.str();
                file.close();

                
                MerkleNode<T>* leafNode = new MerkleNode<T>(data, true);
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


    //Looks where data has been changed and only modifies that data when merging
    void lookForChange(MerkleNode<T>* mRoot, MerkleNode<T>* tRoot, string path) {
        if (mRoot == nullptr && tRoot == nullptr) {
            cout << "Both are null\n";
            return;
        }


        if (!mRoot || !tRoot) {
           
            return;
        }


        if (mRoot->hash != tRoot->hash) {
            if (mRoot->leaf && tRoot->leaf) {
                //Updation Case
                if (mRoot->data != tRoot->data) {
                    cout << path << " - Data not matching: Tree1= " << mRoot->data
                        << "   Tree2= " << tRoot->data << endl;
                    string fileName = mRoot->lineNumber + ".txt";

                    ofstream file(fileName);
                    if (file.is_open()) {
                        file << tRoot->data;
                        file.close();
                        cout << "Updated file " << fileName << " with data " << tRoot->data << endl;
                    }
                    else {
                        cerr << "Error opening file: " << fileName << endl;
                    }
                    mRoot->data = tRoot->data;

                    mRoot->hash = instructorHash(mRoot->data);
                }
            }
            else {
                cout << path << "Hash not matching Tree1 -> " << mRoot->hash
                    << "   Tree2 -> " << tRoot->hash << endl;
                if (mRoot->left && tRoot->left) {
                    lookForChange(mRoot->left, tRoot->left, path + "/left");
                }
                if (mRoot->right && tRoot->right) {
                    lookForChange(mRoot->right, tRoot->right, path + "/right");
                }
                mRoot->hash = instructorHash(mRoot->left->hash + mRoot->right->hash);
            }
        }

    }



    // Get the root hash of the Merkle Tree
    string getRootHash() const {
        cout << "left : " << root->left->hash << " ...right: " << root->right->hash << endl;
        return root ? root->hash : "";
    }
};
