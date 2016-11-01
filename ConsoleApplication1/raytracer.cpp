
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

#ifdef _WIN32
#include "stdafx.h"
#else
 #include <limits>
 #define FLT_MAX std::numeric_limits<float>::max()
#endif // _WINDOWS_MAGIC

int main(int argc, char ** argv)
{
	int nx = 1600;
	int ny = 800;
	if (argc == 3)
	{
		nx = std::stoi(argv[1]);
		ny = std::stoi(argv[2]);
	}
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";

	std::list<hitable*> list;
	list.push_back( new sphere(vec3(0, 0, -2.0), 0.5, new lambertian(vec3(0.8, 0.3, 0.3))));
	list.push_back( new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.2, 0.2, 0.6))));
	list.push_back(new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0)));
	list.push_back(new sphere(vec3(.2, -.2, -.8), 0.2, new dielectric( 1.5)));
	hitable * world = new hitable_list(list);

	vec3 * buffer = new vec3[nx*ny];
	threadRenderer renderer = threadRenderer();
	renderer.renderSection(buffer, nx, ny,0,ny, world);
	for (int i = 0; i < nx*ny; i++)
	{
		std::cout << buffer[i].r() << " " << buffer[i].g() << " " << buffer[i].b() << "\n";
	}


	return 0;
}

