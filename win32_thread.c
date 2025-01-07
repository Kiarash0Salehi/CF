#include "defWindow.h"

#include <process.h>

bool removeThread(thread* Thread)
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

bool initThread(thread* Thread, THREADCALLBACK func, void* _data)
{
    memset(Thread, 0, sizeof(thread));
    if(func == (void*)0) return 0;
    Thread->threadFunc = func;
    Thread->data = _data;
    
	Thread->id = (HANDLE)_beginthread(func, 0, _data);
    return (Thread->id == (void*)0) ? -1 : 0;
}