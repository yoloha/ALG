#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "rnGen.h"
using namespace std;
#define MAX ((size_t)-1)
extern void printGene(const vector<size_t>& );
extern vector<size_t> sex(vector<size_t>,vector<size_t>,int,int,int);
vector<size_t> sex(vector<size_t> F,vector<size_t> M,int groupNum,int index,int sel){
	int num=(groupNum%64==0)? groupNum/64 : groupNum/64+1;
	size_t mask=0xFFFFFFFFFFFFFFFF<<(index%64);
	vector<size_t> C;
	for(int i=0;i<num;i++){
		if(i<index/64){
			if(sel)
				C.push_back(M[i]);
			else
				C.push_back(F[i]);
		}
		else if(i==index/64){
			if(sel)
				C.push_back(((F[i]&mask)|(M[i]&(~mask))));
			else
				C.push_back(((F[i]&(~mask))|(M[i]&mask)));
		}
		else{
			if(sel)
				C.push_back(F[i]);
			else
				C.push_back(M[i]);
		}
	}
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