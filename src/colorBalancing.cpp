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

//BoundingBox{{{
BoundingBox::BoundingBox() 
{
	Bbox_coord.x_left = numeric_limits<int>::max();
	Bbox_coord.x_right = numeric_limits<int>::min();
	Bbox_coord.y_down = numeric_limits<int>::max();
	Bbox_coord.y_up = numeric_limits<int>::min();
}

BoundingBox::~BoundingBox() 
{
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;
	for(int i = 0; i < xmax_w ; ++i){
		delete [] _windows[i];
	}
	delete []  _windows; 
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
	istringstream(word.substr(6))>>omega;
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
	os<<"------------------------DEBUG INFO-----------------------"<<endl;
	os<<"ALPHA : "<<alpha<<endl;
	os<<"BETA : "<<beta<<endl;
	os<<"OMEGA : "<<omega<<endl;
	os<<"Boundary : "<<Block(Bbox_coord);
    for (int i = 0, l = _Cgroup.size(); i < l; i++) { 
        os << "group" << i << ": ";
        for (int j = 0, n = _Cgroup[i]._windows.size(); j < n; j++) { 
            os << _Cgroup[i]._windows[j] -> windowCoord.x_left << "," 
               << _Cgroup[i]._windows[j] -> windowCoord.y_down << "," 
               << _Cgroup[i]._windows[j] -> windowCoord.x_right << "," 
               << _Cgroup[i]._windows[j] -> windowCoord.y_up << "; ";
        }
        os << endl;
    }
	os<<"---------------------------------------------------------"<<endl;
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;
	for(int i=0;i<ymax_w;i++){
		for(int j=0;j<xmax_w;j++){
			os << "WIN[" << i * xmax_w + j + 1 << "]="
               << _windows[j][i].windowCoord.x_left << ","
               << _windows[j][i].windowCoord.y_down << ","
               << _windows[j][i].windowCoord.x_right << ","
               << _windows[j][i].windowCoord.y_up << "("
               << setprecision(2) << fixed << _windows[j][i].densityA << " "
               << setprecision(2) << fixed << _windows[j][i].densityB << ")" <<endl;
		}
	}
	for(size_t i=0;i<_NOgroup.size();i++){
	    os<<"GROUP"<<endl;
	    for(size_t j=0;j<_NOgroup[i]._blocksA.size();j++)
	    	os<<"NO["<<j+1<<"]="<<*_NOgroup[i]._blocksA[j]<<endl;
	}
	for(size_t i=0;i<_Cgroup.size();i++){
	    os<<"GROUP"<<endl;
	    for(size_t j=0;j<_Cgroup[i]._blocksA.size();j++)
	    	os<<"CA["<<j+1<<"]="<<*_Cgroup[i]._blocksA[j]<<endl;
	    for(size_t j=0;j<_Cgroup[i]._blocksB.size();j++)
	    	os<<"CB["<<j+1<<"]="<<*_Cgroup[i]._blocksB[j]<<endl;
	}
}

void BoundingBox::colorBlocks()
{
	int k=0;
	int xmin=Bbox_coord.x_right;
	int xmax=Bbox_coord.x_left;
	int ymin=Bbox_coord.y_up;
	int ymax=Bbox_coord.y_down;
	// find CC
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
    // add to group & update Bbox_coord
    for(int i=0;i<k;i++){
    	if(colorable[i]==true){
    		Group* colorGroup = new Group;
    		for(size_t j=0;j<_blocks.size();++j){
                if(_blocks[j].cc==i){
                	if(_blocks[j].color==1)
                		colorGroup->addA(&_blocks[j]);
                	else
                		colorGroup->addB(&_blocks[j]);
                	if(_blocks[j].blockCoord.x_left < xmin)
		            	xmin=_blocks[j].blockCoord.x_left;
		            if(_blocks[j].blockCoord.x_right > xmax)
		            	xmax=_blocks[j].blockCoord.x_right;
		            if(_blocks[j].blockCoord.y_down < ymin)
		            	ymin=_blocks[j].blockCoord.y_down;
		            if(_blocks[j].blockCoord.y_up > ymax)
		            	ymax=_blocks[j].blockCoord.y_up;
                }
            }
            if(colorGroup->areaA() < colorGroup->areaB())
            	colorGroup->swapAB();
            _Cgroup.push_back(*colorGroup);
    	}
    	else{
    		Group* noGroup = new Group;
    		for(size_t j=0;j<_blocks.size();++j){
                if(_blocks[j].cc==i){
                	noGroup->addA(&_blocks[j]);
                }
            }
            _NOgroup.push_back(*noGroup);
    	}
    }
    Bbox_coord.x_left =xmin;
    Bbox_coord.x_right=xmax;
    Bbox_coord.y_up   =ymax;
    Bbox_coord.y_down =ymin;
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

void BoundingBox::buildWindow()
{
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;

	_windows = new Window*[xmax_w];
	for(int i = 0; i < xmax_w ; ++i){
		_windows[i] = new Window[ymax_w];
	}

	for (int i = 0; i < ymax_w; i++){
		for (int j = 0; j < xmax_w; j++){
			_windows[j][i].windowCoord.x_right = (Bbox_coord.x_left+(j+1)*omega > Bbox_coord.x_right) ? 
				Bbox_coord.x_right : (j+1)*omega+Bbox_coord.x_left;
			_windows[j][i].windowCoord.x_left = _windows[j][i].windowCoord.x_right-omega;
			_windows[j][i].windowCoord.y_up = (Bbox_coord.y_down+(i+1)*omega > Bbox_coord.y_up) ? 
				Bbox_coord.y_up : (i+1)*omega+Bbox_coord.y_down;
			_windows[j][i].windowCoord.y_down = _windows[j][i].windowCoord.y_up-omega;
			_windows[j][i].idx = make_pair(j,i);
			_windows[j][i].densityA = 0;
			_windows[j][i].densityB = 0;
		}
	}

	for (size_t i = 0; i < _Cgroup.size(); i++){

		vector<pair<int,int> > checkedwindow_g;

		buildBlocksAWindow(checkedwindow_g, i);
		buildBlocksBWindow(checkedwindow_g, i);

		if( checkedwindow_g.size()==1 )
			_windows[checkedwindow_g[0].first-1][checkedwindow_g[0].second-1].innerGroup.push_back( &_Cgroup[i] );
		else{
			for (size_t j = 0; j < checkedwindow_g.size(); j++){
				_windows[checkedwindow_g[j].first-1][checkedwindow_g[j].second-1].crossGroup.push_back( &_Cgroup[i] );
			}
		}
	}


}

void BoundingBox::buildBlocksAWindow(vector<pair<int,int> >& checkedwindow_g, int i)
{
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;

	for (size_t k = 0; k < _Cgroup[i]._blocksA.size(); k++){
			bool overlap_r = false;
			bool overlap_u = false;
			Block* thisBlock = _Cgroup[i]._blocksA[k];
			if(Bbox_coord.x_right - thisBlock->blockCoord.x_right < omega)
				overlap_r = true;
			if(Bbox_coord.y_up - thisBlock->blockCoord.y_up < omega)
				overlap_u = true;
			int block_xmax_w = ((thisBlock->blockCoord.x_right-Bbox_coord.x_left) / omega)+1;
			int block_xmin_w = ((thisBlock->blockCoord.x_left-Bbox_coord.x_left) / omega)+1;
			int block_ymax_w = ((thisBlock->blockCoord.y_up-Bbox_coord.y_down) / omega)+1;
			int block_ymin_w = ((thisBlock->blockCoord.y_down-Bbox_coord.y_down) / omega)+1;
			vector<pair<int,int> > checkedwindow_b;

			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[block_xmax_w-1][block_ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmax_w,block_ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmax_w-1][block_ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmax_w,block_ymax_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,block_ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[block_xmin_w-1][block_ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,block_ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][block_ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,block_ymax_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmax_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[block_xmax_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmax_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmax_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmax_w,block_ymin_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[block_xmin_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,block_ymin_w) );
				}
			}
			if(overlap_r==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(xmax_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[xmax_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(xmax_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(xmax_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[xmax_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(xmax_w,block_ymin_w) );
				}
			}
			if(overlap_u==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[block_xmin_w-1][ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,ymax_w) );
				}
			}
			if(overlap_r==true && overlap_u==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(xmax_w,ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksA[k]->windows.push_back( &_windows[xmax_w-1][ymax_w-1] );
				checkedwindow_b.push_back( make_pair(xmax_w,ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(xmax_w,ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[xmax_w-1][ymax_w-1] );
					checkedwindow_g.push_back( make_pair(xmax_w,ymax_w) );
				}
			}
		}
}

void BoundingBox::buildBlocksBWindow(vector<pair<int,int> >& checkedwindow_g, int i)
{
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;

	for (size_t k = 0; k < _Cgroup[i]._blocksB.size(); k++){
			bool overlap_r = false;
			bool overlap_u = false;
			Block* thisBlock = _Cgroup[i]._blocksB[k];
			if(Bbox_coord.x_right - thisBlock->blockCoord.x_right < omega)
				overlap_r = true;
			if(Bbox_coord.y_up - thisBlock->blockCoord.y_up < omega)
				overlap_u = true;
			int block_xmax_w = ((thisBlock->blockCoord.x_right-Bbox_coord.x_left) / omega)+1;
			int block_xmin_w = ((thisBlock->blockCoord.x_left-Bbox_coord.x_left) / omega)+1;
			int block_ymax_w = ((thisBlock->blockCoord.y_up-Bbox_coord.y_down) / omega)+1;
			int block_ymin_w = ((thisBlock->blockCoord.y_down-Bbox_coord.y_down) / omega)+1;
			vector<pair<int,int> > checkedwindow_b;

			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[block_xmax_w-1][block_ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmax_w,block_ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmax_w-1][block_ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmax_w,block_ymax_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,block_ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[block_xmin_w-1][block_ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,block_ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][block_ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,block_ymax_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmax_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[block_xmax_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmax_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmax_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmax_w,block_ymin_w) );
				}
			}
			if( find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[block_xmin_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,block_ymin_w) );
				}
			}
			if(overlap_r==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(xmax_w,block_ymin_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[xmax_w-1][block_ymin_w-1] );
				checkedwindow_b.push_back( make_pair(xmax_w,block_ymin_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(xmax_w,block_ymin_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[xmax_w-1][block_ymin_w-1] );
					checkedwindow_g.push_back( make_pair(xmax_w,block_ymin_w) );
				}
			}
			if(overlap_u==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(block_xmin_w,ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[block_xmin_w-1][ymax_w-1] );
				checkedwindow_b.push_back( make_pair(block_xmin_w,ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmin_w,ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[block_xmin_w-1][ymax_w-1] );
					checkedwindow_g.push_back( make_pair(block_xmin_w,ymax_w) );
				}
			}
			if(overlap_r==true && overlap_u==true &&
				find(checkedwindow_b.begin(),checkedwindow_b.end(),make_pair(xmax_w,ymax_w))==checkedwindow_b.end() ){
				_Cgroup[i]._blocksB[k]->windows.push_back( &_windows[xmax_w-1][ymax_w-1] );
				checkedwindow_b.push_back( make_pair(xmax_w,ymax_w) );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(xmax_w,ymax_w))==checkedwindow_g.end() ){
					_Cgroup[i]._windows.push_back( &_windows[xmax_w-1][ymax_w-1] );
					checkedwindow_g.push_back( make_pair(xmax_w,ymax_w) );
				}
			}
		}
}
    
void 
BoundingBox::calWindowDensity()
{
    int xmax_w = (Bbox_coord.x_right - Bbox_coord.x_left) / omega + 1;
    int ymax_w = (Bbox_coord.y_up - Bbox_coord.y_down) / omega + 1;

    for (int i = 0; i < ymax_w; i++) {
        for (int j = 0; j < xmax_w; j++) {
            double areaA = 0, areaB = 0;
            for (int k = 0, l = _windows[j][i].innerGroup.size(); k < l; k++) {
                areaA += _windows[j][i].innerGroup[k] -> areaA();
                areaB += _windows[j][i].innerGroup[k] -> areaB();
            }
            for (int k = 0, l = _windows[j][i].crossGroup.size(); k < l; k++) {
                areaA += _windows[j][i].crossGroup[k] -> areaA(_windows[j][i].windowCoord);
                areaB += _windows[j][i].crossGroup[k] -> areaB(_windows[j][i].windowCoord);
            }
            _windows[j][i].densityA = areaA / (omega * omega) * 100;
            _windows[j][i].densityB = areaB / (omega * omega) * 100;
        }
    }
}
    
//}}} BoundingBox

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
	<<rhs.blockCoord.x_right<<","<<rhs.blockCoord.y_up<<endl;
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

Window::~Window() {}
//}}}Window
