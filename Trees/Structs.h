#ifndef __OPENCL_C_VERSION__
#pragma once
#include <math.h>
#define float4 cl_float4
#else
#define cl_float4 float4
#define cl_float float
#define cl_int int
#define cl_uint uint
typedef struct Ray Ray;
typedef struct Light Light;
typedef struct Face Face;
typedef struct Mat_Struct Mat_Struct;
#endif

struct Ray
{
	cl_float4 pos;
	cl_float4 dir;
};

#pragma pack(16)
struct Light
{
	cl_float4 pos;
	cl_float4 dir;
	cl_float4 intensity;
	cl_uint type;
	cl_int pad[3];
};

struct Face
{
	cl_float4 v0;
	cl_float4 v1;
	cl_float4 v2;
	cl_float4 n0;
	cl_float4 n1;
	cl_float4 n2;
	cl_uint mat;
	cl_int pad[7];
};

struct Mat_Struct
{
	cl_float4 Ka;
	cl_float4 Kd;
	cl_float4 Ks;
	cl_float phong;
	cl_float pad[3];
};
#pragma pack()
#define DOT(v1,v2) (v1.x*v2.x+v1.y*v2.y+v1.z*v2.z)

typedef struct Hit
{
	float t;
	float4 pos;
	float4 norm;
	int idx;
} Hit;

#ifndef __OPENCL_C_VERSION__
struct Point2f
{
	float x, y;
	Point2f ()
	{
		x = 0;
		y = 0;
	}
	Point2f (double x, double y)
	{
		this->x = (float)x;
		this->y = (float)y;
	}
};

struct Point3f
{
	float x, y, z;

	Point3f () {
		x = y = z = 0;
	}
	Point3f (float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point3f (double x, double y, double z)
	{
		this->x = (float)x;
		this->y = (float)y;
		this->z = (float)z;
	}
	Point3f (std::string s)
	{
		x = (float)atof (substring (s, 0, s.find_first_of (' ')).c_str ());
		y = (float)atof (substring (s, s.find_first_of (' ') + 1, s.find_last_of (' ')).c_str ());
		z = (float)atof (substring (s, s.find_last_of (' ') + 1, s.length ()).c_str ());
	}

	float dot (Point3f p)
	{
		return x * p.x + y * p.y + z * p.z;
	}

	float dot (cl_float4 pt)
	{
		return x * pt.x + y * pt.y + z * pt.z;
	}

	Point3f operator*(const Point3f pt)
	{
		return { x*pt.x, y*pt.y, z*pt.z };
	}

	Point3f operator*(const float f)
	{
		return { x*f, y*f, z*f };
	}

	Point3f operator+(const Point3f pt)
	{
		return { x+pt.x, y+pt.y, z+pt.z };
	}

	Point3f operator-(const cl_float4 pt)
	{
		return { x - pt.x, y - pt.y, z - pt.z };
	}

	operator cl_float4() const
	{
		cl_float4 fl;
		fl.x = x;
		fl.y = y;
		fl.z = z;
		return fl;
	}
};

enum Side
{
	LEFT, RIGHT
};
struct Plane
{
	Point3f pt, normal;
	Side s;

	Hit *intersect (Ray r, float tmin, float tmax)
	{
		Point3f tmp (pt);
		tmp = tmp - r.pos;
		float denom = DOT(r.dir, normal), t;
		if (denom == 0.0f) return NULL;
		t = DOT(tmp, normal);
		if (t < tmin || t > tmax) return NULL;
		Hit *h = new Hit ();
		h->pos = r.dir * t + r.pos;
		h->t = t;
	}
};

struct Voxel
{
	static std::pair<Point3f, Point3f> B (Face t)
	{
		return std::make_pair (
			Point3f (min (t.v0.x, min (t.v1.x, t.v2.x)), min (t.v0.y, min (t.v1.y, t.v2.y)), min (t.v0.z, min (t.v1.z, t.v2.z))),
			Point3f (max (t.v0.x, max (t.v1.x, t.v2.x)), max (t.v0.y, max (t.v1.y, t.v2.y)), max (t.v0.z, max (t.v1.z, t.v2.z))));
	}

	double minx, miny, minz;
	double maxx, maxy, maxz;

	double surfaceArea ()
	{
		return 2 * ((maxx - minx)*(maxy - miny) + (maxx - minx)*(maxz - minz) + (maxy - miny)*(maxz - minz));
	}

	std::pair<Voxel, Voxel> split (Plane p)
	{
		Voxel vl = { minx, miny, minz, maxx, maxy, maxz };
		Voxel vr = { minx, miny, minz, maxx, maxy, maxz };
		if (p.normal.x == 1) {
			vl.maxx = p.pt.x;
			vr.minx = p.pt.x;
		} else if (p.normal.y == 1) {
			vl.maxy = p.pt.y;
			vr.miny = p.pt.y;
		} else {
			vl.maxz = p.pt.z;
			vr.minz = p.pt.z;
		}

		return std::make_pair (vl, vr);
	}

	bool intersect (Face tri)
	{
		auto aabb = Voxel::B(tri);
		return ((aabb.first.x < maxx && aabb.first.x > minx) || (aabb.second.x > minx && aabb.second.x < maxx)) &&
			((aabb.first.y < maxy && aabb.first.y > miny) || (aabb.second.y > miny && aabb.second.y < maxy)) &&
			((aabb.first.z < maxz && aabb.first.z > minz) || (aabb.second.z > minz && aabb.second.z < maxz));
	}

	std::vector<Face> intersect (std::vector<Face> triangles)
	{
		std::vector<Face> ret;
		for each (auto f in triangles)
			if (intersect (f))
				ret.push_back (f);
		return ret;
	}

	std::pair<Hit *, Hit *>intersect (Ray r, float tmin, float tmax)
	{
		Plane pminx { Point3f { minx, 0, 0 }, Point3f { 1.0, 0.0, 0.0 } }, pminy { Point3f { 0, miny, 0 }, Point3f { 0.0, 1.0, 0.0 } }, pminz { Point3f { 0, 0, minz }, Point3f { 0.0, 0.0, 1.0 } },
			pmaxx { Point3f { maxx, 0, 0 }, Point3f { 1.0, 0.0, 0.0 } }, pmaxy { Point3f { 0, maxy, 0 }, Point3f { 0.0, 1.0, 0.0 } }, pmaxz { Point3f { 0, 0, maxz }, Point3f { 0.0, 0.0, 1.0 } };
		Hit *hits[6], *closest, *farthest;
		hits[0] = pminx.intersect (r, tmin, tmax);
		hits[1] = pminy.intersect (r, tmin, tmax);
		hits[2] = pminz.intersect (r, tmin, tmax);
		hits[3] = pmaxx.intersect (r, tmin, tmax);
		hits[4] = pmaxy.intersect (r, tmin, tmax);
		hits[5] = pmaxz.intersect (r, tmin, tmax);

		closest = hits[0];
		for (size_t i = 1; i < 6; i++)
			if (closest == NULL || (hits[i] != NULL && hits[i]->t < closest->t))
				closest = hits[i];
		farthest = hits[0];
		for (size_t i = 1; i < 6; i++)
			if (farthest == NULL || (hits[i] != NULL && hits[i]->t > farthest->t))
				farthest = hits[i];
		return std::make_pair(closest, farthest);
	}
};
#endif