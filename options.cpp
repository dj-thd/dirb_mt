#include <string>

#include "options.hpp"
#include "utils.hpp"
#include "debug.hpp"

using namespace std;

Options::Options()
{
	this->dict_file = "/usr/share/dirb/wordlists/common.txt";
	this->user_agent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1) dirb_mt/1.0";
	this->cookie = "";
	this->vhost = "";
	this->case_insensitive = false;
	this->nthreads = 1;
	this->sleep = 0;
	this->extensions.insert("");
}

bool Options::parse(int argc, char** argv)
{
	string name, value;
	size_t i = 1;
	size_t required = 0;

	if(argc < 2)
	{
		Debug::log(0, "Missing argument: [url]");
		return false;
	}
	this->start_url = string(argv[i]);
	if(argc == 2)
	{
		return true;
	}
	
	i++;

	if(argv[i][0] != '-')
	{
		this->dict_file = string(argv[i]);
		i++;
	}

	for(; i < (unsigned)argc; i++)
	{
		if(argv[i][0] != '-')
		{
			Debug::log(0, "Option not recognized: %s", argv[i]);
			return false;
		}

		required = 0;

		switch(argv[i][1])
		{
			case 'a':
				required = 1;
				break;
			case 'c':
				required = 1;
				break;
			case 'H':
				required = 1;
				break;
			case 'i':
				required = 0;
				break;
			case 'z':
				required = 1;
				break;
			case 'X':
				required = 1;
				break;
			case 'T':
				required = 1;
				break;
			default:
				Debug::log(0, "Option not recognized: %s", argv[i]);
				return false;
		}

		if(i+required >= (unsigned)argc)
		{
			Debug::log(0, "Missing %d argument(s) for option '%s'", argc - (i + required) + 1, argv[i]);
			return false;
		}

		switch(argv[i][1])
		{
			case 'a':
				this->user_agent = string(argv[i+1]);
				break;
			case 'c':
				this->cookie = string(argv[i+1]);
				break;
			case 'H':
				if(!Utils::parse_header(argv[i+1], name, value))
				{
					Debug::log(0, "Invalid header name/value pair: %s", argv[i+1]);
					return false;
				}
				this->headers[name] = value;
				break;
			case 'i':
				this->case_insensitive = true;
				break;
			case 'z':
				this->sleep = (unsigned long)stoi(string(argv[i+1]));
				break;
			case 'X':
				this->extensions.clear();
				if(!Utils::parse_csv(argv[i+1], this->extensions))
				{
					Debug::log(0, "Invalid value for extensions (must be comma separated values): %s", argv[i+1]);
					return false;
				}
				break;
			case 'T':
				this->nthreads = (unsigned short)stoi(string(argv[i+1]));
				break;
		}

		i += required;
	}
	return true;
}

void Options::debug(int level)
{
	Debug::log(level, "DICT_FILE: %s", this->dict_file.c_str());
	Debug::log(level, "USER_AGENT: %s", this->user_agent.c_str());
	Debug::log(level, "COOKIE: %s", this->cookie.c_str());
	Debug::log(level, "VHOST: %s", this->vhost.c_str());
	Debug::log(level, "CASE INSENSITIVE: %s", this->case_insensitive ? "TRUE" : "FALSE");
	Debug::log(level, "THREADS: %d", this->nthreads);
	Debug::log(level, "DELAY: %d", this->sleep);
	Debug::log(level, "ADDITIONAL_HEADERS:");
	for(auto& it : this->headers)
	{
		Debug::log(level, " [%s] -> %s", it.first.c_str(), it.second.c_str());
	}
	Debug::log(level, "EXTENSIONS:");
	for(auto& it : this->extensions)
	{
		Debug::log(level, " [%s]", it.c_str());
	}
}