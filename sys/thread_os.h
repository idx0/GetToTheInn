/******************************************************************************
 * Copyright (2013) Gorilla Software
 * 
 * NOTICE:  All information contained herein is, and remains the property of
 * Gorilla Software and its suppliers, if any.  The intellectual and technical
 * concepts contained herein are proprietary to Gorilla Software and its
 * suppliers and may be covered by U.S. and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.  Dissemination
 * of this information or reproduction of this material is strictly forbidden
 * unless prior written permission is obtained from Gorilla Software.
 *****************************************************************************/

#pragma once

#if (_MSC_VER > 0 && (defined(_WIN32) || defined(__MINGW32__)))
#  define __EVOLVE_WIN32__
#  define WIN32_LEAN_AND_MEAN
#  define NOMINMAX
#  include <windows.h>
#  include <intrin.h>
#  include <process.h>
#else
#  define __EVOLVE_LINUX_
#  include <unistd.h>
#  include <pthread.h>
#endif

#include <errno.h>

namespace sys
{

// mutex prototype
struct mutex
{
#ifdef __EVOLVE_WIN32__
	CRITICAL_SECTION m;
#else /* __EVOLVE_UNIX__ */
	pthread_mutex_t m;
#endif
};

// mutex functions are global
int mutex_init(mutex *m);
int mutex_destroy(mutex *m);

int mutex_lock(mutex *m);
int mutex_unlock(mutex *m);
int mutex_trylock(mutex *m);

struct barrier
{
	int thread_cnt;
#ifdef __EVOLVE_WIN32__
	unsigned int wait_cnt;
	HANDLE sem;
#else
	pthread_barrier_t sem;
#endif
};

void barrier_init(barrier *b, int nthreads);
void barrier_destroy(barrier *b);

void barrier_wait(barrier *b);

extern barrier barrier_update;
extern barrier barrier_calculate;

}