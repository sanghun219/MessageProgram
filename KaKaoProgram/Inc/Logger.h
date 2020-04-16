#pragma once
#include <mutex>
namespace Logger
{
	static std::recursive_mutex m_rm;
	void Log(const char* log, ...);
};
#define LOG(...) Logger::Log(__VA_ARGS__)