#include <iostream>
#include <unordered_set>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <curl/curl.h>

#include "concurrentqueue.hpp"
#include "concurrentset.hpp"
#include "httpclient.hpp"
#include "wordlist.hpp"
#include "options.hpp"
#include "debug.hpp"
#include "crawler.hpp"
#include "utils.hpp"

using namespace std;

unordered_set<string> wordlist;
unordered_set<string> extensions;
ConcurrentQueue<string> pending_scan_urls;
unordered_set<string> discovered_files;
unordered_set<string> scanned_urls;

int main(int argc, char** argv)
{
	Options opt;

	if(!opt.parse(argc, argv))
	{
		return 0;
	}

	curl_global_init(CURL_GLOBAL_ALL);

	opt.debug(0);

	ConcurrentSet<string> discovered_files_concurrent(discovered_files);
	ConcurrentSet<string> scanned_urls_concurrent(scanned_urls);

	pending_scan_urls.push(Utils::force_not_end_with_slash(opt.start_url));

	Wordlist::generate(wordlist, opt.extensions, opt.dict_file, opt.case_insensitive);
	Wordlist::fill_queue_with_wordlist(pending_scan_urls, scanned_urls_concurrent, opt.start_url, wordlist);


	Crawler crawler(opt, wordlist);
	HttpClient main_http_client;

	Debug::log(0, "Calculating not found code...");
	if(!crawler.calculate_not_found(opt.start_url, main_http_client))
	{
		Debug::log(0, "Response codes are too unstable, cannot calculate code; exiting...");
		return 0;
	}

    vector<thread> workers;
    for (int i = 0; i < opt.nthreads; i++)
    {
        workers.push_back(thread([&]() 
        {
        	HttpClient client;
            while(!pending_scan_urls.empty())
			{
				crawler.process_queue(pending_scan_urls, scanned_urls_concurrent, discovered_files_concurrent, client);
			}
        }));
    }

    std::for_each(workers.begin(), workers.end(), [](thread &t) 
    {
        t.join();
    });

    Debug::log(0, "Finished!");

	return 0;
}