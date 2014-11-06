#pragma once

#include "thread_os.h"
#include "listener.h"

namespace sys
{

// define thread local storage variables
#ifdef __EVOLVE_WIN32__
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

#ifdef __EVOLVE_WIN32__
	DWORD m_tls;
#else
	pthread_key_t m_tls;
#endif
};

#define THREAD_DETACHED    0x000001
#define THREAD_JOINABLE    0x000002
#define THREAD_AFFINITY    0x000004
#define THREAD_BARRIER     0x000008

#ifdef __EVOLVE_WIN32__
typedef unsigned int (__stdcall *thread_caller_func)(void *);
#else
typedef void *(*thread_caller_func)(void *);
#endif

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
#ifdef __EVOLVE_WIN32__
	HANDLE getThread() { return m_thread; }
	unsigned getThreadId() { return m_thread_id; }
#else
	pthread_t getThread() { return m_thread; }
	pid_t getThreadId() { return m_thread_id; }
#endif

	void lock();
	void unlock();

protected:

#ifdef __EVOLVE_WIN32__
	HANDLE m_thread;
	unsigned m_thread_id;
#else
	pthread_t m_thread;
	pthread_attr_t m_thread_attr;
	pid_t m_thread_id;
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