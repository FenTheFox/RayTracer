#include "stdafx.h"

#include "RayTracer.h"
#include "Material.h"
#include "Trees.h"

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
			if (tmp.x && tmp.y && dist < min_dist)
				return FALSE;
		}
	}
	return TRUE;
}

void Trees::generateTrees (double width, double height, int num, int age)
{
	vector<std::pair<Point2f, Tree>> gen_trees;
	//double cell_size = min_dist / sqrt (2), r, angle;
	//size_t gwidth = width / cell_size, gheight = height / cell_size;
	//auto grid = vector<vector<Point2f>> (gwidth, vector<Point2f> (gheight));
	//auto worklist = deque<Point2f> ();
	//default_random_engine gen;
	//uniform_real_distribution<double> r_dist (0, 2 * min_dist), angle_dist (0, 2 * M_PI), dist (0.0, 1.0);
	//Point2f pt = Point2f { dist (gen)*width, dist (gen)*height }, new_pt;

	//worklist.push_back (pt);
	//gen_trees.push_back (make_pair (pt, tree_objs[dist (gen)*tree_objs.size ()]));
	//grid[pt.x / cell_size][pt.y / cell_size] = pt;

	//while (!worklist.empty ()) {
	//	pt = worklist[0];
	//	worklist.pop_front ();
	//	random_shuffle (worklist.begin (), worklist.end ());
	//	for (size_t i = 0; i < num; i++) {
	//		r = r_dist (gen);
	//		angle = angle_dist (gen);
	//		new_pt = Point2f { pt.x + r*cos (angle), pt.y + r*sin (angle) };
	//		if (new_pt.x > 0 && new_pt.x < width &&
	//			 new_pt.y > 0 && new_pt.y < height &&
	//			 is_good (grid, new_pt, min_dist, cell_size)) {
	//			worklist.push_back (new_pt);
	//			gen_trees.push_back (make_pair (Point2f { new_pt.x - width / 2, new_pt.y - height / 2 }, tree_objs[dist (gen)*tree_objs.size ()]));
	//			grid[min (new_pt.x / cell_size, gwidth - 1)][min (new_pt.y / cell_size, gheight - 1)] = new_pt;
	//		}
	//	}
	//}

	//gen_trees.push_back (pair<Point2f, Tree> (Point2f { -min_dist, 0.0 }, Tree (tree_objs[0])));
	gen_trees.push_back (pair<Point2f, Tree> (Point2f { 0.0, 0.0 }, Tree (tree_objs[0])));
	//gen_trees.push_back (pair<Point2f, Tree> (Point2f { min_dist, 0.0 }, Tree (tree_objs[0])));
	min = { -1 * width, -1 * height, 0.0 };
	max = { width, height, 0.0 };
	for each (auto tp in gen_trees)
	{
		for each (auto m in tp.second.meshes)
		{
			vector<Face> mfaces (m._verts.size () / 3);
			for (size_t i = 0; i < m._verts.size () / 3; i++) {
				mfaces[i].v0 = Point3f (m._verts[i * 3].x + tp.first.x, m._verts[i * 3].y, m._verts[i * 3].z);
				mfaces[i].v1 = Point3f (m._verts[i * 3 + 1].x + tp.first.x, m._verts[i * 3 + 1].y, m._verts[i * 3 + 1].z);
				mfaces[i].v2 = Point3f (m._verts[i * 3 + 2].x + tp.first.x, m._verts[i * 3 + 2].y, m._verts[i * 3 + 2].z);
				mfaces[i].n0 = Point3f (m._normals[i * 3].x + tp.first.x, m._normals[i * 3].y, m._normals[i * 3].z);
				mfaces[i].n1 = Point3f (m._normals[i * 3 + 1].x + tp.first.x, m._normals[i * 3 + 1].y, m._normals[i * 3 + 1].z);
				mfaces[i].n2 = Point3f (m._normals[i * 3 + 2].x + tp.first.x, m._normals[i * 3 + 2].y, m._normals[i * 3 + 2].z);
				mfaces[i].mat = m.mat.id;
			}
			faces.insert (faces.end (), mfaces.begin (), mfaces.end ());
		}
	}

	for each (auto f in faces)
	{
		if (min (f.v0.x, min (f.v1.x, f.v2.x)) < min.x) min.x = min (f.v0.x, min (f.v1.x, f.v2.x));
		if (min (f.v0.y, min (f.v1.y, f.v2.y)) < min.y) min.y = min (f.v0.y, min (f.v1.y, f.v2.y));
		if (min (f.v0.z, min (f.v1.z, f.v2.z)) < min.z) min.z = min (f.v0.z, min (f.v1.z, f.v2.z));
		if (max (f.v0.x, max (f.v1.x, f.v2.x)) > max.x) max.x = max (f.v0.x, max (f.v1.x, f.v2.x));
		if (max (f.v0.y, max (f.v1.y, f.v2.y)) > max.y) max.y = max (f.v0.y, max (f.v1.y, f.v2.y));
		if (max (f.v0.z, max (f.v1.z, f.v2.z)) > max.z) max.z = max (f.v0.z, max (f.v1.z, f.v2.z));
	}

	Point3f scale (max (-1 * min.x / width, max.x / width), max (-1 * min.y / height, max.y / height), 1);
	if (!(scale.x <= 1 && scale.y <= 1 && scale.z <= 1)) {
		for each (auto f in faces)
		{
			f.v0 = f.v0 / scale;
			f.v1 = f.v1 / scale;
			f.v2 = f.v2 / scale;
		}
	}
}

vector<Material> Trees::parseTreeFile (string file)
{
	ifstream ifs (file);
	string line;

	vector<Material> mats;
	vector<Mesh> meshes;

	while (getline (ifs, line)) {
		if (line.find ("mtllib") != string::npos) {
			mats = Material::parseMaterials (substring (line, line.find_first_of (' ') + 1, line.size ()));
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

	for (size_t i = 0; i < meshes.size (); i++)
		for (size_t j = i + 1; j < meshes.size (); j++)
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

	for each (auto to in tree_objs)
	{
		pair<double, double> min, max;
		for each (Mesh m in t.meshes)
		{
			for each (auto v in m._verts)
			{
				if (v.x < min.first) min.first = v.x;
				if (v.x > max.first) max.first = v.x;
				if (v.z < min.second) min.second = v.z;
				if (v.z > max.second) max.second = v.z;
			}
		}
		if (max.first - min.first > min_dist) min_dist = max.first - min.first;
		if (max.second - min.second > min_dist) min_dist = max.second - min.second;
	}
	
	return mats;
}

Trees::Trees ()
{}

Trees::~Trees ()
{}