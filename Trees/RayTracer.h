#pragma once

#include "Structs.h"
#include "Material.h"

class CRayTracer {
	int width, height, depth;
	std::string kernel_file;

	Face *faceBuff;
	Mat_Struct *materialBuff;
	int numFace, numMaterial;

	void makeRays (Ray *);
public:
	CRayTracer(void);
	CRayTracer (int width, int height);

	void setMatBuffer (std::vector<Material>);
	void setFaceBuffer (std::vector<Face>);
	void raytrace (cl_float4 *out_buff);
};