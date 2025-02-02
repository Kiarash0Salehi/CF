#pragma once
#include "defWindow.h"

#ifndef _WIN32_THREAD_H_
#define _WIN32_THREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif

bool initThread(thread*, THREADCALLBACK, size_t numArgs, ...);
bool stopThread(thread*);
bool resumeThread(thread*);
bool deleteThread(thread*);

#ifdef __cplusplus
}
#endif

#endif // _WIN32_THREAD_H_