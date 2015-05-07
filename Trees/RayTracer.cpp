// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Mesh.h"
#include "RayTracer.h"
void main (Ray *rays, Mat_Struct *mats, Face *faces, int num_faces, cl_float4 *out, int idx);
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

void CRayTracer::setMatBuffer (std::vector<Material> mats)
{
	numMaterial = mats.size ();
	materialBuff = new Mat_Struct[numMaterial];
	for (size_t i = 0; i < mats.size (); i++)
		materialBuff[i] = mats[i];
}

void CRayTracer::setFaceBuffer (std::vector<Face> faces)
{
	numFace = faces.size ();
	faceBuff = new Face[numFace];
	for (size_t i = 0; i < faces.size (); i++)
		faceBuff[i] = faces[i];
}

void CRayTracer::makeRays (Ray * buff)
{
	Point3f cpos (0.0, 0.0, 1.0), cdir (0.0, 0.0, -1.0), cup (0.0, 1.0, 0.0), cright (1.0, 0.0, 0.0);
	double norm_i, norm_j;
	for (size_t i = 0; i < width; i++) {
		norm_i = ((float)i / (float)width) - 0.5;
		for (size_t j = 0; j < height; j++) {
			norm_j = ((float)j / (float)width) - 0.25;
			buff[j*width + i].pos = cpos;
			buff[j*width + i].dir.x = norm_i;
			buff[j*width + i].dir.y = norm_j;
			buff[j*width + i].dir.z = -1;
		}
	}
}

void CRayTracer::raytrace (cl_float4 *img_buff)
{
	cl_int err;
	int rays = width*height;
	Ray *rays_h = new Ray[rays];
	makeRays (rays_h);

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

	cl::Buffer rays_in (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, rays * sizeof (Ray), rays_h, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer mats_in (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numMaterial * sizeof (Mat_Struct), materialBuff, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer faces_in (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, numFace * sizeof (Face), faceBuff, &err);
	checkErr (err, "Buffer::Buffer()");
	cl::Buffer img_out (context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, width * height * sizeof (cl_float4), img_buff, &err);
	checkErr (err, "Buffer::Buffer()");
	//cl::BufferRenderGL outGl (context, CL_MEM_WRITE_ONLY, , &err);

	cl::Kernel kernel (program, "main", &err);
	checkErr (err, "Kernel::Kernel()");
	checkErr (kernel.setArg (0, rays_in), "Kernel::setArg()");
	checkErr (kernel.setArg (1, mats_in), "Kernel::setArg()");
	checkErr (kernel.setArg (2, faces_in), "Kernel::setArg()");
	checkErr (kernel.setArg (3, numFace), "Kernel::setArg()");
	checkErr (kernel.setArg (4, img_out), "Kernel::setArg()");

	cl::CommandQueue queue (context, devices[0], 0, &err);
	checkErr (err, "CommandQueue::CommandQueue()");

	for (size_t i = 0; i < width*height; i++)
		main (rays_h, materialBuff, faceBuff, numFace, img_buff, i);

	//cl::Event event;
	//checkErr (queue.enqueueNDRangeKernel (kernel, cl::NullRange, cl::NDRange (width * height), cl::NDRange (1, 1), NULL, &event), "ComamndQueue::enqueueNDRangeKernel ()");
	//event.wait ();

	//checkErr (queue.enqueueReadBuffer (img_out, CL_TRUE, 0, width * height * sizeof (cl_float4), img_buff), "ComamndQueue::enqueueReadBuffer()");

	auto end = std::chrono::steady_clock::now ();
	std::cerr << (end - start).count () << std::endl;
}

CRayTracer::CRayTracer(int width, int height)
{
	kernel_file = "hw_kernel.cl";
	this->width = width;
	this->height = height;
	numMaterial = 0;
	numFace = 0;
}

CRayTracer::CRayTracer ()
{}