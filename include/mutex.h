
#ifndef MUTEX_H
#define MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

	typedef struct mutex_t
	{
#ifdef _WIN32
		CRITICAL_SECTION handle;
#else
		pthread_mutex_t handle;
#endif
	} mutex_t;
	
	int mutex_init(mutex_t* mutex);
	void mutex_destroy(mutex_t* mutex);

	void mutex_lock(mutex_t* mutex);
	int  mutex_try_lock(mutex_t* mutex);
	void mutex_unlock(mutex_t* mutex);

#ifdef __cplusplus
}
#endif

#endif