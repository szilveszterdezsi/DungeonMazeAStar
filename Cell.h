#pragma once
#include "Tools.h"
//--------------------------------------------------------------------------------
class Cell
{
	public:
		Cell(int x, int y);
		int id, x, y, f, g, h;
		bool isPassable;
		Cell* n;
		Cell* ne;
		Cell* e;
		Cell* se;
		Cell* s;
		Cell* sw;
		Cell* w;
		Cell* nw;
		Cell* p;
		std::vector<Cell*> VonNeumannNeighbourhood(bool isPassable);
		std::vector<Cell*> NextOverVonNeumannNeighbours(bool isPassable);
		std::vector<Cell*> MooreNeighbourhood(bool isPassable, bool cutCorners);
};
//--------------------------------------------------------------------------------
