#ifndef _WORDLIST_HPP_INCLUDED_
#define _WORDLIST_HPP_INCLUDED_

#include <unordered_set>
#include <string>

#include "concurrentqueue.hpp"
#include "concurrentset.hpp"

class Wordlist
{
public:
	static void generate(std::unordered_set<std::string>& dest, std::unordered_set<std::string>& extensions, std::string wordlist_filename, bool case_insensitive);
	static void fill_queue_with_wordlist(ConcurrentQueue<std::string>& queue, ConcurrentSet<std::string>& scanned_set, std::string base, std::unordered_set<std::string>& wordlist);
};

#endif