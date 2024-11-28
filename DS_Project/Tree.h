#pragma once
#include <iostream>
using namespace std;

template<class T>
string to_string_generic(const T& data) {
	std::stringstream ss;
	ss << data;

	return ss.str();
}

template<class T>
class Tree {

public:
	//Comparison fUNCTIONS
	//CHAR
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
		int len1 = c.length();
		int len2 = d.length();
		for (int i = 0; i < min(len1, len2); i++) {
			if (c[i] > d[i]) {
				return 1;
			}
			else if (c[i] < d[i]) {
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
	string instructorHash(const std::string& text) {
		int hash = 1;

		for (char ch : text) {
			hash *= static_cast<int>(ch); // Multiply ASCII values of characters
			hash %= 29;                   // To avoid integer overflow, take modulo 29 after each step
		}

		return to_string_generic(hash);
	}
	virtual string computeHashHelper(Tree<T>* node) { return " "; }
	virtual void computeHash() {}
	virtual void insert(T data) {}
	virtual void display() {}
	virtual void deleteByVal(T data) {}
	virtual void update(T oldData, T newData) {}
};


