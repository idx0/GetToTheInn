#pragma once

#include <stdio.h>
#include <string>
#include <stdarg.h>

#include "util.h"

class Logger : public Utils::Singleton<Logger>
{
public:
	Logger();
	~Logger();

	static void log(const char* fmt, ...);

protected:

	FILE *m_fp;
};

