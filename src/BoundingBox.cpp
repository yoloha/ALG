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

    return true;
}

void BoundingBox::buildGroup(connectBlockFlag flag)
{
	if(flag == GRID_METHOD){
		int grid_x_size = (Bbox_coord.x_right - Bbox_coord.x_left)/alpha + 1;
		int grid_y_size = (Bbox_coord.y_up - Bbox_coord.y_down)/beta + 1;

		_grid = new Grid*[grid_x_size];
		for(size_t i = 0; i < grid_x_size ; ++i){
			_grid[i] = new Grid[grid_y_size];
		}
		for(size_t i = 0; i < _blocks.size(); ++i){
			addBlockToGrid(&_blocks[i]);
		}
		

		for(size_t i = 0; i < grid_x_size ; ++i){
			delete [] _grid[i];
		}
		delete []  _grid;
	}
	else if (flag == CHECKALL_METHOD){
		for(size_t i = 0 ; i < _blocks.size() ; ++i){
			for(size_t j = 0 ; j < _blocks.size() ; ++j){
				if(checkBlockAdj(&_blocks[i],&_blocks[j])&&!checkConnected(&_blocks[i],&_blocks[j])){
					_blocks[i].addAdjBlock(&_blocks[j]);
					_blocks[j].addAdjBlock(&_blocks[i]);
				}
			}
		}
	}
	colorBlocks();
}



void BoundingBox::printInfo(ostream& os)
{
	

	os<<"------------------------DEBUG INFO-----------------------"<<endl;
	os<<"ALPHA : "<<alpha<<endl;
	os<<"BETA : "<<beta<<endl;
	os<<"OMEGA : "<<omega<<endl;
	os<<"Boundary : "<<Block(Bbox_coord);
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;
	for (size_t i = 0 ; i < xmax_w ; ++i){
		for(size_t j = 0 ; j < ymax_w ; ++j)
			os<<_windows[i][j];
	}
	os<<"---------------------------------------------------------"<<endl;
}

void BoundingBox::output(ostream& os)
{
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
               << setprecision(2) << fixed << _windows[j][i].densityB << ")" << endl;
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

void 
BoundingBox::calWindowDensity()
{
    int xmax_w = (Bbox_coord.x_right - Bbox_coord.x_left) / omega + 1;
    int ymax_w = (Bbox_coord.y_up - Bbox_coord.y_down) / omega + 1;

    /*
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
    */
    for (int i = 0; i < ymax_w; i++) {
        for (int j = 0; j < xmax_w; j++) {
            _windows[j][i].densityA = 0;
            _windows[j][i].densityB = 0;
            for (int k = 0, l = _windows[j][i].innerGroup.size(); k < l; k++) {
                _windows[j][i].densityA += _windows[j][i].innerGroup[k] -> areaA();
                _windows[j][i].densityB += _windows[j][i].innerGroup[k] -> areaB();
            }
        }
    }
    for (int i = 0, l = _crossGroup.size(); i < l; i++) {
        for (int j = 0, m = _crossGroup[i] -> _blocksA.size(); j < m; j++) {
            for (int k = 0, n = _crossGroup[i] -> _blocksA[j] -> crossWindowsNum(); k < n; k++) {
                Coordinate windowCoord = _crossGroup[i] -> _blocksA[j] -> getWindowCoord(k); 
                _windows[j][i].densityA += _crossGroup[i] -> _blocksA[j] -> area(windowCoord);
            }
        }
        for (int j = 0, m = _crossGroup[i] -> _blocksB.size(); j < m; j++) {
            for (int k = 0, n = _crossGroup[i] -> _blocksB[j] -> crossWindowsNum(); k < n; k++) {
                Coordinate windowCoord = _crossGroup[i] -> _blocksB[j] -> getWindowCoord(k); 
                _windows[j][i].densityA += _crossGroup[i] -> _blocksB[j] -> area(windowCoord);
            }
        }
    }
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
//}}} BoundingBox
