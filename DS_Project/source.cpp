#include "RedBlack2.h"
#include"AVL.h"
#include "BTree.h"

int main() {
	//RedBlackTree<int> r;
	

	//RedBlackTree2 current insertion time column 1 (Age) 35s (not handling dups currently)
	// 
	//RedBlackTree current insertion time column 1 (Age) 2.78s (not handling dups currently) bro

	AVL<int> r;
	r.insert(10);
	r.insert(11);
	r.insert(12);
	r.insert(13);
	r.insert(14);
	r.insert(15);
	r.insert(16);
	r.insert(17);
	r.insert(18);
	r.insert(19);
	r.insert(20);
	r.insert(21);
	r.insert(22);
	r.insert(23);
	r.insert(24);
	r.insert(25);
	r.insert(26);
	r.insert(27);
	r.insert(28);
	r.insert(29);
	r.insert(30);
	r.insert(31);
	r.insert(32);
	r.insert(33);
	r.insert(34);
	r.insert(35);
	r.insert(36);
	r.display();
	return 0;
}

/*	
	Things to do

	Changing of file format

	24(data)
	26(parent),NULL(leftchild),NULL(rightchild)

	comma seperated instead of space seperated due to issue while reading files

	Aqib Rotation ke function abhi ke liye redblack2 se utha le, mazeed behter ho skte hain lekin bas hoi hoi hai
	RedBlack2 mein abhi sirf insertion hoi hai deletion baad mein krdonga
	again, probably bether ho skti hai lekin im ded  ---->me too
	
























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