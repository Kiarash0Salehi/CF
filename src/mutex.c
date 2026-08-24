#include "mutex.h"
#include <stdio.h>
#include <string.h>

int mutex_init(mutex_t* m)
{
    memset(m, 0, sizeof(mutex_t));
#ifdef _WIN32
    InitializeCriticalSection(&m->handle);
    return 0;
#else
    pthread_mutexattr_t attr;
    int r = pthread_mutexattr_init(&attr);
    if (r != 0)
        return r;
    r = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    if (r != 0)
    {
        pthread_mutexattr_destroy(&attr);
        return r;
    }
    r = pthread_mutex_init(&m->handle, &attr);
    pthread_mutexattr_destroy(&attr);
    return r;
#endif
}

void mutex_destroy(mutex_t* m)
{
#ifdef _WIN32
    DeleteCriticalSection(&m->handle);
#else
    pthread_mutex_destroy(&m->handle);
#endif
}

void mutex_lock(mutex_t* m)
{
#ifdef _WIN32
    EnterCriticalSection(&m->handle);
#else
    pthread_mutex_lock(&m->handle);
#endif
}

void mutex_unlock(mutex_t* m)
{
#ifdef _WIN32
    LeaveCriticalSection(&m->handle);
#else
    pthread_mutex_unlock(&m->handle);
#endif
}

int mutex_try_lock(mutex_t* m)
{
#ifdef _WIN32
    return TryEnterCriticalSection(&m->handle);
#else
    return pthread_mutex_trylock(&m->handle) == 0;
#endif
}
