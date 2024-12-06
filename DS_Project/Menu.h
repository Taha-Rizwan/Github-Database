#pragma once
#include "RedBlack.h"
#include "AVL.h"

struct Menu {

	void main() {
		cout << "1: Create a repository\n";
		cout << "2: Load a repository\n";
		int op;
		cin >> op;
		if (op == 1)
			createRepo();
		else if (op == 2)
			readRepo();
	}

	void createRepo() {
		cout << "1: RedBlack Tree" << endl;
		cout << "2: AVL Tree" << endl;
		cout << "Enter Option: " << endl;
		int opt;
		cin >> opt;
		if (opt == 1) {
			RedBlackTree<string> r;
		}
		else if (opt == 2) {
			AVL<string> r;
		}
		
	}

	void readRepo() {
		cout << "Enter Repo Path: " << endl;
		string path;
		string data;
		cin.ignore();
		getline(cin, path);

		path += ".txt";
		ifstream file(path);
		file >> data;
		file.close();

		
		if (data == "RedBlack") {
			RedBlackTree<string> r(path);
		} 
		else
			AVL<string> r(path);


	}
	



};