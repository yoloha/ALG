#include "colorBalancing.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

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

void BoundingBox::buildBlocksAWindow(vector<pair<int,int> >& checkedwindow_g, int groupNum)
{
	int xmax_w = computeXmaxWindow();
	int ymax_w = computeYmaxWindow();

	for (size_t k = 0; k < _Cgroup[groupNum]._blocksA.size(); k++){
		Block* thisBlock = _Cgroup[groupNum]._blocksA[k];

		int x_right_diff_left = thisBlock->blockCoord.x_right-Bbox_coord.x_left;
		int x_right_diff_right = Bbox_coord.x_right-thisBlock->blockCoord.x_right;
		int block_xmax_w = x_right_diff_left / omega;
		if( x_right_diff_right < omega )
			block_xmax_w=xmax_w-1;

		int x_left_diff_left = thisBlock->blockCoord.x_left-Bbox_coord.x_left;
		int x_left_diff_right = Bbox_coord.x_right-thisBlock->blockCoord.x_left;
		int block_xmin_w = x_left_diff_left / omega;
//		if( x_left_diff_right < omega )
//			block_xmin_w=xmax_w-1;

		int y_up_diff_down = thisBlock->blockCoord.y_up-Bbox_coord.y_down;
		int y_up_diff_up = Bbox_coord.y_up-thisBlock->blockCoord.y_up;
		int block_ymax_w = y_up_diff_down / omega;
		if( y_up_diff_up < omega )
			block_ymax_w=ymax_w-1;

		int y_down_diff_down = thisBlock->blockCoord.y_down-Bbox_coord.y_down;
		int y_down_diff_up = Bbox_coord.y_up-thisBlock->blockCoord.y_down;
		int block_ymin_w = y_down_diff_down / omega;
//		if( y_down_diff_up < omega )
//			block_ymin_w=ymax_w-1;
		
		for (int i = block_ymin_w; i <= block_ymax_w; i++){
			for (int j = block_xmin_w; j <= block_xmax_w; j++){
				thisBlock->windows.push_back( &_windows[j][i] );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[groupNum]._windows.push_back( &_windows[j][i] );
					checkedwindow_g.push_back( make_pair(j,i) );
				}
			}
		}
	}
}

void BoundingBox::buildBlocksBWindow(vector<pair<int,int> >& checkedwindow_g, int groupNum)
{
	int xmax_w = computeXmaxWindow();
	int ymax_w = computeYmaxWindow();

	for (size_t k = 0; k < _Cgroup[groupNum]._blocksB.size(); k++){
		Block* thisBlock = _Cgroup[groupNum]._blocksB[k];

		int x_right_diff_left = thisBlock->blockCoord.x_right-Bbox_coord.x_left;
		int x_right_diff_right = Bbox_coord.x_right-thisBlock->blockCoord.x_right;
		int block_xmax_w = x_right_diff_left / omega;
		if( x_right_diff_right < omega )
			block_xmax_w=xmax_w-1;

		int x_left_diff_left = thisBlock->blockCoord.x_left-Bbox_coord.x_left;
		int x_left_diff_right = Bbox_coord.x_right-thisBlock->blockCoord.x_left;
		int block_xmin_w = x_left_diff_left / omega;
//		if( x_left_diff_right < omega )
//			block_xmin_w=xmax_w-1;

		int y_up_diff_down = thisBlock->blockCoord.y_up-Bbox_coord.y_down;
		int y_up_diff_up = Bbox_coord.y_up-thisBlock->blockCoord.y_up;
		int block_ymax_w = y_up_diff_down / omega;
		if( y_up_diff_up < omega )
			block_ymax_w=ymax_w-1;

		int y_down_diff_down = thisBlock->blockCoord.y_down-Bbox_coord.y_down;
		int y_down_diff_up = Bbox_coord.y_up-thisBlock->blockCoord.y_down;
		int block_ymin_w = y_down_diff_down / omega;
//		if( y_down_diff_up < omega )
//			block_ymin_w=ymax_w-1;
		
		for (int i = block_ymin_w; i <= block_ymax_w; i++){
			for (int j = block_xmin_w; j <= block_xmax_w; j++){
				thisBlock->windows.push_back( &_windows[j][i] );
				if( find(checkedwindow_g.begin(),checkedwindow_g.end(),make_pair(block_xmax_w,block_ymax_w))==checkedwindow_g.end() ){
					_Cgroup[groupNum]._windows.push_back( &_windows[j][i] );
					checkedwindow_g.push_back( make_pair(j,i) );
				}
			}
		}
	}
}

int BoundingBox::computeXmaxWindow()
{
	int xmax_w = ceil((double)(Bbox_coord.x_right-Bbox_coord.x_left)/omega);
	if(xmax_w==0)
		xmax_w=1;
	return xmax_w;
}

int BoundingBox::computeYmaxWindow()
{
	int ymax_w = ceil((double)(Bbox_coord.y_up-Bbox_coord.y_down)/omega);
	if(ymax_w==0)
		ymax_w=1;
	return ymax_w;
}
