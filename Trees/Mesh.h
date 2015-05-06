#pragma once

#include "Structs.h"
#include "Material.h"

class Mesh
{
public:
	Mesh (std::ifstream&, const std::vector<Material>&);
	Mesh ();
	~Mesh ();
	bool Merge (Mesh&);

	std::string name;
	Material mat;
	std::vector<std::tuple<std::pair<int, int>, std::pair<int, int>, std::pair<int, int>>> faces;
	std::vector<Point3f> _verts, _normals;
	static std::vector<Point3f> verts, normals;
};