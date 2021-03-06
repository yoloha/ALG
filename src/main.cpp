#include <iostream>
#include <fstream>
#include "colorBalancing.h"
#include "optMethod.h"
#include "tm_usage.h"
#include "timer.h"

using namespace std;
using namespace CommonNs;

void help_message();

int main(int argc, char const *argv[])
{
	if(argc != 2 && argc != 3) {
	   help_message();
	   return 0;
	}

	timer.totalStart();

	BoundingBox Bbox;

	ifstream dofile(argv[1], ios::in);
	if (!dofile) { 
		cerr << "Cannot open input file \"" << argv[2] << "\"!!" << endl;
		return false;
	}
	Bbox.readBlock(dofile);

	Bbox.buildGroup(CHECKALL_METHOD);
	Bbox.buildWindow();
	Bbox.buildWindowsSet();

	Bbox.opt(GENETIC);
	
	timer.getTotalUsage(timing);

	if (argc == 3) {
		ofstream outfile(argv[2], ios::out);
		Bbox.printInfo(cout);
		Bbox.output(outfile);
	}
	else {
		Bbox.printInfo(cout);
		Bbox.output(cout);
	}

	cout <<"user time: " << timing.uTime / 1000000.0 << "s" << endl; // print period user time in seconds
	cout <<"system time: " << timing.sTime / 1000000.0 << "s" << endl; // print period systemtime in seconds
	cout <<"user+system time:" << (timing.uTime + timing.sTime) / 1000000.0 << "s" << endl; 
	
	return 0;
}

void help_message()
{
	cout << "USAGE: DPT_balance_color <input_file> [<output_file>]" << endl
		 << "DESCRIPTION: output file is optional" << endl;
}
