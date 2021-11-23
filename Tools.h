#pragma once
#include <random>
#include <vector>
//--------------------------------------------------------------------------------
int RandomInt(int max);
int RandomInt(int min, int max);
bool RandomBool();
bool RandomBoolIn(int probability);
template<class T>
bool VectorContains(std::vector<T>& vector, T& o)
{
	return (std::find(vector.begin(), vector.end(), o) != vector.end());
}
//--------------------------------------------------------------------------------