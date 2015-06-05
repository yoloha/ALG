#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

#define MASK(i)  (size_t (1)<<(i))

extern size_t BruteForceSim(vector<vector<int> >& );
extern int vecOneNorm(const vector<int> &);
void printMatrix(ostream&, vector<vector <int> >);
template<class T> void printVector(ostream&, vector<T>);

struct permuMap
{
	vector<int>* key;
	int index;
	friend ostream& operator << (ostream& os, const permuMap p)
	{
		os<<" ( "<<vecOneNorm(*(p.key))<<" , "<<p.index<<" ) ";
		return os;
	}
};

struct Greater
{
	bool operator() (const vector<int>& lhs, const vector<int> & rhs)
	{
		return vecOneNorm(lhs) > vecOneNorm(rhs);
	}
	bool operator() (const permuMap & lhs, const permuMap & rhs)
	{
		return vecOneNorm(*(lhs.key))> vecOneNorm(*(rhs.key));
	}
};




int vecOneNorm(const vector<int>& v)
{
	int norm = 0 ;
	for(size_t i = 0 ; i < v.size() ; ++i)
		norm+=abs(v[i]);
	return norm;
}

void printMatrix(ostream& os , const vector<vector<int> > matrix)
{
	for(size_t j = 0 ; j < matrix[0].size() ; ++j){
		for(size_t i = 0 ; i < matrix.size() ; ++i)
			os<<setw(7)<<matrix[i][j];
		os<<endl;
	}
}

template<class T>
void printVector(ostream& os, const vector<T> v)
{
	for(size_t i = 0 ; i < v.size() ; ++i)
		os<<v[i]<<endl;
}



size_t BruteForceSim (vector<vector<int> >& G)  
{
	if(G.empty()) return 0 ;
	size_t simVal=0,cnt;
	size_t groupNum = G.size(), winNum = G[0].size(), minSim = 0,index;
	int minNorm = 0;
	vector<int> *tmpV;
	vector<permuMap> GMap;
	tmpV = new vector<int> (G[0].size(),0);
	
	Greater greater;

	//Building Map
	for(size_t i = 0 ; i < groupNum ; ++i){
		permuMap temp;
		temp.key = &G[i];
		temp.index = i;
		GMap.push_back(temp);
	}
	printVector(cout,GMap);
	cout<<endl;
	//Sorting matrix G
	sort(GMap.begin(), GMap.end(), greater);
	sort(G.begin(), G.end(), greater);
	printMatrix(cout, G);

	// initial min area value
	for(size_t i = 0 ; i < groupNum ; ++i){
		for(size_t j = 0 ; j < winNum ; ++j){
			(*tmpV)[j] -= G[i][j];
			G[i][j] = 2*G[i][j];
		} 
	}
	cout<<endl;
	printVector(cout,*tmpV);
	cout<<endl;
	minNorm = vecOneNorm(*tmpV);
	cout<<"initial norm : "<<vecOneNorm(*tmpV)<<endl;
	size_t j;

	for(size_t i = 1 ; i < MASK(groupNum) ; ++i){
		index = 0;
		cnt = i;
		while((cnt&MASK(0))==0){
			++index;
			cnt = cnt >> 1;
		}
		simVal = simVal^MASK(index);

		if((cnt&size_t(3))==1){
			for(j = 0 ; j < tmpV->size(); ++j)
				(*tmpV)[j] = (*tmpV)[j] + G[index][j];
			if(minNorm>vecOneNorm(*tmpV)) {
				minNorm = vecOneNorm(*tmpV);
				minSim = simVal;	
				bitset<64> x(simVal);
				cout<<x<<endl;	
				cout<<minNorm<<endl;		
			}
		}
		else if((cnt&size_t(3))==3){
			for(j = 0 ; j < tmpV->size(); ++j)
				(*tmpV)[j] = (*tmpV)[j] - G[index][j];
			if(minNorm>vecOneNorm(*tmpV)) {
				minNorm = vecOneNorm(*tmpV);
				minSim = simVal;
				bitset<64> x(simVal);
				cout<<x<<endl;
				cout<<minNorm<<endl;
			}	
		}
	}
	printVector(cout,GMap);
	bitset<64> x(minSim);
	cout<<x<<endl;

	//perute back
	size_t ret = 0;
	for (size_t i = 0 ; i < GMap.size(); ++i){
		if((simVal&MASK(i))==1) ret = ret | MASK(GMap[i].index);
	}
	
	return simVal;
}

