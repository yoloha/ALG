#ifndef COLOR_BALANCING_H
#define COLOR_BALANCING_H

#include <iostream>
#include <vector>
#include "optMethod.h"

using namespace std;

class Window;
class Block;
class Group;
class Grid;

struct Coordinate
{
	int x_left;
	int x_right;
	int y_up;
	int y_down;
};

// For optimization
struct WindowsSet
{
	vector<Window*> _disjointWindows;
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
	void printInfo(ostream &);
	void output(ostream &);
	
private:
	int                 alpha, beta, omega;
	Window**            _windows;
    vector<WindowsSet>  _windowsSet;
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

    const int crossWindowsNum(); 
    const Coordinate getWindowCoord(const int&);
    
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

public:
	Group();
	~Group();

	void addA(Block*);
	void addB(Block*);
	int areaA();
	int areaA(const Coordinate&);
	int areaB();
	int areaB(const Coordinate&);
	void swapAB();
	void addwindow(Window*);
	//friend ostream& operator << (ostream& , const Group);

private:
	int area;
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

public:
	Window();
	Window(Coordinate);
	~Window();
	friend ostream& operator << (ostream&, const Window&);
    const Coordinate getWindowCoord();

private:
	Coordinate      windowCoord;
	vector<Group*>  innerGroup;
	vector<Group*>  crossGroup;
	static int      omega;
	pair<int,int>   idx;
	double          densityA;
	double          densityB;
};

#endif
