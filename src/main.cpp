#include <iostream>
#include <fstream>
#include "colorBalancing.h"
#include "OptMethod.h"

using namespace std;

int main(int argc, char const *argv[])
{
 	BoundingBox Bbox;
 	ifstream ifs("inputs/case_ex");
 	Bbox.readBlock(ifs);
 	Bbox.buildGroup(CHECKALL_METHOD);
	Bbox.buildWindow();
    Bbox.calWindowDensity();
 	Bbox.printInfo(cout);
 	Bbox.output(cout);
}
