#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "AVL.h"
#include "RedBlack.h"
using namespace std;

class CSVReader {


public:
    void readCSVColumn(int targetColumnIndex, AVL<string>& tree) {
        ifstream file("healthcare_dataset.csv");
        if (!file.is_open()) {
            cerr << "Error: Could not open file!" << endl;
            return;
        }

        string line;
        getline(file, line); // Read the header line and skip it

        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            int currentColumnIndex = 0;

            // Parse the line using ',' as a delimiter
            while (getline(ss, cell, ',')) {
                if (currentColumnIndex == targetColumnIndex) {
                    tree.insert(cell);
                    break; // No need to process further columns for this line
                }
                currentColumnIndex++;
            }
        }

        file.close();
    }
    void readCSVColumn(int targetColumnIndex, RedBlackTree<string>& tree) {
        ifstream file("healthcare_dataset.csv");
        if (!file.is_open()) {
            cerr << "Error: Could not open file!" << endl;
            return;
        }

        string line;
        getline(file, line); // Read the header line and skip it

        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            int currentColumnIndex = 0;

            // Parse the line using ',' as a delimiter
            while (getline(ss, cell, ',')) {
                if (currentColumnIndex == targetColumnIndex) {
                    tree.insert(cell);
                  
                    break; // No need to process further columns for this line
                }
       
                currentColumnIndex++;
            }
        }

        file.close();
    }

};