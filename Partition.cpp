#include "Partition.h"
//--------------------------------------------------------------------------------
Partition::Partition() : x(0), y(0), w(0), h(0) {}
Partition::Partition(int width, int height) : x(0), y(0), w(width), h(height) {}
Partition::Partition(int x, int y, int width, int height) :
	x(x), y(y), w(width), h(height) {}
//--------------------------------------------------------------------------------