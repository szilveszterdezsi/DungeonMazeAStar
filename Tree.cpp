#include "Tree.h"
//--------------------------------------------------------------------------------
Tree::Tree(Partition* p, int maxW, int maxH, double whRatio,
	double hwRatio, int maxDepth) : partition(*p), branches()
{
	if (maxDepth != 0 && ((p->w * whRatio) > maxW) && ((p->h * hwRatio) > maxH)) {
		Partition* bs = BranchTree(*p, whRatio, hwRatio);
		branches[0] = new Tree(&bs[0], maxW, maxH, whRatio, hwRatio, maxDepth - 1);
		branches[1] = new Tree(&bs[1], maxW, maxH, whRatio, hwRatio, maxDepth - 1);
	}
}
//--------------------------------------------------------------------------------
Partition* Tree::BranchTree(Partition p, double whR, double hwR)
{
	Partition* bs = new Partition[2];
	if (RandomBool())
	{
		if (p.w / p.h > whR)
		{
			int w = RandomInt(p.w * whR, p.w - (p.w * whR));
			bs[0] = Partition(p.x, p.y, w, p.h);
			bs[1] = Partition(p.x + w, p.y, p.w - w, p.h);
		}
		else
			return BranchTree(p, whR, hwR);
	}
	else
	{
		if (p.h / p.w > hwR)
		{
			int h = RandomInt(p.h * hwR, p.h - (p.h * hwR));
			bs[0] = Partition(p.x, p.y, p.w, h);
			bs[1] = Partition(p.x, p.y + h, p.w, p.h - h);
		}
		else
			return BranchTree(p, whR, hwR);
	}
	return bs;
}
//--------------------------------------------------------------------------------
std::vector<Partition*> Tree::GetAllPartitions()
{
	return GetAllPartitions(this);
}
//--------------------------------------------------------------------------------
std::vector<Partition*> Tree::GetLeafPartitions()
{
	return GetLeafPartitions(this);
}
//--------------------------------------------------------------------------------
std::vector<Partition*> Tree::GetAllPartitions(Tree* root)
{
    std::vector<Tree*> b = *(new std::vector<Tree*>);
    std::vector<Partition*> p = *(new std::vector<Partition*>);
    b.push_back(root);
    int i = 0;
    while (i != b.size()) {
        if (b[i] != NULL) {
			Tree* bA = b[i]->branches[0];
			Tree* bB = b[i]->branches[1];
			if (bA != nullptr)
			{
				b.push_back(bA);
				p.push_back(&bA->partition);
			}
			if (bB != nullptr)
			{
				b.push_back(bB);
				p.push_back(&bB->partition);
			}
        }
        i++;
    }
    return p;
}
std::vector<Partition*> Tree::GetLeafPartitions(Tree* root)
{
	static std::vector<Partition*> leaves = *(new std::vector<Partition*>);
	Tree* bA = root->branches[0];
	Tree* bB = root->branches[1];
	if (bA == nullptr && bB == nullptr)
		leaves.push_back(&root->partition);
	else
	{
		GetLeafPartitions(bA);
		GetLeafPartitions(bB);
	}	
	return leaves;
}
//--------------------------------------------------------------------------------