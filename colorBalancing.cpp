#include "colorBalancing.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <limits>
#include <cmath>

using namespace std;
Coordinate parseBlock(string);

//BoundingBox{{{
BoundingBox::BoundingBox() 
{
	Bbox_coord.x_left = numeric_limits<int>::max();
	Bbox_coord.x_right = numeric_limits<int>::min();
	Bbox_coord.y_down = numeric_limits<int>::max();
	Bbox_coord.y_up = numeric_limits<int>::min();
}

bool BoundingBox::readBlock(istream & is)
{
	string word,line;

	is>>word;
	if(word.substr(0,6)!="ALPHA="){
		cerr<<"ERROR** BoundingBox::read() : Wrong input format!! "<<endl;
		cerr<<"Fail to recognize "<<word.substr(0,6)<<endl;
		return false;
	}
	istringstream(word.substr(6))>>alpha;
	is>>word;
	if(word.substr(0,5)!="BETA="){
		cerr<<"ERROR** BoundingBox::read() : Wrong input format!! "<<endl;
		cerr<<"Fail to recognize"<<word.substr(0,5)<<endl;
		return false;
	}
	istringstream(word.substr(5))>>beta;
	is>>word;
	if(word.substr(0,6)!="OMEGA="){
		cerr<<"ERROR** BoundingBox::read() : Wrong input format!! "<<endl;
		cerr<<"Fail to recognize"<<word.substr(0,6)<<endl;
		return false;
	}
	istringstream(word.substr(7))>>omega;
	getline(is,line);
	while(getline(is,line)){
		Coordinate temp = parseBlock(line);
		_blocks.push_back(Block(temp));

		if(Bbox_coord.x_left>temp.x_left)
			Bbox_coord.x_left = temp.x_left;

		if(Bbox_coord.y_down>temp.y_down)
			Bbox_coord.y_down = temp.y_down;

		if(Bbox_coord.x_right < temp.x_right)
			Bbox_coord.x_right = temp.x_right;

		if(Bbox_coord.y_up < temp.y_up)
			Bbox_coord.y_up = temp.y_up;
	}
}

Coordinate parseBlock(string _coordstr)
{
	string token;
	stringstream ss(_coordstr);
	Coordinate _coord;
	
	getline(ss,token,',');
	istringstream(token)>>_coord.x_left;  
	
	getline(ss,token,',');
	istringstream(token)>>_coord.y_down ;
	
	getline(ss,token,',');
	istringstream(token)>>_coord.x_right;
	
	getline(ss,token,',');
	istringstream(token)>>_coord. y_up;

	return _coord;
}

void BoundingBox::buildGroup()
{
	//Plot grid
	int grid_x_size = (Bbox_coord.x_right - Bbox_coord.x_left)/alpha + 1;
	int grid_y_size = (Bbox_coord.y_up - Bbox_coord.y_down)/beta + 1;

	_grid = new Grid*[grid_x_size];
	for(size_t i = 0; i < grid_x_size ; ++i){
		_grid[i] = new Grid[grid_y_size];
	}
	for(size_t i = 0; i < _blocks.size(); ++i){
		addBlockToGrid(&_blocks[i]);
	}
	colorBlocks();

	for(size_t i = 0; i < grid_x_size ; ++i){
		delete [] _grid[i];
	}
	delete []  _grid;
}

void BoundingBox::addBlockToGrid(Block* _block)
{
	Coordinate _coord = _block->blockCoord;
	//get adjacent grid
	int grid_x_size = (Bbox_coord.x_right - Bbox_coord.x_left)/alpha + 1;
	int grid_y_size = (Bbox_coord.y_up - Bbox_coord.y_down)/beta + 1;

	size_t left_idx = (_coord.x_left - Bbox_coord.x_left)/alpha;
	size_t right_idx = (_coord.x_right - Bbox_coord.x_left)/alpha;
	size_t down_idx = (_coord.y_down - Bbox_coord.y_down)/beta;
	size_t up_idx = (_coord.y_up - Bbox_coord.y_down)/beta;

	for(size_t x = left_idx ; x <= right_idx ; ++x){
		for(size_t i = 0; i < _grid[x][down_idx].blocks.size();++i){
			if(checkBlockAdj(_block,_grid[x][down_idx].blocks[i])&&!(checkConnected(_block,_grid[x][down_idx].blocks[i]))){
				_block->addAdjBlock(_grid[x][down_idx].blocks[i]);
				_grid[x][down_idx].blocks[i]->addAdjBlock(_block);
			}
		}
		_grid[x][down_idx].addBlock(_block);
		for(size_t i = 0; i < _grid[x][up_idx].blocks.size();++i){
			if(checkBlockAdj(_block,_grid[x][up_idx].blocks[i])&&!(checkConnected(_block,_grid[x][up_idx].blocks[i]))){
				_block->addAdjBlock(_grid[x][up_idx].blocks[i]);
				_grid[x][up_idx].blocks[i]->addAdjBlock(_block);
			}
		}
		_grid[x][up_idx].addBlock(_block);

		if(down_idx>=1){
			for(size_t i = 0; i < _grid[x][down_idx-1].blocks.size();++i){
				if(checkBlockAdj(_block,_grid[x][down_idx-1].blocks[i])&&!(checkConnected(_block,_grid[x][down_idx-1].blocks[i]))){
					_block->addAdjBlock(_grid[x][down_idx-1].blocks[i]);
					_grid[x][down_idx-1].blocks[i]->addAdjBlock(_block);
				}
				
			}
		}
		if(up_idx+1< grid_y_size){
			for(size_t i = 0; i < _grid[x][up_idx+1].blocks.size();++i){
				if(checkBlockAdj(_block,_grid[x][up_idx+1].blocks[i])&&!(checkConnected(_block,_grid[x][up_idx+1].blocks[i]))){
					_block->addAdjBlock(_grid[x][up_idx+1].blocks[i]);
					_grid[x][up_idx+1].blocks[i]->addAdjBlock(_block);
				}
			}
		}
	}

	for(size_t y = down_idx ; y <= up_idx ; ++y){
		for(size_t i = 0; i < _grid[left_idx][y].blocks.size();++i){
			if(checkBlockAdj(_block,_grid[left_idx][y].blocks[i])&&!(checkConnected(_block,_grid[left_idx][y].blocks[i]))){
				_block->addAdjBlock(_grid[left_idx][y].blocks[i]);
				_grid[left_idx][y].blocks[i]->addAdjBlock(_block);
			}
		}
		_grid[down_idx][y].addBlock(_block);
		for(size_t i = 0; i < _grid[right_idx][y].blocks.size();++i){
			if(checkBlockAdj(_block,_grid[right_idx][y].blocks[i])&&!(checkConnected(_block,_grid[right_idx][y].blocks[i]))){
				_block->addAdjBlock(_grid[right_idx][y].blocks[i]);
				_grid[right_idx][y].blocks[i]->addAdjBlock(_block);
			}
		}
		_grid[right_idx][y].addBlock(_block);

		if(left_idx>=1){
			for(size_t i = 0; i < _grid[left_idx-1][y].blocks.size();++i){
				if(checkBlockAdj(_block,_grid[left_idx-1][y].blocks[i])&&!(checkConnected(_block,_grid[left_idx-1][y].blocks[i]))){
					_block->addAdjBlock(_grid[left_idx-1][y].blocks[i]);
					_grid[left_idx-1][y].blocks[i]->addAdjBlock(_block);
				}
				
			}
		}
		if(right_idx+1< grid_x_size){
			for(size_t i = 0; i < _grid[right_idx+1][y].blocks.size();++i){
				if(checkBlockAdj(_block,_grid[right_idx+1][y].blocks[i])&&!(checkConnected(_block,_grid[right_idx+1][y].blocks[i]))){
					_block->addAdjBlock(_grid[right_idx+1][y].blocks[i]);
					_grid[right_idx+1][y].blocks[i]->addAdjBlock(_block);
				}
			}
		}
	}
}
bool BoundingBox::checkBlockAdj(Block* b1, Block* b2)
{
	bool adj = false, x_overlap = false, y_overlap = false;
	Coordinate _block = b2->blockCoord ,blockCoord = b1->blockCoord;

	x_overlap = !(_block.x_right < blockCoord.x_left || _block.x_left > blockCoord.x_right);
	y_overlap = !(_block.y_up < blockCoord.y_down || _block.y_down > blockCoord.y_up);
	
	if(x_overlap){
		if(abs(_block.y_down - blockCoord.y_up) <=beta||abs(blockCoord.y_down - _block.y_up )<= beta)
			adj = true;
	}
	if(y_overlap){
		if(abs(_block.x_left - blockCoord.x_right) <=alpha||abs(blockCoord.x_left - _block.x_right) <= alpha)
			adj = true;
	}
	return adj;
}

bool BoundingBox::checkConnected(Block* b1,Block* b2)
{
	if(b1==b2) return true;
	for(size_t i = 0 ; i < b1->adjBlocks.size();++i){
		if(b1->adjBlocks[i]==b2) {
			return true; 
		}
	}
	return false;
}

void BoundingBox::printInfo(ostream& os)
{
	for(size_t i =0; i< _blocks.size(); ++i){
		os<<"Block:          "<<_blocks[i];
		os<<"---------------------------------------"<<endl;
		os<<"Adjacent Block:"<<endl;
		for(size_t j = 0 ; j < _blocks[i].adjBlocks.size() ; ++j)
			os<<"                "<<*_blocks[i].adjBlocks[j];
		os<<"\n\n\n";
	}
	os<<"ALPHA : "<<alpha<<endl;
	os<<"BETA : "<<beta<<endl;
	os<<"Boundary : "<<Block(Bbox_coord);
}

void BoundingBox::colorBlocks()
{
	int k=0;
	for(size_t i=0; i<_blocks.size();++i){
	    _blocks[i].visited=false;
	    _blocks[i].cc=0; //connected component index starts from 0
	    _blocks[i].color=0;
	}
    for(size_t i =0;i<_blocks.size();++i){
        if(_blocks[i].visited==false){
        	DFSvisit(&_blocks[i],k);
        	k=k+1;
        }
    }
    // color
    vector<bool> colorable;
    for(int i=0;i<=k;i++){
    	colorable.push_back(true);
        for(size_t j=0;j<_blocks.size();++j){
            if(_blocks[j].cc==i){
            	bool colorusage[2]={false,false};
                for(size_t q=0;q<_blocks[j].adjBlocks.size();++q){
                    if(_blocks[j].adjBlocks[q]->color==1)
                    	colorusage[0]=true;
                    else if(_blocks[j].adjBlocks[q]->color==2)
                    	colorusage[1]=true;
                }
                if(colorusage[0]==false)
                	_blocks[j].color=1;
                else if(colorusage[1]==false)
                	_blocks[j].color=2;
                else{
                	colorable[i]=false;
                	break;
                }
            }
        }
    }
    // debug info
    for(int i=0;i<k;i++){
    	cout<<" CC: "<<i<<endl;
    	cout<<"colorable ? "<<colorable[i]<<endl;
        for(size_t j=0;j<_blocks.size();++j){
            if(_blocks[j].cc==i)
            	cout<<_blocks[j];
        }
        cout<<endl;
    }
}
bool BoundingBox::DFSvisit(Block* b,int k)
{
    b->visited=true;
    b->cc=k;
    for(size_t i = 0; i<b->adjBlocks.size(); ++i){
    	if(b->adjBlocks[i]->visited==false)
    		DFSvisit(b->adjBlocks[i],k);
    }
}
BoundingBox::~BoundingBox() {}
//}}} BoundingBox

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
	<<rhs.blockCoord.x_right<<","<<rhs.blockCoord.y_up<<endl;
}

Block::~Block() {}
//}}}Block

// Group{{{
Group::Group() {}

Group::~Group() {}
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
Window::~Window(){}
//}}}Window