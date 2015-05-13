#ifndef COLOR_BALANCING_H
#define COLOR_BALANCING_H

#include <vector>
#include <iostream>

using namespace std;

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
    vector<Window*> 
};


class BoundingBox
{
public:
    BoundingBox();
    ~BoundingBox();
    bool read(istream &);

private:
    vector<Window> _windows;
    vector<Block> _blocks;
    vector<Group> _group;
};

class Block
{
public:
    Block();
    ~Block();
    int area();

private:
    Coordinate blockCoord;
    vector<Block*> adjBlocks;
    vector<Window*> windows;
};

class Group
{
public:
    Group();
    ~Group();

private:
    int area;
    vector<Block*> blocks_A;
    vector<Block*> blocks_B;
    vector<Window*> windows;
};

class Grid
{
public:
    Grid();
    ~Grid();

private:
    Coordinate gridCoord;
    vector<blocks*> blocks;
    static int alpha;
    static int beta;
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
