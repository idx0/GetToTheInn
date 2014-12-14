#pragma once

#include "platform.h"

namespace sys
{

// mutex prototype
struct mutex
{
#ifdef __PLATFORM_WIN32__
	CRITICAL_SECTION m;
#else /* __PLATFORM_UNIX__ */
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
#ifdef __PLATFORM_WIN32__
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

// define thread local storage variables
#ifdef __PLATFORM_WIN32__
typedef long thread_control_t;
#  define EVOLVE_THREAD_DEFAULT_CONTROL 0
#else
typedef pthread_once_t thread_control_t;
#  define EVOLVE_THREAD_DEFAULT_CONTROL PTHREAD_ONCE_INIT
#endif

// thread local storage
class thread_tls
{
public:
	thread_tls();
	virtual ~thread_tls();

	thread_control_t m_tlsControl;

#ifdef __PLATFORM_WIN32__
	DWORD m_tls;
#else
	pthread_key_t m_tls;
#endif
};

#define THREAD_DETACHED    0x000001
#define THREAD_JOINABLE    0x000002
#define THREAD_AFFINITY    0x000004
#define THREAD_BARRIER     0x000008

#ifdef __PLATFORM_WIN32__
typedef unsigned int (__stdcall *thread_caller_func)(void *);
#else
typedef void *(*thread_caller_func)(void *);
#endif

class listener;

// main thread class
class thread
{
public:
	explicit thread(unsigned int flags);
	virtual ~thread();

	// returns the number of detected processors
	int nCpus();

	// creates the thread and runs thread_func()
	void create_thread();
	void exit_thread(unsigned int result);

	int once(thread_tls *tls);

	void join();

	virtual void thread_func() = 0;

	unsigned int getFlags() { return m_flags; }
	void setFlags(unsigned int flags) { m_flags = flags; }
	unsigned int getResult() { return m_result; }

	listener *getListener() { return m_listener; }
#ifdef __PLATFORM_WIN32__
	HANDLE getThread() { return m_thread; }
	unsigned getThreadId() { return m_thread_id; }
#else
	pthread_t getThread() { return m_thread; }
	pid_t getThreadId() { return m_thread_id; }
#endif

	void lock();
	void unlock();

protected:

#ifdef __PLATFORM_WIN32__
	HANDLE m_thread;
	unsigned m_thread_id;
#else
	pthread_t m_thread;
	pthread_attr_t m_thread_attr;
public:
	pid_t m_thread_id;
protected:
#endif
	thread_caller_func m_function;

	listener *m_listener;
	mutex m_mutex;
private:
	unsigned int m_flags;
	unsigned int m_result;

	virtual void run_once();
};

int thread_join(thread *t, void **result);

int thread_detach(thread *t);

}
