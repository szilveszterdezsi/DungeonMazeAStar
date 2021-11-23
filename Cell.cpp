#include "Cell.h"
//--------------------------------------------------------------------------------
Cell::Cell(int x, int y) : id (-1), x(x), y(y), f(0), g(0), h(0),
	isPassable(false), n(nullptr), ne(nullptr), e(nullptr), se(nullptr),
	s(nullptr), sw(nullptr), w(nullptr), nw(nullptr), p(nullptr) {}
//--------------------------------------------------------------------------------
std::vector<Cell*> Cell::VonNeumannNeighbourhood(bool isPassable)
{
	std::vector<Cell*> nV;
	if (n != nullptr)
		if (n->isPassable == isPassable)
			nV.push_back(n);
	if (e != nullptr) 
		if (e->isPassable == isPassable)
			nV.push_back(e);
	if (s != nullptr) 
		if (s->isPassable == isPassable)
			nV.push_back(s);
	if (w != nullptr) 
		if (w->isPassable == isPassable)
			nV.push_back(w);
	return nV;
}
//--------------------------------------------------------------------------------
std::vector<Cell*> Cell::NextOverVonNeumannNeighbours(bool isPassable)
{
	std::vector<Cell*> nV;
	if (n != nullptr)
		if (n->n != nullptr)
			if (n->n->isPassable == isPassable)
				nV.push_back(n->n);
	if (e != nullptr)
		if (e->e != nullptr)
			if (e->e->isPassable == isPassable)
				nV.push_back(e->e);
	if (s != nullptr)
		if (s->s != nullptr)
			if (s->s->isPassable == isPassable)
				nV.push_back(s->s);
	if (w != nullptr)
		if (w->w != nullptr)
			if (w->w->isPassable == isPassable)
				nV.push_back(w->w);
	return nV;
}
//--------------------------------------------------------------------------------
std::vector<Cell*> Cell::MooreNeighbourhood(bool isPassable, bool cutCorners)
{
	std::vector<Cell*> nV;
	if (n != nullptr)
		if (n->isPassable == isPassable)
			nV.push_back(n);	
	if (ne != nullptr)
		if (ne->isPassable == isPassable)
		{
			if (cutCorners)
				nV.push_back(ne);
			else
			{
				if (n != nullptr && e != nullptr)
					if (n->isPassable && e->isPassable)
						nV.push_back(ne);
			}
		}
	if (e != nullptr)
		if (e->isPassable == isPassable)
			nV.push_back(e);
	if (se != nullptr)
		if (se->isPassable == isPassable)
		{
			if (cutCorners)
				nV.push_back(se);
			else
			{
				if (s != nullptr && e != nullptr)
					if (s->isPassable && e->isPassable)
						nV.push_back(se);
			}
		}
	if (s != nullptr)
		if (s->isPassable == isPassable)
			nV.push_back(s);
	if (sw != nullptr)
		if (sw->isPassable == isPassable)
		{
			if (cutCorners)
				nV.push_back(sw);
			else
			{
				if (s != nullptr && w != nullptr)
					if (s->isPassable && w->isPassable)
						nV.push_back(sw);
			}
		}
	if (w != nullptr)
		if (w->isPassable == isPassable)
			nV.push_back(w);
	if (nw != nullptr)
		if (nw->isPassable == isPassable)
		{
			if (cutCorners)
				nV.push_back(nw);
			else
			{
				if (n != nullptr && w != nullptr)
					if (n->isPassable && w->isPassable)
						nV.push_back(nw);
			}
		}
	return nV;
}
//--------------------------------------------------------------------------------