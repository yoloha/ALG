#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "rnGen.h"
using namespace std;
#define MAX ((size_t)-1)
extern vector<size_t> evolution(vector<size_t>& ,vector<size_t>& );
extern void printGene(const vector<size_t>& );
vector<size_t> sex(vector<bool>,vector<bool>,int,int,int);
vector<size_t> evolution(vector<size_t> &F,vector<size_t> &M,const int groupNum,RandomNumGen &rng){
	int num=(groupNum%64==0)? groupNum/64 : groupNum/64+1;
	size_t temp;
	vector<bool> Fbits,Mbits;
	Fbits.resize(num*64,0);
	Mbits.resize(num*64,0);
	for(int i=0;i<num;i++){
		temp=F[i];
		for(int j=0;j<64;j++){
			if(temp>=1){
				Fbits[i*64+j]=temp%2;
				temp/=2;
			}
			else
				Fbits[i*64+j]=0;
		}
	}
	for(int i=0;i<num;i++){
		temp=M[i];
		for(int j=0;j<64;j++){
			if(temp>=1){
				Mbits[i*64+j]=temp%2;
				temp/=2;
			}
			else
				Mbits[i*64+j]=0;
		}
	}
	return sex(Fbits,Mbits,groupNum,rng(groupNum),rng(2));
}
vector<size_t> sex(vector<bool> Fbits,vector<bool> Mbits,int groupNum,int index,int sel){
	int num=(groupNum%64==0)? groupNum/64 : groupNum/64+1;
	vector<size_t> C;
	vector<bool> Cbits;
	Cbits.resize(num*64,0);
	for(int i=0;i<num;i++){
		for(int j=0;j<64;j++){
			if(i*64+j >= index)
				Cbits[i*64+j]=(sel)?Fbits[i*64+j]:Mbits[i*64+j];
			else
				Cbits[i*64+j]=(sel)?Mbits[i*64+j]:Fbits[i*64+j];
		}
	}
	for(int i=0;i<num;i++){
		size_t temp=0,add=1;
		for(int j=0;j<64;j++){
			if(Cbits[i*64+j])
				temp += add;
			add *= 2;
		}
		C.push_back(temp);
	}
	//cout<<"\t\tindex="<<index<<"   sel="<<sel<<endl;
	return C;
}
void printGene(const vector<size_t> &gene,const int groupNum){
	size_t temp;
	vector<bool> genebits;
	genebits.resize(gene.size()*64,0);
	for(size_t i=0;i<gene.size();i++)
		cout<<"\tsize_t #"<<i<<":"<<gene[i]<<endl;
	for(size_t i=0;i<gene.size();i++){
		temp=gene[i];
		for(int j=0;j<64;j++){
			if(temp>=1){
				genebits[i*64+j]=temp%2;
				temp/=2;
			}
			else
				genebits[i*64+j]=0;
		}
	}
	for(int i=groupNum;i>=0;i--)
		cout<<genebits[i];
	cout<<endl;
}