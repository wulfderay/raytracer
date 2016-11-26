
#include <iostream>

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
#include "threads.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#ifdef _WIN32
#include "stdafx.h"
#else
 #include <limits>
 #define FLT_MAX std::numeric_limits<float>::max()
#endif // _WINDOWS_MAGIC

int main(int argc, char ** argv)
{
	int nx = 600;
	int ny = 300;
	if (argc == 3)
	{
		nx = std::stoi(argv[1]);
		ny = std::stoi(argv[2]);
	}

	std::list<hitable*> list;
	list.push_back( new sphere(vec3(0, 0, -2.0), 0.5, new lambertian(vec3(0.8, 0.3, 0.3))));
	list.push_back( new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.2, 0.2, 0.6))));
	list.push_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0)));
	list.push_back(new sphere(vec3(.2, -.2, -.8), 0.2, new dielectric( 1.5)));
	hitable * world = new hitable_list(list);

	vec3 * buffer = new vec3[nx*ny];
	threadRenderer renderer = threadRenderer();
	RENDERCONTEXT rc;
	rc.buffer = buffer;
	rc.bufferx = nx;
	rc.buffery = ny;
	rc.rendery = 0;
	rc.sizey = ny;
	rc.world = world;
	rc.samples = 500;
	renderer.renderSection(&rc);
	uint8_t * raw_buffer= new uint8_t[nx*ny * 3];
	for (int i = 0; i < nx*ny; i++)
	{
		raw_buffer[i * 3] = buffer[i].r();
		raw_buffer[(i * 3) + 1] = buffer[i].g();
		raw_buffer[(i * 3) + 2] = buffer[i].b();
	}
	

	stbi_write_png("out.png", nx, ny, 3, raw_buffer, 0);
	return 0;
}

