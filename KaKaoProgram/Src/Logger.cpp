#include "Logger.h"
#include "ServLibrary.h"

void Logger::Log(const char * log, ...)
{
	std::lock_guard<std::recursive_mutex> lock(m_rm);

	static char buf[1024];
	va_list ap;
	//strcpy_s(buf, "[ERROR] ");

	va_start(ap, log);
	vsprintf_s(buf + strlen(buf), sizeof(buf), log, ap);
	va_end(ap);

	std::cout << buf << std::endl;
	buf[0] = { 0, };
}