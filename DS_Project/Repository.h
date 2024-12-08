#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Tree.h"
#include <vector>
using namespace std;
using namespace std::filesystem;

//Addtiion in current data
struct Addition {
    vector<string> rowData;
    int lineNumber;
    Addition(vector<string> rowData, int lineNumber) : rowData(rowData), lineNumber(lineNumber) {}
    string metaData() {
        string data = "Addition\n";
        for (int i = 0; i < rowData.size(); i++) {
            data += rowData[i] + "\n";
        }

        data += to_string(lineNumber) + "\n";
        return data;
    }
};


//Deletion in current data
struct Deletion {
    string data;
    int lineNumber;
    vector<string>rowData;
    Deletion(string data, int lineNumber,vector<string>& rowData) : data(data), lineNumber(lineNumber),rowData(rowData) {}
    string metaData() {
        string data = "Deletion\n";
        data += this->data + "\n";
        data += to_string(lineNumber) + "\n";
        for (int i = 0; i < rowData.size(); i++)
            data += rowData[i] + '\n';
        return data;
    }
};


//Updation in current data
struct Updation {
    vector<string> rowData;
    string old;
    int lineNumber;
    int column;
    Updation(vector<string> rowData, int lineNumber, int column, string old) : rowData(rowData), lineNumber(lineNumber), column(column), old(old) {}
    string metaData() {
        string data = "Updation\n";
        data += old + "\n" + rowData[column] + "\n" +  to_string(column) + "\n" + to_string(lineNumber) + "\n";
        for (int i = 0; i < rowData.size(); i++) {
            data += rowData[i] + "\n";
        }
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
        cout << "\t6: Visualize" << endl;
        cout << "\t7: Commit" << endl;
        cout << "\t8: View Data" << endl;
        cout << "\t9: Merge Branch " << endl;
        cout << "\t10: Roll Back to Version " << endl;
        cout << "\t11: Update All " << endl;
        cout << "\t12: Delete All " << endl;
        cout << "\t0: Save and Exit " << endl;
        cout << "\tChoose: ";
    }

public:
    string treeType;
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
    bool useSha;
    float currVersion;

    Repository(Tree<T>* tree,string treeType) :tree(tree), currVersion(0.1),treeType(treeType) {
    }


    void create() {
        cout << "Enter Repo Name: ";
        cin >> name;
        cout << "Enter csv path: ";
        cin >> csv_path;
      
        cout << "Select Hashing Algorithm: \n0.Instructor's Hash \n1.SHA256\nSelect: ";
        cin >> useSha;
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
        getline(file, line);
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
        cout << "Columns: " << endl;
        for (int i = 0; i < header.size(); i++) {
            cout << i << ": " << header[i] << endl;
        }

        cout << "Enter column Number(0-indexed): ";
        cin >> column;

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
        tree->make();
        //tree->computeHash();
        tree->merkle = new MerkleTree<T>(tree->order,useSha);
        tree->changeBranch(tree->getRootFile());
        //cout << "Root Hash: " << tree->merkle->buildMerkleTree(tree->rootFile) << endl;
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "AFTER READING ...DATA FOLDER: " << dataFolder << endl << endl;
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
        roots.push_back(tree->getRootFile());
        cout << endl;

        ofstream log(name + "/" + currBranch + "/log.txt");
        log.close();


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
            case 9:
                mergeBranch();
                break;
            case 10:
                rollBackToVersion();
                break;
            case 11:
                updateAll();
                break;
            case 12:
                deleteAll();
                break;
            default:
                logic = false;
                break;
            }
        }
    }
    //Read Data from a line Number
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
    //Write Data to a Line Number
    void writeFileByLineNumber(int ln, vector<string>& rowData) {
        ofstream dataFile;
        dataFile.open(name + "/" + currBranch + "/data/" + to_string(ln) + ".txt");
        for (int i = 0; i < rowData.size(); i++) {
            dataFile << rowData[i] << '\n';
        }
        dataFile.close();
    }


    //Add node to staging(Won't actually work unless commited)
    void addNode() {
        // T val;
        // cout << "Value to add: ";
        // cin >> val;
        vector<string> rowData;
        string data;
        cin.ignore();
        for (int i = 0; i < header.size(); i++) {
            cout << header[i] << ": ";

            getline(cin, data);
            //getline(data);
            rowData.push_back(data);
        }
        additions.emplace_back(Addition(rowData, ln));
    }

    //Permanently commit data
    void addDataFr(Addition& addition) {

        writeFileByLineNumber(ln, addition.rowData);
        tree->insert(addition.rowData[column], ln++);
        for (int i = 0; i < branches.size(); i++) {
            if (branches[i] == currBranch) {
                roots[i] = tree->getRootFile();
                return;
            }
        }
        //tree->computeHash();
    }

    //Delete node from staging(Won't actually work unless commited)
    void deleteNode() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        vector<int> l = tree->searchData(val);
        if (!l.empty()) {

                int opt;
                cout << "From which line number do you want to see data: ";
                for (int i = 0; i < l.size(); i++) {
                    bool alreadyGone = false;
                    for (int j = 0; j < deletions.size(); j++) {
                        cout << deletions[j].lineNumber << " " << l[i] << endl;
                        if (deletions[j].lineNumber == l[i]) {
                            alreadyGone = true;
                            break;
                        }
                    }
                    if(!alreadyGone)
                        cout << "Line Number: " << l[i] << endl;
                }
                cin >> opt;
                bool find = false;
                for (int i = 0; i < l.size(); i++) {
                    if (l[i] == opt) {
                        find = true;
                        break;
                    }
                }
                if (find) {
                    vector<string> rowData = readFileByLineNumber(opt);
                    deletions.push_back(Deletion(val, opt,rowData));
                }
        }
        else {
            for (int i = 0; i < additions.size(); i++) {
                if (additions[i].rowData[column] == val) {
                    additions.erase(additions.begin() + i);
                    //cout << "Deleted from line number: " << val << endl;
                    return;
                }
            }

           // cout << "Not Found: " << endl;
        }
    }
    void deleteAll() {
        T val;
        cout << "Value to delete: ";
        cin >> val;
        vector<int> l = tree->searchData(val);
        if (!l.empty()) {

            for (int i = 0; i < l.size(); i++) {
                bool alrGone = false;
                for (int j = 0; j < deletions.size(); j++) {
                    if (l[i] == deletions[j].lineNumber) {
                        alrGone = true;
                        break;
                    }
                   }
                if (!alrGone) {

                    vector<string> rowData = readFileByLineNumber(l[i]);
                    deletions.push_back(Deletion(val, l[i], rowData));
                }
              }
            
        }
        else {
            for (int i = 0; i < additions.size(); i++) {
                if (additions[i].rowData[column] == val) {
                    additions.erase(additions.begin() + i);
                    //cout << "Deleted from line number: " << val << endl;
                    return;
                }
            }

            // cout << "Not Found: " << endl;
        }
    }
    //Permanently delete data
    void deleteDataFr(Deletion& deletion) {

        cout << deletion.data << " " << deletion.lineNumber << endl;

        tree->deleteByVal(deletion.data, deletion.lineNumber);

        cout << "Deleted from line number: " << deletion.lineNumber << endl;
        remove((name + "/" + currBranch + "/data/" + to_string_generic(deletion.lineNumber) + ".txt").c_str());
        for (int i = 0; i < branches.size(); i++) {
            if (branches[i] == currBranch) {
                roots[i] = tree->getRootFile();
                return;
            }
        }
    }

    //Update data to staging
    void updateNode() {
        T val, newVal;
        cout << "Value to update: ";
        cin >> val;
        //cout << "searching for data\n";
        vector<int> l = tree->searchData(val);


        if (!l.empty()) {
            //for (int i = 0; i < deletions.size(); i++) {
            //    if (deletions[i].lineNumber == ln) {
            //       // cout << "Not Found!" << endl;
            //        return;
            //    }
            //}

            int ln;
            cout << "From which line number do you want to see data: ";
            for (int i = 0; i < l.size(); i++) {
                bool alreadyGone = false;
                for (int j = 0; j < deletions.size(); j++) {
                    if (deletions[j].lineNumber == l[i]) {
                        alreadyGone = true;
                        if (l.size() == 1)
                            return;
                        break;
                    }
                }
                if (!alreadyGone)
                    cout << "Line Number: " << l[i] << endl;
            }
            cin >> ln;
            bool find = false;
            for (int i = 0; i < l.size(); i++) {
                if (l[i] == ln) {
                    find = true;
                    break;
                }
            }


            if (find) {
                cout << "What do you want to change: " << endl;
                for (int i = 0; i < header.size(); i++) {
                    cout << i << ": " << header[i] << endl;
                }
                int opt;
                cin >> opt;
                if (opt >= 0 && opt < header.size()) {
                    vector<string> rowData = readFileByLineNumber(ln);
                    cout << "Current " << header[opt] << ": " << rowData[opt] << endl;
                    cout << "Updated " << header[opt] << ": ";
                    string data;
                    cin.ignore();
                    getline(cin, data);
                    string old = rowData[opt];
                    rowData[opt] = data;
                    updations.emplace_back(Updation(rowData, ln, opt, old));

                    //writeFileByLineNumber(ln, rowData);
                    // if (opt==column) {
                    //     tree->deleteByVal(old,ln);
                    //     tree->insert(data,ln);
                    // }
                }
            }
        }
        else {
            for (int i = 0; i < additions.size(); i++) {
                if (additions[i].rowData[column] == val) {
                    cout << "What do you want to change: " << endl;
                    for (int i = 0; i < header.size(); i++) {
                        cout << i << ": " << header[i] << endl;
                    }
                    int opt;
                    cin >> opt;
                    if (opt >= 0 && opt < header.size()) {
                        cout << "Current " << header[opt] << ": " << additions[i].rowData[opt] << endl;
                        cout << "Updated " << header[opt] << ": ";
                        string data;
                        cin.ignore();
                        getline(cin, data);
                        string old = additions[i].rowData[opt];
                        additions[i].rowData[opt] = data;
                        return;
                    }
                    else
                        break;

                }
            }
        }
        cout << "Not Found: " << endl;
    }

    void updateAll() {
        T val, newVal;
        cout << "Value to update: ";
        cin >> val;
        cout << "searchind for dta\n";
        vector<int> l = tree->searchData(val);


        if (!l.empty()) {
  
                cout << "What do you want to change: " << endl;
                for (int i = 0; i < header.size(); i++) {
                    cout << i << ": " << header[i] << endl;
                }
                int opt;
                cin >> opt;
                cout << "New " << header[opt] << ": ";
                string data;
                cin.ignore();
                getline(cin, data);
                for (int i = 0; i < l.size(); i++) {
                    int ln = l[i];
                    if (opt >= 0 && opt < header.size()) {
                        vector<string> rowData = readFileByLineNumber(ln);
                        string old = rowData[opt];
                        rowData[opt] = data;
                        updations.emplace_back(Updation(rowData, ln, opt, old));

                        //writeFileByLineNumber(ln, rowData);
                        // if (opt==column) {
                        //     tree->deleteByVal(old,ln);
                        //     tree->insert(data,ln);
                        // }
                    }
                }
               
        }
    }

    //Update data permanently
    void updateDataFr(Updation& update) {
        writeFileByLineNumber(update.lineNumber, update.rowData);
        if (update.column == column) {
            cout << "Updating\n";
            tree->deleteByVal(update.old, update.lineNumber);
            tree->insert(update.rowData[column], update.lineNumber);

            for (int i = 0; i < branches.size(); i++) {
                if (branches[i] == currBranch) {
                    roots[i] = tree->getRootFile();
                    return;
                }
            }


        }

    }


    void commit() {
        if (additions.empty() && deletions.empty() && updations.empty()) {
            cout << "Nothing to update" << endl;
            return;
        }
    
        //Write commit to commit file whilst also commiting everything
        cout << "Commit Message: ";
        string message;
        cin.ignore();
        getline(cin, message);
        ofstream file;
        std::ostringstream versionStream;
        versionStream << fixed << setprecision(1) << currVersion;
        string versionStr = versionStream.str();
        string path = name + "/" + currBranch + "/commit" + currBranch
            + (versionStr) + ".txt";
        file.open(path, ios::trunc);
        file << "Version: " << currVersion << endl;
        for (int i = 0; i < additions.size(); i++) {
            addDataFr(additions[i]);
            file << additions[i].metaData();
        }
        for (int i = 0; i < deletions.size(); i++) {
            deleteDataFr(deletions[i]);
            //cout<<"Deleting: "<<deletions[i].data<<endl;
            file << deletions[i].metaData();
        }
        for (int i = 0; i < updations.size(); i++) {
            updateDataFr(updations[i]);

            file << updations[i].metaData();
        }
        file << message << endl;
        file.close();
        currVersion += 0.1;
        ofstream log(name + "/" + currBranch + "/log.txt", ios::app);
        log << path << endl;
        log.close();
        tree->emptyTable();

        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
    
        //Clearing up everything for future
        updations.clear();
        deletions.clear();
        additions.clear();
        cout << updations.size()<<endl;
        cout << deletions.size()<<endl;
        cout << additions.size()<<endl;
    }

    void viewNodeData() {

        T data;
        cout << "Enter data to view";
        cin >> data;
        vector<int> toBeViewed = tree->searchData(data);

        if (toBeViewed.empty()) {
            cout << "Data not found!" << endl;
            return;
        }
        else {
            for (int i = 0; i < toBeViewed.size(); i++) {
                vector<string> rowData = readFileByLineNumber(toBeViewed[i]);

                for (int j = 0; j < rowData.size(); j++) {
                    cout << header[j] << ": " << rowData[j] << endl;
                }
            }
           

        }
    }

    void visualizeTree() {
        tree->display();
    }
    //Switching branches
    void switchBranch() {

        if (!additions.empty() && !deletions.empty() && !updations.empty()) {
            cout << "Current Branch has uncommited changes!" << endl;
            return;
        }

        cout << "Select which branch you want: ";
        int n;
        for (int i = 0; i < branches.size(); i++) {
            cout << i + 1 << ": " << branches[i] << endl;
        }
        cin >> n;
        if (n - 1 >= 0 && n <= branches.size()) {
            tree->emptyTable();

            tree->changeBranch(roots[n - 1]);
            currBranch = branches[n - 1];
            cout << "Current Branch is set to: " << currBranch << endl;
        }
        else {
            cout << "Branch not found!" << endl;
        }


    }


    // Adding branch by copying current branch
    void addBranch() {
        if (!additions.empty() && !deletions.empty() && !updations.empty()) {
            cout << "Current Branch has uncommited changes!" << endl;
            return;
        }
        string newBranch;
        cout << "Enter the name for your new branch: ";
        cin >> newBranch;

        tree->emptyTable();
        branches.push_back(newBranch);

        tree->changeBranch(tree->getRootFile());
        currBranch = newBranch;
        roots.push_back(tree->getRootFile());

        cout << "New branch has been created and cloned by current version of main" << endl;
        cout << "Current Branch is set to: " << currBranch << endl;
        tree->merkle->currBranch = newBranch;

        path sourceDir = name + "/" + "main";
        path destinationDir = name + "/" + newBranch;

        create_directory(destinationDir);

        for (const auto& entry : recursive_directory_iterator(sourceDir)) {
            const auto& sourcePath = entry.path();
            auto destinationPath = destinationDir / relative(sourcePath, sourceDir);

            if (is_directory(sourcePath)) {
                create_directory(destinationPath);
            }
            else {
                copy_file(sourcePath, destinationPath, copy_options::overwrite_existing);
            }
        }
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;
    }


    //Saving current state of repository to the file
    void saveRepoToFile() {
        ofstream repoFile(name + ".txt");
        repoFile << treeType << endl;
        repoFile << name << endl;
        repoFile << column << endl;
        repoFile << ln << endl;
        repoFile << branches.size() << endl;
        for (int i = 0; i < branches.size(); i++) {
            repoFile << branches[i] << endl;
        }
        for (int i = 0; i < roots.size(); i++) {
            repoFile << roots[i] << endl;
        }
        repoFile << header.size() << endl;
        for (int i = 0; i < header.size(); i++) {
            repoFile << header[i] << endl;
        }
        repoFile << currBranch << endl;
        repoFile << currVersion << endl;
        repoFile.close();
    }
    //Reading repository state from file
    void readFromFile(const string& path) {
        ifstream repoFile(path);
        if (!repoFile.is_open()) {
            cerr << "Error: Could not open file " << path << endl;
            return;
        }

        getline(repoFile, treeType);
        getline(repoFile, name);
        repoFile >> column;
        repoFile >> ln;
        cout << treeType<<endl;
        cout << name << endl;
        cout << column << endl;
        int branchesSize;
        repoFile >> branchesSize;
        branches.clear();
        repoFile.ignore(); // Ignore the newline character after branchesSize
        for (int i = 0; i < branchesSize; i++) {
            string branch;
            getline(repoFile, branch);
            cout << branch << endl;
            branches.push_back(branch);
        }
        roots.clear(); 
        for (int i = 0; i < branchesSize; i++) {
            string branch;
            getline(repoFile, branch);
            cout << branch << endl;
            roots.push_back(branch);
        }
        int headerSize;
        repoFile >> headerSize;
        header.clear();
        repoFile.ignore(); // Ignore the newline character after headerSize
        for (int i = 0; i < headerSize; i++) {
            string headerItem;
            getline(repoFile, headerItem);
            header.push_back(headerItem);
        }

        getline(repoFile, currBranch);
        repoFile >> currVersion;

        repoFile.close();


        tree->changeBranch(roots[0]);
        currBranch = branches[0];
        tree->merkle = new MerkleTree<T>(tree->order,useSha);
        string dataFolder = name + "\\" + currBranch + "\\" + "data";
        cout << "Root Hash: " << tree->merkle->buildMerkleTree(dataFolder)->hash << endl;

       

    }

    //merging of branches 
    void mergeBranch() {
        if (!additions.empty() && !deletions.empty() && !updations.empty()) {
            cout << "Current Branch has uncommited changes!" << endl;
            return;
        }
        tree->emptyTable();
        cout << "Enter the name of the branch to merge with: ";
        string targetBranch;
        cin >> targetBranch;
        //now merge current branch adn target branch
        string currFolder = name + "\\" + currBranch + "\\" + "data";
        string targetFolder = name + "\\" + targetBranch + "\\" + "data";
        cout << currFolder << " and " << targetFolder << endl;
        
        MerkleTree<T>* targetMerkle = new MerkleTree<T>(tree->order,useSha);
        string targetHash = targetMerkle->buildMerkleTree(targetFolder)->hash;
        string currHash=tree->merkle->buildMerkleTree(currFolder)->hash;
        cout << "Currhash: " << currHash << endl;
        if (targetHash == currHash) {
            cout << "No change in data\n";
            cout << currBranch << " and " << targetBranch << " merged successfully\n";
        }
        else {
            cout << "Data has been changed in the currBranch...We need to merge\n";
            string path = "root";
            tree->merkle->lookForChange(targetMerkle->root, tree->merkle->root, path);
            compareLogs(currBranch, targetBranch);
        }
    }

    void compareLogs(string currBranch, string targetBranch) {
        ifstream curr(name + "\\" + currBranch + "\\" + "log.txt");
        ifstream target(name + "\\" + targetBranch + "\\" + "log.txt");

        if (!curr.is_open() || !target.is_open()) {
            cerr << "Error: Could not open log files for comparison." << endl;
            return;
        }

        vector<string> currLines;
        vector<string> targetLines;
        string line;

        // Read lines from the current branch log file
        while (getline(curr, line)) {
            currLines.push_back(line);
        }

        // Read lines from the target branch log file
        while (getline(target, line)) {
            targetLines.push_back(line);
        }

        curr.close();
        target.close();
        // Output the lines for verification
        int similar = 0;

        for (int i = 0; i < targetLines.size() && i < currLines.size(); i++) {
            if (currLines[i] != targetLines[i])
                break;
            similar++;
        }

        std::filesystem::copy(name + "\\" + targetBranch + "\\" + "log.txt", name + "\\" + currBranch + "\\" + "log.txt", std::filesystem::copy_options::overwrite_existing);
        makeChanges(targetBranch,currLines, similar);
    }

    void makeChanges(string& targetBranch, vector<string>& currLines, int& similar) {
   
        for (int i = 0; i < branches.size(); i++) {
            if (targetBranch == branches[i]) {
                tree->changeBranch(roots[i]);
                currBranch = targetBranch;
                break;
            }
        }
        //Making changes after reading logs
        for (int i = similar; i < currLines.size(); i++) {
            string str = readCommit(currLines[i]);
            performCommit(targetBranch, str);
        }
    }

    void performCommit(string& targetBranch, string & commit) {
        stringstream ss(commit);
        string line;
        while (getline(ss, line)) {
            if (line == "Addition") {
                //cout << "Wow addition" << endl;
                vector<string> rowData;
                for (int i = 0; i < header.size(); i++) {
                    getline(ss, line);
                    rowData.push_back(line);
                }

                getline(ss, line);
                int lineNumber = stoi(line);
                //cout << rowData[column] << " " << lineNumber << endl;
                tree->insert(rowData[column], lineNumber);

                writeFileByLineNumber(lineNumber, rowData);
                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }

            }
            else if (line == "Deletion") {
                //cout << "Wow Deletion" << endl;

                getline(ss, line);
                string data = line;
                getline(ss, line);
                int lineNumber = stoi(line);

                //cout << data << " " << lineNumber << endl;
                tree->deleteByVal(data, lineNumber);
                remove(name + "\\" + currBranch + "\\data\\" + to_string(lineNumber) + ".txt");
                for (int i = 0; i < header.size(); i++)
                    getline(ss, line);
                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }
            }
            else if (line == "Updation") {
                //cout << "wow Updation" << endl;

                getline(ss, line);
                string old = line;
                getline(ss, line);
                string newD = line;
                getline(ss, line);
                int cn = stoi(line);
                getline(ss, line);
                int lineNumber = stoi(line);
                vector<string> rowData;
                for (int i = 0; i < header.size(); i++) {
                    getline(ss, line);
                    rowData.push_back(line);
                }
                writeFileByLineNumber(lineNumber, rowData);
                if (cn == column) {
                    tree->deleteByVal(old, lineNumber);
                    tree->insert(newD, lineNumber);
                }
                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }


                /*
                  string metaData() {
                string data = "Updation\n";
                data += old + "\n" + rowData[column] + "\n" +  to_string(column) + "\n" + to_string(lineNumber) + "\n";
                return data;
                 }
     */
            }
            else {
                return;
            }
        }
     
    }

    string readCommit(string& path) {
        ifstream file(path);
        string result;

        if (!file.is_open()) {
            throw runtime_error("Could not open file: " + path);
        }

        string line;
        bool firstLineSkipped = false;

        while (getline(file, line)) {
            if (!firstLineSkipped) {
                firstLineSkipped = true; // Skip the first line
                continue;
            }
            result += line + "\n"; // Append the line with its newline character
        }

        if (!result.empty() && result.back() == '\n') {
            result.pop_back(); // Remove the trailing newline character (if needed)
        }

        return result;
    }


    void rollBackToVersion() {
        ifstream file(name + "\\" + currBranch + "\\" + "log.txt");
        vector<string> targetLines;
        string line;
        
        // Read lines from the current branch log file
        while (getline(file, line)) {
            targetLines.push_back(line);
        }
        file.close();
        cout << "Select which version you want to rollback to(0 to go back to default): " << endl;
        for (int i = 0; i < targetLines.size(); i++) {
            cout << i+1<<": " << targetLines[i] << endl;
        }
        //Select which version to roll back to
        int opt;
        cin >> opt;

        for (int i = targetLines.size() - 1; i >= opt; i--) {
            
                string str = readCommit(targetLines[i]);
                cout << str << endl;
                performReverse(str);   
                tree->emptyTable();
                currVersion-=0.1;
        }

        ofstream file2(name + "\\" + currBranch + "\\" + "log.txt",ios::trunc);
        for (int i = 0; i < opt && i < targetLines.size(); i++) {
            file2 << targetLines[i]<<endl;
        }
        file2.close();
    }
    void performReverse( string& commit) {
        stringstream ss(commit);
        string line;
        while (getline(ss, line)) {
            if (line == "Addition") {
                //cout << "Wow addition" << endl;
                vector<string> rowData;
                for (int i = 0; i < header.size(); i++) {
                    getline(ss, line);
                    rowData.push_back(line);
                }

                getline(ss, line);
                int lineNumber = stoi(line);
                cout << rowData[column] << " " << lineNumber << endl;
                tree->deleteByVal(rowData[column], lineNumber);

                remove(name + "\\" + currBranch + "\\data\\" + to_string(lineNumber) + ".txt");
                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }

            }
            else if (line == "Deletion") {
                //cout << "Wow Deletion" << endl;

                getline(ss, line);
                string data = line;
                getline(ss, line);
                int lineNumber = stoi(line);

                //cout << data << " " << lineNumber << endl;
                vector<string> rowData;
                for (int i = 0; i < header.size(); i++) {

                    getline(ss, line);
                    rowData.push_back(line);
                }
                tree->insert(rowData[column], lineNumber);
                writeFileByLineNumber(lineNumber, rowData);
                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }
            }
            else if (line == "Updation") {
                //cout << "wow Updation" << endl;

                getline(ss, line);
                string old = line;
                getline(ss, line);
                string newD = line;
                getline(ss, line);
                int cn = stoi(line);
                getline(ss, line);
                int lineNumber = stoi(line);
                vector<string> rowData;
                for (int i = 0; i < header.size(); i++) {

                    getline(ss, line);
                    if (i != column)
                        rowData.push_back(line);
                    else
                        rowData.push_back(old);
                
                }
                writeFileByLineNumber(lineNumber, rowData);
                if (cn == column) {
                    tree->deleteByVal(newD, lineNumber);
                    tree->insert(old, lineNumber);
                }

                for (int i = 0; i < branches.size(); i++) {
                    if (branches[i] == currBranch) {
                        roots[i] = tree->getRootFile();
                        return;
                    }
                }

                /*
                  string metaData() {
                string data = "Updation\n";
                data += old + "\n" + rowData[column] + "\n" +  to_string(column) + "\n" + to_string(lineNumber) + "\n";
                return data;
                 }
     */
            }
            else {
                cout << line << endl;
                return;
            }
        }

    }


    ~Repository() {
        saveRepoToFile();
    }


};

