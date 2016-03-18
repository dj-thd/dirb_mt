#ifndef __CRAWLER_HPP_INCLUDED_
#define __CRAWLER_HPP_INCLUDED_

#include <unordered_set>
#include <string>

#include "options.hpp"
#include "concurrentqueue.hpp"
#include "concurrentset.hpp"
#include "httpclient.hpp"

class Crawler
{
private:
	Options& options;
	std::unordered_set<std::string>& wordlist;
	int code_not_found;
	size_t mean_size_not_found;

public:
	Crawler(Options& opt, std::unordered_set<std::string>& wordlist) : options(opt), wordlist(wordlist) {};
	bool test_stable_directory(std::string& url, HttpClient& http_client);
	bool is_not_found(int code);
	bool calculate_not_found(std::string& base_url, HttpClient& http_client);
	bool confirm_directory(std::string& base_url, HttpClient& http_client);
	void process_queue(ConcurrentQueue<std::string>& scan_queue, ConcurrentSet<std::string>& scanned_set, ConcurrentSet<std::string>& discovered_files, HttpClient& http_client);
	void search_for_urls(ConcurrentQueue<std::string>& scan_queue, ConcurrentSet<std::string>& scanned_set, const std::string& url, const std::string& text);
};

#endif