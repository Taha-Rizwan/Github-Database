#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

template<class T>
class Tree {

public:
	//Comparison fUNCTIONS
	//CHAR
	
	virtual string computeHashHelper(Tree<T>* node) {
		return "";
	}
	virtual void computeHash() {}
	virtual void insert(T data) {}
	virtual void display(){}
	virtual void deleteByVal(T data) {}
	virtual void update(T oldData, T newData) {}
	virtual void makeTree(string rootPath) {}
};


