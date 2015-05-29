#include <iostream>
#include <vector>
#include <algorithm>
#include "colorBalancing.h"
#include "rnGen.h"

using namespace std;

#define MASK(i)  (size_t (1)<<i)

RandomNumGen rnGen(0);


void  WindowsSet::randSim(int max_time)
{
	while(max_time>0){
		simulate(rnGen(MASK(63)));
		--max_time;
	}
}

void WindowsSet::linearSolve()
{
	
}