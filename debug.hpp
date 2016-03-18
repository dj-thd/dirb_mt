#ifndef __DEBUG_HPP_INCLUDED__
#define __DEBUG_HPP_INCLUDED__

#include <mutex>

class Debug
{
private:
	static std::mutex mutex_;
public:
	static void log(int level, const char* fmt, ...);
	static void log_raw(int level, const char* fmt, ...);
};

#endif