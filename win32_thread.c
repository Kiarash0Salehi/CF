#include "defWindow.h"

#include <process.h>

bool deleteThread(thread* Thread)
{
	return CloseHandle(Thread->id);
}

bool stopThread(thread* Thread)
{
	DWORD e = SuspendThread(Thread->id);
	return (e > (DWORD)-1) ? 1 : 0;
}

bool resumeThread(thread* Thread)
{
	DWORD e = ResumeThread(Thread->id);
	return (e > (DWORD)-1) ? 1 : 0;
}

bool initThread(thread* Thread, THREADCALLBACK func, size_t numArgs, ...)
{
    memset(Thread, 0, sizeof(thread));
    if(func == (void*)0) return 0;
    Thread->threadFunc = func;
	void** data = calloc(numArgs, sizeof(void*));
	va_list list;
	va_start(list, numArgs);
	for (size_t i = 0; i < numArgs; i++)
	{
		void* val = va_arg(list, void*);
		data[i] = val;
	}
    
	Thread->id = (HANDLE)_beginthread(func, 0, data);
    return (Thread->id == (void*)0) ? -1 : 0;
}