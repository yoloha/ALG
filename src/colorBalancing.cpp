#include "colorBalancing.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>


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
	os<<"Window Id : ("<< w.idx.first <<","<< w.idx.second << ")"<<endl;
	os<<"InnerGroup : "<<w.innerGroup.size()<<endl;
	return os;
}
	
const Coordinate 
Window::getWindowCoord() 
{ 
	return windowCoord;
}
//}}}Window

/********************* WindowsSet *************************/
	
WindowsSet::WindowsSet(Window* w) :  _densityDiffSum(size_t(1)<<63)
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
	_groupNum++;
}
	
void
WindowsSet::initialize()
{
	buildAreaMatrix();
	initSim();
	calWinDensity(); //
}

void 
WindowsSet::buildAreaMatrix()
{
	GrpAreaInWin init;
	init.resize(_groupNum, 0);
	_areaMatrix.resize(_windows.size(), init);

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
	//----------------------------------------------------------------------------------------
	//    Uses the same method as calWinDensity                                              |
	//----------------------------------------------------------------------------------------
	double densityDiffSum = 0;
	int index=0;
	WindowsSet cpy = *this;
	for (int i = 0, l = cpy._windows.size(); i < l; i++) {
		cpy._windows[i] -> densityA = 0;
		cpy._windows[i] -> densityB = 0;
		for (int j = 0, n = cpy._windows[i] -> innerGroup.size(); j < n; j++,index++) {
			if (grpColorInSim(sim, index)) {
				cpy._windows[i] -> densityA += cpy._windows[i] -> innerGroup[j] -> areaA();
				cpy._windows[i] -> densityB += cpy._windows[i] -> innerGroup[j] -> areaB();
			}
			else {
				cpy._windows[i] -> densityA += _windows[i] -> innerGroup[j] -> areaB();
				cpy._windows[i] -> densityB += _windows[i] -> innerGroup[j] -> areaA();
			}
		}
	}
	for (int i = 0, l = cpy._crossGroup.size(); i < l; i++,index++) {
		if (grpColorInSim(sim, index)) {
			for (int j = 0, m = cpy._crossGroup[i] -> getBlocksANum(); j < m; j++) {
				Block* block = cpy._crossGroup[i] -> getBlocksA(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityA += block -> area(win -> windowCoord);
				}
			}
			for (int j = 0, m = cpy._crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = cpy._crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityB += block -> area(win -> windowCoord);
				}
			}
		}
		else {
			for (int j = 0, m = cpy._crossGroup[i] -> getBlocksANum(); j < m; j++) {
				Block* block = cpy._crossGroup[i] -> getBlocksA(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityB += block -> area(win -> windowCoord);
				}
			}
			for (int j = 0, m = cpy._crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = cpy._crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityA += block -> area(win -> windowCoord);
				}
			}
		}
	}
	for (int i = 0, l = cpy._windows.size(); i < l; i++) {
		cpy._windows[i] -> densityA /= (Window::omega * Window::omega) / 100; 
		cpy._windows[i] -> densityB /= (Window::omega * Window::omega) / 100; 
		densityDiffSum +=abs(_windows[i] ->densityA- _windows[i] ->densityB);
	}
	if (densityDiffSum < _densityDiffSum) {
		_sim = sim;
		_densityDiffSum = densityDiffSum;
	}
	return densityDiffSum;
	/*
	//----------------------------------------------------------------------------------------
	//    Uses Area matrix (current version have some problem)                               |
	//----------------------------------------------------------------------------------------
	//if (sim == _sim) return;
	GrpAreaInWin totalArea;
	totalArea.resize(_areaMatrix.size(),0);

	//size_t simTmp = sim;
	for (int i = 0, l = _areaMatrix.size(); i < l ; i++) {
		//if (simTmp % 2) {
		for (int j = 0; j < _groupNum; j++){
			if (grpColorInSim(sim, j))  totalArea[i] += _areaMatrix[i][j];
			else  totalArea[i] -= _areaMatrix[i][j];
		}
		//simTmp /=2;
	}

	double densityDiffSum = 0;

	for (int i = 0, l = totalArea.size(); i < l; i++)
		densityDiffSum += abs(totalArea[i]);

	densityDiffSum /= (Window::omega * Window::omega) / 100; 

	if (densityDiffSum < _densityDiffSum) {
		_sim = sim;
		_densityDiffSum = densityDiffSum;
		//return true;
	}
	//return false;
	return densityDiffSum;
	
	*/
}

int
WindowsSet::grpColorInSim(const vector<size_t>& sim, const int& grpPos)
{
	int i = grpPos / SIMLEN;
	int p = grpPos % SIMLEN;
	return (sim[i] >> p) % 2;
}
	
void
//WindowsSet::calWinDensityDiffSum()
WindowsSet::calWinDensity()
{
	/*
	vector<double> densityA, densityB;
	for (int i = 0, l = _windows.size(); i < l; i++) {
		densityA.push_back(_windows[i] -> densityA);
		densityB.push_back(_windows[i] -> densityB);
	}
	*/

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
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityA += block -> area(win -> windowCoord);
				}
			}
			for (int j = 0, m = _crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityB += block -> area(win -> windowCoord);
				}
			}
		}
		else {
			for (int j = 0, m = _crossGroup[i] -> getBlocksANum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksA(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityB += block -> area(win -> windowCoord);
				}
			}
			for (int j = 0, m = _crossGroup[i] -> getBlocksBNum(); j < m; j++) {
				Block* block = _crossGroup[i] -> getBlocksB(j);
				for (int k = 0, n = block -> crossWindowsNum(); k < n; k++) {
					Window* win = block -> getWindow(k); 
					win -> densityA += block -> area(win -> windowCoord);
				}
			}
		}
	}

	//double densityDiffSum = 0;
	double totalDen = 0;
	for (int i = 0, l = _windows.size(); i < l; i++) {
		_windows[i] -> densityA /= (Window::omega * Window::omega) / 100; 
		_windows[i] -> densityB /= (Window::omega * Window::omega) / 100; 
		totalDen +=abs(_windows[i] ->densityA- _windows[i] ->densityB);
		cout<<"Win "<<i<<" : "<<_windows[i] ->densityA<<","<< _windows[i] ->densityB<<endl;
	}
	cout<<"TOTAL DEN : "<<totalDen<<endl;

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
	os<<"Density Difference : "<<w._densityDiffSum;
	return os;
}
/********************* WindowsSet *************************/
