#pragma once

struct Voxel;

struct KDNode
{
	KDNode () {}
	KDNode (Voxel aabb, Plane p, KDNode *lchild, KDNode *rchild, int tri_below)
	{
		this->aabb = aabb;
		this->p = p;
		this->lchild = lchild;
		this->rchild = rchild;
		this->tri_below = tri_below;
		isLeaf = false;
	}
	KDNode (std::vector<Face> f) {
		faces = f;
		lchild = rchild = NULL;
		isLeaf = true;
		this->tri_below = f.size ();
	}

	Voxel aabb;
	Plane p;
	KDNode *lchild, *rchild;
	int tri_below;
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
