
#include <iostream>
#include <chrono>

#include "hitables/hitable.h"
#include "hitables/sphere.h"
#include "hitables/hitablelist.h"
#include <string>
#include <random>
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include <list>
#include "hitables/transformation.h"

#include "threads.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <sstream>

#include "stb_image_write.h"
#include "textures/constant_texture.h"
#include "textures/checker_texture.h"
#include "engine/camera.h"
#include "materials/light.h"
#include "hitables/box.h"
#include "hitables/bvh.h"
#include "hitables/rotate.h"
#include "hitables/translate.h"
#include "standard_scenes.h"
#include "hitables/mesh.h"
#include "hitables/scale.h"
#include "rapidobj/rapidobj.hpp"
#ifdef _WIN32
#include "stdafx.h"
#else
 #include <limits>
 #define FLT_MAX std::numeric_limits<float>::max()
#endif // _WINDOWS_MAGIC

using namespace std::chrono;

RENDERCONTEXT getOptionsFromInput(int argc, char** argv);
uint8_t clamp(float value);



 

// can nyquist limit help with supersampling?
mesh * object;

int main(int argc, char ** argv)
{

	//hitable * world = new hitable_list(cornel_box());

	

	auto rc = getOptionsFromInput(argc, argv);

	auto cornel = standard_scenes::cornel_box();

	hitable * world;
	material * white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	if ( !rc.object_filename.empty())
	{
		// load obj, turn it into a mesh and add it to the cornel box.
		rapidobj::Result result = rapidobj::ParseFile(rc.object_filename.c_str());

		if (result.error) {
			std::cout << result.error.code.message() << '\n';
			return EXIT_FAILURE;
		}

		bool success = rapidobj::Triangulate(result);

		if (!success) {
			std::cout << result.error.code.message() << '\n';
			return EXIT_FAILURE;
		}

		size_t num_triangles{};

		for (const auto& shape : result.shapes) {
			num_triangles += shape.mesh.num_face_vertices.size();
		}

		std::cout << "Shapes:    " << result.shapes.size() << '\n';
		std::cout << "Materials: " << result.materials.size() << '\n';
		std::cout << "Triangles: " << num_triangles << '\n';

		std::vector<vec3> vertices;
		vertices.reserve(result.attributes.positions.size() / 3);
		std::vector<vec3> normals;
		normals.reserve(result.attributes.positions.size() / 3);

		for (size_t i = 0; i < result.attributes.positions.size(); i += 3)
		{
			vertices.emplace_back(
				result.attributes.positions[i],
				result.attributes.positions[i + 1],
				result.attributes.positions[i + 2]
			);
			normals.emplace_back(
				result.attributes.normals[i],
				result.attributes.normals[i + 1],
				result.attributes.normals[i + 2]
			);
		}

		for (const auto& shape : result.shapes)
		{
			std::vector<unsigned int> vertex_indices;
			vertex_indices.reserve(shape.mesh.indices.size());
			std::vector<unsigned int> vertex_normal_indices;
			vertex_normal_indices.reserve(shape.mesh.indices.size());

			for (const auto& index : shape.mesh.indices)
			{
				vertex_indices.push_back(
					static_cast<unsigned int>(index.position_index)
				);
				vertex_normal_indices.push_back(
					static_cast<unsigned int>(index.normal_index)
				);
			}

			object = new mesh(vertices, normals, vertex_indices,vertex_normal_indices, white);
			aabb bounding_box;
			if (object->bounding_box(0,0,bounding_box))
			{
				std::cout << "x bounds: " << bounding_box._min.x() <<" - " << bounding_box._max.x() << std::endl;
				std::cout << "y bounds: " << bounding_box._min.y() <<" - " << bounding_box._max.y() << std::endl;
				std::cout << "z bounds: " << bounding_box._min.z() <<" - " << bounding_box._max.z() << std::endl;
			}
			else
			{
				std::cout << "No bounds for object" << std::endl;
			}
			//determine scale such that the max coordinate fits in 200x200x200
			float scale_factor = scale::scale_to_largest_axis((*object), 300, 0,0);
			// This works if the mesh indices refer to the global vertex array,
			// as rapidobj's position indices do.
			cornel.push_back(new translate(
				new rotate_y(
				new scale(object, scale_factor),
				45),

				vec3(300,20,100)));
		}


	}

	world = new bvh_node(cornel.data(), static_cast<int>(cornel.size()), 0.0f,0.0f);




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

	std::cout << "Starting render: "
		  << rc.buffer_width << "x" << rc.buffer_height
		  << ", " << rc.samples << " samples\n";
	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	threadRenderer renderer = threadRenderer();
	renderer.renderSection(&rc);
	milliseconds end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	auto total_ms = (end - start).count();

	std::cout << "Total duration " << total_ms / 1000 << "." << total_ms % 1000 << "s" << std::endl;;
	uint8_t * raw_buffer= new uint8_t[rc.buffer_width*rc.buffer_height * 3];
	for (int i = 0; i < rc.buffer_width*rc.buffer_height; i++)
	{
		raw_buffer[i * 3] = clamp(rc.buffer[i].r());
		raw_buffer[(i * 3) + 1] = clamp(rc.buffer[i].g());
		raw_buffer[(i * 3) + 2] = clamp(rc.buffer[i].b());
	}
	

	stbi_write_png(rc.filename.c_str(), rc.buffer_width, rc.buffer_height, 3, raw_buffer, 0);
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
	std::cout << "Usage: " << progname << " [-h height] [-w width] [-s number_multisamples] [-o output_filename] [-f input obj filename]" << std::endl;
	exit(EXIT_FAILURE);
}

RENDERCONTEXT getOptionsFromInput(int argc, char** argv)
{

	RENDERCONTEXT rc;
	rc.buffer_height = 200;
	rc.buffer_width = 400;
	rc.samples = 100;


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
		if (!strcmp(argv[i], "-o"))
		{
			rc.filename = argv[i + 1];
			++i;
			continue;
		}
		if (!strcmp(argv[i], "-c"))
		{
			rc.cores = std::stoi(argv[i + 1]);
			++i;
			continue;
		}
		// input filename
		if (!strcmp(argv[i], "-f"))
		{
			rc.object_filename = argv[i + 1];
			++i;
			continue;
		}
		printUsageAndExit(argv[0]);
	}
	// decorate the name
	if ( rc.filename.empty())
	{
		std::ostringstream filename;
		filename << "multithreaded_" << rc.buffer_width << "x" << rc.buffer_height << "_"<< rc.samples << ".png";
		rc.filename = filename.str();
	}
	rc.buffer = new vec3[rc.buffer_height*rc.buffer_width];
	return rc;
}


/* I want to make some quality of life improvements to the raytracer.
 * For example, I want to be able to load scenes.  Which means that I need to have a scene format.
 * I should probably look into what blender can export.
 * I also want to move over to using triangles/meshes instead of primitives.
 * That will likely need a bunch of optimizations for the bvh etc.
 * I should also add some telemetry.
*/