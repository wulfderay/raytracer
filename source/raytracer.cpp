
#include <iostream>
#include <chrono>

#include "hitable.h"
#include "sphere.h"
#include "hitablelist.h"
#include <limits>
#include <string>
#include <random>
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include <list>

#include <cstring>
#include "threads.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "constant_texture.h"
#include "checker_texture.h"

#ifdef _WIN32
#include "stdafx.h"
#else
 #include <limits>
 #define FLT_MAX std::numeric_limits<float>::max()
#endif // _WINDOWS_MAGIC

using namespace std::chrono;

RENDERCONTEXT getOptionsFromInput(int argc, char** argv);

int main(int argc, char ** argv)
{

	std::list<hitable*> list;
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)),
		new constant_texture(vec3(0.9, 0.9, 0.9)));
	list.push_back(new sphere(vec3(0, -100.5, -1), 100, new lambertian(checker)));
	list.push_back( new sphere(vec3(0, 0, -2.0), 0.5, new lambertian(new constant_texture(vec3(0.8, 0.3, 0.3)))));
	list.push_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0)));
	list.push_back(new sphere(vec3(.2, -.2, -.8), 0.2, new dielectric( 1.5)));
	hitable * world = new hitable_list(list);


	auto rc = getOptionsFromInput(argc, argv);
	rc.y_offset = 0;
	rc.num_rows_to_render = rc.buffer_height;
	rc.world = world;

	std::cout << "Starting Render..." << std::endl;
	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	threadRenderer renderer = threadRenderer();
	renderer.renderSection(&rc);
	milliseconds end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	auto total_ms = (end - start).count();

	std::cout << "Total durations " << total_ms / 1000 << "." << total_ms % 1000 << "s" << std::endl;;
	uint8_t * raw_buffer= new uint8_t[rc.buffer_width*rc.buffer_height * 3];
	for (int i = 0; i < rc.buffer_width*rc.buffer_height; i++)
	{
		raw_buffer[i * 3] = rc.buffer[i].r();
		raw_buffer[(i * 3) + 1] = rc.buffer[i].g();
		raw_buffer[(i * 3) + 2] = rc.buffer[i].b();
	}
	

	stbi_write_png(rc.filename, rc.buffer_width, rc.buffer_height, 3, raw_buffer, 0);
	return 0;
}

void printUsageAndExit(const char * progname)
{
	// Unknown parameter. print usage and exit.
	std::cout << "Usage: " << progname << " [-h height] [-w width] [-s number_multisamples] [-f output_filename]" << std::endl;
	exit(EXIT_FAILURE);
}

RENDERCONTEXT getOptionsFromInput(int argc, char** argv) // hmm.. what about filename? 
{

	RENDERCONTEXT rc;
	rc.buffer_height = 200;
	rc.buffer_width = 400;
	rc.samples = 100;
	rc.filename ="out.png";

	for (auto i = 1; i <argc; i ++) 
	{
		if (i + 1 >= argc)
			printUsageAndExit(argv[0]);
		if (!strcmp(argv[i], "-h")) // height
		{
			rc.buffer_height = std::stoi(argv[i+1]);
			++i;
			continue;
		}
		if (!strcmp(argv[i], "-w"))
		{
			rc.buffer_width = std::stoi(argv[i + 1]);
			++i;
			continue;
		}
		if (!strcmp(argv[i], "-s"))
		{
			rc.samples = std::stoi(argv[i + 1]);
			++i;
			continue;
		}
		if (!strcmp(argv[i], "-f"))
		{
			rc.filename = argv[i + 1];
			++i;
			continue;
		}
		printUsageAndExit(argv[0]);
	}
	rc.buffer = new vec3[rc.buffer_height*rc.buffer_width];
	return rc;
}
