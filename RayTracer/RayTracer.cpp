// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RayTracer.h"

const std::string hw ("Hello World\n");

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

// This is the constructor of a class that has been exported.
// see RayTracer.h for the class definition
CRayTracer::CRayTracer()
{
	int i = 0;
	cl_int err;
	std::vector < cl::Platform > platformList;
	std::vector < cl::Device > devices;
	std::string platformVendor;
	
	cl::Platform::get (&platformList);
	checkErr (platformList.size () != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
	std::cerr << "Platform number is: " << platformList.size () << std::endl;

	for each (cl::Platform p in platformList)
	{
		p.getInfo ((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
		std::cerr << "Platform " << i++ << " is by: " << platformVendor << "\n";
	}

	cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0 };
	cl::Context context (CL_DEVICE_TYPE_CPU, cprops, NULL, NULL, &err);
	checkErr (err, "Context::Context()");

	char *outH = new char[hw.length() + 1];

	cl::Buffer outCl (context,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, hw.length() + 1, outH, &err);
	checkErr (err, "Buffer::Buffer()");

	devices = context.getInfo<CL_CONTEXT_DEVICES> ();
	checkErr (devices.size () > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	std::ifstream file ("lesson1_kernels.cl");
	checkErr (file.is_open () ? CL_SUCCESS : -1, "lesson1_kernel.cl");

	return;
}