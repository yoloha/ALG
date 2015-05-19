#include <iostream>
#include <fstream>
#include "colorBalancing.h"

using namespace std;

int main(int argc, char const *argv[])
{
 	BoundingBox Bbox;
 	ifstream ifs("inputs/case_ex");
 	Bbox.readBlock(ifs);
 	Bbox.buildGroup();
	Bbox.buildWindow();
    Bbox.calWindowDensity();
 	Bbox.printInfo(cout);
}
