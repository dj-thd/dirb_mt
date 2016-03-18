#include <cstdarg>
#include <cstdio>

#include "debug.hpp"

using namespace std;

void Debug::log(int level, const char* fmt, ...)
{
	std::unique_lock<std::mutex> mlock(Debug::mutex_);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
	fflush(stdout);
	mlock.unlock();
}

void Debug::log_raw(int level, const char* fmt, ...)
{
	std::unique_lock<std::mutex> mlock(Debug::mutex_);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
	mlock.unlock();
}

mutex Debug::mutex_;