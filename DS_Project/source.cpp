#include <iostream>
#include "CSVReader.h"
using namespace std;


int main() {
	CSVReader j;

	RedBlackTree<string> r;
	j.readCSVColumn(1, r);
	/*r.insert(30);
	r.insert(62);
	r.insert(76);
	r.insert(28);
	r.insert(43);
	r.insert(36);
	r.insert(21);
	r.insert(20);*/
	//r.insert(82);
	//r.insert(58);
	//r.insert(72);
	//r.insert(38);
	r.display();

	return 0;
}

