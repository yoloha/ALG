#include <iostream>
#include <fstream>
#include "colorBalancing.h"

using namespace std;

int main(int argc, char const *argv[])
{
 	BoundingBox test;
 	ifstream ifs("../inputs/case1.in");
 	test.readBlock(ifs);
 	test.buildGroup();
 	test.printInfo(cout);
}