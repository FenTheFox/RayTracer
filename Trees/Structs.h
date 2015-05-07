#ifndef __OPENCL_C_VERSION__
#pragma once
struct Point2f
{
	float x, y;
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
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point3f (std::string s)
	{
		x = atof (substring (s, 0, s.find_first_of (' ')).c_str ());
		y = atof (substring (s, s.find_first_of (' ') + 1, s.find_last_of (' ')).c_str ());
		z = atof (substring (s, s.find_last_of (' ') + 1, s.length()).c_str ());
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
#else
#define cl_float4 float4
#define cl_float float
#define cl_int int
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
	cl_int type;
};

struct Face
{
	cl_float4 v0;
	cl_float4 v1;
	cl_float4 v2;
	cl_float4 n0;
	cl_float4 n1;
	cl_float4 n2;
	cl_int mat;
};

struct Mat_Struct
{
	cl_float4 Ka;
	cl_float4 Kd;
	cl_float4 Ks;
	cl_float phong;
};
#pragma pack()