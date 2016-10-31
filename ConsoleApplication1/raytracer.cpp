// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include "vec3.h"
#include "ray.h"
#include "hitable.h"
#include "sphere.h"
#include "hitablelist.h"
#include "camera.h"
#include <limits>
#include <string>
#include <random>
#include "lambertian.h"
#include "metal.h"
#include "dielectric.h"
#include <list>

#define MAX_THREADS 10

DWORD WINAPI RenderThread(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);
void renderBythreads(vec3* buffer, int bufferx, int buffery, const hitable * world);

vec3 color(const ray& r, const hitable *world, int max_depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (max_depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation*color(scattered, world, max_depth + 1);
		}
		return vec3(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5*(unit_direction.y() + 1.0);
	return (1.0f - t)* vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}
const int DEPTH_MAX = 500;


/*
Ok. So to divide this up into threadable chunks, I need to be able to call a function with the state of the scene and a coordinate range to render...
once the picture is rnedered, the array needs to be written out. one interesting thing is that we need to keep everything in memory until it's all 
rendered because otherwise we won't write things in the right order.
*/

/*
buffer: where the results go.
bufferx: how wide the buffer is.
buffery: how tall the buffer is.
renderx: starting coord of the area to render
rendery: starting coord of the area to render
sizex: width of the block to render
sizey: height of the block to render
world: scene to render
*/
bool renderSection(vec3* buffer,int bufferx,int buffery, int rendery, int sizey, const hitable * world)
{
	camera cam(90, float(bufferx)/float(buffery));
	int ns = 1000;
	for (int j = rendery; j < sizey+rendery; j++)  // why the fuck do I need the /3?????
	{
		for (int i = 0; i < bufferx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + float(rand()) / RAND_MAX) / float(bufferx);
				float v = float(j + float(rand()) / RAND_MAX) / float(buffery);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}
			col /= float(ns);
			col = vec3(int(255.99 *sqrt(col[0])), int(255.99 *sqrt(col[1])), int(255.99 *sqrt(col[2])));
			
			buffer[ ((buffery -j -1)*bufferx) + i ] = col;
		}
	}
	return true;
}


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
	renderBythreads(buffer, nx, ny, world);

	for (int i = 0; i < nx*ny; i++)
	{
		std::cout << buffer[i].r() << " " << buffer[i].g() << " " << buffer[i].b() << "\n";
	}
	

	return 0;
}

typedef struct RenderContext {
	vec3* buffer;
	int bufferx;
	int buffery;
	int rendery;
	int sizey;
	const hitable * world;
} RENDERCONTEXT, *PRENDERCONTEXT;

void renderBythreads(vec3* buffer, int bufferx, int buffery, const hitable * world)
{
	PRENDERCONTEXT pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];

	// Create MAX_THREADS worker threads.

	for (int i = 0; i<MAX_THREADS; i++)
	{
		// Allocate memory for thread data.

		pDataArray[i] = (PRENDERCONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(RENDERCONTEXT));

		if (pDataArray[i] == NULL)
		{
			// If the array allocation fails, the system is out of memory
			// so there is no point in trying to print an error message.
			// Just terminate execution.
			ExitProcess(2);
		}

		// Generate unique data for each thread to work with.

		pDataArray[i]->buffer = buffer;
		pDataArray[i]->bufferx = bufferx;
		pDataArray[i]->buffery = buffery;
		pDataArray[i]->rendery = i*buffery/MAX_THREADS;
		pDataArray[i]->sizey = buffery/MAX_THREADS;
		pDataArray[i]->world = world;


		// Create the thread to begin execution on its own.

		hThreadArray[i] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			RenderThread,       // thread function name
			pDataArray[i],          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 


									// Check the return value for success.
									// If CreateThread fails, terminate execution. 
									// This will automatically clean up threads and memory. 

		if (hThreadArray[i] == NULL)
		{
			ErrorHandler(TEXT("CreateThread"));
			ExitProcess(3);
		}
	} // End of main thread creation loop.

	  // Wait until all threads have terminated.

	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

	// Close all thread handles and free memory allocations.

	for (int i = 0; i<MAX_THREADS; i++)
	{
		CloseHandle(hThreadArray[i]);
		if (pDataArray[i] != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pDataArray[i]);
			pDataArray[i] = NULL;    // Ensure address is not reused.
		}
	}
}



DWORD WINAPI RenderThread(LPVOID lpParam)
{
	//HANDLE hStdout;
	PRENDERCONTEXT pRenderContext = (PRENDERCONTEXT) lpParam;
	// now I know that passing a pointer to a buffer into a thread is not threadsafe, but given that the threads will all be accessing different parts of the buffer, it should be fine.
	renderSection(pRenderContext->buffer, pRenderContext->bufferx, pRenderContext->buffery, pRenderContext->rendery, pRenderContext->sizey, pRenderContext->world);
	return 0;
}



void ErrorHandler(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code.

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}