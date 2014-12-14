#include <time.h>
#include <string.h>

#include "logger.h"

namespace sys
{

logger *logger::m_instance = static_cast<logger*>(0);

logger::logger(const char *filename)
{
	m_defaultLevel = LOG_DEBUG;

#ifdef __PLATFORM_WIN32__
	fopen_s(&m_fp, filename, "w");
#else
	m_fp = fopen(filename, "w");
#endif
	mutex_init(&m_mutex);
}


logger::~logger(void)
{
	mutex_destroy(&m_mutex);
	fclose(m_fp);
}

bool logger::createLogger(const char *filename)
{
	bool ret = false;

	if (m_instance == static_cast<logger*>(0)) {
		m_instance = new logger(filename);
		ret = true;
	}

	return ret;
}

void logger::destroyLogger()
{
	if (m_instance != static_cast<logger*>(0)) {
		delete m_instance;
	}
}

void logger::log(const char *fmt, ...)
{
	lock();
	va_list va;
	time_t t;
	char buf[256];

	time(&t);
#ifdef __PLATFORM_WIN32__
	struct tm timeinfo;
	localtime_s(&timeinfo, &t);
	memset(buf, 0, 256);
	strftime(buf, 256, "%c", &timeinfo);
#else
	struct tm *timeinfo;
	timeinfo = localtime(&t);
	memset(buf, 0, 256);
	strftime(buf, 256, "%c", timeinfo);
#endif

	va_start(va, fmt);
	fprintf(getInstance()->m_fp, "%s DEBUG: ", buf);
	vfprintf(getInstance()->m_fp, fmt, va);
	fprintf(getInstance()->m_fp, "\n");

	fflush(getInstance()->m_fp);

	va_end(va);
	unlock();
}

void logger::logAt(loglevel level, const char *fmt, ...)
{
}

void logger::lock()
{
	mutex_lock(&m_instance->m_mutex);
}

void logger::unlock()
{
	mutex_unlock(&m_instance->m_mutex);
}

}