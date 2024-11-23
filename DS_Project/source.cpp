#include <iostream>
#include "AVL.h"
#include "CSVReader.h"
using namespace std;


int main() {
	CSVReader j;
	AVL<string> tree;
	j.readCSVColumn(0,tree);
	tree.display();

	return 0;
}

