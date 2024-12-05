#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tree.h"

#include <vector>
using namespace std;
using namespace std::filesystem;


struct Addition {
    vector<string> rowData;
    int lineNumber;
    Addition(vector<string> rowData,int lineNumber) : rowData(rowData),lineNumber(lineNumber) {}
    string metaData() {
        string data = "Addition: ";
        for (int i = 0; i < rowData.size(); i++) {
            data += rowData[i] + " ";
        }
        return data;
    }
};

struct Deletion {
    string data;
    int lineNumber;
    Deletion(string data, int lineNumber) : data(data), lineNumber(lineNumber) {}
    string metaData() {
        string data = "Deletion: ";
        data += this->data;
        data+= " at line number: " + to_string(lineNumber);
        return data;
    }
};

struct Updation {
    vector<string> rowData;
    string old;
    int lineNumber;
    int column;
    Updation(vector<string> rowData, int lineNumber, int column,string old) : rowData(rowData), lineNumber(lineNumber), column(column),old(old) {}
    string metaData() {
        string data = "Updation: ";
        data += "Old: " + old + " New: " + rowData[column] + "at column: " + to_string(column) + " at line number: " + to_string(lineNumber);
        return data;
    }
};


template<class T>
class Repository {
private:
    void printCommands() {
        cout << "\t1: Add a Node" << endl;
        cout << "\t2: Update a Node" << endl;
        cout << "\t3: Delete a Node" << endl;
        cout << "\t4: Switch Branch" << endl;
        cout << "\t5: Add A Branch" << endl;
        cout << "\t6: Hash" << endl;
        cout << "\t7: Commit" << endl;
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
    vector<Addition> additions;
    vector<Deletion> deletions;
    vector<Updation> updations;
    string currBranch;
    Tree<T>* tree;
    float version;
    Repository(Tree<T>* tree) :tree(tree),version(0.1) {

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
        tree->merkle = new MerkleTree<T>(tree->order);
        tree->changeBranch(tree->getRootFile());
        //cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "AFTER READING ...DATA FOLDER: " << dataFolder << endl << endl;
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
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
                commit();
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
        additions.emplace_back(Addition(rowData,ln));
    }

    void addDataFr(Addition& addition) {

        writeFileByLineNumber(ln, addition.rowData);
        tree->insert(addition.rowData[column], ln++);
        //tree->computeHash();
    }

    void deleteNode() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        int l = tree->searchData(val);
        if (l != -1) {
            deletions.emplace_back(Deletion(val, l));
        } else {
            for (int i = 0;i<additions.size();i++) {
                if (additions[i].rowData[column]==val) {
                    additions.erase(additions.begin() + i);
                    //cout << "Deleted from line number: " << val << endl;
                    return;
                }
            }

            cout<<"Not Found: "<<endl;
        }
    }

    void deleteDataFr(Deletion& deletion) {
        tree->deleteByVal(deletion.data, deletion.lineNumber);

        cout << "Deleted from line number: " << deletion.data << endl;
        remove((name + "/" + currBranch + "/data/" + deletion.data + ".txt").c_str());
    }
    void updateNode() {
        T val, newVal;
        cout << "Value to update: ";
        cin >> val;

        int ln = tree->searchData(val);



        if (ln!=-1) {
            for (int i = 0;i<deletions.size();i++) {
                if (deletions[i].lineNumber ==ln) {
                    cout<<"Not Found!"<<endl;
                    return;
                }
            }

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
                updations.emplace_back(Updation(rowData, ln, opt,old));

                //writeFileByLineNumber(ln, rowData);
                // if (opt==column) {
                //     tree->deleteByVal(old,ln);
                //     tree->insert(data,ln);
                // }
            }

        } else {
            for (int i = 0;i<additions.size();i++) {
                if (additions[i].rowData[column]==val) {
                    cout<<"What do you want to change: "<<endl;
                    for (int i = 0;i<header.size();i++) {
                        cout<<i<<": "<<header[i]<<endl;
                    }
                    int opt;
                    cin>>opt;
                    if (opt>=0 &&opt<header.size()) {
                        cout<<"Current "<<header[opt]<<": "<<additions[i].rowData[opt]<<endl;
                        cout<<"Updated "<<header[opt]<<": ";
                        string data;
                        cin.ignore();
                        getline(cin, data);
                        string old = additions[i].rowData[opt];
                        additions[i].rowData[opt] = data;
                        return;
                    } else
                        break;

                }
            }
        }
        cout<<"Not Found: "<<endl;
    }
    void updateDataFr(Updation& update) {
        writeFileByLineNumber(update.lineNumber, update.rowData);
        if (update.column==column) {
            tree->deleteByVal(update.old, update.lineNumber);
            tree->insert(update.rowData[column],ln);
        }

    }
    void commit() {
        if (!additions.size() && !deletions.size() && !updations.size()) {
            cout<<"Nothing to update"<<endl;
            return;
        }
        ofstream file;
        file.open(name+"/"+currBranch+"/commit"+to_string(version)+".txt",ios::app);
        file << "Version: " << version << endl;
        for (int i = 0;i<additions.size();i++) {
            addDataFr(additions[i]);
            file<<additions[i].metaData()<<endl;
        }
        for (int i = 0;i<deletions.size();i++) {
            deleteDataFr(deletions[i]);
            //cout<<"Deleting: "<<deletions[i].data<<endl;
            file<<deletions[i].metaData()<<endl;
        }
        for (int i = 0;i<updations.size();i++) {
            updateDataFr(updations[i]);

            file<<updations[i].metaData()<<endl;
        }
        file.close();
        version += 0.1;
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
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
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
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