
#include <iostream>
#include <chrono>

#include "hitable.h"
#include "sphere.h"
#include "hitablelist.h"
#include <string>
#include <random>
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include <list>
#include "transformation.h"

#include "threads.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "constant_texture.h"
#include "checker_texture.h"
#include "camera.h"
#include "light.h"
#include "box.h"

#ifdef _WIN32
#include "stdafx.h"
#else
 #include <limits>
 #define FLT_MAX std::numeric_limits<float>::max()
#endif // _WINDOWS_MAGIC

using namespace std::chrono;

RENDERCONTEXT getOptionsFromInput(int argc, char** argv);
uint8_t clamp(float value);


std::list<hitable*> cornel_box() {
	std::list<hitable*> list;
	material * red = new lambertian(new constant_texture(vec3(0.65, 0.05,0.05)));
	material * white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material * green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material * light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list.push_back(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
	list.push_back(new yz_rect(0, 555, 0, 555, 0, red));
	list.push_back(new xz_rect(213, 343, 227, 332, 554, light));
	list.push_back(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
	list.push_back(new xz_rect(0, 555, 0, 555, 0, white));
	list.push_back(new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)));
	return list;
}



int main(int argc, char ** argv)
{
	/*
	std::list<hitable*> list;
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)),
		new constant_texture(vec3(0.9, 0.9, 0.9)));
	list.push_back(new sphere(vec3(0, -100.5, -1), 100, new lambertian(checker)));
	//list.push_back( new sphere(vec3(0, 0, -2.0), 0.5, new lambertian(new constant_texture(vec3(0.8, 0.3, 0.3)))));
	list.push_back(new xy_rect(-1, 40, -4, 20, -10, new diffuse_light(new constant_texture(vec3(4, 4, 4)))));
	//list.push_back( new sphere(vec3(1, 2, 0), 0.5, new diffuse_light(new constant_texture(vec3(3.1, 3.1, 3.1)))));
	//list.push_back(new sphere(vec3(0, 1.5, -1), 1, new diffuse_light(new constant_texture(vec3(3.1, 3.1, 3.5)))));
	list.push_back(new sphere(vec3(1, 0, -1), 0.5, new lambertian(new constant_texture(vec3(1.0, 0.2, 0.2)))));
	list.push_back(new sphere(vec3(.2, -.2, -.8), 0.2, new dielectric(1.5)));

	*/

	hitable * world = new hitable_list(cornel_box());

	

	auto rc = getOptionsFromInput(argc, argv);
	rc.y_offset = 0;
	rc.num_rows_to_render = rc.buffer_height;
	rc.world = world;

	vec3 lookfrom = vec3(278, 278, -800);
	vec3 lookat = vec3(278, 278, 0);
	float dist_to_focus = 10;// (lookfrom - lookat).length();
	float aperture = 0.0;
	float vfov = 40;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(rc.buffer_width) / float(rc.buffer_height), aperture, dist_to_focus);

	rc.cam = &cam;

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
		raw_buffer[i * 3] = clamp(rc.buffer[i].r());
		raw_buffer[(i * 3) + 1] = clamp(rc.buffer[i].g());
		raw_buffer[(i * 3) + 2] = clamp(rc.buffer[i].b());
	}
	

	stbi_write_png(rc.filename, rc.buffer_width, rc.buffer_height, 3, raw_buffer, 0);
	return 0;
}

uint8_t clamp(float value)
{
	if (value > 255)
		return 255;
	if (value < 0)
		return 255;
	return value;
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
