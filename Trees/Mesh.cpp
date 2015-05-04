#include "stdafx.h"

#include "Material.h"
#include "Mesh.h"

using namespace std;

std::vector<Point3d> Mesh::verts = std::vector<Point3d> ();
std::vector<Point3d> Mesh::normals = std::vector<Point3d> ();

bool Mesh::Merge (Mesh& other)
{
	if (name != other.name || mat.name != other.mat.name)
		return false;

	faces.insert (faces.end (), other.faces.begin (), other.faces.end ());
	return true;
}

Mesh::Mesh (ifstream& ifs, const vector<Material>& mats)
{
	bool done = false;
	string line;

	while (getline (ifs, line) && !(done && line == "")) {
		if (line.find ("vn") != string::npos)
			normals.emplace_back (line.substr (3));
		else if (line.find ("v") != string::npos)
			verts.emplace_back (line.substr (3));
		else if (line.find ("g") != string::npos)
			name = line.substr (2);
		else if (line.find ("usemtl") != string::npos) {
			for each (Material mat in mats)
				if (mat.name == line.substr (7))
					this->mat = mat;
		} else if (line.find ("s") != string::npos) {

		} else if (line.find ("f") != string::npos) {
			done = true;
			line = line.substr (line.find (" ") + 1);
			auto p1 = pair<int, int> (atoi (substring (line, 0, line.find ("//")).c_str ()), atoi (substring (line, line.find ("//") + 2, line.find (" ")).c_str ()));
			line = line.substr (line.find (" ") + 1);
			auto p2 = pair<int, int> (atoi (substring (line, 0, line.find ("//")).c_str ()), atoi (substring (line, line.find ("//") + 2, line.find (" ")).c_str ()));
			line = line.substr (line.find (" ") + 1);
			auto p3 = pair<int, int> (atoi (substring (line, 0, line.find ("//")).c_str ()), atoi (substring (line, line.find ("//") + 2, line.find (" ")).c_str ()));
			faces.push_back (make_tuple (p1, p2, p3));
		} else if (line == "") {
			continue;
		} else {
			exit (1001);
		}
	}
}

Mesh::~Mesh ()
{}
