#pragma once
#include <mutex>
namespace Logger
{
	static std::recursive_mutex m_rm;
	void Log(const char* log, ...);
};

namespace LoggerW
{
	static std::recursive_mutex m_rm;
	void Log(const wchar_t* log, ...);
}
#define LOG(...) Logger::Log(__VA_ARGS__)
#define LOGW(...) LoggerW::Log(__VA_ARGS__)