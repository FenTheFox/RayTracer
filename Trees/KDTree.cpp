#include "stdafx.h"
#include "Structs.h"
#include "KDTree.h"

#define K_t 1
#define K_i 1.2

enum EVENT_TYPE
{
	END, PLANAR, START
};
struct Event
{
	float pos;
	EVENT_TYPE type;

	bool operator< (Event e)
	{
		return (pos < e.pos || (pos == e.pos && type < e.type));
	}
};

template <DIMENTION k>
bool minFace (Face pl, Face pr)
{
	return min (pl.v0.s[k], min (pl.v1.s[k], pl.v2.s[k])) < min (pr.v0.s[k], min (pr.v1.s[k], pr.v2.s[k]));
}
template <DIMENTION k>
bool maxFace (Face pl, Face pr)
{
	return max (pl.v0.s[k], max (pl.v1.s[k], pl.v2.s[k])) < max (pr.v0.s[k], max (pr.v1.s[k], pr.v2.s[k]));
}

template <DIMENTION k>
double fmin (Face f)
{
	return min (f.v0.s[k], min (f.v1.s[k], f.v2.s[k]));
}
template <DIMENTION k>
double fmax (Face f)
{
	return max (f.v0.s[k], max (f.v1.s[k], f.v2.s[k]));
}

std::pair<double, Side> SAH (Voxel v, Plane p, int Nl, int Nr, int Np)
{
	auto vlvr = v.split (p);
	float Pl = vlvr.first.surfaceArea () / v.surfaceArea (), Pr = vlvr.second.surfaceArea () / v.surfaceArea (), Cl, Cr;
	Cl = K_t + K_i*(Pl*(Nl + Np) + Pr*Nr);
	Cr = K_t + K_i*(Pl*Nl + Pr*(Nr + Np));
	if (Nl == 0)
		Cr *= 0.8;
	if (Nr == 0)
		Cl *= 0.8;
	if (Cl <= Cr)
		return std::make_pair (Cl, LEFT);
	else
		return std::make_pair (Cr, RIGHT);
}

std::pair<Plane, double> findSplit (std::vector<Face> triangles, Voxel v)
{
	std::pair<Plane, double> ret { Plane {}, CL_MAXFLOAT };
	for (size_t k = 0; k < 3; k++) {
		Plane split;
		std::vector<Event> E;

		if (k == 0)
			split.normal.x = 1;
		else if (k == 1)
			split.normal.y = 1;
		else if (k == 2)
			split.normal.z = 1;

		for each (auto tri in triangles)
		{
			if (tri.v0.s[k] == tri.v1.s[k] && tri.v1.s[k] == tri.v2.s[k])
				E.push_back (Event { tri.v0.s[k], PLANAR });
			else {
				E.push_back (Event { min (tri.v0.s[k], min (tri.v1.s[k], tri.v2.s[k])), START });
				E.push_back (Event { max (tri.v0.s[k], max (tri.v1.s[k], tri.v2.s[k])), END });
			}
		}

		std::sort (E.begin (), E.end ());

		float pos;
		int Nl = 0, Np = 0, Nr = triangles.size (), pstart, pend, pplanar;
		for (size_t i = 0; i < E.size ();) {
			pos = E[i].pos;
			pstart = pend = pplanar = 0;

			if (k == 0)
				split.pt.x = pos;
			else if (k == 1)
				split.pt.y = pos;
			else if (k == 2)
				split.pt.z = pos;

			while (i < E.size () && E[i].pos == pos && E[i].type == END) {
				pend++; i++;
			}
			while (i < E.size () && E[i].pos == pos && E[i].type == PLANAR) {
				pplanar++; i++;
			}
			while (i < E.size () && E[i].pos == pos && E[i].type == START) {
				pstart++; i++;
			}
			//move plane onto split
			Np = pplanar;
			Nr -= pend - pplanar;
			auto p = SAH (v, split, Nl, Nr, Np);
			if (p.first < ret.second) {
				ret.first = split;
				ret.first.s = p.second;
				ret.second = p.first;
			}
			//move plane over split
			Nl += pstart + pplanar;
			Np = 0;
		}
	}
	return ret;
}

KDNode *KDTree::RecBuild (std::vector<Face> faces, Voxel v)
{
	//p = FindPlane (T, V) { Find a “good” plane p to split V }
	auto split = findSplit (faces, v);
	if (split.second > 0.8*K_i*faces.size ())
		return new KDLeaf (faces);
	//(VL, VR) = Split V with p
	auto vlvr = v.split (split.first);
	//return new node (p, RecBuild (TL, VL), RecBuild (TR, VR))
	std::vector<Face> tl = vlvr.first.intersect (faces), tr = vlvr.second.intersect (faces);
	return new KDNode (split.first, RecBuild (tl, vlvr.first), RecBuild (tr, vlvr.second));
}

KDTree::KDTree (std::vector<Face> faces)
{
	Voxel v { fmin<X> (*std::min_element (faces.begin (), faces.end (), minFace<X>)), fmin<Y> (*std::min_element (faces.begin (), faces.end (), minFace<Y>)), fmin<Z> (*std::min_element (faces.begin (), faces.end (), minFace<Z>)),
		fmax<X> (*std::max_element (faces.begin (), faces.end (), maxFace<X>)), fmax<Y> (*std::max_element (faces.begin (), faces.end (), maxFace<Y>)), fmax<Z> (*std::max_element (faces.begin (), faces.end (), maxFace<Z>))
	};
	head = RecBuild (faces, v);
	std::cout << std::endl;
}


KDTree::KDTree ()
{}


KDTree::~KDTree ()
{}
