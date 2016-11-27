#pragma once
#include "hitable.h"


typedef struct RenderContext {
	vec3* buffer;
	int buffer_width;
	int buffer_height;
	int y_offset;
	int num_rows_to_render;
	const hitable * world;
	int samples = 100;
	char * filename;
	// probably want something to do with the camera?
} RENDERCONTEXT, *PRENDERCONTEXT;


class Renderer {
public:
	virtual ~Renderer()
	{
	}

	// nope. this is not what we want exactly...
	virtual bool renderSection(PRENDERCONTEXT rc) = 0; // although we probably want it to take a render context instead..
};


vec3 color(const ray& r, const hitable *world, int max_depth);