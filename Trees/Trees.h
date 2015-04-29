#pragma once

#include "resource.h"

struct Point2d
{
	double x, y;
};

struct Point3d
{
	double x, y, z;
};

class Trees
{
public:
	std::list<Point2d> points;
	Trees (double w, double h, double dist, int num);
	~Trees ();
};
