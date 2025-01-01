#include "defWindow.h"

#include <process.h>

thread Thread;
void* data;

static bool startThread()
{
    if(Thread.threadFunc == (void*)0) return 0;
	Thread.id = (HANDLE)_beginthread(Thread.threadFunc, 0, data);
}

static bool removeThread()
{
	return CloseHandle(Thread.id);
}

static bool stopThread()
{
	DWORD e = SuspendThread(Thread.id);
	return (e > (DWORD)-1) ? 1 : 0;
}

static bool resumeThread()
{
	DWORD e = ResumeThread(Thread.id);
	return (e > (DWORD)-1) ? 1 : 0;
}

bool initThread(thread* Thread, void (*func)(void*), void* _data)
{
    memset(Thread, 0, sizeof(thread));
    Thread->threadFunc = func;
    data = _data;
    Thread->start = startThread;
    Thread->stop = stopThread;
    Thread->resume = resumeThread;
    Thread->shutdown = removeThread;
}