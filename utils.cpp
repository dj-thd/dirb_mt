#include <string>
#include <unordered_set>
#include <algorithm>

#include "utils.hpp"
#include "debug.hpp"

using namespace std;

bool Utils::parse_header(const char* str, string& name, string& value)
{
	int state = 0;
	const char* name_begin = str;
	const char* value_begin = 0;
	size_t name_length = 0;
	size_t value_length = 0;
	size_t i = 0;

	for(; str[i] && str[i] != 0x0A && str[i] != 0x0D; i++)
	{
		switch(state)
		{
			case 0:
				if(str[i] == ':')
				{
					name_length = i;
					i++;
					state = 1;
				}
				break;
			case 1:
				value_begin = str + i;
				state = 2;
				break;
		}
	}

	if(state != 2)
	{
		return false;
	}

	value_length = str + i - value_begin;

	if(name_length == 0 || value_length == 0)
	{
		return false;
	}

	name = string(name_begin, name_length);
	value = string(value_begin, value_length);
	return true;
}

bool Utils::parse_csv(const char* str, unordered_set<string>& result)
{
	const char* item_start = str;
	size_t item_length = 0;
	size_t i = 0;
	int state = 0;

	for(; str[i]; i++)
	{
		switch(state)
		{
			case 0:
				if(str[i] == ',')
				{
					item_length = str + i - item_start;
					if(!item_start || item_length == 0)
					{
						result.insert("");
					} else {
						result.insert(string(item_start, item_length));
						item_start = 0;
					}
					state = 1;
				}
				break;
			case 1:
				item_start = str + i;
				state = 0;
				break;
		}
	}

	if(state == 0)
	{
		item_length = str + i - item_start;
		if(!item_start || item_length == 0)
		{
			result.insert("");
		} else {
			result.insert(string(item_start, item_length));
		}
	}

	return true;
}

std::string Utils::trim(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isspace(*it))
	{
		it++;
	}

	std::string::const_reverse_iterator rit = s.rbegin();
	while (rit.base() != it && isspace(*rit))
	{
		rit++;
	}

	return std::string(it, rit.base());
}

bool Utils::ends_with(const std::string& value, const std::string& ending)
{
	if (ending.size() > value.size())
	{
		return false;
	}
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool Utils::begins_with(const std::string& value, const std::string& beginning)
{
	if (beginning.size() > value.size())
	{
		return false;
	}
	return std::equal(beginning.begin(), beginning.end(), value.begin());
}

string Utils::random_string(size_t min_len, size_t max_len)
{
	string result(max_len, 0);
	generate_n(result.begin(), max_len, []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		return charset[ rand() % (sizeof(charset) - 1) ];
	});
	return result;
}

string Utils::force_end_with_slash(const string& original)
{
	if(!Utils::ends_with(original, "/"))
	{
		return original + "/";
	}
	return original;
}

string Utils::force_not_end_with_slash(const string& original)
{
	if(Utils::ends_with(original, "/"))
	{
		return original.substr(0, original.length()-1);
	}
	return original;
}

// new path must begin with slash
string Utils::replace_url_path(const std::string& url, const std::string& new_path)
{
	auto pos = url.find_first_of("/", 8);
	if(pos)
	{
		return url.substr(0, pos) + new_path;
	}
	return Utils::force_not_end_with_slash(url) + new_path;
}

string Utils::url_basedir(const std::string& url)
{
	string url_fixed = Utils::force_not_end_with_slash(url);
	auto pos = url_fixed.find_last_of("/");
	if(pos)
	{
		return Utils::force_end_with_slash(url_fixed.substr(0, pos));
	}
	return Utils::force_end_with_slash(url_fixed);
}