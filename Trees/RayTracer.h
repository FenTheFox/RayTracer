#pragma once

#include "Structs.h"
#include "Material.h"
#include "KDTree.h"

class RayTracer {
	int width, height, depth;
	std::string kernel_file;

	Face *faceBuff;
	KDTree kdtree;
	Mat_Struct *materialBuff;
	int numFace, numMaterial;

	void makeRays (Ray *);
public:
	RayTracer(void);
	RayTracer (int width, int height);

	void setMatBuffer (std::vector<Material>);
	void setFaceBuffer (std::vector<Face>);
	void raytrace (cl_float4 *out_buff);
};