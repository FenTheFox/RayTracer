// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Mesh.h"
#include "RayTracer.h"

const std::string hw ("Hello World\n");

void printCwd ()
{
	char cwd[FILENAME_MAX];
	_getcwd (cwd, FILENAME_MAX);
	std::cerr << cwd << std::endl;
}

inline void checkErr (cl_int err, const char* name)
{
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit (EXIT_FAILURE);
	}
}

void CRayTracer::createPointBuffer (BUFF b, std::vector<Point3f> pts, int *len, cl_float3 **buff)
{
	cl_float3 fl;
	cl_float3 *tmp = new cl_float3 [pts.size () + *len];
	memcpy (tmp, *buff, *len);
	*buff = tmp;
	switch (b) {
	case CRayTracer::VERTEX_BUFF:
	case CRayTracer::NORMAL_BUFF:
		for (size_t i = 0; i < pts.size (); i++) {
			fl.x = pts[i].x;
			fl.y = pts[i].y;
			fl.z = pts[i].z;
			(*buff)[i] = fl;
		}
		*len += pts.size ();
		break;
	default:
		break;
	}
}

void CRayTracer::createMatBuffer (BUFF b, std::vector<Material> mats, int *len, cl_float16 **buff)
{
	cl_float16 fl;
	*buff = new cl_float16[mats.size ()];
	switch (b) {
	case CRayTracer::MATERIAL_BUFF:
		for (size_t i = 0; i < mats.size (); i++) {
			fl.s0 = mats[i].id;
			fl.s1 = mats[i].Ka.x;
			fl.s2 = mats[i].Ka.y;
			fl.s3 = mats[i].Ka.z;
			fl.s4 = mats[i].Kd.x;
			fl.s5 = mats[i].Kd.y;
			fl.s6 = mats[i].Kd.z;
			fl.s7 = mats[i].Ks.x;
			fl.s8 = mats[i].Ks.y;
			fl.s9 = mats[i].Ks.z;
			(*buff)[i] = fl;
		}
		*len += mats.size ();
		break;
	default:
		break;
	}
}

void CRayTracer::setPointBuffer (BUFF b, std::vector<Point3f> pts)
{
	switch (b) {
	case CRayTracer::VERTEX_BUFF:
		createPointBuffer (b, pts, &vert_len, &vert_buf);
		break;
	case CRayTracer::NORMAL_BUFF:
		createPointBuffer (b, pts, &norm_len, &norm_buf);
		break;
	default:
		break;
	}
}

void CRayTracer::setMatBuffer (BUFF b, std::vector<Material> mats)
{
	createMatBuffer (b, mats, &mat_len, &mat_buf);
}

void CRayTracer::setObjBuffer (BUFF b, Mesh m)
{
	int id = obj_len++;
	createPointBuffer (VERTEX_BUFF, m._verts, &vert_len, &vert_buf);
	createPointBuffer (VERTEX_BUFF, m._normals, &norm_len, &norm_buf);
	cl_int8 i;
	i.s0 = id;
	i.s1 = vert_len;
	i.s2 = norm_len;
	createPointBuffer (VERTEX_BUFF, m._verts, &vert_len, &vert_buf);
	createPointBuffer (VERTEX_BUFF, m._normals, &norm_len, &norm_buf);
	i.s3 = vert_len-1;
	i.s4 = norm_len-1;
	i.s5 = m.mat.id;
	obj_buff[id] = i;
}


void CRayTracer::raytrace (cl_float4 *out_buff)
{
	cl_int err;
	cl_float3 *rays_h = new cl_float3[width*height * 2];

	auto start = std::chrono::steady_clock::now ();

	std::ifstream file (kernel_file);
	std::vector < cl::Platform > platformList;
	std::vector < cl::Device > devices;

	checkErr (file.is_open () ? CL_SUCCESS : -1, kernel_file.c_str ());

	cl::Platform::get (&platformList);
	checkErr (platformList.size () != 0 ? CL_SUCCESS : -1, "cl::Platform::get");

	cl_context_properties cprops[] = { CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext (),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC (),
		CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0 };
	cl::Context context (CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	checkErr (err, "Context::Context()");

	devices = context.getInfo<CL_CONTEXT_DEVICES> ();
	checkErr (devices.size () > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	std::string prog (std::istreambuf_iterator<char> (file), (std::istreambuf_iterator<char> ()));
	cl::Program::Sources source (1, std::make_pair (prog.c_str (), prog.length () + 1));
	cl::Program program (context, source);
	err = program.build (devices, "-g");
	auto bld_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG> (devices[0]);
	checkErr (err, bld_log.c_str());

	cl::Buffer rays (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2 * sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer verts (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, vert_len * sizeof (cl_float3), vert_buf, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer norms (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, norm_len * sizeof (cl_float3), norm_buf, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer mats (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, mat_len * sizeof (cl_float16), mat_buf, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer objs (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, obj_len * sizeof (cl_float3), obj_buff, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer outCl (context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, width * height * sizeof (cl_float4), out_buff, &err);
	checkErr (err, "Buffer::Buffer()");
	//cl::BufferRenderGL outGl (context, CL_MEM_WRITE_ONLY, , &err);

	cl::Kernel kernel (program, "main", &err);
	checkErr (err, "Kernel::Kernel()");
	checkErr (kernel.setArg (0, rays), "Kernel::setArg()");
	checkErr (kernel.setArg (1, verts), "Kernel::setArg()");
	checkErr (kernel.setArg (2, norms), "Kernel::setArg()");
	checkErr (kernel.setArg (3, mats), "Kernel::setArg()");
	checkErr (kernel.setArg (4, objs), "Kernel::setArg()");
	checkErr (kernel.setArg (5, obj_len), "Kernel::setArg()");
	checkErr (kernel.setArg (6, outCl), "Kernel::setArg()");

	cl::CommandQueue queue (context, devices[0], 0, &err);
	checkErr (err, "CommandQueue::CommandQueue()");

	cl::Event event;
	checkErr (queue.enqueueNDRangeKernel (kernel, cl::NullRange, cl::NDRange (width * height), cl::NDRange (1, 1), NULL, &event), "ComamndQueue::enqueueNDRangeKernel ()");
	event.wait ();

	checkErr (queue.enqueueReadBuffer (outCl, CL_TRUE, 0, width * height * sizeof (cl_float4), out_buff), "ComamndQueue::enqueueReadBuffer()");

	auto end = std::chrono::steady_clock::now ();
	std::cerr << (end - start).count () << std::endl;
}

CRayTracer::CRayTracer(int width, int height)
{
	kernel_file = "hw_kernel.cl";
	this->width = width;
	this->height = height;
	obj_len = 0;
	vert_len = 0;
	norm_len = 0;
	mat_len = 0;
	return;
}

CRayTracer::CRayTracer ()
{}