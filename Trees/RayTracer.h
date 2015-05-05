#pragma once

#include "Structs.h"
#include "Material.h"

class Mesh;

class CRayTracer {
	int width, height;
	std::string kernel_file;

	cl_int8 *obj_buff;
	cl_float3 *vert_buf, *norm_buf;
	cl_float16 *mat_buf;
	int obj_len, vert_len, norm_len, mat_len;
public:
	CRayTracer(void);
	CRayTracer (int width, int height);

	static enum BUFF { VERTEX_BUFF, NORMAL_BUFF, MATERIAL_BUFF, OBJECT_BUFF };
	static void createPointBuffer (BUFF b, std::vector<Point3d> pts, int *len, cl_float3 **buff);
	static void createMatBuffer (BUFF b, std::vector<Material> mats, int *len, cl_float16 **buff);

	void initBuffs (int num_objs)
	{
		obj_buff = new cl_int8[num_objs];
		vert_buf = new cl_float3[num_objs];
		norm_buf = new cl_float3[num_objs];
	}
	void setPointBuffer (BUFF b, std::vector<Point3d> in);
	void setMatBuffer (BUFF b, std::vector<Material> in);
	void setObjBuffer (BUFF b, Mesh m);
	void raytrace (cl_float4 *out_buff);
};