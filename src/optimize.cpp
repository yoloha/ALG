#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "colorBalancing.h"
#include "linear_optimize.h"
#include "rnGen.h"

using namespace std;
#define MASK(i)  (size_t (1)<<(i))


RandomNumGen rnGen(0);

size_t BruteForceSim(vector<vector<int> >& );
int vecOneNorm(const vector<int> &);
void printMatrix(ostream &, const vector<vector<int > >);

void  WindowsSet::randSim(int max_time)
{
	while(max_time>0){
		//simulate(rnGen(MASK(63)));
		--max_time;
	}
}

void WindowsSet::linearSolve()
{
	if(_groupNum<=30){
		//transpose
		vector<int> tmp(_areaMatrix.size(),0);
		vector<vector<int> >G(_areaMatrix[0].size(),tmp);
		for(size_t i = 0 ; i < _areaMatrix.size() ; ++i){
			for(int j = 0 ; j < _groupNum; ++j){
				//cout<<_areaMatrix[i][j]<<endl;
				G[j][i] = _areaMatrix[i][j];
			}
			//cout<<endl;
		}
		//print G
		

		//solving
		cout<<"Solving WindowsSet ..."<<endl;
		_sim[0] = BruteForceSim(G);
		//cout<<"G size"<<G.size()<<endl;
	}
}

void WindowsSet::directSim()
{
	if(_groupNum<=25){
		cout<< MASK(_groupNum)<<endl;
		for(size_t i = 0 ; i < MASK(_groupNum); ++i){
			vector<size_t> temp;
			temp.push_back(i);
			simulate(temp);
		}
	}
}
