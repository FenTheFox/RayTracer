#include "stdafx.h"
#include "Structs.h"

#define float4 cl_float4

typedef struct Hit
{
	float t;
	float4 pos;
	float4 norm;
} Hit;

Hit hit_shape (Face f, Ray ray, float tmax)
{
	Hit h = {};
	float x[3] = { ray.dir.x, ray.dir.y, ray.dir.z },
		y[3] = { f.v2.x - f.v0.x, f.v2.y - f.v0.y, f.v2.z - f.v0.z },
		z[3] = { f.v2.x - f.v1.x, f.v2.y - f.v1.y, f.v2.z - f.v1.z },
		d[3] = { f.v2.x - ray.pos.x, f.v2.y - ray.pos.y, f.v2.z - ray.pos.z };
	float det = x[0] * y[1] * z[2] + x[1] * y[2] * z[0] + x[2] * y[0] * z[1] - z[0] * y[1] * x[2] - y[0] * x[1] * z[2] - x[0] * z[1] * y[2],
		t = d[0] * y[1] * z[2] + d[1] * y[2] * z[0] + d[2] * y[0] * z[1] - z[0] * y[1] * d[2] - y[0] * d[1] * z[2] - d[0] * z[1] * y[2],
		a = x[0] * d[1] * z[2] + x[1] * d[2] * z[0] + x[2] * d[0] * z[1] - z[0] * d[1] * x[2] - d[0] * x[1] * z[2] - x[0] * z[1] * d[2],
		b = x[0] * y[1] * d[2] + x[1] * y[2] * d[0] + x[2] * y[0] * d[1] - d[0] * y[1] * x[2] - y[0] * x[1] * d[2] - x[0] * d[1] * y[2];

	if (det == 0) return h;

	t = t / det;
	a = a / det;
	b = b / det;

	if (t < 0 || t < 0.00001f || t > tmax || a < 0 || b < 0 || a + b > 1) return h;

	f.n0 *= a;
	f.n1 *= b;
	f.n2 *= 1 - a - b;

	h.t = t;
	h.pos = ray.dir*t + ray.pos;
	h.norm = f.n0 + f.n1 + f.n2;
	h.norm *= sqrt (f.n0*f.n0 + f.n1*f.n1 + f.n2*f.n2);
	return h;
}

void main (Ray *rays, Mat_Struct *mats, Face *faces, int num_faces, float4 *out, int idx)
{
	Ray r = rays[idx];
	Hit h = { 1000, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, shape_hit;
	for (int i = 0; i < num_faces; i++) {
		if ((shape_hit = hit_shape (faces[i], r, h.t)).t != 0)
			h = shape_hit;
	}
	if (h.t < 1000) {
		out[idx].s0 = 1;
		out[idx].s1 = 0;
		out[idx].s2 = 0;
		out[idx].s3 = 0;
	}
}