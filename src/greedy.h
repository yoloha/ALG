#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

void greedyN(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT);
void greedyN2(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT);
void greedyN_oneEntry(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT);

struct GrpSimMap;
bool greaterSum(const GrpSimMap&, const GrpSimMap&);
bool greaterOneNorm(const GrpSimMap&, const GrpSimMap&);
bool greaterOneEntry(const GrpSimMap&, const GrpSimMap&);
int vectorSum(const vector<int>&);
int vectorOneNorm(const vector<int>&);
void vectorKey(const vector<int>&, int& key, int& keyId);
void findMax2Front(vector<GrpSimMap>& matrix);
void changeGrpColorInSim(vector<size_t>& sim, const int& grpPos);
void printMatrixTMap(ostream&, const vector<GrpSimMap>&);

struct GrpSimMap
{
	vector<int> _index;
	vector<int> _grpInWins;
	
	// for one entry
	int         _key;
	int         _keyId;
};

bool
greedySim(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT, const int& method)
{
	switch(method) {
		case 1:
			greedyN(sim, areaMatrixT);
			break;
		case 2:
			greedyN2(sim, areaMatrixT);
			break;
		case 3:
			greedyN_oneEntry(sim, areaMatrixT);
			break;
		default:
			return false;
	}

	return true;
}

void
greedyN(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT)
{
	vector<GrpSimMap> areaMatrixTMap;

	for (int i = 0, l = areaMatrixT.size(); i < l; i++) {
		GrpSimMap tmp;
		tmp._index.push_back(i);
		tmp._grpInWins = areaMatrixT[i];
		areaMatrixTMap.push_back(tmp);
	}

	//printMatrixTMap(cout, areaMatrixTMap);

	//sort(areaMatrixTMap.begin(), areaMatrixTMap.end(), greaterSum);
	sort(areaMatrixTMap.begin(), areaMatrixTMap.end(), greaterOneNorm);

	vector<size_t> newSim(sim.size(), 0);
	vector<int> totalAreaInWins(areaMatrixT[0].size(), 0);

	//printMatrixTMap(cout, areaMatrixTMap);

	/*
	for (int i = 0, l = areaMatrixTMap.size(); i < l; i++) {
		if (vectorSum(totalAreaInWins) > 0) {
			for (int j = 0, m = areaMatrixTMap[i]._grpInWins.size(); j < m; j++)
				totalAreaInWins[j] -= areaMatrixTMap[i]._grpInWins[j];
		}
		else {
			for (int j = 0, m = areaMatrixTMap[i]._grpInWins.size(); j < m; j++)
				totalAreaInWins[j] += areaMatrixTMap[i]._grpInWins[j];
			changeGrpColorInSim(newSim, areaMatrixTMap[i]._index[0]);
		}
		//cout << "sum   = " << vectorSum(totalAreaInWins) << endl;
	}
	*/
	for (int i = 0, l = areaMatrixTMap.size(); i < l; i++) {
		vector<int> tmp1(areaMatrixT[0].size(), 0);
		for (int j = 0, m = areaMatrixTMap[i]._grpInWins.size(); j < m; j++)
			tmp1[j] = totalAreaInWins[j] + areaMatrixTMap[i]._grpInWins[j];
		vector<int> tmp2(areaMatrixT[0].size(), 0);
		for (int j = 0, m = areaMatrixTMap[i]._grpInWins.size(); j < m; j++)
			tmp2[j] = totalAreaInWins[j] - areaMatrixTMap[i]._grpInWins[j];
		if (vectorOneNorm(tmp1) > vectorOneNorm(tmp2))
			totalAreaInWins = tmp2;
		else {
			totalAreaInWins = tmp1;
			changeGrpColorInSim(newSim, areaMatrixTMap[i]._index[0]);
		}
		//cout << "sum   = " << vectorSum(totalAreaInWins) << endl;
	}
	sim = newSim;
	//cout << "area  = " << vectorOneNorm(totalAreaInWins) << endl;
	//cout << "total = " << (double)vectorOneNorm(totalAreaInWins) / (Window::getOmega() * Window::getOmega()) * 100 << endl;
}

void
greedyN2(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT)
{
	vector<GrpSimMap> areaMatrixTMap;

	for (int i = 0, l = areaMatrixT.size(); i < l; i++) {
		GrpSimMap tmp;
		tmp._index.push_back(i);
		tmp._grpInWins = areaMatrixT[i];
		areaMatrixTMap.push_back(tmp);
	}

	sort(areaMatrixTMap.begin(), areaMatrixTMap.end(), greaterOneNorm);

	vector<size_t> newSim(sim.size(), 0);
	while (areaMatrixTMap.size() > 1) {
		int b = 1;
		vector<int>	newGrpInWins(areaMatrixTMap[0]._grpInWins.size(), 0);
		for (int j = 0, m = areaMatrixTMap[0]._grpInWins.size(); j < m; j++)
			newGrpInWins[j] = areaMatrixTMap[0]._grpInWins[j] - areaMatrixTMap[1]._grpInWins[j];
	   	int totalArea = vectorOneNorm(newGrpInWins);

		for (int i = 2, l = areaMatrixTMap.size(); i < l; i++) {
			for (int j = 0, m = areaMatrixTMap[0]._grpInWins.size(); j < m; j++)
				newGrpInWins[j] = areaMatrixTMap[0]._grpInWins[j] - areaMatrixTMap[i]._grpInWins[j];
			int sum = vectorOneNorm(newGrpInWins);
			if (totalArea > sum) {
				b = i;
				totalArea = sum;
			}
		}
		int x = areaMatrixTMap[0]._index.size();
		if (x == 1) changeGrpColorInSim(newSim, areaMatrixTMap[0]._index[0]);
		int y = areaMatrixTMap[b]._index.size();
		if (y > 1) {
			for (int i = 0; i < y; i++)
				changeGrpColorInSim(newSim, areaMatrixTMap[b]._index[i]);
		}
			
		GrpSimMap newGrpSimMap;
		for (int i = 0; i < x; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[0]._index[i]);
		for (int i = 0; i < y; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[b]._index[i]);
		for (int i = 0, l = areaMatrixTMap[0]._grpInWins.size(); i < l; i++)
			newGrpInWins[i] = areaMatrixTMap[0]._grpInWins[i] - areaMatrixTMap[b]._grpInWins[i];
		newGrpSimMap._grpInWins = newGrpInWins;

		areaMatrixTMap[0] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();
		if (b == (int)areaMatrixTMap.size())
			areaMatrixTMap[0] = areaMatrixTMap.back();
		else
			areaMatrixTMap[b] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();

		areaMatrixTMap.push_back(newGrpSimMap);
	}
	/*
	vector<size_t> newSim(sim.size(), 0);
	while (areaMatrixTMap.size() > 1) {
		int a = 1, b = 0;
		vector<int>	newGrpInWins(areaMatrixTMap[0]._grpInWins.size(), 0);
		for (int i = 0, n = areaMatrixTMap[0]._grpInWins.size(); i < n; i++)
			newGrpInWins[i] = areaMatrixTMap[1]._grpInWins[i] - areaMatrixTMap[0]._grpInWins[i];
	   	int totalArea = vectorOneNorm(newGrpInWins);

		for (int i = 2, l = areaMatrixTMap.size(); i < l; i++) {
			for (int j = 0; j < i; j++) {
				for (int k = 0, n = areaMatrixTMap[0]._grpInWins.size(); k < n; k++)
					newGrpInWins[k] = areaMatrixTMap[i]._grpInWins[k] - areaMatrixTMap[j]._grpInWins[k];
				int sum = vectorOneNorm(newGrpInWins);
				if (totalArea > sum) {
					a = i; b = j;
					totalArea = sum;
				}
			}
		}
		int x = areaMatrixTMap[a]._index.size();
		if (x == 1) changeGrpColorInSim(newSim, areaMatrixTMap[a]._index[0]);
		int y = areaMatrixTMap[b]._index.size();
		if (y > 1) {
			for (int i = 0; i < y; i++)
				changeGrpColorInSim(newSim, areaMatrixTMap[b]._index[i]);
		}
			
		GrpSimMap newGrpSimMap;
		for (int i = 0; i < x; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[a]._index[i]);
		for (int i = 0; i < y; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[b]._index[i]);
		for (int i = 0, l = areaMatrixTMap[0]._grpInWins.size(); i < l; i++)
			newGrpInWins[i] = areaMatrixTMap[a]._grpInWins[i] - areaMatrixTMap[b]._grpInWins[i];
		newGrpSimMap._grpInWins = newGrpInWins;

		areaMatrixTMap[a] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();
		if (b == (int)areaMatrixTMap.size())
			areaMatrixTMap[a] = areaMatrixTMap.back();
		else
			areaMatrixTMap[b] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();

		areaMatrixTMap.push_back(newGrpSimMap);
	}
	*/
	sim = newSim;
}

void
greedyN_oneEntry(vector<size_t>& sim, const vector< vector<int> >& areaMatrixT)
{
	vector<GrpSimMap> areaMatrixTMap;

	for (int i = 0, l = areaMatrixT.size(); i < l; i++) {
		GrpSimMap tmp;
		tmp._index.push_back(i);
		tmp._grpInWins = areaMatrixT[i];
		vectorKey(tmp._grpInWins, tmp._key, tmp._keyId);
		areaMatrixTMap.push_back(tmp);
	}

	//printMatrixTMap(cout, areaMatrixTMap);

	vector<size_t> newSim(sim.size(), 0);
	vector<int> totalAreaInWins(areaMatrixT[0].size(), 0);

	while (areaMatrixTMap.size() > 1) {
		findMax2Front(areaMatrixTMap);
		int id = areaMatrixTMap[0]._keyId;
		int min = areaMatrixTMap[0]._key - abs(areaMatrixTMap[1]._grpInWins[id]);
		int minIndex = 1;
		for (int i = 2, l = areaMatrixTMap.size(); i < l; i++) {
			if (min > areaMatrixTMap[0]._key - abs(areaMatrixTMap[i]._grpInWins[id])) {
				min = min > areaMatrixTMap[0]._key - abs(areaMatrixTMap[i]._grpInWins[id]);
				minIndex = i;
			}
		}
		
		vector<int>	newGrpInWins(areaMatrixTMap[0]._grpInWins.size(), 0);
		int x = areaMatrixTMap[0]._index.size();
		int y = areaMatrixTMap[minIndex]._index.size();
		if (areaMatrixTMap[0]._grpInWins[id] * areaMatrixTMap[minIndex]._grpInWins[id] > 0) {
			for (int i = 0, n = areaMatrixTMap[0]._grpInWins.size(); i < n; i++)
				newGrpInWins[i] = areaMatrixTMap[0]._grpInWins[i] - areaMatrixTMap[minIndex]._grpInWins[i];
			
			if (x == 1) changeGrpColorInSim(newSim, areaMatrixTMap[0]._index[0]);
			if (y > 1) {
				for (int i = 0; i < y; i++)
					changeGrpColorInSim(newSim, areaMatrixTMap[minIndex]._index[i]);
			}
		}
		else {
			for (int i = 0, n = areaMatrixTMap[0]._grpInWins.size(); i < n; i++)
				newGrpInWins[i] = areaMatrixTMap[0]._grpInWins[i] + areaMatrixTMap[minIndex]._grpInWins[i];
			
			if (x == 1) changeGrpColorInSim(newSim, areaMatrixTMap[0]._index[0]);
			if (y == 1) changeGrpColorInSim(newSim, areaMatrixTMap[minIndex]._index[0]);
		}

			
		GrpSimMap newGrpSimMap;
		for (int i = 0; i < x; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[0]._index[i]);
		for (int i = 0; i < y; i++)
			newGrpSimMap._index.push_back(areaMatrixTMap[minIndex]._index[i]);
		newGrpSimMap._grpInWins = newGrpInWins;
		vectorKey(newGrpSimMap._grpInWins, newGrpSimMap._key, newGrpSimMap._keyId);

		areaMatrixTMap[0] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();
		if (minIndex == (int)areaMatrixTMap.size())
			areaMatrixTMap[0] = areaMatrixTMap.back();
		else	
			areaMatrixTMap[minIndex] = areaMatrixTMap.back();
		areaMatrixTMap.pop_back();

		areaMatrixTMap.push_back(newGrpSimMap);
	}
	sim = newSim;
}

bool 
greaterSum(const GrpSimMap& lhs, const GrpSimMap& rhs)
{
	if (vectorSum(lhs._grpInWins) != vectorSum(rhs._grpInWins))
		return vectorSum(lhs._grpInWins) > vectorSum(rhs._grpInWins);
	else {
		for (int i = 0, l = lhs._grpInWins.size(); i < l; i++) {
			if (lhs._grpInWins[i] != rhs._grpInWins[i])
				return lhs._grpInWins[i] > rhs._grpInWins[i];
		}
		return false;
	}
}

bool 
greaterOneNorm(const GrpSimMap& lhs, const GrpSimMap& rhs)
{
	if (vectorOneNorm(lhs._grpInWins) != vectorOneNorm(rhs._grpInWins))
		return vectorOneNorm(lhs._grpInWins) > vectorOneNorm(rhs._grpInWins);
	else {
		for (int i = 0, l = lhs._grpInWins.size(); i < l; i++) {
			if (lhs._grpInWins[i] != rhs._grpInWins[i])
				return lhs._grpInWins[i] > rhs._grpInWins[i];
		}
		return false;
	}
}

bool 
greaterOneEntry(const GrpSimMap& lhs, const GrpSimMap& rhs)
{
	if (lhs._key != rhs._key)
		return lhs._key > rhs._key;
	else {
		if (vectorOneNorm(lhs._grpInWins) != vectorOneNorm(rhs._grpInWins))
			return vectorOneNorm(lhs._grpInWins) > vectorOneNorm(rhs._grpInWins);
		else {
			for (int i = 0, l = lhs._grpInWins.size(); i < l; i++) {
				if (lhs._grpInWins[i] != rhs._grpInWins[i])
					return lhs._grpInWins[i] > rhs._grpInWins[i];
			}
			return false;
		}
	}
}

int 
vectorSum(const vector<int>& vec)
{
	int sum = 0;
	for (int i = 0, l = vec.size(); i < l; i++)
		sum += vec[i];
	return sum;
}

int 
vectorOneNorm(const vector<int>& vec)
{
	int sum = 0;
	for (int i = 0, l = vec.size(); i < l; i++)
		sum += abs(vec[i]);
	return sum;
}

void
vectorKey(const vector<int>& vec, int& key, int& keyId)
{
	key = 0;
	keyId = -1;
	for (int i = 0, l = vec.size(); i < l; i++) {
		if (key < abs(vec[i])) {
			key = abs(vec[i]);
			keyId = i;
		}
	}
}

void
findMax2Front(vector<GrpSimMap>& matrix)
{
	for (int i = 1, l = matrix.size(); i < l; i++) {
		if (greaterOneEntry(matrix[i], matrix[0])) {
			GrpSimMap tmp = matrix[0];
			matrix[0] = matrix[i];
			matrix[i] = tmp;
		}
	}
}

void
changeGrpColorInSim(vector<size_t>& sim, const int& grpPos)
{
	int i = grpPos / SIMLEN;
	int p = grpPos % SIMLEN;

	size_t mask = size_t(1 << p);
	sim[i] ^= mask;
}
	
void
printMatrixTMap(ostream& os, const vector<GrpSimMap>& matrix)
{
	if(matrix.empty()) return;

	for (size_t j = 0, l = matrix[0]._grpInWins.size(); j < l; j++) {
		for (size_t i = 0, m = matrix.size(); i < m; i++)
			os << setw(8) << matrix[i]._grpInWins[j];
		os << endl;
	}
	os << endl;
	for(size_t i = 0 ; i < matrix.size() ; ++i)
		os << setw(8) << vectorSum(matrix[i]._grpInWins);
	os << endl;
	for(size_t i = 0 ; i < matrix.size() ; ++i)
		os << setw(8) << vectorOneNorm(matrix[i]._grpInWins);
	os << endl << endl;
}
