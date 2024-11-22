#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class CSVReader {


public:
    void readCSV() {
        ifstream file;
        file.open("healthcare_dataset.csv");
        string line;
        getline(file, line);
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

};