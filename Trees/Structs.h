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
	Point3f (std::string s)
	{
		x = atof (substring (s, 0, s.find_first_of (' ') - 1).c_str ());
		y = atof (substring (s, s.find_first_of (' ') + 1, s.find_last_of (' ') - 1).c_str ());
		z = atof (substring (s, s.find_last_of (' ') + 1).c_str ());
	}
};