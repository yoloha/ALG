#include "colorBalancing.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>
#include <utility>

using namespace std;

Coordinate parseBlock(string);

//Block{{{

Block::Block() {}
Block::~Block() {}

Block::Block(Coordinate _coord)
{
	blockCoord = _coord;
}

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
	os<<rhs.blockCoord.x_left<<","<<rhs.blockCoord.y_down<<","
	<<rhs.blockCoord.x_right<<","<<rhs.blockCoord.y_up;
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

void Group::swapAB()
{
	swap(_blocksA,_blocksB);
}
//}}}Group

//Grid{{{
Grid::Grid() {}

void Grid::addBlock(Block* _block)
{
	for(size_t i = 0 ; i < blocks.size() ; ++i){
		if(blocks[i]==_block) return;
	}
	blocks.push_back(_block);
}
Grid::~Grid() {}
//}}}Grid

//Window{{{
Window::Window() {}

Window::Window(Coordinate _coord)
{
	windowCoord = _coord;
}

ostream& operator << (ostream& os, const Window& w)
{
	os<<"Window Id : ( "<< w.idx.first <<","<< w.idx.second << ")"<<endl;
	os<<"InnerGroup : "<<w.innerGroup.size()<<endl;
	os<<"CrossGroup : "<<w.crossGroup.size()<<endl;
}

Window::~Window() {}
//}}}Window
