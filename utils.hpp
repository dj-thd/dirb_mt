#ifndef __UTILS_HPP_INCLUDED_
#define __UTILS_HPP_INCLUDED_

#include <string>
#include <unordered_set>

class Utils
{
public:
	static bool parse_header(const char* str, std::string& name, std::string& value);
	static bool parse_csv(const char* str, std::unordered_set<std::string>& result);
	static std::string trim(const std::string& s);
	static bool ends_with(const std::string& value, const std::string& ending);
	static bool begins_with(const std::string& value, const std::string& beginning);
	static std::string force_end_with_slash(const std::string& original);
	static std::string force_not_end_with_slash(const std::string& original);
	static std::string random_string(size_t min_len, size_t max_len);
	static std::string replace_url_path(const std::string& url, const std::string& new_path);
	static std::string url_basedir(const std::string& url);
};

#endif