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
	double min_dist;
	std::list<Point2d> points;
public:
	Trees (int w, int h, double dist, int num);
	~Trees ();
};
