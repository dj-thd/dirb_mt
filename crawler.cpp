#include <string>
#include <regex>

#include "crawler.hpp"
#include "options.hpp"
#include "concurrentqueue.hpp"
#include "concurrentset.hpp"
#include "httpclient.hpp"
#include "wordlist.hpp"
#include "debug.hpp"
#include "utils.hpp"

using namespace std;

regex url_regex(R"((https?:\/\/([\w_-]+(?:(?:\.[\w_\-\:]+)+))|\"(?!http))([\w.,^%:\/~+-]*[\w^%\/~+-])?)");

bool Crawler::test_stable_directory(string& url, HttpClient& http_client)
{
	string url_fixed = Utils::force_end_with_slash(url);
	for(size_t i = 0; i < 10; i++)
	{
		string rnd = Utils::random_string(10, 10);
		if(!this->is_not_found(http_client.get(url_fixed + rnd)))
		{
			return false;
		}
	}
	return true;
}

bool Crawler::is_not_found(int code)
{
	if(code == 404 || code == this->code_not_found || code == 0)
	{
		return true;
	} else {
		return false;
	}
}

bool Crawler::calculate_not_found(string& base_url, HttpClient& http_client)
{
	string base_url_fixed = Utils::force_end_with_slash(base_url);
	vector<int> codes;
	vector<size_t> sizes;

	codes.reserve(10);
	sizes.reserve(10);

	for(size_t i = 0; i < 10; i++)
	{
		string rnd = Utils::random_string(10, 10);
		codes.push_back(http_client.get(base_url_fixed + rnd));
		sizes.push_back(http_client.get_last_response_size());
	}

	if(!equal(codes.begin()+1, codes.end(), codes.begin()))
	{
		return false;
	}

	this->code_not_found = codes[0];

	size_t sum = 0;
	for(size_t i : sizes)
	{
		sum += i;
	}
	sum /= 10;
	this->mean_size_not_found = sum;

	return true;
}

bool Crawler::confirm_directory(string& url, HttpClient& http_client)
{
	string url_with_slash = Utils::force_end_with_slash(url);
	int code_with_slash = http_client.get(url_with_slash);
	if(this->is_not_found(code_with_slash))
	{
		return false;
	} else {
		return true;
	}
}

void Crawler::process_queue(ConcurrentQueue<std::string>& scan_queue, ConcurrentSet<std::string>& scanned_set, ConcurrentSet<std::string>& discovered_files, HttpClient& http_client)
{
	int code = 0;
	string url = "";
	if(scan_queue.pop_nb(url))
	{
		if(discovered_files.count(url))
		{
			return;
		}
		Debug::log_raw(0, "testing %s                                                 \r", url.c_str());
		code = http_client.get(url);
		if(this->is_not_found(code))
		{
			return;
		}
		switch(code)
		{
			// MOD_NEGOTIATION
			case 300:
				this->search_for_urls(scan_queue, scanned_set, url, http_client.get_last_response_body());
				break;
			// File
			case 401:
			case 200:
				Debug::log_raw(0, "[F] %s (CODE:%d|SIZE:%d)                                  \n", url.c_str(), code, http_client.get_last_response_size());
				discovered_files.insert(url);
				this->search_for_urls(scan_queue, scanned_set, url, http_client.get_last_response_body());
				break;
			// Directory or error
			default:
				if(this->confirm_directory(url, http_client))
				{
					Debug::log_raw(0, "[D] %s (CODE:%d|SIZE:%d)                                  \n", url.c_str(), code, http_client.get_last_response_size());
					if(this->test_stable_directory(url, http_client))
					{
						Wordlist::fill_queue_with_wordlist(scan_queue, scanned_set, url, this->wordlist);
					} else {
						Debug::log_raw(0, "[D] %s is not a stable directory, will not recurse    \n", url.c_str());
					}
				} else {
					Debug::log_raw(0, "[F] %s (CODE:%d|SIZE:%d)                                  \n", url.c_str(), code, http_client.get_last_response_size());
					this->search_for_urls(scan_queue, scanned_set, url, http_client.get_last_response_body());
				}
				discovered_files.insert(url);
				break;
		}
	}
}

void Crawler::search_for_urls(ConcurrentQueue<std::string>& scan_queue, ConcurrentSet<std::string>& scanned_set, const std::string& url, const std::string& text)
{
	smatch url_base_match;
	string domain, this_domain, matched_path, new_url;
	
	if(regex_match(url, url_base_match, url_regex))
	{
		domain = url_base_match[2].length() ? url_base_match[2].str() : "";
		transform(domain.begin(), domain.end(), domain.begin(), [](unsigned char c) {return toupper(c);});

		sregex_iterator it(text.begin(), text.end(), url_regex);
		sregex_iterator it_end;

		for(; it != it_end; it++)
		{
			if((*it)[2].length())
			{
				this_domain = (*it)[2].str();
				transform(this_domain.begin(), this_domain.end(), this_domain.begin(), [](unsigned char c) {return toupper(c);});
				if(domain.compare(this_domain) != 0)
				{
					continue;
				}
			}

			matched_path = (*it)[3].str();
			if(Utils::begins_with(matched_path, "//"))
			{
				// Should test if domain matches, then include the path
				continue;
			}

			if(this->options.case_insensitive)
			{
				transform(matched_path.begin(), matched_path.end(), matched_path.begin(), [](unsigned char c) {return toupper(c);});
			}

			if(Utils::begins_with(matched_path, "/"))
			{
				new_url = Utils::replace_url_path(url, matched_path);
			} else {
				new_url = Utils::url_basedir(url) + matched_path;
			}

			if(scanned_set.insert_atomic(new_url))
			{
				scan_queue.push(new_url);
			}
		}
	}
}