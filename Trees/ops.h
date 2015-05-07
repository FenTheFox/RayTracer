//
//  stringops.h
//  sqlite-test
//
//  Created by Timm Allman on 10/22/14.
//  Copyright (c) 2014 UMass. All rights reserved.
//
#pragma once

std::string trim (std::string, std::string = " \t");

std::string substring (std::string, size_t, size_t);

cl_float4 operator/(const cl_float4, const float);
cl_float4 operator*(const cl_float4 clf, const float f);
cl_float4 operator*(const cl_float4 clf, const cl_float4 clf2);
cl_float4 operator+(const cl_float4 clf, const cl_float4 clf2);
cl_float4 operator*=(const cl_float4 clf, const float f);
cl_float4 operator*=(const cl_float4 clf, const cl_float4 f);
cl_float4 sqrt (cl_float4);


struct Point3f;
cl_float4 operator/(const cl_float4, const Point3f);
Point3f operator/(const Point3f, const Point3f);