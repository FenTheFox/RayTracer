#include "stdafx.h"

#include "Trees.h"
#include "RandomQueue.h"

//Read about Poisson disk sampling and dart throwing -- simple idea to generate visually pleasing distribution patterns. Also think about using Autodesk 123D to capture real tree mdels
using namespace std;

bool is_good (vector<vector<Point2d>> grid, Point2d pt, double min_dist, double cell_size)
{
	int x = ceil (pt.x / cell_size), y = ceil (pt.y / cell_size), w = grid.size (), h = grid[0].size ();
	double dist;
	Point2d tmp;
	for (int i = x - 2; i <= x + 2 && i < w; i++) {
		for (int j = y - 2; j <= y + 2 && j < h; j++) {
			if (i < 0 || j < 0) continue;
			tmp = grid[i][j];
			dist = sqrt (pow (tmp.x - pt.x, 2) + pow (tmp.y - pt.y, 2));
			if ((tmp.x && tmp.y) && dist < min_dist)
				return FALSE;
		}
	}
	return TRUE;
}

Trees::Trees (double width, double height, double min_dist, int num)
{
	double cell_size = min_dist / sqrt (2), gwidth = width / cell_size, gheight = height / cell_size, r, angle;
	auto grid = vector<vector<Point2d>> (gwidth, vector<Point2d> (gheight));
	auto worklist = deque<Point2d> ();
	default_random_engine gen;
	uniform_real_distribution<double> r_dist (0, 2), angle_dist (0, 2 * M_PI), dist (0.0, 1.0);
	Point2d pt = Point2d { dist (gen)*width, dist (gen)*height }, new_pt;

	worklist.push_back (pt);
	points.push_back (pt);
	grid[pt.x / cell_size][pt.y / cell_size] = pt;

	while (!worklist.empty ()) {
		pt = worklist[0];
		worklist.pop_front ();
		random_shuffle (worklist.begin (), worklist.end ());
		for (size_t i = 0; i < num; i++) {
			r = r_dist (gen);
			angle = angle_dist (gen);
			new_pt = Point2d { pt.x + r*cos (angle), pt.y + r*sin (angle) };
			if (new_pt.x > 0 && new_pt.x < width &&
				 new_pt.y > 0 && new_pt.y < height &&
				 is_good (grid, new_pt, min_dist, cell_size)) {
				worklist.push_back (new_pt);
				points.push_back (new_pt);
				grid[min (new_pt.x / cell_size, gwidth - 1)][min (new_pt.y / cell_size, gheight - 1)] = new_pt;
			}
		}
	}
}

Trees::~Trees ()
{}