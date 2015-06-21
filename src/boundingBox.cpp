#include "colorBalancing.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <string>
#include <limits>
#include <cmath>
#include "timer.h"

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
	//int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;
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
	Window::omega = omega;
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
		size_t grid_x_size = (Bbox_coord.x_right - Bbox_coord.x_left)/alpha + 1;
		   size_t grid_y_size = (Bbox_coord.y_up - Bbox_coord.y_down)/beta + 1;

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

void BoundingBox::colorBlocks()
{
	int k=0;
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
	int xmin=Bbox_coord.x_right;
	int xmax=Bbox_coord.x_left;
	int ymin=Bbox_coord.y_up;
	int ymax=Bbox_coord.y_down;
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
	if(_Cgroup.size() != 0){
		Bbox_coord.x_left =xmin;
		Bbox_coord.x_right=xmax;
		Bbox_coord.y_up   =ymax;
		Bbox_coord.y_down =ymin;
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

			if( Bbox_coord.x_right - Bbox_coord.x_left < omega ){
				_windows[j][i].windowCoord.x_left = Bbox_coord.x_left;
				_windows[j][i].windowCoord.x_right = Bbox_coord.x_left + omega;
			}
			else if( Bbox_coord.x_left+(j+1)*omega > Bbox_coord.x_right ){
				_windows[j][i].windowCoord.x_right = Bbox_coord.x_right;
				_windows[j][i].windowCoord.x_left = _windows[j][i].windowCoord.x_right - omega;
			}
			else{
				_windows[j][i].windowCoord.x_right = (j+1)*omega+Bbox_coord.x_left;
				_windows[j][i].windowCoord.x_left = _windows[j][i].windowCoord.x_right - omega;

			}

			if( Bbox_coord.y_up - Bbox_coord.y_down < omega ){
				_windows[j][i].windowCoord.y_down = Bbox_coord.y_down;
				_windows[j][i].windowCoord.y_up = Bbox_coord.y_down + omega;
			}
			else if( Bbox_coord.y_down+(i+1)*omega > Bbox_coord.y_up ){
				_windows[j][i].windowCoord.y_up = Bbox_coord.y_up;
				_windows[j][i].windowCoord.y_down = _windows[j][i].windowCoord.y_up - omega;
			}
			else{
				_windows[j][i].windowCoord.y_up = (i+1)*omega+Bbox_coord.y_down;
				_windows[j][i].windowCoord.y_down = _windows[j][i].windowCoord.y_up - omega;
			}

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
			_crossGroup.push_back( &_Cgroup[i] );
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
			_windows[j][i].densityA = 0;
			_windows[j][i].densityB = 0;
			for (int k = 0, l = _windows[j][i].innerGroup.size(); k < l; k++) {
				if (_windows[j][i].innerGroup[k] -> getColor()) {
					_windows[j][i].densityA += _windows[j][i].innerGroup[k] -> areaA();
					_windows[j][i].densityB += _windows[j][i].innerGroup[k] -> areaB();
				}
				else {
					_windows[j][i].densityA += _windows[j][i].innerGroup[k] -> areaB();
					_windows[j][i].densityB += _windows[j][i].innerGroup[k] -> areaA();
				}
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

	for (int i = 0; i < ymax_w; i++) {
		for (int j = 0; j < xmax_w; j++) {
			_windows[j][i].densityA /= (omega * omega) / 100;
			_windows[j][i].densityB /= (omega * omega) / 100;
		}
	}
	
}
	
void 
BoundingBox::buildWindowsSet()
{
	vector<WindowsSet*> sets;
	int xmax_w = (Bbox_coord.x_right - Bbox_coord.x_left) / omega + 1;
	int ymax_w = (Bbox_coord.y_up - Bbox_coord.y_down) / omega + 1;
	for (int i = 0; i < ymax_w; i++) {
		for (int j = 0; j < xmax_w; j++) {
			sets.push_back(new WindowsSet(&_windows[j][i]));
			_windows[j][i]._windowSet = sets.back(); 
		}
	}

	for (int i = 0, l = _crossGroup.size(); i < l; i++) {
		Window* winLead = _crossGroup[i] -> _windows[0];
		for (int j = 1, m = _crossGroup[i] -> _windows.size(); j < m; j++) {
			Window* win = _crossGroup[i] -> _windows[j];
			if(winLead -> _windowSet != win -> _windowSet) {
				WindowsSet* windowsSet = win -> _windowSet;
				for (int k = 0, n = windowsSet -> _windows.size(); k < n; k++) { 
					winLead -> _windowSet -> addWindow(windowsSet -> _windows[k]);
					windowsSet -> _windows[k] -> _windowSet = winLead -> _windowSet;
				}
				windowsSet -> _windows.clear();
			}
		}
		winLead -> _windowSet -> addCrossGroup(_crossGroup[i]);
	}

	for (int i = 0, l = sets.size(); i< l; i++) {
		if (sets[i] -> _windows.empty()) delete sets[i];
		else {
			_windowsSet.push_back(sets[i]);
			//sets[i] -> calWinDensityDiffSum(); 
		}
	}

    for (int i = 0, l = _windowsSet.size(); i < l; i++)
        _windowsSet[i] -> initialize();
}

void BoundingBox::printInfo(ostream& os)
{
	os<<"------------------------DEBUG INFO-----------------------"<<endl;
	os<<"Parameter : "<<endl;
	os<<"ALPHA : "<<alpha<<endl;
	os<<"BETA : "<<beta<<endl;
	os<<"OMEGA : "<<omega<<endl;
	os<<"Boundary : "<<Block(Bbox_coord)<<endl<<endl;
	
	int xmax_w = (Bbox_coord.x_right-Bbox_coord.x_left)/omega + 1;
	int ymax_w = (Bbox_coord.y_up-Bbox_coord.y_down)/omega + 1;
	/*for (int i = 0 ; i < xmax_w ; ++i){
		for(int j = 0 ; j < ymax_w ; ++j)
			os<<_windows[i][j];
	}*/
	for (size_t i = 0 ; i <_windowsSet.size();++i){
		os << "* WindowsSet Id    : " << i + 1 << endl
		   << *_windowsSet[i] << endl<<endl;
	}
	
	double density = 0;

	for(int i=0;i<ymax_w;i++){
		for(int j=0;j<xmax_w;j++){
			os << "WIN[" << i * xmax_w + j + 1 << "]=" 
			   << _windows[j][i].windowCoord.x_left << ","
			   << _windows[j][i].windowCoord.y_down << ","
			   << _windows[j][i].windowCoord.x_right << ","
			   << _windows[j][i].windowCoord.y_up << "("
			   << setprecision(2) << fixed << _windows[j][i].densityA << " "
			   << setprecision(2) << fixed << _windows[j][i].densityB << ")" << endl;
			density+=abs(_windows[j][i].densityA - _windows[j][i].densityB);
		}
	}
	os<<endl;
	os<<"TOTAL DENSITY : "<<density<<endl;
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

void BoundingBox::opt()
{
	size_t size = _windowsSet.size();
	for(size_t i = 0 ; i <_windowsSet.size() ; ++i)
	{
		timer.periodStart();

		_windowsSet[i]->linearSolve();
		//_windowsSet[i]->directSim();

		checkTimeLeft();
		cout<<endl;
		cout<<"opt windows set : "<<i+1<<" / "<<size<<endl;
		reportTime();

		if (interrupt==true)
			break;
	}
}

void BoundingBox::gen()
{
	size_t size = _windowsSet.size();
	for(size_t i = 0 ; i <_windowsSet.size() ; ++i){

		timer.periodStart();

		cout<<"#################################  Window Set"<<i+1<<"  ##################################"<<endl;
		_windowsSet[i]->genSim();

		checkTimeLeft();
		cout<<endl;
		cout<<"gen windows set : "<<i+1<<" / "<<size<<endl;
		reportTime();

		if (interrupt==true)
			break;
	}
}

//}}} BoundingBox

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
