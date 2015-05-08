#ifndef __OPENCL_C_VERSION__
#pragma once
#include <math.h>
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

	Point3f () {}
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

	operator cl_float4() const
	{
		cl_float4 fl;
		fl.x = x;
		fl.y = y;
		fl.z = z;
		return fl;
	}
};

struct Plane
{
	Point3f pt, normal;
};

struct Voxel
{
	static std::pair<Point3f, Point3f> B (Face t)
	{
		std::vector<Face> tri (1);
		tri[0] = t;
		Voxel v (tri);
		return std::make_pair (Point3f (v.minx, v.miny, v.minz), Point3f (v.maxx, v.maxy, v.maxz));
	}

	double minx, miny, minz, maxx, maxy, maxz;

	Voxel () {}
	Voxel (std::vector<Face> triangles)
	{
		minx = maxx = triangles[0].v0.x;
		miny = maxy = triangles[0].v0.y;
		minz = maxz = triangles[0].v0.z;
		for each (auto t in triangles)
		{
			if (min (t.v0.x, min (t.v1.x, t.v2.x)) < minx) minx = min (t.v0.x, min (t.v1.x, t.v2.x));
			if (min (t.v0.y, min (t.v1.y, t.v2.y)) < miny) miny = min (t.v0.y, min (t.v1.y, t.v2.y));
			if (min (t.v0.z, min (t.v1.z, t.v2.z)) < minz) minz = min (t.v0.z, min (t.v1.z, t.v2.z));
			if (max (t.v0.x, max (t.v1.x, t.v2.x)) > maxx) maxx = max (t.v0.x, max (t.v1.x, t.v2.x));
			if (max (t.v0.y, max (t.v1.y, t.v2.y)) > maxy) maxy = max (t.v0.y, max (t.v1.y, t.v2.y));
			if (max (t.v0.z, max (t.v1.z, t.v2.z)) > maxz) maxz = max (t.v0.z, max (t.v1.z, t.v2.z));
		}
		this->triangles = triangles;
	}

	double surfaceArea ()
	{
		return 2 * ((maxx - minx)*(maxy - miny) + (maxx - minx)*(maxz - minz) + (maxy - miny)*(maxz - minz));
	}

	std::pair<Voxel, Voxel> split (Plane p)
	{
		//l is side with p.normal
		std::vector<Face> l (triangles.size ()), r (triangles.size ());
		for each (auto t in triangles)
		{
			if (p.normal.dot (t.v0) > 0 && p.normal.dot (t.v1) > 0 && p.normal.dot (t.v2) > 0) {
				l.push_back (t);
			} else if (p.normal.dot (t.v0) < 0 && p.normal.dot (t.v1) < 0 && p.normal.dot (t.v2) < 0) {
				r.push_back (t);
			} else {
				r.push_back (t); l.push_back (t);
			}
		}
		return std::make_pair (Voxel (r), Voxel (l));
	}

	bool intersect (Face tri)
	{
		double tminx = min (tri.v0.x, min (tri.v1.x, tri.v2.x)), tminy = min (tri.v0.y, min (tri.v1.y, tri.v2.y)), tminz = min (tri.v0.z, min (tri.v1.z, tri.v2.z)),
			tmaxx = max (tri.v0.x, max (tri.v1.x, tri.v2.x)), tmaxy = max (tri.v0.y, max (tri.v1.y, tri.v2.y)), tmaxz = max (tri.v0.z, max (tri.v1.z, tri.v2.z));
		return tminx > maxx || tmaxx < minx || tminy > maxy || tmaxy < minx || tminz > maxz || tmaxz < minx;
	}

	std::vector<Face> triangles;
};
#endif