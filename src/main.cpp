#include <iostream>
#include <fstream>
#include "colorBalancing.h"
#include "optMethod.h"

using namespace std;

void help_message();

int main(int argc, char const *argv[])
{
	if(argc != 2 && argc != 3) {
	   help_message();
	   return 0;
	}

	//TmUsage tmusg;
	//TmStat stat;
	//tmusg.totalStart();

	BoundingBox Bbox;

	ifstream dofile(argv[1], ios::in);
	if (!dofile) { 
		cerr << "Cannot open input file \"" << argv[2] << "\"!!" << endl;
		return false;
	}
	Bbox.readBlock(dofile);

	//tmusg.periodStart();
	Bbox.buildGroup(CHECKALL_METHOD);
	//tmusg.getPeriodUsage(stat);
	
	Bbox.buildWindow();

	//cout <<"user time: " << stat.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
	//cout <<"system time: " << stat.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
	//cout <<"user+system time:" << (stat.uTime + stat.sTime) / 1000000.0 << "s" << endl; 

	Bbox.calWindowDensity(); // for debug
//	Bbox.buildWindowsSet();
	
	if (argc == 3) {
		ofstream outfile(argv[2], ios::out);
		Bbox.printInfo(outfile);
		Bbox.output(outfile);
	}
	else {
		Bbox.printInfo(cout);
		Bbox.output(cout);
	}

	return 0;
}

void help_message()
{
	cout << "USAGE: DPT_balance_color <input_file> [<output_file>]" << endl
		 << "DESCRIPTION: output file is optional" << endl;
}
