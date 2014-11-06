#include "worker.h"

namespace sys {

worker::worker() : thread(THREAD_JOINABLE), m_currentWork(NULL)
{
}


void worker::thread_func()
{
	mutex_lock(&m_mutex);

	if (m_currentWork) {
		m_currentWork->work();
		m_currentWork = NULL;
	}

	mutex_unlock(&m_mutex);
}

bool worker::finished()
{
	bool ret = false;

	if (mutex_trylock(&m_mutex) == 0) {
		mutex_unlock(&m_mutex);
		ret = true;
	}

	return ret;
}

void worker::work(workable* w)
{
	if ((finished()) && (!m_currentWork)) {
		m_currentWork = w;
		create_thread();
	}
}

}
