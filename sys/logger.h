#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "thread.h"

namespace sys
{

class logger
{
public:
	enum loglevel
	{
		LOG_DEBUG = 0,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR,
		LOG_CRITICAL
	};

	static logger *getInstance() { return m_instance; }
	static bool createLogger(const char *filename);
	static void destroyLogger();

	static void log(const char *fmt, ...);
	static void logAt(loglevel level, const char *fmt, ...);
protected:
	static void lock();
	static void unlock();

	explicit logger(const char *filename);
	virtual ~logger(void);

private:
	FILE *m_fp;
	loglevel m_defaultLevel;

	static logger *m_instance;
	mutex m_mutex;
};

}