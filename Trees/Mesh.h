#pragma once

#include "Structs.h"
#include "Material.h"

class Mesh
{
public:
	Mesh (std::ifstream&, const std::vector<Material>&);
	~Mesh ();
	bool Merge (Mesh&);

	std::string name;
	Material mat;
	std::vector<std::tuple<std::pair<int, int>, std::pair<int, int>, std::pair<int, int>>> faces;

	static std::vector<Point3d> verts, normals;
};