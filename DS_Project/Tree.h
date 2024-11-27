#pragma once
#include <iostream>
using namespace std;

template<class T>
class Tree {

public:
	virtual void insert(T data) {}
	virtual void display(){}
	virtual void deleteByVal(T data) {}
	virtual void computeHash() {}
};