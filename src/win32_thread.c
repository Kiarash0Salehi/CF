
/*
* Copyright (c) 2024-2025 Ki(y)arash Salehi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
* to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of
* the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#define USE_WINDOW_STRUCT_
#include "./header/defWindow.h"

#include <process.h>

#define THREAD_FAILED 1
#define THREAD_SUCCESS 0


bool deleteThread(thread* Thread)
{
	return CloseHandle(Thread->id);
}

bool stopThread(thread* Thread)
{
	DWORD e = SuspendThread(Thread->id);
	return (e > (DWORD)-1) ? THREAD_SUCCESS : THREAD_FAILED;
}

bool resumeThread(thread* Thread)
{
	DWORD e = ResumeThread(Thread->id);
	return (e > (DWORD)-1) ? THREAD_SUCCESS : THREAD_FAILED;
}

bool initThread(thread* Thread, THREADCALLBACK func, size_t numArgs, ...)
{
	if(Thread == 0) return THREAD_FAILED;
	if(func == (void*)0) return THREAD_FAILED;
    memset(Thread, 0, sizeof(thread));
	
	void** data;
	if(numArgs != 0)
	{
		data = calloc(numArgs, sizeof(void*));
		va_list list;
		va_start(list, numArgs);
		for (size_t i = 0; i < numArgs; i++)
		{
			void* val = va_arg(list, void*);
			data[i] = val;
		}
	}
	else
	{
		data = calloc(1, sizeof(void*));
	}
	

    Thread->callback = func;
	Thread->user_data = data;
	Thread->id = (HANDLE)_beginthread(func, 0, data);
    return (Thread->id == (void*)0) ? THREAD_FAILED : THREAD_SUCCESS;
}

thread getWindowThread(Window* window)
{
	return ((WindowStruct*)window)->this_thread;
}
