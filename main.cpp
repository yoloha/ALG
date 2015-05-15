#include <iostream>
#include <fstream>
#include "colorBalancing.h"

using namespace std;

int main(int argc, char const *argv[])
{
 	BoundingBox test;
 	ifstream ifs("./case_ex");
 	test.readBlock(ifs);
 	test.buildGroup();
 	test.printInfo(cout);
}