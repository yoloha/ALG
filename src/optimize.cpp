#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "colorBalancing.h"
#include "genetic_alg.h" 
#include "rnGen.h"

using namespace std;
#define MASK(i)  (size_t (1)<<(i))


RandomNumGen rnGen(0);

bool optimalSim(vector<size_t>&, vector<vector<int> >&);
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
	if(_groupNum<70){
		//transpose
		vector<int> tmp(_areaMatrix.size(),0);
		vector<vector<int> >G(_areaMatrix[0].size(),tmp);
		for(size_t i = 0 ; i < _areaMatrix.size() ; ++i){
			for(int j = 0 ; j < _groupNum; ++j)
				G[j][i] = _areaMatrix[i][j];
		}
		//solving
		cout<<"Solving WindowsSet ..."<<endl<<endl;
		cout<<optimalSim(_sim,G);
	}
	else{
		size_t max_time = MASK(20);
		while(max_time > 1){
			vector<size_t> temp(3,0);
			temp[0] = rnGen(MASK(63));
			temp[1] = rnGen(MASK(63));
			temp[2] = rnGen(MASK(63));
			simulate(temp);
			--max_time;
			//if(max_time < 1000) cout<<max_time;
		}
	}
	
	updateWinDensity();
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

void WindowsSet::genSim()
{
	// if group number is small enough to implement brute force, should use directSim() instead
	RandomNumGen rng(time(0));
	int num=(_groupNum%64==0)? _groupNum/64 : _groupNum/64+1;
	cout<<"*-----------------Calling Genetic simulation-----------------*"<<endl;
	cout<<"using "<<num<<" size_t numbers for each genetic simulation"<<endl;
	// alpha genes
	vector<size_t> F;
	vector<size_t> M;
	for(int i=0;i<num;i++){
		F.push_back(rng(MAX));
		M.push_back(rng(MAX));
	}
	/*
	cout<<"father: "<<endl;
	printGene(F,_groupNum);
	if(simulate(F))cout<<"father updated"<<endl;
	cout<<"mother: "<<endl;
	printGene(M,_groupNum);
	if(simulate(M))cout<<"mother updated"<<endl;
	*/
	int generation_limit=50;
	for(int gen=0;gen<generation_limit;gen++){
		for(int off=0;off<5;off++){
			RandomNumGen rngtime(time(0)+gen+off);
			vector<size_t> C = evolution(F,M,_groupNum,rngtime);
			//for(int i=0;i<num;i++)
			//	cout<<"child "<<C[i]<<endl;
			if(simulate(C)){
				cout<<"child updated at generation = "<<gen<<"    offspring = "<<off<<endl;
				generation_limit += gen;
				if(rng(2)){
					cout<<"child replaces father"<<endl;
					F=C;
				}
				else{
					cout<<"child replaces mother"<<endl;
					M=C;
				}
			}
		}
		vector<size_t> S;
		for(int i=0;i<num;i++)
			S.push_back(rng(MAX));
		if(simulate(S)){
			cout<<"stranger updated at generation = "<<gen<<endl;
			generation_limit += gen;
			if(rng(2)){
				cout<<"stranger replaces father"<<endl;
				F=S;
			}
			else{
				cout<<"stranger replaces mother"<<endl;
				M=S;
			}
		}
	}
}

