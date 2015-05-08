#pragma once

#include "resource.h"
#include "Mesh.h"

struct Tree
{
	std::string name;
	std::vector<Mesh> meshes;
};

class Trees
{
	std::vector<Tree> tree_objs;
	Point3f min, max;
	double min_dist;
public:
	Trees ();
	~Trees ();

	std::vector<Material> parseTreeFile (std::string);
	void generateTrees (double w, double h, int num, int age);

	std::vector<Tree> trees;
	std::vector<Face> faces;
};