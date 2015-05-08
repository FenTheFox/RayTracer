// RayTracer.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Mesh.h"
#include "KDTree.h"
#include "RayTracer.h"
void main (Ray *rays, Light *lights, int num_lights, Mat_Struct *mats, Face *faces, int num_faces, cl_float4 *out, KDNode *head, int idx);

void printCwd ()
{
	char cwd[FILENAME_MAX];
	auto ret = _getcwd (cwd, FILENAME_MAX);
	std::cerr << cwd << std::endl;
}

inline void checkErr (cl_int err, const char* name)
{
	if (err != CL_SUCCESS) {
		std::cerr << "ERROR: " << name << " (" << err << ")" << std::endl;
		exit (EXIT_FAILURE);
	}
}

void RayTracer::setMatBuffer (std::vector<Material> mats)
{
	numMaterial = mats.size ();
	materialBuff = new Mat_Struct[numMaterial];
	for (size_t i = 0; i < mats.size (); i++)
		materialBuff[i] = mats[i];
}

void RayTracer::setFaceBuffer (std::vector<Face> faces)
{
	numFace = faces.size ();
	faceBuff = new Face[numFace];
	for (size_t i = 0; i < faces.size (); i++)
		faceBuff[i] = faces[i];

	kdtree = KDTree (faces);
	numLight = 100;
	lightBuff = new Light[numLight];
	cl_float4 pos, intensity;
	for (size_t i = 0; i < 10; i++) {
		for (size_t j = 0; j < 10; j++) {
			pos.x = (i - 5) * 100;
			pos.y = 100;
			pos.z = (j - 5) * 100;
			intensity.s0 = 1000;
			intensity.s1 = 1000;
			intensity.s2 = 1000;
			intensity.s3 = 1000;
			lightBuff[0].pos = pos;
			lightBuff[0].intensity = intensity;
			lightBuff[0].type = 0;
		}
	}
}

void RayTracer::makeRays (Ray * buff)
{
	Point3f cpos (0.0, 0.0, 500.0),
		cdir (0.0, 0.0, -1.0),
		cup (0.0, 1.0, 0.0),
		cright (cdir.y*cup.z - cdir.z*cup.y, cdir.z*cup.x - cdir.x*cup.z, cdir.x*cup.y - cdir.y*cup.x);
	cup = { cright.y*cdir.z - cright.z*cdir.y, cright.z*cdir.x - cright.x*cdir.z, cright.x*cdir.y - cright.y*cdir.x };
	double norm_i, norm_j;
	for (size_t i = 0; i < width; i++) {
		norm_i = ((float)i / (float)width) - 0.5;
		for (size_t j = 0; j < height; j++) {
			norm_j = ((float)j / (float)height) - 0.5;
			buff[j*width + i].pos = cpos;
			buff[j*width + i].dir.x = norm_i;
			buff[j*width + i].dir.y = norm_j;
			buff[j*width + i].dir.z = -1;
		}
	}
}

void RayTracer::raytrace (cl_float4 *img_buff)
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
	checkErr (err, bld_log.c_str ());

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

	BYTE *pixles = new BYTE[width*height * 3];

	for (size_t i = 0; i < width*height; i++) {
		main (rays_h, lightBuff, numLight, materialBuff, faceBuff, numFace, img_buff, kdtree.head, i);
		pixles[i * 3] = img_buff[i].x * 255;
		pixles[i * 3 + 1] = img_buff[i].y * 255;
		pixles[i * 3 + 2] = img_buff[i].z * 255;
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity ();
		glBindTexture (GL_TEXTURE_2D, 1);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixles);

		glClearColor (0, 0, 1, 1);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (-1, 1, 1, -1, 1, 100);
		glMatrixMode (GL_MODELVIEW);
		glEnable (GL_TEXTURE_2D);
		glLoadIdentity ();
		glBegin (GL_QUADS);
		glTexCoord2f (0, 1); glVertex3f (-1, -1, -1);
		glTexCoord2f (0, 0); glVertex3f (-1, 1, -1);
		glTexCoord2f (1, 0); glVertex3f (1, 1, -1);
		glTexCoord2f (1, 1); glVertex3f (1, -1, -1);
		glEnd ();
		SwapBuffers (wglGetCurrentDC ());
	}

	//cl::Event event;
	//checkErr (queue.enqueueNDRangeKernel (kernel, cl::NullRange, cl::NDRange (width * height), cl::NDRange (1, 1), NULL, &event), "ComamndQueue::enqueueNDRangeKernel ()");
	//event.wait ();

	//checkErr (queue.enqueueReadBuffer (img_out, CL_TRUE, 0, width * height * sizeof (cl_float4), img_buff), "ComamndQueue::enqueueReadBuffer()");

	auto end = std::chrono::steady_clock::now ();
	std::cerr << (end - start).count () << std::endl;
}

RayTracer::RayTracer (int width, int height)
{
	kernel_file = "hw_kernel.cl";
	this->width = width;
	this->height = height;
	numMaterial = 0;
	numFace = 0;
}

RayTracer::RayTracer ()
{}