#include "colorBalancing.h"
#include <iostream>
#include <vector>
#include <cmath>

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
	os<<"CrossGroup : "<<w.crossGroup.size()<<endl;
    return os;
}
    
const Coordinate 
Window::getWindowCoord() 
{ 
    return windowCoord;
}
//}}}Window

/********************* WindowsSet *************************/
    
void 
WindowsSet::simulate(const size_t& sim) 
{
    if (sim == _sim) return;

    size_t simTmp = sim;
    for (int i = 0, l = _windows.size(); i < l; i++) {
        for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; n++) {
            _windows[i] -> innerGroup[j] -> setColor(simTmp % 2);
            simTmp /= 2;
        }
    }
    for (int i = 0, l = _crossGroup.size(); i < l; i++) {
        _crossGroup[i] -> setColor(simTmp % 2);
        simTmp /= 2;
    }

    if (calWinDensityDiffSum()) _sim = sim;
    else {
        simTmp = _sim;
        for (int i = 0, l = _windows.size(); i < l; i++) {
            for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; n++) {
                _windows[i] -> innerGroup[j] -> setColor(simTmp % 2);
                simTmp /= 2;
            }
        }
        for (int i = 0, l = _crossGroup.size(); i < l; i++) {
            _crossGroup[i] -> setColor(simTmp % 2);
            simTmp /= 2;
        }
    }
}
    
bool 
WindowsSet::calWinDensityDiffSum()
{
    vector<double> densityA, densityB;
    for (int i = 0, l = _windows.size(); i < l; i++) {
        densityA.push_back(_windows[i] -> densityA);
        densityB.push_back(_windows[i] -> densityB);
    }

    for (int i = 0, l = _windows.size(); i < l; i++) {
        _windows[i] -> densityA = 0;
        _windows[i] -> densityB = 0;
        for (int j = 0, n = _windows[i] -> innerGroup.size(); j < n; n++) {
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

    double densityDiffSum = 0;
    for (int i = 0, l = _windows.size(); i < l; i++) {
        _windows[i] -> densityA /= (_windows[i] -> omega * _windows[i] -> omega) * 100; 
        _windows[i] -> densityB /= (_windows[i] -> omega * _windows[i] -> omega) * 100; 
        densityDiffSum += abs(_windows[i] -> densityA - _windows[i] -> densityB);
    }

    if (densityDiffSum < _densityDiffSum) return true;
    else {
        for (int i = 0, l = _windows.size(); i < l; i++) {
            _windows[i] -> densityA = densityA[i]; 
            _windows[i] -> densityB = densityB[i]; 
        }
        return false;
    }
}

/********************* WindowsSet *************************/
