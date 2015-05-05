#pragma once

#include "resource.h"
#include "Mesh.h"

class CRayTracer;

struct Tree
{
	std::string name;
	std::vector<Mesh> meshes;
};

class Trees
{
	std::vector<Tree> tree_objs;
public:
	Trees ();
	~Trees ();

	void parseTreeFile (std::string, CRayTracer *);
	void generateTrees (double w, double h, double dist, int num, int age);

	std::vector<std::pair<Point2d, Tree>> trees;
};