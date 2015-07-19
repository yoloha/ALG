#include "colorBalancing.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>

bool optimalSim(vector<size_t>&, vector<vector<int> >&);
int vecOneNorm(const vector<int> &);
void printMatrix(ostream &, const vector<vector<int > >);
using namespace std;

//Block{{{
Block::Block() {}

Block::Block(Coordinate _coord)
{
	blockCoord = _coord;
}

Block::~Block() {}

int Block::area()
{
	return (blockCoord.x_right - blockCoord.x_left)*(blockCoord.y_up - blockCoord.y_down);
}
	
int 
Block::area(const Coordinate& windowCoord)
{
	int x_right, x_left, y_up, y_down;
	if (blockCoord.x_right < windowCoord.x_right) x_right = blockCoord.x_right;
	else x_right = windowCoord.x_right;
	if (blockCoord.x_left > windowCoord.x_left) x_left = blockCoord.x_left;
	else x_left = windowCoord.x_left;
	if (blockCoord.y_up < windowCoord.y_up) y_up = blockCoord.y_up;
	else y_up = windowCoord.y_up;
	if (blockCoord.y_down > windowCoord.y_down) y_down = blockCoord.y_down;
	else y_down = windowCoord.y_down;
	
	int width = x_right - x_left;
	int height = y_up - y_down;
	if (width > 0 && height > 0) return width * height;
	else return 0;
}

void Block::addAdjBlock(Block* _block)
{
	adjBlocks.push_back(_block);
}

ostream& operator << (ostream& os, const Block& rhs)
{
	os << rhs.blockCoord.x_left << "," << rhs.blockCoord.y_down << ","
	   << rhs.blockCoord.x_right << "," << rhs.blockCoord.y_up;
	return os;
}
//}}}Block

// Group{{{
Group::Group() {}
Group::~Group() {}

void Group::addA(Block* b)
{
	_blocksA.push_back(b);
}

void Group::addB(Block* b)
{
	_blocksB.push_back(b);
}

void Group::swapAB()
{
	swap(_blocksA,_blocksB);
}
	
void 
Group::setColor(const int& color)
{
	if (color) _color = true;
	else _color = false;
}

int Group::areaA()
{
	int area=0;
	for(size_t i=0;i<_blocksA.size();i++)
		area += _blocksA[i]->area();
	return area;
}
	
int 
Group::areaA(const Coordinate& windowCoord)
{
	int area = 0;
	for (size_t i = 0, n = _blocksA.size(); i < n; i++)
		area += _blocksA[i] -> area(windowCoord);
	return area;
}

int Group::areaB()
{
	int area=0;
	for(size_t i=0;i<_blocksB.size();i++)
		area += _blocksB[i]->area();
	return area;
}

int 
Group::areaB(const Coordinate& windowCoord)
{
	int area = 0;
	for (size_t i = 0, n = _blocksB.size(); i < n; i++)
		area += _blocksB[i] -> area(windowCoord);
	return area;
}

ostream& operator << (ostream& os, const Group& g)
{
	for (int i = 0; i < g._windows.size(); i++)
		os <<"("<< g._windows[i]->getIdx().first <<","<< g._windows[i]->getIdx().second<<") ";
	return os;
}
//}}}Group

//Grid{{{
Grid::Grid() {}
Grid::~Grid() {}

void Grid::addBlock(Block* _block)
{
	for(size_t i = 0 ; i < blocks.size() ; ++i){
		if(blocks[i]==_block) return;
	}
	blocks.push_back(_block);
}
//}}}Grid

//Window{{{
Window::Window() {}

Window::Window(Coordinate _coord)
{
	windowCoord = _coord;
}

Window::~Window() {}

int
Window::omega = 0;

ostream& operator << (ostream& os, const Window& w)
{
	os<<"Window Id : ("<< w.idx.first <<","<< w.idx.second << ")";
	return os;
}

int 
Window::getOmega()
{
	return omega;
}
	
const Coordinate 
Window::getWindowCoord() 
{ 
	return windowCoord;
}

const pair<int,int>
Window::getIdx()
{
	return idx;
}
//}}}Window

/********************* WindowsSet *************************/
WindowsSet::WindowsSet(Window* w) :  _areaDiffSum(size_t(1)<<63)
{
	_windows.push_back(w);
	_groupNum = w -> innerGroup.size();
}
	
void 
WindowsSet::addWindow(Window* w)
{
	_windows.push_back(w);
	_groupNum += w -> innerGroup.size();
}
	
void 
WindowsSet::addCrossGroup(Group* g)
{
	_crossGroup.push_back(g);
	//cout<<"\t"<<*(g->_blocksA[0])<<endl;
	_groupNum++;
	//cout<<"\t windowset pointer: "<<this<<"\tgroup num = "<<_groupNum<<endl;
}
	
void
WindowsSet::initialize()
{
	buildAreaMatrix();
	initSim();
	calWinDensity();
}

void 
WindowsSet::buildAreaMatrix()
{
	
	vector<vector<int> >areaMatrixA,areaMatrixB;
	GrpAreaInWin init;
	init.resize(_groupNum, 0);
	_areaMatrix.resize(_windows.size(), init);
	areaMatrixA.resize(_windows.size(), init);
	areaMatrixB.resize(_windows.size(), init);
	
	int cnt = 0;
	for (int i = 0, l = _windows.size(); i < l; i++) {
		for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; j++, cnt++) {
			_areaMatrix[i][cnt] += _windows[i] -> innerGroup[j] -> areaA();
			_areaMatrix[i][cnt] -= _windows[i] -> innerGroup[j] -> areaB();
		}
	}
	for (int i = 0, l = _crossGroup.size(); i < l; i++, cnt++) {
		for (int j = 0, n = _windows.size(); j < n; j++) {
			Coordinate winCoord = _windows[j] -> getWindowCoord();
			_areaMatrix[j][cnt] += _crossGroup[i] -> areaA(winCoord);
			_areaMatrix[j][cnt] -= _crossGroup[i] -> areaB(winCoord);
	
		}
	}
}

void
WindowsSet::initSim()
{
	int n = _groupNum / SIMLEN + 1;
	_sim.resize(n, 0);
}

double 
//WindowsSet::simulate(const size_t& sim) 
WindowsSet::simulate(const vector<size_t>& sim) 
{
	GrpAreaInWin totalArea;
	totalArea.resize(_areaMatrix.size(), 0);
	for (int i = 0, l = _areaMatrix.size(); i < l ; i++) {
		for (int j = 0; j < _groupNum; j++){
			if (grpColorInSim(sim, j)) totalArea[i] += _areaMatrix[i][j];
			else totalArea[i] -= _areaMatrix[i][j];
		}
	}
	double areaDiffSum = 0;
	for (int i = 0, l = totalArea.size(); i < l; i++)
		areaDiffSum += abs(totalArea[i]);
	if (areaDiffSum < _areaDiffSum) {
		_sim = sim;
		_areaDiffSum = areaDiffSum;
	}
	return areaDiffSum;
}

int
WindowsSet::grpColorInSim(const vector<size_t>& sim, const int& grpPos)
{
	int i = grpPos / SIMLEN;
	int p = grpPos % SIMLEN;
	return (sim[i] >> p) % 2;
}
	
void
WindowsSet::calWinDensity()
{
	vector<vector<int> >areaMatrixA,areaMatrixB;
	GrpAreaInWin init;
	init.resize(_groupNum, 0);
	areaMatrixA.resize(_windows.size(), init);
	areaMatrixB.resize(_windows.size(), init);
	for (int i = 0, l = _windows.size(); i < l; i++) {
		_windows[i] -> densityA = 0;
		_windows[i] -> densityB = 0;
		for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; j++) {
			if (_windows[i] -> innerGroup[j] -> getColor()) {
				_windows[i] -> densityA += _windows[i] -> innerGroup[j] -> areaA();
				_windows[i] -> densityB += _windows[i] -> innerGroup[j] -> areaB();
			}
			else {
				_windows[i] -> densityA += _windows[i] -> innerGroup[j] -> areaB();
				_windows[i] -> densityB += _windows[i] -> innerGroup[j] -> areaA();
			}
		}
	}
	for (int i = 0, l = _crossGroup.size(); i < l; i++) {
		if (_crossGroup[i] -> getColor()) {
			for (int j = 0, m = _crossGroup[i] -> getBlocksANum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksA(j);
				for (int k = 0, n = _windows.size(); k < n; k++) {
					
					_windows[k] -> densityA += block -> area(_windows[k]->windowCoord);
				}
			}
			for (int j = 0, m = _crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = _windows.size(); k < n; k++) {
					
					_windows[k] -> densityB += block -> area(_windows[k]->windowCoord);
				}
			}
		}
		else {
			for (int j = 0, m = _crossGroup[i] -> getBlocksANum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksA(j);
				for (int k = 0, n = _windows.size(); k < n; k++) {
					 
					_windows[k] -> densityB += block -> area(_windows[k]->windowCoord);
				}
			}
			for (int j = 0, m = _crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = _windows.size(); k < n; k++) {
					
					_windows[k] -> densityA += block -> area(_windows[k]->windowCoord);
				}
			}
		}
	}
	double totalDen = 0;
	for (int i = 0, l = _windows.size(); i < l; i++) {
		_windows[i] -> densityA /= (Window::omega * Window::omega) / 100; 
		_windows[i] -> densityB /= (Window::omega * Window::omega) / 100; 
		totalDen += abs(_windows[i] -> densityA - _windows[i] -> densityB);
		cout << "Win " << i << " : " << _windows[i] -> densityA << "," << _windows[i] -> densityB << endl;
	}
	cout << "TOTAL DEN : " << totalDen << endl;
}

void
WindowsSet::updateWinDensity()
{
	int cnt = 0;
	for (int i = 0, l = _windows.size(); i < l; i++) {
		for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; j++, cnt++)
			_windows[i] -> innerGroup[j] -> setColor(grpColorInSim(_sim, cnt));
	}
	for (int i = 0, l = _crossGroup.size(); i < l; i++, cnt++)
		_crossGroup[i] -> setColor(grpColorInSim(_sim, cnt));

	calWinDensity();
}
	
ostream& operator <<(ostream& os,const WindowsSet& w)
{
	os << "Window Number      : " << w._windows.size() << endl
	   << "crossGroup Number  : " << w._crossGroup.size() << endl
	   << "Simulate Value     : "; 
	int n = w._groupNum / SIMLEN + 1;
	for (int i = 0; i < n; i++)
		os << bitset<SIMLEN>(w._sim[i]);
	os << endl
	   << "Total Group Number : " << w._groupNum<<endl;
	//os << "Density Difference : " << w._densityDiffSum;
	os << "Density Difference : " << w._areaDiffSum / (Window::getOmega() * Window::getOmega()) * 100;
	os <<endl<< "Cross Group : "<<endl;
	for (int i = 0; i < w._crossGroup.size(); i++)
		os << *w._crossGroup[i]<<endl;
	return os;
}
/********************* WindowsSet *************************/
