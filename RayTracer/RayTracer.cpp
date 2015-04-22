// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "RayTracer.h"

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL\cl.hpp>
#include <direct.h>
#include <chrono>

using namespace std;

const string hw ("Hello World\n");

void printCwd ()
{
	char cwd[FILENAME_MAX];
	_getcwd (cwd, FILENAME_MAX);
	cerr << cwd << endl;
}

inline void checkErr (cl_int err, const char* name)
{
	if (err != CL_SUCCESS) {
		cerr << "ERROR: " << name << " (" << err << ")" << endl;
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
CRayTracer::CRayTracer(string kernel_file)
{
	int i = 0;
	cl_int err;
	char *outH = new char[hw.length () + 1];

	auto start = chrono::steady_clock::now();

	ifstream file (kernel_file);
	vector < cl::Platform > platformList;
	vector < cl::Device > devices;

	checkErr (file.is_open () ? CL_SUCCESS : -1, kernel_file.c_str ());
	
	cl::Platform::get (&platformList);
	checkErr (platformList.size () != 0 ? CL_SUCCESS : -1, "cl::Platform::get");

	cl_context_properties cprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0 };
	cl::Context context (CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &err);
	checkErr (err, "Context::Context()");

	cl::Buffer outCl (context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, hw.length() + 1, outH, &err);
	checkErr (err, "Buffer::Buffer()");

	devices = context.getInfo<CL_CONTEXT_DEVICES> ();
	checkErr (devices.size () > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

	string prog (istreambuf_iterator<char> (file), (istreambuf_iterator<char> ()));
	cl::Program::Sources source (1, make_pair (prog.c_str (), prog.length () + 1));
	cl::Program program (context, source);
	checkErr (program.build (devices), "Program::build()");

	cl::Kernel kernel (program, "hello", &err);
	checkErr (err, "Kernel::Kernel()");
	checkErr (kernel.setArg (0, outCl), "Kernel::setArg()");

	cl::CommandQueue queue (context, devices[0], 0, &err);
	checkErr (err, "CommandQueue::CommandQueue()");
	
	cl::Event event;
	checkErr (queue.enqueueNDRangeKernel (kernel, cl::NullRange, cl::NDRange (hw.length () + 1), cl::NDRange (1, 1), NULL, &event), "ComamndQueue::enqueueNDRangeKernel ()");
	event.wait ();

	checkErr (queue.enqueueReadBuffer (outCl, CL_TRUE, 0, hw.length () + 1, outH), "ComamndQueue::enqueueReadBuffer()");
	cout << outH;

	auto end = chrono::steady_clock::now ();
	cerr << (end - start).count() << endl;

	return;
}