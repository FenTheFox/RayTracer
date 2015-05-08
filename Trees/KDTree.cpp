#include "stdafx.h"
#include "Structs.h"
#include "KDTree.h"

#define K_t 1
#define K_i 10

enum EVENT_TYPE
{
	PLANAR, START, END
};
struct Event
{
	Face t;
	float pos;
	EVENT_TYPE type;
};

std::pair<Plane, double> findSplit (Voxel v)
{
	std::vector<Plane> planes;
	std::vector<Event> E;
	for each (auto tri in v.triangles)
	{
		if (tri.v0.x == tri.v1.x && tri.v1.x == tri.v2.x)
			E.push_back (Event {tri, tri.v0.x, PLANAR});
		else {
			E.push_back (Event { tri, min (tri.v0.x, min (tri.v1.x, tri.v2.x)), START });
			E.push_back (Event { tri, max (tri.v0.x, max (tri.v1.x, tri.v2.x)), END });
		}
		if (tri.v0.y == tri.v1.y && tri.v1.y == tri.v2.y)
			E.push_back (Event { tri, tri.v0.y, PLANAR });
		else {
			E.push_back (Event { tri, min (tri.v0.y, min (tri.v1.y, tri.v2.y)), START });
			E.push_back (Event { tri, max (tri.v0.y, max (tri.v1.y, tri.v2.y)), END });
		}
		if (tri.v0.z == tri.v1.z && tri.v1.z == tri.v2.z)
			E.push_back (Event { tri, tri.v0.z, PLANAR });
		else {
			E.push_back (Event { tri, min (tri.v0.z, min (tri.v1.z, tri.v2.z)), START });
			E.push_back (Event { tri, max (tri.v0.z, max (tri.v1.z, tri.v2.z)), END });
		}
		/*auto aabb = Voxel::B (tri);
		planes.push_back (Plane { aabb.first, Point3f (1.0, 0.0, 0.0) });
		planes.push_back (Plane { aabb.first, Point3f (0.0, 1.0, 0.0) });
		planes.push_back (Plane { aabb.first, Point3f (0.0, 0.0, 1.0) });
		planes.push_back (Plane { aabb.second, Point3f (1.0, 0.0, 0.0) });
		planes.push_back (Plane { aabb.second, Point3f (0.0, 1.0, 0.0) });
		planes.push_back (Plane { aabb.second, Point3f (0.0, 0.0, 1.0) });*/
	}
	Plane best;
	double best_cost = CL_MAXFLOAT, cost;
	for each (Plane p in planes)
	{
		auto vlvr = v.split (p);
		cost = K_t + K_i*(vlvr.first.triangles.size ()*vlvr.first.surfaceArea () + vlvr.second.triangles.size ()*vlvr.second.surfaceArea ()) / v.surfaceArea ();
		if (cost < best_cost) {
			best = p;
			best_cost = cost;
		}
	}
	auto end = std::chrono::steady_clock::now ();
	return std::make_pair (best, best_cost);
}

bool done (std::vector<Face> faces, Voxel v)
{
	return findSplit (v).second > K_i*v.triangles.size ();
}

KDNode *KDTree::RecBuild (std::vector<Face> faces, Voxel v)
{
	if (done (faces, v)) return new KDLeaf (faces);
	//p = FindPlane (T, V) { Find a “good” plane p to split V }
	Plane p = findSplit (v).first;
	//(VL, VR) = Split V with p
	auto vlvr = v.split (p);
	//return new node (p, RecBuild (TL, VL), RecBuild (TR, VR))
	return new KDNode (p, RecBuild (vlvr.first.triangles, vlvr.first), RecBuild (vlvr.second.triangles, vlvr.second));
}


KDTree::KDTree (std::vector<Face> faces)
{
	Voxel v (faces);
	RecBuild (faces, v);
}


KDTree::KDTree ()
{}


KDTree::~KDTree ()
{}
