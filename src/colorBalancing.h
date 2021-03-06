#ifndef COLOR_BALANCING_H
#define COLOR_BALANCING_H

#include <iostream>
#include <vector>
#include "optMethod.h"

using namespace std;

#define SIZE_T sizeof(size_t)
#define SIMLEN (SIZE_T * 8)

class Block;
class Group;
class Grid;
class Window;
class WindowsSet;

typedef vector<int> GrpAreaInWin;

struct Coordinate
{
	int x_left;
	int x_right;
	int y_up;
	int y_down;
};

class BoundingBox
{
public:
	BoundingBox();
	~BoundingBox();
	bool readBlock(istream &);
	void buildGroup(connectBlockFlag);
	void colorBlocks();
	void buildWindow();
	void calWindowDensity();
	void buildWindowsSet();
	void opt(optFlag);
	//void gen();
	void printInfo(ostream &);
	void output(ostream &);
	
private:
	int                 alpha, beta, omega;
	Window**            _windows;
	vector<WindowsSet*> _windowsSet;
	vector<Block>       _blocks;
	vector<Group>       _Cgroup;
	vector<Group*>      _crossGroup;
	vector<Group>       _NOgroup;
	Grid**              _grid;
	Coordinate          Bbox_coord;

	void addBlockToGrid(Block*);
	bool checkBlockAdj(Block*,Block*);
	bool checkConnected(Block*,Block*);
	bool DFSvisit(Block*,int);
	void buildBlocksAWindow(vector<pair<int,int> >&, int i);
	void buildBlocksBWindow(vector<pair<int,int> >&, int i);
	//void calWindowDensity();
	//void buildWindowsSet();
	int computeXmaxWindow();
	int computeYmaxWindow();
};

class Block
{
friend class BoundingBox;

public:
	Block();
	Block(Coordinate);
	~Block();

	int area();
	int area(const Coordinate&);
	void addAdjBlock(Block*);
	friend ostream& operator << (ostream&, const Block&);

	const int crossWindowsNum() { return windows.size(); } 
	Window* getWindow(const int& i) { return windows[i]; }
	
private:
	Coordinate      blockCoord;
	vector<Block*>  adjBlocks;
	vector<Window*> windows;
	bool            visited;
	int             color;
	int             cc;
};

class Group
{
friend class BoundingBox;
friend class WindowsSet;
public:
	Group();
	~Group();

	void addA(Block*);
	void addB(Block*);
	void swapAB();
	void addwindow(Window*);
	friend ostream& operator << (ostream& , const Group&);

	const bool getColor() { return _color; }
	void setColor(const int&);
	int areaA();
	int areaA(const Coordinate&);
	int areaB();
	int areaB(const Coordinate&);
	const int getBlocksANum() { return _blocksA.size(); }
	const int getBlocksBNum() { return _blocksB.size(); }
	Block* getBlocksA(const int& i) { return _blocksA[i]; }
	Block* getBlocksB(const int& i) { return _blocksB[i]; }

private:
	bool            _color;
	//int area;
	vector<Block*>  _blocksA;
	vector<Block*>  _blocksB;
	//Note: area(A) > area(B)
	vector<Window*> _windows;
};

class Grid
{
friend class BoundingBox;

public:
	Grid();
	~Grid();
	void addBlock(Block*);

private:
	//Coordinate gridCoord;
	vector<Block*> blocks;
};

class Window
{
friend class BoundingBox;
friend class WindowsSet;

public:
	Window();
	Window(Coordinate);
	~Window();

	static int getOmega();
	const Coordinate getWindowCoord();
	const pair<int,int> getIdx();

	friend ostream& operator << (ostream&, const Window&);

private:
	static int      omega;
	Coordinate      windowCoord;
	WindowsSet*     _windowSet;
	vector<Group*>  innerGroup;
	pair<int,int>   idx;
	double          densityA;
	double          densityB;
};

// For optimization
class WindowsSet
{
friend class BoundingBox;

public:
	//WindowsSet() : _groupNum(0), _sim(0), _densityDiffSum(-1) {}
	//WindowsSet() : _groupNum(0), _densityDiffSum(-1) {}
	WindowsSet() : _groupNum(0) {}
	WindowsSet(Window*);
	~WindowsSet() {}

	//void simulate(const size_t&);
	double simulate(const vector<size_t>&);
	void randSim(int max_time);
	
	void linearSolve();
	void directSim();
	void genSim();
	void greedySolve(const int&);

	friend ostream& operator <<(ostream& ,const WindowsSet&);

private:
	vector<Window*>         _windows;
	vector<Group*>          _crossGroup;
	vector<GrpAreaInWin>    _areaMatrix;
	int                     _groupNum;
	//size_t                  _sim;
	vector<size_t>          _sim;
	//double                  _densityDiffSum;
	double                  _areaDiffSum;

	void addWindow(Window*);
	void addCrossGroup(Group*);
	void initialize();
	void buildAreaMatrix();
	void initSim();

	int  grpColorInSim(const vector<size_t>&, const int&);
	//bool calWinDensityDiffSum();
	void calWinDensity();
	void updateWinDensity();
};

#endif
