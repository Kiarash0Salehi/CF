#pragma once
#include "defWindow.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool initThread(thread* Thread, void (*func)(void*), void* data);

#ifdef __cplusplus
}
#endif

