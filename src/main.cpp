#include <iostream>
#include <fstream>
#include "colorBalancing.h"
#include "OptMethod.h"
//#include "tm_usage.h"

using namespace std;

int main(int argc, char const *argv[])
{
 	TmUsage tmusg;
 	TmStat stat;
 	tmusg.totalStart();


 	BoundingBox Bbox;
 	ifstream ifs("../inputs/case_ex");
 	Bbox.readBlock(ifs);


 	tmusg.periodStart();

 	Bbox.buildGroup(CHECKALL_METHOD);
	tmusg.getPeriodUsage(stat);
	

	Bbox.buildWindow();
 	

 	Bbox.printInfo(cout);
 	//Bbox.output(cout);

 	cout <<"user time: " << stat.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
	cout <<"system time: " << stat.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
	cout <<"user+system time:" << (stat.uTime + stat.sTime) / 1000000.0 << "s" << endl; 
}