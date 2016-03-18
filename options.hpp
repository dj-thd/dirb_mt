#ifndef __OPTIONS_HPP_INCLUDED_
#define __OPTIONS_HPP_INCLUDED_

#include <string>
#include <unordered_map>
#include <unordered_set>

class Options
{
public:
	std::string start_url;
	std::string dict_file;
	std::string user_agent;
	std::string cookie;
	std::string vhost;
	std::unordered_map<std::string, std::string> headers;
	std::unordered_set<std::string> extensions;
	bool case_insensitive;
	unsigned short nthreads;
	unsigned sleep;

	Options();
	void debug(int level);
	bool parse(int argc, char** argv);
};

#endif