#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
#include <iomanip>
#include <stdio.h>

using namespace std;

#define MASK(i)  (size_t (1)<<(i))

int globalcounter = 0;

size_t BruteForceSim(vector<vector<int> >& );
bool BruteForceSim (vector<vector<int> >&G, int& minNorm , size_t& minSim , vector<int>* offset = NULL) ;
int vecOneNorm(const vector<int> &);
bool optimalSim(vector<size_t>&, vector<vector<int> >&);
void printMatrix(ostream&, vector<vector <int> >);
template<class T> void printVector(ostream&, vector<T>);
template<class T> void printRowVector(ostream&, vector<T>);


/*Matrix Operation*/
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
		if(vecOneNorm(lhs)!=vecOneNorm(rhs))
			return vecOneNorm(lhs) > vecOneNorm(rhs);
		for(size_t i = 0 ; i < lhs.size(); ++i){
			if(lhs[i]!=rhs[i])
				return lhs[i] > rhs[i];
		}
		return false;
	}
	bool operator() (const permuMap & lhs, const permuMap & rhs)
	{
		if(vecOneNorm(*(lhs.key))!=vecOneNorm(*(rhs.key))) 
			return vecOneNorm(*(lhs.key))> vecOneNorm(*(rhs.key));
		for(size_t i = 0 ; i < lhs.key->size(); ++i){
			if((*lhs.key)[i]!=(*rhs.key)[i])
				return (*lhs.key)[i] > (*rhs.key)[i];
		}
		return false;
	}
};

struct vecBucket
{
	vector<int> vec;
	int num;
	int pos;
	friend ostream& operator << (ostream& os ,const vecBucket& v)
	{
		os<<v.num<<endl<<endl;
		//printVector(os,v.vec);
		//os<<endl;
		return os;	
	}
};

struct vectorCounter
{
	vector<vecBucket > *bucketPtr;
	vector<int> key;
	vector<int> pos;
	vector<int> tmpIdx;
	vector<int>* ret;

	void init_vecCnt(vector<vecBucket>* _bucketPtr)
	{
		bucketPtr = _bucketPtr;
		for(size_t i = 0 ; i < (*_bucketPtr).size(); ++i){
			key.push_back ((*_bucketPtr)[i].num);
			pos.push_back((*_bucketPtr)[i].pos);
		}
		tmpIdx.resize(key.size(),0);
		ret = new vector<int>((*_bucketPtr)[0].vec.size(),0);
	}

	bool operator ++ (int)
	{
		size_t i = 0;
		while(tmpIdx[i]>=key[i]){
			++i;
			if(i == key.size()) return false;
		}
		++tmpIdx[i];
		for(size_t j = 0 ; j < i ; ++j)
			tmpIdx[j] = 0;
		return true;
	}

	vector<int>& operator()()
	{
		for (size_t j = 0; j < ((*bucketPtr)[0]).vec.size(); ++j)
				(*ret)[j]=0;
		for(size_t i = 0 ; i < key.size(); ++i){
			int coeff = 2*tmpIdx[i] - key[i];
			for (size_t j = 0; j < ((*bucketPtr)[0]).vec.size(); ++j){
				(*ret)[j]+=coeff*((*bucketPtr)[i].vec)[j];
			}
		}
		return  *ret;
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
	if(matrix.empty()) return;
	for(size_t j = 0 ; j < matrix[0].size() ; ++j){
		for(size_t i = 0 ; i < matrix.size() ; ++i)
			os<<setw(7)<<matrix[i][j];
		os<<endl;
	}
	os<<endl;
	for(size_t i = 0 ; i < matrix.size() ; ++i)
		os<<setw(7)<<vecOneNorm(matrix[i]);
	os<<endl<<endl;
}

template<class T>
void printVector(ostream& os, const vector<T> v)
{
	for(size_t i = 0 ; i < v.size() ; ++i)
		os<<v[i]<<endl;
}

template<class T>
void printRowVector(ostream& os, const vector<T> v)
{	
	os<<"(";
	for(size_t i = 0 ; i < v.size() ; ++i)
		os<<setw(3)<<v[i]<<",";
	os<<char(8)<<")";
}

vector<vector< int> > subMatrix(vector<int>& index, vector<vector<int> > matrix)
{
	vector<vector<int> > ret;
	for(size_t i = 0 ; i < index.size(); ++i)
		ret.push_back(matrix[index[i]]);
	return ret;
}

/*Optimize Method*/
bool optimalSim(vector<size_t> & sim, vector<vector<int> > & G)
{
	Greater greater;
	size_t simResult;
	vector<permuMap> GMap;
	vector<int> submatrixIdx;

	//Building Map
	for(size_t i = 0 ; i < G.size() ; ++i){
		permuMap temp;
		temp.key = &G[i];
		temp.index = i;
		GMap.push_back(temp);
	}
	//Sorting matrix G
	sort(GMap.begin(), GMap.end(), greater);
	sort(G.begin(), G.end(), greater);
	cout<<"Primal Matrix : "<<endl;
	printMatrix(cout, G);

	//Parsing Matrix
	vector<vecBucket> _buckets;
	vecBucket tempVec = {G[0],1,0};
	for(size_t i = 0 ; i < G.size(); ++i){
		if(i > 0){
			if(G[i]==G[i-1]) ++tempVec.num;
			else{
				if(tempVec.num >= 3){
					_buckets.push_back(tempVec);
					for(int j = 0 ; j < tempVec.num ; ++j) submatrixIdx.pop_back();
				}
				tempVec.num = 1;
				tempVec.vec = G[i];
				tempVec.pos = i;
			}
		}
		submatrixIdx.push_back(i);
	}
	if(tempVec.num >= 3){
		_buckets.push_back(tempVec);
		for(int j = 0 ; j < tempVec.num ; ++j) submatrixIdx.pop_back();
	}
	
	

	if(!_buckets.empty()){
		vectorCounter vCnt;
		vCnt.init_vecCnt(&_buckets);
		vector<vector<int> > reducedMatrix;
		reducedMatrix = subMatrix(submatrixIdx,G);
		vector<int> minCntSimResult;

		cout<<"Reduced Matrix (extracted repetitive colum) : "<<endl;
		printMatrix(cout,reducedMatrix);
		cout<<"reducedMatrix Index : "<<endl;
		printVector(cout,submatrixIdx);
		cout<<"repetitive colum vector number : "<<endl;
		printRowVector(cout,vCnt.key);

		int minNorm = MASK(30);
		while(vCnt++){
			
			if(BruteForceSim(reducedMatrix,minNorm, simResult, &vCnt())){
				cout<<endl<<endl<<"Find and Update Minimum !!"<<endl;
				minCntSimResult = vCnt.tmpIdx;
			}
			//BruteForceSim(reducedMatrix);

			for(int i = 0 ; i < 80 ; ++i) cout<<'\r';
			fflush(stdout);
			cout<<"Simulation Progress : # ";
			printRowVector(cout,vCnt.tmpIdx);
			cout<<"/";
			printRowVector(cout,vCnt.key);
		}
		cout<<endl<<"Total minCntSimResult"<<endl;
		printRowVector(cout,minCntSimResult);
		cout<<endl;
		cout<<"Minimum Value Appear Times : "<<globalcounter<<endl;
		//permute back
		for(size_t i = 0 ; i <submatrixIdx.size() ; ++i){
			if((simResult&MASK(i))!=0){
				sim[GMap[submatrixIdx[i]].index/64] = sim[GMap[submatrixIdx[i]].index/64] | MASK(GMap[submatrixIdx[i]].index%64);
			}
		}

		for(size_t i = 0 ; i < vCnt.key.size() ; ++i){
			for(int j = 0 ; j < minCntSimResult[i] ; ++j){
				sim[GMap[(vCnt.pos)[i]+j].index/64] = sim[GMap[(vCnt.pos)[i]+j].index/64] | MASK(GMap[(vCnt.pos)[i]+j].index%64);
			}
		}
	}
	
	else {
		simResult = BruteForceSim(G);
		//permute back
		sim[0] = 0;
		for (size_t i = 0 ; i < GMap.size(); ++i){
			if((simResult&MASK(i))!=0) sim[0] = sim[0] | MASK(GMap[i].index);
		}
			//printInfo
		cout<<"After permute : "<<endl;
		bitset<64> z(sim[0]);
		cout<<z<<endl;
	}

	
	

	
	return true;
}


bool BruteForceSim (vector<vector<int> >&G, int& minNorm , size_t& minSim , vector<int>* offset )  
{
	if(G.empty()) {
		if(minNorm > vecOneNorm(*offset)){
			minNorm = vecOneNorm(*offset);
			return true;
		}
		return false ;
	}
	size_t simVal=0,cnt,j;
	size_t groupNum = G.size(), winNum = G[0].size(),index;
	vector<int> *tmpV ;
	bool ret = false;

	tmpV = new vector<int> (G[0].size(),0);
	if(offset!=NULL) *tmpV = *offset;

	// initial min area value
	for(size_t i = 0 ; i < groupNum ; ++i){
		for(size_t j = 0 ; j < winNum ; ++j){
			(*tmpV)[j] -= G[i][j];
		} 
	}

	//cout<<"initial norm : "<<vecOneNorm(*tmpV)<<endl;

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
				(*tmpV)[j] = (*tmpV)[j] + 2*G[index][j];
			if(minNorm>=vecOneNorm(*tmpV)) {
				minNorm = vecOneNorm(*tmpV);
				if(minNorm == 254383) {
					globalcounter++;
					cout<<endl<<globalcounter<<endl;
					bitset<64> x(simVal);
					cout<<endl<<"Temporary Simulation Value : "<<x<<endl;
					cout<<"Temporary Area Difference  : "<<minNorm;	
				}
				minSim = simVal;
				ret = true;
				//printInfo	
				bitset<64> x(simVal);
				//cout<<endl<<"Temporary Simulation Value : "<<x<<endl;
				//cout<<"Temporary Area Difference  : "<<minNorm;		
			}
		}
		else if((cnt&size_t(3))==3){
			for(j = 0 ; j < tmpV->size(); ++j)
				(*tmpV)[j] = (*tmpV)[j] - 2*G[index][j];
			if(minNorm>=vecOneNorm(*tmpV)) {
				minNorm = vecOneNorm(*tmpV);
				if(minNorm == 254383) {
					globalcounter++;
					cout<<endl<<globalcounter<<endl;
					bitset<64> x(simVal);
					cout<<endl<<"Temporary Simulation Value : "<<x<<endl;
					cout<<"Temporary Area Difference  : "<<minNorm;	
				}
				minSim = simVal;
				ret = true;
				//printInfo
				bitset<64> x(simVal);
				//cout<<endl<<"Temporary Simulation Value : "<<x<<endl;
				//cout<<"Temporary Area Difference  : "<<minNorm;	
			}	
		}
	}
	return ret;
}

size_t BruteForceSim(vector<vector<int> >&  G)
{
	if(G.empty()) return 0 ;
	size_t simVal=0,cnt;
	size_t groupNum = G.size(), winNum = G[0].size(),index,minSim = 0;
	int minNorm = 0;
	vector<int> *tmpV;
	tmpV = new vector<int> (G[0].size(),0);

	// initial min area value
	for(size_t i = 0 ; i < groupNum ; ++i){
		for(size_t j = 0 ; j < winNum ; ++j){
			(*tmpV)[j] -= G[i][j];
			G[i][j] = 2*G[i][j];
		} 
	}
	/*
	cout<<endl;
	printVector(cout,*tmpV);
	cout<<endl;
	*/
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
				//printInfo	
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
				//printInfo
				bitset<64> x(simVal);
				cout<<x<<endl;
				cout<<minNorm<<endl;
			}	
		}
	}
	return minSim;
}

/*
vector<size_t> greedyDimReduce(vector<vector<int> >G, int n )//use greedy method to reduce dimension to n
{
	int cnt = n;
	vector<size_t> simVal;
	while(cnt>0){
		cnt = cnt/64;
		simVal.push_back(0);
	}
	if(G.size()<n) return  simVal;
}
*/

