#include "logger.h"

Logger::Logger()
{
	m_fp = fopen("gtti.log", "w");
}

Logger::~Logger()
{
	fclose(m_fp);
}

void Logger::log(const char* fmt, ...)
{
	static const unsigned int sBUFFER_LEN = 1024;

	char buffer[sBUFFER_LEN];
	FILE *fp = getInstance()->m_fp;

	va_list args;
	va_start(args, fmt);

	vsnprintf(buffer, sBUFFER_LEN, fmt, args);
	fprintf(fp, buffer);
	
	va_end(args);
}
