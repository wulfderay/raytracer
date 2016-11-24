#ifndef THREADS_H_INCLUDED
#define THREADS_H_INCLUDED

#include "renderer.h"

#define MAX_THREADS 4 // should use std::thread::hardware_concurrency(); instead...



class threadRenderer :Renderer
{
public:
	// probably want a constructor that takes the max threads etc...
	bool renderSection(PRENDERCONTEXT rc) override;
private:
    bool renderByThread(vec3* buffer, int bufferx, int buffery, int rendery, int sizey, const hitable * world);
};

#endif // THREADS_H_INCLUDED
