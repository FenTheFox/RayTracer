#include "stdafx.h"
#include "Structs.h"
#include "KDTree.h"

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

	f.n0 = f.n0 * a;
	f.n1 = f.n1 * b;
	f.n2 = f.n2 * (1 - a - b);

	h.t = t;
	h.pos = ray.dir*t + ray.pos;
	h.norm = f.n0 + f.n1 + f.n2;
	h.norm = h.norm * sqrt (f.n0*f.n0 + f.n1*f.n1 + f.n2*f.n2);
	h.idx = f.mat;
	return h;
}

struct StackElem
{
	KDNode *node;
	float entry_dist, exit_dist;
};

void main (Ray *rays, Light *lights, int num_lights, Mat_Struct *mats, Face *faces, int num_faces, float4 *out, KDNode *head, int idx)
{
	Ray r = rays[idx];
	Hit h = { 10000, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, shape_hit;
	for (int i = 0; i < num_faces; i++) {
		if ((shape_hit = hit_shape (faces[i], r, h.t)).t != 0)
			h = shape_hit;
	}

	float4 ambient, background;
	ambient.s0 = 0.1;
	ambient.s1 = 0.1;
	ambient.s2 = 0.1;
	ambient.s3 = 1.0;

	background.s0 = 0.1;
	background.s1 = 0.1;
	background.s2 = 0.1;
	background.s3 = 1.0;

	if (h.t == 10000) {
		out[idx] = background;
		return;
	}

	Mat_Struct mat = mats[h.idx];
	out[idx] = ambient * mat.Ka;
	Light l;
	float4 R, N = h.norm, V = r.dir;
	V = V * -1.0f;
	float d, s, len;
	for (int i = 0; i < num_lights; i++) {
		if (lights[i].type == 0) l.dir = lights[i].pos - h.pos;
		l.pos = lights[i].pos;
		l.intensity = lights[i].intensity;
		len =  sqrt (DOT (l.dir, l.dir));
		l.intensity = l.intensity / len;
		l.dir = l.dir / len;

		R = h.norm;
		R = R * 2 * DOT (l.dir, N);
		R = R - l.dir;
		d = max (DOT (l.dir, N), 0);
		float sp = max (DOT (R, V), 0);
		s = pow (sp, mat.phong);

		l.pos = l.pos - h.pos;
		//Hit sh = { 10000, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
		//Ray sray = {h.pos, l.dir};
		//for (int j = 0; j < num_faces; j++) {
		//	sh = hit_shape (faces[j], sray, sh.t);
		//}

		//if (sh.t < sqrt (DOT(l.pos, l.pos)))
		//	continue;

		out[idx] = out[idx] + l.intensity * mat.Kd * d + l.intensity * mat.Ks * s;
	}
	
	
	out[idx].s3 = 1;
}