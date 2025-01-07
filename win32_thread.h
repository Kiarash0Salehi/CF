#pragma once
#include "defWindow.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool initThread(thread*, THREADCALLBACK, void*);
bool stopThread(thread*);
bool resumeThread(thread*);
bool deleteThread(thread*);

#ifdef __cplusplus
}
#endif

