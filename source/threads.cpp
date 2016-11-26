#include "threads.h"
#include "camera.h"
#ifdef _WIN32
#include "stdafx.h"
#endif // _WINDOWS_MAGIC

/*
Ok. So to divide this up into threadable chunks, I need to be able to call a function with the state of the scene and a coordinate range to render...
once the picture is rnedered, the array needs to be written out. one interesting thing is that we need to keep everything in memory until it's all
rendered because otherwise we won't write things in the right order.
*/

bool renderByThread(PRENDERCONTEXT rc)
{
    vec3 lookfrom = vec3(-1, 2, 1);
	vec3 lookat = vec3(.2, -.2, -.8);
	float dist_to_focus = (lookfrom - lookat).length();
	float aperture = 1.0;
	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(rc->bufferx) / float(rc->buffery), aperture, dist_to_focus);
	for (int j = rc->rendery; j < rc->sizey + rc->rendery; j++)
	{
		for (int i = 0; i < rc->bufferx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < rc->samples; s++)
			{
				float u = float(i + float(rand()) / RAND_MAX) / float(rc->bufferx);
				float v = float(j + float(rand()) / RAND_MAX) / float(rc->buffery);
				ray r = cam.get_ray(u, v);
				col += color(r, rc->world, 0);
			}
			col /= float(rc->samples);
			col = vec3(int(255.99 *sqrt(col[0])), int(255.99 *sqrt(col[1])), int(255.99 *sqrt(col[2])));

			rc->buffer[((rc->buffery - j - 1)*rc->bufferx) + i] = col;
		}
	}
	return 0;
}



#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


DWORD WINAPI RenderThread(LPVOID lpParam);
void ErrorHandler(LPTSTR lpszFunction);

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
bool threadRenderer::renderSection(PRENDERCONTEXT rc)
{
	//TODO:  alter this to only render the sectoin specified.

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

		pDataArray[i]->buffer = rc->buffer;
		pDataArray[i]->bufferx = rc->bufferx;
		pDataArray[i]->buffery = rc->buffery;
		pDataArray[i]->rendery = i*rc->buffery / MAX_THREADS;
		pDataArray[i]->sizey = rc->buffery / MAX_THREADS;
		pDataArray[i]->world = rc->world;
		pDataArray[i]->samples = rc->samples;

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
	return true;
}


DWORD WINAPI RenderThread(LPVOID lpParam)
{
	//HANDLE hStdout;
	PRENDERCONTEXT pRenderContext = (PRENDERCONTEXT)lpParam;
	// now I know that passing a pointer to a buffer into a thread is not threadsafe, but given that the threads will all be accessing different parts of the buffer, it should be fine.
    renderByThread(pRenderContext);
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
#else
// pthreads implementation.
#include <pthread.h>

void *RenderThread(void * param);

bool threadRenderer::renderSection(PRENDERCONTEXT rc)
{
	RENDERCONTEXT pDataArray[MAX_THREADS];
	pthread_t  hThreadArray[MAX_THREADS];

	// Create MAX_THREADS worker threads.

	for (int i = 0; i<MAX_THREADS; i++)
	{
		pDataArray[i]->buffer = rc->buffer;
		pDataArray[i]->bufferx = rc->bufferx;
		pDataArray[i]->buffery = rc->buffery;
		pDataArray[i]->rendery = i*rc->buffery / MAX_THREADS;
		pDataArray[i]->sizey = rc->buffery / MAX_THREADS;
		pDataArray[i]->world = rc->world;
		pDataArray[i]->samples = rc->samples;

		// Create the thread to begin execution on its own.

		int ret = pthread_create(&hThreadArray[i], NULL, RenderThread, (void*)(&pDataArray[i]));


		if (hThreadArray[i] == NULL)
		{
			// error.
		}
	} // End of main thread creation loop.

	for (int i = 0; i<MAX_THREADS; i++)
	{
		pthread_join(hThreadArray[i], NULL);
	}

}



void *RenderThread(void * param)
{
	//HANDLE hStdout;
	PRENDERCONTEXT pRenderContext = (PRENDERCONTEXT)param;
	// now I know that passing a pointer to a buffer into a thread is not threadsafe, but given that the threads will all be accessing different parts of the buffer, it should be fine.
	renderByThread(pRenderContext);
	return 0; // this is probs wron g.
}


#endif
