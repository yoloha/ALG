#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "colorBalancing.h"
#include "genetic_alg.h" 
#include "greedy.h"
#include "rnGen.h"
#include "timer.h"

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

		static int count=0;
		count++;
		if (count%check_interval==0){
			checkTimeLeft();
			if (interrupt==true)
				break;
		}
	}
}

void WindowsSet::linearSolve()
{
	if(_groupNum<90){
		//transpose
		vector<int> tmp(_areaMatrix.size(),0);
		vector<vector<int> >G(_areaMatrix[0].size(),tmp);
		for(size_t i = 0 ; i < _areaMatrix.size() ; ++i){
			for(int j = 0 ; j < _groupNum; ++j)
				G[j][i] = _areaMatrix[i][j];
		}
		//solving
		cout<<"Solving WindowsSet ..."<<endl<<endl;
		printMatrix(cout,_areaMatrix);
		optimalSim(_sim,G);
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
			static bool start=true;
			static bool check=true;
			if (check==true){
				check = checkTimeElapsed(start);
				start=false;
			}
			static int count=0;
			count++;
			if (count%check_interval==0){
				checkTimeLeft();
				if (interrupt==true)
					break;
			}
		}
	}
	
	updateWinDensity();
}

void WindowsSet::directSim()
{
	if(_groupNum<=25){
		cout<<"*-----------------------------Calling directSim----------------------------*"<<endl;
		//cout<<MASK(_groupNum)<<endl;
		for(size_t i = 0 ; i < MASK(_groupNum); ++i){
			vector<size_t> temp;
			temp.push_back(i);
			simulate(temp);

			if (i%check_interval==0){
				checkTimeLeft();
				if (interrupt==true)
					break;
			}
		}
	}
}

void WindowsSet::genSim()
{
	RandomNumGen rng(time(0));
	int num=(_groupNum%64==0)? _groupNum/64 : _groupNum/64+1;
	cout<<"*-------------------------Calling Genetic simulation-----------------------*"<<endl;
	// alpha genes
	vector<size_t> F;
	vector<size_t> M;
	double F_record, M_record, result;
	for(int i=0;i<num;i++){
		if(i != num-1){
			F.push_back(rng(MAX));
			M.push_back(rng(MAX));
		}
		else{
			F.push_back(rng(MASK(_groupNum%64)));
			M.push_back(rng(MASK(_groupNum%64)));
		}
	}
	F_record = simulate(F)/((Window::omega * Window::omega))*100;
	M_record = simulate(M)/((Window::omega * Window::omega))*100;
	//********************************************************************************************************************
	//Note that the current update on generation_limit may cause the program run for a long long time
	// if the result improves with a huge step at large gen number
	//******************************************************************************************************************** 
	cout<<"groupNum = "<<_groupNum<<endl;
	int generation_limit = _groupNum*_groupNum*5;
	int sex_limit = _groupNum*10; // don't let the same couple have sex too many times if their children are rubbish XD
	int sex_time = 0;
	for(int gen=0;gen<generation_limit;gen++){
		if( (sex_time <= sex_limit) && (F_record != M_record) ){
			for(int off=0;off<3;off++,sex_time++){
				RandomNumGen rngtime(time(0)+gen+off);
				vector<size_t> C = sex(F,M,_groupNum,rngtime(_groupNum),rngtime(2));
				result = simulate(C)/((Window::omega * Window::omega))*100;;
				if(result < F_record){
					sex_time = 0;
					generation_limit += gen*(F_record-result);
					F_record = result;
					F = C;
				}
				else if(result < M_record && result != F_record){
					sex_time = 0;
					M_record = result;
					M = C;
				}
			}
		}
		vector<size_t> S;
		for(int i=0;i<num;i++){
			if(i != num-1)
				S.push_back(rng(MAX));
			else
				S.push_back(rng(MASK(_groupNum%64)));
		}
		result = simulate(S)/((Window::omega * Window::omega))*100;
		if(result < F_record){
			sex_time = 0; // new partner, resets available sex time
			generation_limit += gen*(F_record-result);
			F_record = result;
			F = S;
		}
		else if(result < M_record && result != F_record){
			sex_time = 0; // new partner, resets available sex time
			M_record = result;
			M = S;
		}
		//try-out sex with the best one
		vector<size_t> T;
		if(F_record < M_record)
			T = sex(S,F,_groupNum,rng(_groupNum),rng(2));
		else
			T = sex(S,M,_groupNum,rng(_groupNum),rng(2));
		result = simulate(T)/((Window::omega * Window::omega))*100;
		if(result < F_record){
			sex_time = 0;
			generation_limit += gen*(F_record-result);
			F_record = result;
			F = T;
		}
		else if(result < M_record && result != F_record){
			sex_time = 0;
			M_record = result;
			M = T;
		}
		cout<<"(F,M,T) = ( "<<setw(8)<<F_record<<" , "<<setw(8)<<M_record<<" , "<<setw(8)<<result<<" )";
		cout<<"\t(gen / generation_limit) = ("<<setw(8)<<gen<<" / "<<setw(8)<<generation_limit<<" )\r";
		static bool start=true;
		static bool check=true;
		if (check==true){
			check = checkTimeElapsed(start);
			start=false;
		}
		static int count=0;
		count++;
		if (count%check_interval==0){
			checkTimeLeft();
			if (interrupt==true)
				break;
		}
	}
	cout<<endl;
	updateWinDensity();
}

void
WindowsSet::greedySolve(const int& method)
{
	cout<<"*-------------------------Calling Greedy Algorithm-----------------------*"<<endl;
	vector<int> grpInWins(_areaMatrix.size(), 0);
	vector< vector<int> > areaMatrixT(_areaMatrix[0].size(), grpInWins);
	for (int i = 0, l = _areaMatrix.size(); i < l; i++) {
		for (int j = 0; j < _groupNum; j++)
			areaMatrixT[j][i] = _areaMatrix[i][j];
	}

	greedySim(_sim, areaMatrixT, method);
	simulate(_sim); //
	updateWinDensity();
}
