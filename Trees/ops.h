//
//  stringops.h
//  sqlite-test
//
//  Created by Timm Allman on 10/22/14.
//  Copyright (c) 2014 UMass. All rights reserved.
//
#pragma once
struct Point3f;
struct Face;

std::string trim (std::string, std::string = " \t");
std::string substring (std::string, size_t, size_t);

enum DIMENTION {X, Y, Z};
template <DIMENTION c>
bool comp (Face f1, Face f2)
{
	return min (f1.v0.s[c], min (f1.v2.s[c], f1.v2.s[c])) < min (f2.v0.s[c], min (f2.v2.s[c], f2.v2.s[c]));
}

cl_float4 operator/(const cl_float4, const float);
cl_float4 operator*(const cl_float4 clf, const float f);
cl_float4 operator*(const cl_float4 clf, const cl_float4 clf2);
cl_float4 operator+(const cl_float4 clf, const cl_float4 clf2);
cl_float4 operator-(const cl_float4 clf, const cl_float4 clf2);
cl_float4 operator*=(const cl_float4 clf, const float f);
cl_float4 operator*=(const cl_float4 clf, const cl_float4 f);
cl_float4 sqrt (cl_float4);

cl_float4 operator/(const cl_float4, const Point3f);
Point3f operator/(const Point3f, const Point3f);