#include "colorBalancing.h"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void BoundingBox::addBlockToGrid(Block* _block)
{
	Coordinate _coord = _block->blockCoord;
	//get adjacent grid
	size_t grid_x_size = (Bbox_coord.x_right - Bbox_coord.x_left)/alpha + 1;
	size_t grid_y_size = (Bbox_coord.y_up - Bbox_coord.y_down)/beta + 1;

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

bool BoundingBox::DFSvisit(Block* b,int k)
{
    b->visited=true;
    b->cc=k;
    for(size_t i = 0; i<b->adjBlocks.size(); ++i){
    	if(b->adjBlocks[i]->visited==false)
    		DFSvisit(b->adjBlocks[i],k);
    }
    return true;
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
