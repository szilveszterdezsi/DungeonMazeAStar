#pragma once
#include "Tools.h"
#include "Partition.h"
//--------------------------------------------------------------------------------
class Tree
{
	public:
		Tree(Partition* p, int maxW, int maxH, double whRatio, double hwRatio,
			int maxDepth = 32);
		std::vector<Partition*> GetAllPartitions();
		std::vector<Partition*> GetLeafPartitions();
		Partition partition;
		Tree* branches[2];
	private:
		Partition* BranchTree(Partition p, double whR, double hwR);
		std::vector<Partition*> GetAllPartitions(Tree* root);
		std::vector<Partition*> GetLeafPartitions(Tree* root);
};
//--------------------------------------------------------------------------------
