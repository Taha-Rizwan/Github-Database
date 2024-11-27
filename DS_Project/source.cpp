#include <iostream>
#include "RedBlack.h"
#include"AVL.h"
using namespace std;


int main() {

	RedBlackTree<string> t;
	AVL<int> tree;
	tree.insert(21);
	tree.insert(26);
	tree.insert(30);
	tree.insert(9);
	tree.insert(4);
	tree.insert(14);
	tree.insert(28);
	tree.insert(1);
	tree.insert(31);
	tree.print();
	tree.deleteByVal(28);
	tree.print();
	return 0;
}

/*

	Also inherit each tree from Tree.h
	things each tree will have in common

	some functions with the SAME NAME in order to streamline stuff
	void insert 
	void display
	void deleteByVal
	





	File Creation

	Based on Name
	- For Dups // Concat the line number aswell as the date maybe??
	
	Updating children and parent for every rotation
	when creating a child you must also go to its parent and update it
	
	
	
	Format

	data

	pathToParent
	children paths left to right with space







*/