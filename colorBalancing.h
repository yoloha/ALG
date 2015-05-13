#ifndef COLOR_BALANCING_H
#define COLOR_BALANCING_H

#include <vector>
#include <iostream>

using namespace std;

class Window;

struct Coordinate
{
    int _x_left;
    int _x_right;
    int _y_up;
    int _y_down;
};

// For optimization
struct WindowsSet
{
    vector<Window*> 
};


class BoundingBox
{
public:
    BoundingBox();
    ~BoundingBox();
    bool read(istream &);

private:
    vector<Window*> _windows;
    vector<Block*>  _blocks;
    vector<Group*>  _group;
};

class Block
{
public:
    Block();
    ~Block();
    int area();

private:
    Coordinate      _blockCoord;
    vector<Block*>  _adjBlocks;
    vector<Window*> _windows;
};

class Group
{
public:
    Group();
    ~Group();

private:
    int             _area;
    vector<Block*>  _blocks_A;
    vector<Block*>  _blocks_B;
    vector<Window*> _windows;
};

class Grid
{
public:
    Grid();
    ~Grid();

private:
    Coordinate      _gridCoord;
    vector<blocks*> _blocks;
    static int      _alpha;
    static int      _beta;
};

class Window
{
public:
    Window();
    ~Window();

private:
    Coordinate      _windowCoord;
    vector<Group*>  _innerGroup;
    vector<Group*>  _crossGroup;
    static int      _omega;
};

#endif
