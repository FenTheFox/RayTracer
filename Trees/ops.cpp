#include "stdafx.h"
#include "Structs.h"

std::string trim (std::string str, std::string whitespace)
{
	auto strBegin = str.find_first_not_of (whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	auto strEnd = str.find_last_not_of (whitespace);
	auto strRange = strEnd - strBegin + 1;

	return str.substr (strBegin, strRange);
}

std::string substring (std::string str, size_t start, size_t end)
{
	return str.substr (start, end - start);
}

cl_float4 operator/(const cl_float4 clf, const float f)
{
	cl_float4 res;
	res.w = clf.w / f;
	res.x = clf.x / f;
	res.y = clf.y / f;
	res.z = clf.z / f;
	return res;
}

cl_float4 operator*(const cl_float4 clf, const float f)
{
	cl_float4 res;
	res.w = clf.w * f;
	res.x = clf.x * f;
	res.y = clf.y * f;
	res.z = clf.z * f;
	return res;
}

cl_float4 operator*=(cl_float4 clf, const float f)
{
	clf.w = clf.w * f;
	clf.x = clf.x * f;
	clf.y = clf.y * f;
	clf.z = clf.z * f;
	return clf;
}

cl_float4 operator*(const cl_float4 clf, const cl_float4 clf2)
{
	cl_float4 res;
	res.w = clf.w * clf2.w;
	res.x = clf.x * clf2.x;
	res.y = clf.y * clf2.y;
	res.z = clf.z * clf2.z;
	return res;
}


cl_float4 operator+(const cl_float4 clf, const cl_float4 clf2)
{
	cl_float4 res;
	res.w = clf.w + clf2.w;
	res.x = clf.x + clf2.x;
	res.y = clf.y + clf2.y;
	res.z = clf.z + clf2.z;
	return res;
}


cl_float4 operator/(const cl_float4 clf, const Point3f p)
{
	cl_float4 res;
	res.w = clf.w / 1;
	res.x = clf.x / p.x;
	res.y = clf.y / p.y;
	res.z = clf.z / p.z;
	return res;
}

cl_float4 operator*=(cl_float4 clf, const cl_float4 f)
{
	clf.w = clf.w * f.w;
	clf.x = clf.x * f.x;
	clf.y = clf.y * f.y;
	clf.z = clf.z * f.z;
	return clf;
}

cl_float4 sqrt (cl_float4 clf)
{
	cl_float4 res;
	res.w = sqrt(clf.w);
	res.x = sqrt (clf.x);
	res.y = sqrt (clf.y);
	res.z = sqrt (clf.z);
	return res;
}

Point3f operator/(const Point3f p1, const Point3f p2)
{
	return Point3f (p1.x / p2.x, p1.y / p2.y, p1.z / p2.z);
}