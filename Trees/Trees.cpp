#include "stdafx.h"

#include "RayTracer.h"
#include "Material.h"
#include "Trees.h"
#include "stringops.h"

//Read about Poisson disk sampling and dart throwing -- simple idea to generate visually pleasing distribution patterns. Also think about using Autodesk 123D to capture real tree mdels
using namespace std;

bool is_good (vector<vector<Point2f>> grid, Point2f pt, double min_dist, double cell_size)
{
	int x = ceil (pt.x / cell_size), y = ceil (pt.y / cell_size), w = grid.size (), h = grid[0].size ();
	double dist;
	Point2f tmp;
	for (int i = x - 2; i <= x + 2 && i < w; i++) {
		for (int j = y - 2; j <= y + 2 && j < h; j++) {
			if (i < 0 || j < 0) continue;
			tmp = grid[i][j];
			dist = sqrt (pow (tmp.x - pt.x, 2) + pow (tmp.y - pt.y, 2));
			if ((tmp.x && tmp.y) && dist < min_dist)
				return FALSE;
		}
	}
	return TRUE;
}

void Trees::generateTrees (double width, double height, double min_dist, int num, int age)
{
	trees.push_back (pair<Point2f, Tree> (Point2f (), tree_objs[0]));
	return;

	double cell_size = min_dist / sqrt (2), gwidth = width / cell_size, gheight = height / cell_size, r, angle;
	auto grid = vector<vector<Point2f>> (gwidth, vector<Point2f> (gheight));
	auto worklist = deque<Point2f> ();
	default_random_engine gen;
	uniform_real_distribution<double> r_dist (0, 2), angle_dist (0, 2 * M_PI), dist (0.0, 1.0);
	Point2f pt = Point2f { dist (gen)*width, dist (gen)*height }, new_pt;

	worklist.push_back (pt);
	trees.push_back (make_pair (pt, tree_objs[dist (gen)*tree_objs.size ()]));
	grid[pt.x / cell_size][pt.y / cell_size] = pt;

	while (!worklist.empty ()) {
		pt = worklist[0];
		worklist.pop_front ();
		random_shuffle (worklist.begin (), worklist.end ());
		for (size_t i = 0; i < num; i++) {
			r = r_dist (gen);
			angle = angle_dist (gen);
			new_pt = Point2f { pt.x + r*cos (angle), pt.y + r*sin (angle) };
			if (new_pt.x > 0 && new_pt.x < width &&
				 new_pt.y > 0 && new_pt.y < height &&
				 is_good (grid, new_pt, min_dist, cell_size)) {
				worklist.push_back (new_pt);
				trees.push_back (make_pair (new_pt, tree_objs[dist (gen)*tree_objs.size ()]));
				grid[min (new_pt.x / cell_size, gwidth - 1)][min (new_pt.y / cell_size, gheight - 1)] = new_pt;
			}
		}
	}
}

void Trees::parseTreeFile (string file, CRayTracer *rt)
{
	ifstream ifs (file);
	string line;

	vector<Material> mats;
	vector<Mesh> meshes;

	while (getline(ifs,line)) {
		if (line.find ("mtllib") != string::npos) {
			mats = Material::parseMaterials (substring (line, line.find_first_of (' ') + 1));
			break;
		}
	}

	while (getline (ifs, line, 'v')) {
		if (ifs.good ()) {
			ifs.putback ('v');
			Mesh m (ifs, mats);
			meshes.push_back (m);
		}
	}

	for (size_t i = 0; i < meshes.size(); i++)
		for (size_t j = i + 1; j < meshes.size(); j++)
			if (meshes[i].Merge (meshes[j]))
				meshes.erase (meshes.begin () + j);

	for (size_t i = 0; i < meshes.size (); i++) {
		for (size_t j = 0; j < meshes[i].faces.size (); j++) {
			pair<int, int> p1, p2, p3;
			std::tie (p1, p2, p3) = meshes[i].faces[j];
			meshes[i]._verts.push_back (Mesh::verts[p1.first - 1]);
			meshes[i]._verts.push_back (Mesh::verts[p2.first - 1]);
			meshes[i]._verts.push_back (Mesh::verts[p3.first - 1]);
			meshes[i]._normals.push_back (Mesh::normals[p1.second - 1]);
			meshes[i]._normals.push_back (Mesh::normals[p2.second - 1]);
			meshes[i]._normals.push_back (Mesh::normals[p3.second - 1]);
		}
	}
		
	Tree t;
	for each (Mesh m in meshes)
	{
		if (t.name != m.name) {
			tree_objs.push_back (t);
			t = Tree ();
			t.name = m.name;
		}
		t.meshes.push_back (m);
	}
	tree_objs[0] = t;

	rt->setMatBuffer (CRayTracer::MATERIAL_BUFF, mats);
}

Trees::Trees ()
{
}

Trees::~Trees ()
{}