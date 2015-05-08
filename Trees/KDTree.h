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

	Plane p;
	KDNode *lchild, *rchild;
};

struct KDLeaf : public KDNode
{
	KDLeaf (std::vector<Face> f) { faces = f; }

	std::vector<Face> faces;
};

class KDTree
{
	KDNode *RecBuild (std::vector<Face> faces, Voxel v);
	KDNode *head;
public:
	KDTree ();
	KDTree (std::vector<Face> faces);
	~KDTree ();
};
