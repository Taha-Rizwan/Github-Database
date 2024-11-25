#include <iostream>
#include "CSVReader.h"
using namespace std;


int main() {

	RedBlackTree<int> r;

	r.insert(10);
	r.insert(20);
	r.insert(30);
	r.insert(40);
	r.insert(50);
	r.insert(35);
	r.deleteByVal(30);
	r.display();
	RedBlackTree<int> t;
	
	t.insert(10);
	t.insert(20);
	t.insert(30);
	t.insert(40);
	t.deleteByVal(10);
	t.display();
	RedBlackTree<int> p;

	p.insert(10);
	p.insert(20);
	p.insert(30);
	p.insert(40);
	p.deleteByVal(10);
	p.insert(10);
	p.deleteByVal(40);

	p.insert(10);
	p.insert(20);
	p.insert(30);
	p.insert(40);
	p.deleteByVal(10);
	p.insert(10);
	p.deleteByVal(40);
	p.insert(80);
	p.deleteByVal(10);
	p.display();


	RedBlackTree<int> k;
	k.insert(65);
	k.insert(50);
	k.insert(80);
	k.insert(90);
	k.insert(70);
	k.insert(60);
	k.insert(10);
	k.insert(62);
	k.deleteByVal(90);
	k.deleteByVal(80);
	k.deleteByVal(70);
	//k.deleteByVal(20);
	k.display();
	return 0;
}

