#include "threads.h"
#include "camera.h"
#include <chrono>
#include <thread>
#ifdef _WIN32
#include "stdafx.h"
#endif // _WINDOWS_MAGIC


using namespace std::chrono;
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
	camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(rc->buffer_width) / float(rc->buffer_height), aperture, dist_to_focus);
	for (int j = rc->y_offset; j < rc->num_rows_to_render + rc->y_offset; j++)
	{
		for (int i = 0; i < rc->buffer_width; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < rc->samples; s++)
			{
				float u = float(i + float(rand()) / RAND_MAX) / float(rc->buffer_width);
				float v = float(j + float(rand()) / RAND_MAX) / float(rc->buffer_height);
				ray r = cam.get_ray(u, v);
				col += color(r, rc->world, 0);
			}
			col /= float(rc->samples);
			col = vec3(int(255.99 *sqrt(col[0])), int(255.99 *sqrt(col[1])), int(255.99 *sqrt(col[2])));

			rc->buffer[((rc->buffer_height - j - 1)*rc->buffer_width) + i] = col;
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

bool threadRenderer::renderSection(PRENDERCONTEXT rc)
{
	//TODO:  alter this to only render the sectoin specified.
	int num_threads = std::thread::hardware_concurrency();
	PRENDERCONTEXT pDataArray[MAX_THREADS];
	DWORD   dwThreadIdArray[MAX_THREADS];
	HANDLE  hThreadArray[MAX_THREADS];

	// Create MAX_THREADS worker threads.

	for (int i = 0; i<num_threads; i++)
	{
		// Allocate memory for thread data.

		pDataArray[i] = (PRENDERCONTEXT)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
			sizeof(RENDERCONTEXT));

		if (pDataArray[i] == nullptr)
		{
			// If the array allocation fails, the system is out of memory
			// so there is no point in trying to print an error message.
			// Just terminate execution.
			ExitProcess(2);
		}

		// Generate unique data for each thread to work with.

		pDataArray[i]->buffer = rc->buffer;
		pDataArray[i]->buffer_width = rc->buffer_width;
		pDataArray[i]->buffer_height = rc->buffer_height;
		pDataArray[i]->y_offset = i*rc->buffer_height / num_threads;
		pDataArray[i]->num_rows_to_render = rc->buffer_height / num_threads;
		pDataArray[i]->world = rc->world;
		pDataArray[i]->samples = rc->samples;

		// Create the thread to begin execution on its own.

		hThreadArray[i] = CreateThread(
			nullptr,                   // default security attributes
			0,                      // use default stack size
			RenderThread,       // thread function name
			pDataArray[i],          // argument to thread function
			0,                      // use default creation flags
			&dwThreadIdArray[i]);   // returns the thread identifier


									// Check the return value for success.
									// If CreateThread fails, terminate execution.
									// This will automatically clean up threads and memory.

		if (hThreadArray[i] == nullptr)
		{
			ErrorHandler(TEXT("CreateThread"));
			ExitProcess(3);
		}
		std::cout << "Started thread  " << GetThreadId(hThreadArray[i]) << std::endl;
	} // End of main thread creation loop.

	  // Wait until all threads have terminated.

	WaitForMultipleObjects(num_threads, hThreadArray, TRUE, INFINITE);

	// Close all thread handles and free memory allocations.

	for (int i = 0; i<num_threads; i++)
	{
		CloseHandle(hThreadArray[i]);
		if (pDataArray[i] != nullptr)
		{
			HeapFree(GetProcessHeap(), 0, pDataArray[i]);
			pDataArray[i] = nullptr;    // Ensure address is not reused.
		}
	}
	return true;
}


DWORD WINAPI RenderThread(LPVOID lpParam)
{
	milliseconds start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//HANDLE hStdout;
	PRENDERCONTEXT pRenderContext = (PRENDERCONTEXT)lpParam;
	// now I know that passing a pointer to a buffer into a thread is not threadsafe, but given that the threads will all be accessing different parts of the buffer, it should be fine.
    renderByThread(pRenderContext);
	milliseconds end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	auto total_ms = (end - start).count();

	std::cout << "Finished thread  "<< GetCurrentThreadId() << " in " << total_ms / 1000 << "." << total_ms % 1000 << "s" << std::endl;
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
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, nullptr);

	// Display the error message.

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(nullptr, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

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
		pDataArray[i]->buffer_width = rc->buffer_width;
		pDataArray[i]->buffer_height = rc->buffer_height;
		pDataArray[i]->y_offset = i*rc->buffer_height / MAX_THREADS;
		pDataArray[i]->num_rows_to_render = rc->buffer_height / MAX_THREADS;
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
