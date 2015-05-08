#pragma once

struct Voxel;

struct KDNode
{
	KDNode () {}
	KDNode (Plane p, KDNode *lchild, KDNode *rchild)
	{
		this->p = p;
		this->lchild = lchild;
		this->rchild = rchild;
	}
	KDNode (std::vector<Face> f) { faces = f; }

	Plane p;
	KDNode *lchild, *rchild;
	bool isLeaf;
	std::vector<Face> faces;
};

class KDTree
{
	KDNode *RecBuild (std::vector<Face> faces, Voxel v);
public:
	KDTree ();
	KDTree (std::vector<Face> faces);
	~KDTree ();

	KDNode *head;
};
