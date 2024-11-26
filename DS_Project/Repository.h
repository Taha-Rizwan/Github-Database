#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tree.h"
#include <vector>
using namespace std;


template<class T>
class Repository {
	string name;
	string csv_path;
	int column;
    //Create vector class later
	vector<string> branches;
    string currBranch;
public:
	Repository(Tree<T>* tree) {
        cout << "Enter Repo Name: ";
        cin >> name;
        cout << "Enter csv path: ";
        cin >> csv_path;
        cout << "Enter column Number(0-indexed): ";
        cin >> column;
        ifstream file(csv_path);
        if (!file.is_open()) {
            cerr << "Error: Could not open file!" << endl;
            return;
        }

        string line;
        getline(file, line); // Read the header line and skip it
        cout << "Reading CSV to main branch(default): "<<endl;
        currBranch = "main";
        branches.push_back("main");
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            int currentColumnIndex = 0;

            // Parse the line using ',' as a delimiter
            while (getline(ss, cell, ',')) {
                if (currentColumnIndex == column) {
                    tree->insert(cell);

                    break; // No need to process further columns for this line
                }

                currentColumnIndex++;
            }
        }

        file.close();
        tree->display();
	}
};

