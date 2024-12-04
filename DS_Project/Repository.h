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
        cout << "Reading CSV to main branch(default): " << endl;
        ln = 2;

        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            int currentColumnIndex = 0;
          //  vector<string> rowData;
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
                    ln++;
                    break;


                }

                currentColumnIndex++;
                //rowData.push_back(cell);
            }

           // ofstream dataFile;
           // dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
          //  for (int i = 0; i < rowData.size(); i++) {
         //       dataFile << rowData[i] << '\n';
         //   }
         //   dataFile.close();
         //   ln++;
        }
        cout << "reading done\n";
        file.close();
        tree->merkle = new MerkleTree<T>(tree->order, name, currBranch);
        cout << "Merkle Tree root hash: " << tree->merkle->buildMerkleTree(tree->rootFile()) << endl;
        tree->computeHash();
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
                deleteBranch();
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
    void addNode() {
        T val;
        cout << "Value to add: ";
        cin >> val;
        tree->insert(val,ln++);
        tree->computeHash();
    }
    void deleteNode() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        tree->deleteByVal(val,false);
        tree->computeHash();
    }
    void updateNode() {
        T val,newVal;
        cout << "Value to update: ";
        cin >> val;
        cout << "Updated Value: ";
        cin >> newVal;
        int num =  tree->deleteByVal(val,true);
        tree->insert(newVal,num);
     
        tree->computeHash();
        //tree->updateNode(val);
    }

    void viewNodeData() {

        T data;
	    cout<<"Enter data to view";
	    cin>>data;
	    int toBeViewed = tree->searchData(data);

	    ifstream file(csv_path);
	    if (!file.is_open()) {
	        cerr << "Error: Could not open file!" << endl;
	        return;
	    }

	    string line;
	    int currentLine = 2;
	    vector<string> rowData;

	    while (getline(file, line)) {
	        currentLine++;
	        if (currentLine == toBeViewed) {
	            stringstream ss(line);
	            string cell;

	            while (getline(ss, cell, ',')) {
	                // Check if the cell starts with a quotation mark
	                if (!cell.empty() && cell.front() == '"') {
	                    string quotedCell = cell;
	                    // Continue accumulating until we find the closing quote
	                    while (!quotedCell.empty() && quotedCell.back() != '"') {
	                        string nextPart;
	                        if (getline(ss, nextPart, ',')) {
	                            quotedCell += "," + nextPart; // Add delimiter and next part
	                        } else {
	                            break; // Exit if no more parts
	                        }
	                    }
	                    // Remove enclosing quotes
	                    if (!quotedCell.empty() && quotedCell.front() == '"' && quotedCell.back() == '"') {
	                        quotedCell = quotedCell.substr(1, quotedCell.size() - 2);
	                    }
	                    cell = quotedCell; // Update the cell to the accumulated quoted content
	                }
	                rowData.push_back(cell);
	            }
	            break; // Stop reading after the desired line
	        }
	    }

	    file.close();

	    // Print the rowData
	    for (const auto& cell : rowData) {
	        cout << cell << " ";
	    }
	    cout << endl;
	}

    void visualizeTree() {
        
        tree->display();
    }
    void switchBranch() {

        

        cout << "Select which branch you want: ";
        int n;
        for (int i = 0; i < branches.size(); i++) {
            cout << i + 1 << ": " << branches[i]<<endl;
        }
        cin >> n;
        if (n - 1 >= 0 && n <= branches.size()) {
            currBranch = branches[n - 1];
            tree->changeBranch(roots[n-1]);
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


        path sourceDir = name +  "/"+ "main";
        path destinationDir = name +"/"+ newBranch;

        create_directory(destinationDir);

        for (const auto& entry : directory_iterator(sourceDir)) {
            const auto& sourcePath = entry.path();
            auto destinationPath = destinationDir / sourcePath.filename();

                // Copy files
                copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);
            
        }
    }
    void deleteBranch() {

    }
    void mergeBranch() {

    }
};

