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
        cout << "\t8: View Data" << endl;
        cout << "\tChoose: ";
    }

public:
    string name;
    string csv_path;
    int column, ln;
    //Create vector class later
    vector<string> branches;
    vector<string> roots;
    vector<string> header;
    string currBranch;
    Tree<T>* tree;
    Repository(Tree<T>* tree) :tree(tree) {

    }
    void create() {
        cout << "Enter Repo Name: ";
        cin >> name;
        cout << "Enter csv path: ";
        cin >> csv_path;
        cout << "Enter column Number(0-indexed): ";
        cin >> column;
        create_directory(name);
        currBranch = "main";
        create_directory(name + "/" + currBranch);
        create_directory(name + "/" + currBranch + "/data");
        branches.push_back("main");

        tree->createNil();
        ifstream file(csv_path);
        if (!file.is_open()) {
            cerr << "Error: Could not open file!" << endl;
            return;
        }


        string line;
        getline(file, line); // Read the header line and skip it
        stringstream ss(line);
        string cell;
        int currentColumnIndex = 0;
        // Parse the line using ',' as a delimiter
        while (getline(ss, cell, ',')) {
            // Check if the cell starts with a quotation mark
            if (!cell.empty() && cell.front() == '"') {
                string quotedCell = cell;
                // Continue accumulating until we find the closing quote
                while (!quotedCell.empty() && quotedCell.back() != '"') {
                    string nextPart;
                    if (getline(ss, nextPart, ',')) {
                        quotedCell += "," + nextPart; // Add delimiter and next part
                    }
                    else {
                        break; // Exit if no more parts
                    }
                }
                // Remove enclosing quotes
                if (!quotedCell.empty() && quotedCell.front() == '"' && quotedCell.back() == '"') {
                    quotedCell = quotedCell.substr(1, quotedCell.size() - 2);
                }
                cell = quotedCell; // Update the cell to the accumulated quoted content
            }
            currentColumnIndex++;
            header.push_back(cell);
        }
        cout << "Reading CSV to main branch(default): " << endl;
        ln = 2;

        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            int currentColumnIndex = 0;
            vector<string> rowData;
              // Parse the line using ',' as a delimiter
            while (getline(ss, cell, ',')) {
                // Check if the cell starts with a quotation mark
                if (!cell.empty() && cell.front() == '"') {
                    string quotedCell = cell;
                    // Continue accumulating until we find the closing quote
                    while (!quotedCell.empty() && quotedCell.back() != '"') {
                        string nextPart;
                        if (getline(ss, nextPart, ',')) {
                            quotedCell += "," + nextPart; // Add delimiter and next part
                        }
                        else {
                            break; // Exit if no more parts
                        }
                    }
                    // Remove enclosing quotes
                    if (!quotedCell.empty() && quotedCell.front() == '"' && quotedCell.back() == '"') {
                        quotedCell = quotedCell.substr(1, quotedCell.size() - 2);
                    }
                    cell = quotedCell; // Update the cell to the accumulated quoted content
                }

                if (currentColumnIndex == column) {
                    tree->insert(cell, ln);

                }

                currentColumnIndex++;
                rowData.push_back(cell);
            }

            ofstream dataFile;
            dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
            for (int i = 0; i < rowData.size(); i++) {
                dataFile << rowData[i] << '\n';
            }
            dataFile.close();
            ln++;
        }
        cout << "reading done\n";
        file.close();
        //tree->computeHash();
        tree->merkle = new MerkleTree<T>(tree->order, name, currBranch);
        tree->changeBranch(tree->getRootFile());
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
        roots.push_back(tree->getRootFile());
        cout << endl;
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
                visualizeTree();
                break;
            case 7:
                mergeBranch();
                break;
            case 8:
                viewNodeData();
                break;
            default:
                logic = false;
                break;
            }
        }
    }

    vector<string> readFileByLineNumber(int lineNumber) {
        vector<string> rowData;
        string filePath = name + "/" + currBranch + "/data/" + to_string(lineNumber) + ".txt";
        ifstream file(filePath);

        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filePath << endl;
            return rowData;
        }

        string line;
        while (getline(file, line)) {
            rowData.push_back(line);
        }

        file.close();
        return rowData;
    }

    void writeFileByLineNumber(int ln, vector<string>&rowData) {
        ofstream dataFile;
        dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
        for (int i = 0; i < rowData.size(); i++) {
            dataFile << rowData[i] << '\n';
        }
        dataFile.close();
    }

    void addNode() {
        // T val;
        // cout << "Value to add: ";
        // cin >> val;
        vector<string> rowData;
        string data;
        cin.ignore();
        for (int i = 0;i<header.size();i++) {
            cout << header[i] << ": ";

            getline(cin, data);
            //getline(data);
            rowData.push_back(data);
        }
        writeFileByLineNumber(ln, rowData);
        tree->insert(rowData[column], ln++);
        //tree->computeHash();
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;

    }
    void deleteNode() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        int l = tree->deleteByVal(val, false);
        if (l != -1) {
            cout << "Deleted from line number: " << l << endl;
            remove((name + "/" + currBranch + "/data/" + to_string(l) + ".txt").c_str());
        }
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
    }
    void updateNode() {
        T val, newVal;
        cout << "Value to update: ";
        cin >> val;

        int ln = tree->searchData(val);
        if (ln!=-1) {
            cout<<"What do you want to change: "<<endl;
            for (int i = 0;i<header.size();i++) {
                cout<<i<<": "<<header[i]<<endl;
            }
            int opt;
            cin>>opt;
            if (opt>=0 &&opt<header.size()) {
                vector<string> rowData =readFileByLineNumber(ln);
                cout<<"Current "<<header[opt]<<": "<<rowData[opt]<<endl;
                cout<<"Updated "<<header[opt]<<": ";
                string data;
                cin.ignore();
                getline(cin, data);
                string old = rowData[opt];
                rowData[opt] = data;
                writeFileByLineNumber(ln, rowData);
                if (opt==column) {
                    tree->deleteByVal(old,ln);
                    tree->insert(data,ln);
                }
            }

        }

        // cout << "Updated Value: ";
        // cin >> newVal;
        //int num = tree->deleteByVal(val, true);
        // tree->insert(newVal, num);
        //
        // tree->computeHash();
        // cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
    }

    void viewNodeData() {

        T data;
        cout << "Enter data to view";
        cin >> data;
        int toBeViewed = tree->searchData(data);

        if (toBeViewed == -1) {
            cout << "Data not found!" << endl;
            return;
        } else {
            vector<string> rowData = readFileByLineNumber(toBeViewed);

            for (int i =0;i<rowData.size();i++) {
                cout << header[i] << ": " << rowData[i] << endl;
            }

        }
    }

    void visualizeTree() {
        //tree->display();
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
    }
    void switchBranch() {



        cout << "Select which branch you want: ";
        int n;
        for (int i = 0; i < branches.size(); i++) {
            cout << i + 1 << ": " << branches[i] << endl;
        }
        cin >> n;
        if (n - 1 >= 0 && n <= branches.size()) {
            currBranch = branches[n - 1];
            tree->changeBranch(roots[n - 1]);
            cout << "Current Branch is set to: " << currBranch << endl;
        }
        else {
            cout << "Branch not found!" << endl;
        }


    }
    void addBranch() {
        string newBranch;
        cout << "Enter the name for your new branch: ";
        cin >> newBranch;


        branches.push_back(newBranch);
        roots.push_back(tree->getRootFile());
        tree->changeBranch(tree->getRootFile());

        currBranch = newBranch;
        cout << "New branch has been created and cloned by current version of main" << endl;
        cout << "Current Branch is set to: " << currBranch << endl;


        path sourceDir = name + "/" + "main";
        path destinationDir = name + "/" + newBranch;

        create_directory(destinationDir);

        for (const auto& entry : recursive_directory_iterator(sourceDir)) {
            const auto& sourcePath = entry.path();
            auto destinationPath = destinationDir / relative(sourcePath, sourceDir);

            if (is_directory(sourcePath)) {
                create_directory(destinationPath);
            } else {
                copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);
            }
        }

        cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
    }
    void deleteBranch() {

    }
    void mergeBranch() {

    }
};