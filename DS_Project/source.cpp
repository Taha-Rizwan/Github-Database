#include <iostream>
#include "RedBlack.h"
#include"AVL.h"
using namespace std;


int main() {

	RedBlackTree<string> t;
	//AVL<string> tree;
	
	return 0;
}

/*	
	Things to do

	Dealing with dups:
		Still haven't figured out how to deal with dups, one idea is using line numbers where the line number can be used to insert either to left or to the right rather than storing line numbers every time, although idk about deletion
	Loading a tree from a folder:
		in case of switching trees or when we load a repo, we'll have to make / load a tree from the folder of the a branch.
	Comparing hash of two trees:
		When merging branches or commiting, how to recursively go down both trees and fix things
	B-Tree and SHA-256:
		Self Explanatory
	
	SERVERS

	The repository folder will have a branches folder and a server folder (could be a servers folder because the project overview talks about multiple servers??)

	Each commit will commit that branch over to the server folder using the root hash to determine the necessary changes
	
	Each commit will have a seperate txt file with data such as the message, timestamp and a unique identifier which I guess is the version number

	VERSIONING

	Each commit means new version?? Does this mean you're supposed to save the entire branch before a commit in a seperate folder of versions

	MERGING BRANCHES
	This I'm assuming just checks the hashes and recursively goes down to the places with a changed hash and just changes the files there

	SAVE REPO TO FILE
	Saves the current repo to file, the tree structure, root node and other stuff etc and similarly loading that would be a function
*/