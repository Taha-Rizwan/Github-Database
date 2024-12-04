#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<filesystem>
#include"Repository.h"
//#include <openssl/sha.h> //error couldnt find header

using namespace std;

template<class T>
string to_string_generic(const T& data) {
	std::stringstream ss;
	ss << data;

	return ss.str();
}




template<class T>
class MerkleNode {
	//data field only for leaf nodes
	T data; 
	string hash;
	MerkleNode* left;
	MerkleNode* right;
	bool leaf;
public:
	MerkleNode(T data, bool leaf) :data(data),left(nullptr),right(nullptr), leaf(leaf),hash(" ") {
		hash = instructorHash(data);
	}
	MerkleNode(MerkleNode* left, MerkleNode* right) : left(left), right(right), leaf(false) {
		hash = instructorHash(left->hash + right->hash); 
	}
};

//Node for reading from File
template<class T>
struct readNode {
public:
	T data;
	int noOfChildren;
	string parentFile;
	string* childFiles;
	readNode(T data, int order) : data(data), noOfChildren(order),parentFile(" ") {
		childFiles = new string[noOfChildren];
	}

	readNode(int order):noOfChildren(order), parentFile(" ") {
		childFiles = new string[noOfChildren];
	}

};


template<class T>
class MerkleTree {
	MerkleNode<T>* root;
	string repoName;
	string currBranch;
	int order;
public:
	MerkleTree(int order, string repoName,string branch) :order(order), root(nullptr),repoName(repoName),currBranch(branch) {
	
	}

	readNode<T>* readNodeFromFile(string filePath) {
		if (filePath == "NULL")
			return nullptr;
		string dataStr = filePath.substr(0, filePath.find(".txt"));
		fstream file;
		file.open(repoName+ "/" +currBranch + "/" + filePath);
		if (!file.is_open()) {
			cerr << "Cannot open file: " << filePath << endl;
			throw runtime_error("Unable to open file: " + filePath);
		}
		string nodeData;
		string line;

		readNode<T>* node(order);


		getline(file, line);
		node->data = line;
		getline(file, node->parentPath);
		for (int i = 0;i < order;i++) {
			getline(file, node->childFiles[i]);
		}

		file.close();

		return node;
	}

	//computes Hash,,,(gpt)
	//string computeHash(const string& data) {
	//	unsigned char hash[SHA256_DIGEST_LENGTH];
	//	SHA256((unsigned char*)data.c_str(), data.size(), hash);

	//	char buffer[2 * SHA256_DIGEST_LENGTH + 1];
	//	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
	//		sprintf(buffer + i * 2, "%02x", hash[i]);
	//	return std::string(buffer);
	//}

	//instructor's hash
	string instructorHash(string text) {
		int hash = 1;

		for (int i = 0; i < text.length();i++) {
			hash *= text[i]; // Multiply ASCII values of characters

			hash %= 29;     // To avoid integer overflow, take modulo 29 after each step
		}

		return to_string_generic(hash);
	}

	//gathers all the nodes and computes their hashes
	vector<string> computeLeafFilesFromRoot(string rootFile) {
		vector<string> leafFiles; 

		vector<string> trav = { rootFile };
		while (!trav.empty()) {
			string currentFile = trav.back();
			trav.pop_back();

			readNode<T>* node = readNodeFromFile(currentFile);

			if (node == nullptr) {
				continue;
			}

			leafFiles.push_back(to_string_generic(node->data) + ".txt");

			for (int i = 0; i < node->noOfChildren; i++) {
				if (node->childFiles[i] != "NULL") {
					trav.push_back(node->childFiles[i]);
					leafFiles.push_back(to_string_generic(node->data) + ".txt");
				}
			}
		}

		return leafFiles;
	}


	vector<MerkleNode<T>*> createLeafNodes(string rootFile) {
		vector<string> leafFiles = computeLeafFilesFromRoot(rootFile);

		vector<MerkleNode<T>*> leafNodes;

		for (const auto& file : leafFiles) {
			readNode<T>* node = readNodeFromFile(file);
			if (node != nullptr) {
				MerkleNode<T>* leafNode = new MerkleNode<T>(node->data, true);
				leafNodes.push_back(leafNode);
			}
		}

		return leafNodes;
	}

	// Build the Merkle tree after creating the leaf nodes
	MerkleNode<T>* buildMerkleTree(string rootFile) {
		vector<MerkleNode<T>*> leafNodes = createLeafNodes(rootFile);
		if (leafNodes.empty()) return nullptr;

		vector<MerkleNode<T>*> currentLevel = leafNodes;

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

	string getRootHash() {
		return root ? root->hash : "";
	}


};
