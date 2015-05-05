#pragma once

struct Point2d
{
	double x, y;
};

struct Point3d
{
	double x, y, z;
	Point3d () {}
	Point3d (double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Point3d (std::string s)
	{
		x = atof (substring (s, 0, s.find_first_of (' ') - 1).c_str ());
		y = atof (substring (s, s.find_first_of (' ') + 1, s.find_last_of (' ') - 1).c_str ());
		z = atof (substring (s, s.find_last_of (' ') + 1).c_str ());
	}
};