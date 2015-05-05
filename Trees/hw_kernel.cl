typedef struct Ray {
	float3 o;
	float3 d;
} Ray;

typedef struct Light{
	float3 pos;
	float4 color;
} Light;

typedef struct Material{
	float4 Ka;
	float4 Kd;
	float4 Ks;
} Material;

typedef struct face {
	float3 v0;
	float3 v1;
	float3 v2;
	float3 n0;
	float3 n1;
	float3 n2;
} face;

typedef struct Hit {
	float t;
	float3 pos;
	float3 norm;
} Hit;

Hit hit_shape(face f, Ray ray, float tmax)
{
	Hit h = {};
	float x[3] = {ray.d.x, ray.d.y, ray.d.z},
			y[3] = {f.v2.x - f.v0.x, f.v2.y - f.v0.y, f.v2.z - f.v0.z},
			z[3] = {f.v2.x - f.v1.x, f.v2.y - f.v1.y, f.v2.z - f.v1.z},
			d[3] = {f.v2.x - ray.o.x, f.v2.y - ray.o.y, f.v2.z - ray.o.z};
	float det = x[0]*y[1]*z[2] + x[1]*y[2]*z[0] + x[2]*y[0]*z[1] - z[0]*y[1]*x[2] - y[0]*x[1]*z[2] - x[0]*z[1]*y[2],
	t = det = d[0]*y[1]*z[2] + d[1]*y[2]*z[0] + d[2]*y[0]*z[1] - z[0]*y[1]*d[2] - y[0]*d[1]*z[2] - d[0]*z[1]*y[2],
	a = det = x[0]*d[1]*z[2] + x[1]*d[2]*z[0] + x[2]*d[0]*z[1] - z[0]*d[1]*x[2] - d[0]*x[1]*z[2] - x[0]*z[1]*d[2],
	b = det = x[0]*y[1]*d[2] + x[1]*y[2]*d[0] + x[2]*y[0]*d[1] - d[0]*y[1]*x[2] - y[0]*x[1]*d[2] - x[0]*d[1]*y[2];
	
	if(det == 0) return h;
		
	t = t/det;
	a = a/det;
	b = b/det;
		
	if(t < 0 || t < 0.00001f || t > tmax || a < 0 || b < 0 || a + b > 1) return h;
		
	f.n0 *= a;
	f.n1 *= b;
	f.n2 *= 1-a-b;
		
	h.t = t;
	h.pos = t*ray.d + ray.o;
	h.norm = f.n0 + f.n1 + f.n2;
	h.norm *= native_sqrt(f.n0*f.n0 + f.n1*f.n1 + f.n2*f.n2);
	return h;
}

__kernel void main(__global float3 *rays, __global float3 *verts, __global float3 *norms, __global float4 *mats, __global int4 *objs, int num_objs, __global float4 *out) {
	const int idx = get_global_id(0);
	Ray r = {rays[idx*2], rays[idx*2+1]};
	Hit h, shape_hit;
	float tmax = 10000;
	int voff = 0, noff = 0;
	for(int i = 0; i < num_objs; i++) {
		face f = {verts[(voff++) + i], verts[(voff++) + i], verts[(voff++) + i], norms[(noff++) + i], norms[(noff++) + i], norms[(noff++) + i]};
		if((shape_hit = hit_shape(f, r, tmax)).t != 0)
			h = shape_hit;
	}

}