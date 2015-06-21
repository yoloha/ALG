#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "colorBalancing.h"
#include "genetic_alg.h" 
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
		cout<<"*-----------------Calling directSim-----------------*"<<endl;
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
	if(_groupNum<=25){
		directSim();
		updateWinDensity();
		return;
	}
	RandomNumGen rng(time(0));
	int num=(_groupNum%64==0)? _groupNum/64 : _groupNum/64+1;
	cout<<"*-----------------Calling Genetic simulation-----------------*"<<endl;
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
	F_record = simulate(F);
	M_record = simulate(M);
	//********************************************************************************************************************
	//Note that the current update on generation_limit may cause the program run for a long long time
	// if the result improves with a huge step at large gen number
	//******************************************************************************************************************** 
	int generation_limit = _groupNum*_groupNum*100;
	int sex_limit = _groupNum; // don't let the same couple have sex too many times if their children are rubbish XD
	int sex_time = 0;
	for(int gen=0;gen<generation_limit;gen++){
		if( (sex_time <= sex_limit) && (F_record != M_record) ){
			for(int off=0;off<3;off++,sex_time++){
				RandomNumGen rngtime(time(0)+gen+off);
				vector<size_t> C = sex(F,M,_groupNum,rngtime(_groupNum),rngtime(2));
				result = simulate(C);
				if(result < min(F_record, M_record)){
					sex_time = 0; // child proves to be good, let parents have more sex
					if(rng(2)){
						//cout<<"child replaces father at generation = "<<gen<<"    offspring = "<<off<<"   density = "<<result<<endl;
						F = C;
						F_record = result;
					}
					else{
						//cout<<"child replaces mother at generation = "<<gen<<"    offspring = "<<off<<"   density = "<<result<<endl;
						M = C;
						M_record = result;
					}
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
		result = simulate(S);
		if(result < F_record){
			sex_time = 0; // new partner, resets available sex time
			generation_limit += gen*(F_record-result);
			F_record = result;
			//cout<<"stranger replaces father at generation = "<<gen<<"   density = "<<result<<"\r"<<endl;
			F=S;
		}
		else if(result < M_record){
			sex_time = 0; // new partner, resets available sex time
			generation_limit += gen*(M_record-result);
			M_record = result;
			//cout<<"stranger replaces mother at generation = "<<gen<<"   density = "<<result<<"\r"<<endl;
			M=S;
		}
		cout<<"(F,M) = ( "<<setw(8)<<F_record<<" , "<<setw(8)<<M_record<<" )";
		cout<<"\t(gen / gen_limit) = ("<<setw(8)<<gen<<" / "<<setw(8)<<generation_limit<<" )\r";

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

