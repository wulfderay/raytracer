#pragma once
#include "hitable.h"


typedef struct RenderContext {
	vec3* buffer;
	int bufferx;
	int buffery;
	int rendery;
	int sizey;
	const hitable * world;
	int samples = 100;
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