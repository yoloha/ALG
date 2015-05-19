#include "colorBalancing.h"
#include <iostream>
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

Block::Block(Coordinate _coord)
{
	blockCoord = _coord;
}
int Block::area()
{
	return (blockCoord.x_right - blockCoord.x_left)*(blockCoord.y_up - blockCoord.y_down);
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

Block::~Block() {}
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
}
int Group::areaB()
{
	int area=0;
	for(size_t i=0;i<_blocksB.size();i++)
	    area += _blocksB[i]->area();
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