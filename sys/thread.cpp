#include "thread.h"

namespace sys
{

int mutex_lock(mutex *m)
{
#ifdef __EVOLVE_WIN32__
	EnterCriticalSection(&m->m);
	return 0;
#else /* __EVOLVE_UNIX__ */
	return pthread_mutex_lock(&m->m);
#endif
}

int mutex_unlock(mutex *m)
{
#ifdef __EVOLVE_WIN32__
	LeaveCriticalSection(&m->m);
	return 0;
#else /* __EVOLVE_UNIX__ */
	return pthread_mutex_unlock(&m->m);
#endif
}

int mutex_trylock(mutex *m)
{
#ifdef __EVOLVE_WIN32__
	return (TryEnterCriticalSection(&m->m) ? 0 : EBUSY);
#else /* __EVOLVE_UNIX__ */
	return pthread_mutex_trylock(&m->m);
#endif
}

int mutex_init(mutex *m)
{
#ifdef __EVOLVE_WIN32__
	InitializeCriticalSection(&m->m);
	return 0;
#else /* __EVOLVE_UNIX__ */
	return pthread_mutex_init(&m->m, NULL);
#endif
}

int mutex_destroy(mutex *m)
{
#ifdef __EVOLVE_WIN32__
	DeleteCriticalSection(&m->m);
	return 0;
#else /* __EVOLVE_UNIX__ */
	return pthread_mutex_destroy(&m->m);
#endif
}

void barrier_init(barrier *b, int nthreads)
{
	b->thread_cnt = nthreads;
#ifdef __EVOLVE_WIN32__
	b->wait_cnt = nthreads;
	b->sem = CreateSemaphore(0, 0, nthreads, NULL);
#else
	pthread_barrier_init(&b->sem, NULL, nthreads);
#endif
}

void barrier_destroy(barrier *b)
{
#ifdef __EVOLVE_WIN32__
	CloseHandle(b->sem);
#else
	pthread_barrier_destroy(&b->sem);
#endif
}

void barrier_wait(barrier *b)
{
#ifdef __EVOLVE_WIN32__
	if (InterlockedDecrement(&(b->wait_cnt)) == 0) {
		b->wait_cnt = b->thread_cnt;
		ReleaseSemaphore(b->sem, (b->thread_cnt - 1), 0);
	} else {
		WaitForSingleObject(b->sem, INFINITE);
	}
#else
	pthread_barrier_wait(&b->sem);
#endif
}

static thread_tls global_tls;

thread_tls::thread_tls()
{
#ifdef __EVOLVE_WIN32__
	m_tls = TlsAlloc();
	if (m_tls == TLS_OUT_OF_INDEXES) _exit(ENOMEM);
#else
	pthread_key_create(&m_tls, NULL);
#endif
}

barrier barrier_update;
barrier barrier_calculate;

thread_tls::~thread_tls()
{
#ifdef __EVOLVE_WIN32__
	TlsFree(m_tls);
#else
	pthread_key_delete(m_tls);
#endif
}

#ifdef __EVOLVE_WIN32__
static unsigned int __stdcall __win32_callback_wrapper(void *parg)
{
	thread *t = (thread *)parg;
	
//	t->once(&global_tls);
//	TlsSetValue(global_tls.m_tls, t);
	
	if (t) { t->thread_func(); }
	
//	while (t->getThread() == (HANDLE)-1) {
//		YieldProcessor();
//		_ReadWriteBarrier();
//	}

	_endthreadex(t->getResult());
	
	return t->getResult();
}
#else
static void *__linux_callback_wrapper(void *parg)
{
	thread *t = (thread *)parg;
	
	dpl_thread_once(&_dpl_tls_control, __dpl_tls_init);
	pthread_setspecific(_dpl_tls, t);
	
	if (t) {
		t->m_thread_id = getpid();
		t->thread_func();
	}

	pthread_exit(&m_result);
}
#endif


thread::thread(unsigned int flags)
{
	int rc = 1;

	m_flags = flags;
	m_result = NULL;
		
#ifdef __EVOLVE_WIN32__
	m_thread = (HANDLE)-1;
	m_thread_id = 0;
#else
	m_thread_id = getpid();
#endif

	mutex_init(&m_mutex);
	m_listener = new listener(this);
}

thread::~thread()
{
	delete m_listener;
	mutex_destroy(&m_mutex);
}

void thread::lock()
{
	mutex_lock(&m_mutex);
}

void thread::unlock()
{
	mutex_unlock(&m_mutex);
}

int thread::once(thread_tls *tls)
{
	tls->m_tlsControl = EVOLVE_THREAD_DEFAULT_CONTROL;

#ifdef __EVOLVE_WIN32__
	int state = (int)tls->m_tlsControl;
	
	_ReadWriteBarrier();
	
	while (state != 1) {
		if ((!state) && (!_InterlockedCompareExchange(&(tls->m_tlsControl), 2, 0))) {
			run_once();
			tls->m_tlsControl = 1;
			
			return 0;
		}
		
		YieldProcessor();
		_ReadWriteBarrier();
		
		state = (int)tls->m_tlsControl;
	}
	
	return 0;
#else
	return pthread_once(tls->m_tlsControl, run_once);
#endif
}

void thread::create_thread()
{
#ifdef __EVOLVE_WIN32__
	m_thread = (HANDLE)-1;
	_ReadWriteBarrier();
	
	m_thread = (HANDLE)_beginthreadex(NULL, 0,
		&__win32_callback_wrapper, this, 0, &m_thread_id);
#else
	pthread_attr_init(&m_thread_attr);
	if (m_flags & THREAD_JOINABLE) {
		pthread_attr_setdetachstate(&m_thread_attr, PTHREAD_CREATE_JOINABLE);
	}
	
	if (pthread_create(&m_thread, &m_thread_attr,
		&__linux_callback_wrapper, this)) {
		
		_exit(ENOMEM);
	}
	
	pthread_attr_destroy(&m_thread_attr);
#endif

	if (m_flags & THREAD_DETACHED) {
		thread_detach(this);
	}
}

void thread::exit_thread(unsigned int result)
{
	m_result = result;

#ifdef __EVOLVE_WIN32__
	_endthreadex(m_result);
#else
	pthread_exit(&m_result);
#endif
}

void thread::join()
{
	thread_join(this, NULL);
}

int thread_detach(thread *t)
{
#ifdef __EVOLVE_WIN32__
	CloseHandle(t->getThread());
	_ReadWriteBarrier();
	
	return 0;
#else
	return pthread_detach(t->getThread());
#endif
}

int thread_join(thread *t, void **result)
{
	if (!(t->getFlags() & THREAD_JOINABLE)) return 1;
	
#ifdef __EVOLVE_WIN32__	
	WaitForSingleObject(t->getThread(), INFINITE);
	CloseHandle(t->getThread());
	
	if (result) { *result = (void *)t->getResult(); }
	
	return 0;
#else
	return pthread_join(t->getThread(), result);
#endif
}

void thread::run_once()
{
}

}