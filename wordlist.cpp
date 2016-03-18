#include <unordered_set>
#include <string>
#include <fstream>
#include <algorithm>

#include "wordlist.hpp"
#include "debug.hpp"
#include "utils.hpp"

using namespace std;

void Wordlist::generate(unordered_set<string>& dest, unordered_set<string>& extensions, string wordlist_filename, bool case_insensitive)
{
	Debug::log(0, "Generating wordlist [%s]...", wordlist_filename.c_str());
	ifstream infile(wordlist_filename);
	string line;
	while(getline(infile, line))
	{
		string line_trimmed = Utils::trim(line);
		if(line_trimmed.length() == 0)
		{
			continue;
		}
		for(auto& it : extensions)
		{
			string line_with_ext = line_trimmed + it;
			if(case_insensitive)
			{
				transform(line_with_ext.begin(), line_with_ext.end(), line_with_ext.begin(), [](unsigned char c) {return toupper(c);});
			}
			dest.insert(line_with_ext);
		}
	}
	Debug::log(0, "Wordlist generation complete: %d words generated", dest.size());
}

void Wordlist::fill_queue_with_wordlist(ConcurrentQueue<std::string>& queue, ConcurrentSet<std::string>& scanned_set, std::string base, std::unordered_set<string>& wordlist)
{
	string base_fixed = Utils::force_end_with_slash(base);
	auto queue_lock = queue.get_lock();
	auto set_lock = scanned_set.get_lock();
	for(auto& it : wordlist)
	{
		string str = base_fixed + it;
		if(!scanned_set.count_unsafe(str))
		{
			scanned_set.insert_unsafe(str);
			queue.push_unsafe(str);
		}
	}
	set_lock.unlock();
	queue_lock.unlock();
}