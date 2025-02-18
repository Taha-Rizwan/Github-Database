#pragma once
#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include"Merkle.h"
using namespace std;



template<class T>
class Tree {
public:
	virtual void make() {}
	MerkleTree<T>* merkle;
	string rootFile;
	int order;
	//Comparison fUNCTIONS
	//CHAR
	void toLower(string& data) {
		for (int i = 0; i < data.length(); i++) {
			if (data[i] >= 'A' && data[i] <= 'Z')
				data[i] += 32;
		}
	}
	int isEqual(char c, char d) {
		return isEqual(int(c), int(d));
	}


	//int
	int isEqual(int c, int d) {
		if (c == d) {
			return 0;
		}
		else if (c > d) {
			return 1;
		}
		else {
			return -1;
		}
	}

	//string
	int isEqual(string c, string d) {
		toLower(c);
		toLower(d);
		int len1 = c.length();
		int len2 = d.length();
		for (int i = 0; i < min(len1, len2); i++) {
			if (c[i] > d[i] && len1>=len2) {
				return 1;
			}
			else if (c[i] < d[i] && len2>=len1) {
				return -1;
			}
		}

		if (len1 > len2) {
			return 1;
		}
		else if (len1 < len2) {
			return -1;
		}

		return 0;
	}


	//instructor's hash

	string instructorHash(int number) {
		int hash = 1;

		while (number > 0) {
			int digit = number % 10; // Extract the last digit
			hash *= digit;           // Multiply the digit
			number /= 10;            // Remove the last digit
		}

		return to_string_generic(hash); // Take the result modulo 29
	}

	// Function to calculate hash for a string
	string instructorHash( string text) {
		int hash = 1;

		for (int i = 0; i < text.length();i++) {
			hash *= text[i]; // Multiply ASCII values of characters

			hash %= 29;     // To avoid integer overflow, take modulo 29 after each step
		}
	
		return to_string_generic(hash);
	}
	/*virtual string computeHashHelper(Tree<T>* node) { return " "; }
	virtual void computeHash() {}*/
	virtual void insert(T data,int ln) {}
	virtual void display() {}
	//Return Line Number of where data is deleted from
	virtual int deleteByVal(T data,bool updation) {return 1;}
	virtual int deleteByVal(T data,int ln) {return 1;}
	virtual void update(T oldData, T newData) {}
	virtual void createNil() {

	}
	virtual void emptyTable() {

	}
	virtual void changeBranch(const string& path){}
	virtual string getRootFile() {
		return "nil";
	}
	virtual vector<int> searchData(T data) {
		return {};
	}

};


