// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RayTracer.h"

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL\cl.hpp>
#include <direct.h>
#include <chrono>

using namespace cl;

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

// This is an example of an exported variable
RAYTRACER_API int nRayTracer=0;

// This is an example of an exported function.
RAYTRACER_API int fnRayTracer(void)
{
	return 42;
}

void CRayTracer::raytrace (cl_float3 *verts, int verts_len, cl_float3 *norms, int norms_len, int *faces, int faces_len, cl_float4 *mats, int mats_len)
{
	cl_int err;
	cl_float3 *rays_h = new cl_float3[width*height * 2];
	cl_float4 *img_h = new cl_float4[width*height];

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
	program.build (devices);
	auto bld_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG> (devices[0], &err);
	checkErr (err, "Program::build()");

	cl::Buffer rays (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2 * sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer verts (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer norms (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2 * sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer faces (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2 * sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer mats (context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2 * sizeof (cl_float3), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer outCl (context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, width * height * sizeof (cl_float4), img_h, &err);
	checkErr (err, "Buffer::Buffer()");

	cl::Kernel kernel (program, "main", &err);
	checkErr (err, "Kernel::Kernel()");
	checkErr (kernel.setArg (0, rays), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");
	checkErr (kernel.setArg (1, outCl), "Kernel::setArg()");

	cl::CommandQueue queue (context, devices[0], 0, &err);
	checkErr (err, "CommandQueue::CommandQueue()");

	cl::Event event;
	checkErr (queue.enqueueNDRangeKernel (kernel, cl::NullRange, cl::NDRange (hw.length () + 1), cl::NDRange (1, 1), NULL, &event), "ComamndQueue::enqueueNDRangeKernel ()");
	event.wait ();

	checkErr (queue.enqueueReadBuffer (outCl, CL_TRUE, 0, hw.length () + 1, outH), "ComamndQueue::enqueueReadBuffer()");
	std::cout << outH;

	auto end = std::chrono::steady_clock::now ();
	std::cerr << (end - start).count () << std::endl;
}

// This is the constructor of a class that has been exported.
// see RayTracer.h for the class definition
CRayTracer::CRayTracer(std::string kernel_file, int width, int height)
{
	this->kernel_file = kernel_file;
	this->width = width;
	this->height = height;

	return;
}

CRayTracer::CRayTracer ()
{}