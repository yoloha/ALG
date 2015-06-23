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
	//Bbox.opt(LINEAR);
	// case 1: 
	// case 2: 
	// case 3: 
	// case 4: 
	// case 5: 

	Bbox.opt(GENETIC);
	// case 1: 14.57
	// case 2:*5.97
	// case 3:*79.52      12.6011 11.6306
	// case 4: 1.85
	// case 5:*1.54 

	//Bbox.opt(GREEDY_N);
	// case 1: 14.57
	// case 2: 49.86
	// case 3: 273.18
	// case 4: 1.85
	// case 5: 99.91

	//Bbox.opt(GREEDY_N2);
	// case 1: 16.94
	// case 2: 75.31
	// case 3: 325.97
	// case 4: 1.85
	// case 5: 101.98
	
	timer.getTotalUsage(timing);


	
	if (argc == 3) {
		ofstream outfile(argv[2], ios::out);
		//Bbox.printInfo(outfile);
		Bbox.output(outfile);
	}
	else {
		Bbox.printInfo(cout);
		//Bbox.output(cout);
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
