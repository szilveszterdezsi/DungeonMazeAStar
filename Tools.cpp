#include "Tools.h"
//--------------------------------------------------------------------------------
int RandomInt(int inclusiveMin, int inclusiveMax)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> dist(inclusiveMin, inclusiveMax);
	return dist(rd);
}
//--------------------------------------------------------------------------------
int RandomInt(int inclusiveMax)
{
	return RandomInt(0, inclusiveMax);
}
//--------------------------------------------------------------------------------
bool RandomBool()
{
	return RandomInt(0, 1);
}
//--------------------------------------------------------------------------------
bool RandomBoolIn(int probability)
{
	return !RandomInt(0, probability - 1);
}
//--------------------------------------------------------------------------------
