#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tree.h"
#include <vector>
using namespace std;
using namespace std::filesystem;

template<class T>
class Repository {
private:
    void printCommands() {
        cout << "\t1: Add a Node" << endl;
        cout << "\t2: Update a Node" << endl;
        cout << "\t3: Delete a Node" << endl;
        cout << "\t4: Switch Branch" << endl;
        cout << "\t5: Add A Branch" << endl;
        cout << "\t6: Delete A Branch" << endl;
        cout << "\t7: Merge Branches" << endl;
        cout << "\tChoose: ";
    }
	
public:
    string name;
    string csv_path;
    int column;
    //Create vector class later
    vector<string> branches;
    string currBranch;
    Tree<T>* tree;
	Repository(Tree<T>* tree):tree(tree) {
      
	}
    void create() {
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
        create_directory(name);

        string line;
        getline(file, line); // Read the header line and skip it
        cout << "Reading CSV to main branch(default): " << endl;
        currBranch = "main";
        create_directory(name + "/" + currBranch);
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

    void main() {
        
     
        bool logic = true;
        while (logic) {
            printCommands();
            int opt;
            cin >> opt;

            switch (opt) {
            case 1:
                addNode();
                break;
            case 3:
                deleteNode();
                break;
            case 2:
                updateNode();
                break;
            case 4:
                switchBranch();
                break;
            case 5:
                addBranch();
                break;
            case 6:
                deleteBranch();
                break;
            case 7:
                mergeBranch();
            default:
                logic = false;
                break;
            }
        }
       
        


    }
    void addNode() {
        T val;
        cout << "Value to add: ";
        cin >> val;
        tree->insert(val);
        tree->display();
    }
    void deleteNode() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        tree->deleteByVal(val);
        tree->display();
    }
    void updateNode() {
        T val,newVal;
        cout << "Value to update: ";
        cin >> val;
        cout << "Updated Value: ";
        cin >> newVal;
        tree->deleteByVal(val);
        tree->insert(newVal);
        tree->display();
        //tree->updateNode(val);
    }
    void switchBranch() {

    }
    void addBranch() {

    }
    void deleteBranch() {

    }
    void mergeBranch() {

    }
};

