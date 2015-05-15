#ifndef  COLOR_BALANCING_H
#define COLOR_BALANCING_H

#include <vector>
#include <iostream>


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
	void buildGroup();
	void printInfo(ostream &);
	void colorBlocks();
private:
	int alpha, beta, omega;
	vector<Window> _windows;
	vector<Block> _blocks;
	vector<Group> _Cgroup;
	vector<Group> _NOgroup;
	Grid** _grid;
	Coordinate Bbox_coord;

	void addBlockToGrid(Block*);
	bool checkBlockAdj(Block*,Block*);
	bool checkConnected(Block*,Block*);
	bool DFSvisit(Block*,int);
};

class Block
{
friend class BoundingBox;
public:
	Block();
	Block(Coordinate);
	~Block();

	int area();
	void addAdjBlock(Block*);
	friend ostream& operator << (ostream&, const Block&);
private:
	Coordinate blockCoord;
	vector<Block*> adjBlocks;
	vector<Window*> windows;
	bool visited;
	int color;
	int cc;
};

class Group
{
public:
	Group();
	~Group();

	void addA(Block*);
	void addB(Block*);
	void addwindow(Window*)
private:
	int area;
	vector<Block*> blocks_A;
	vector<Block*> blocks_B;
	//Note: area(A) > area(B)
	vector<Window*> windows;
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
public:
	Window();
	~Window();
private:
	Coordinate windowCoord;
	vector<Group*> innerGroup;
	vector<Group*> crossGroup;
	static int omega;
};

#endif