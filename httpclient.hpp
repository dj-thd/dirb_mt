
#ifndef _HTTPCLIENT_HPP_INCLUDED_
#define _HTTPCLIENT_HPP_INCLUDED_

#include <string>
#include <map>
#include <curl/curl.h>

class HttpClient
{
private:
	CURL* curl;
	std::string last_response_body;
	size_t last_response_size;
	std::map<std::string,std::string> last_response_headers;
	int last_http_code;

	static size_t write_function(void* buffer, size_t size, size_t nmemb, void* userp);
	static size_t header_function(void* buffer, size_t size, size_t nmemb, void* userp);

public:
	HttpClient();
	~HttpClient();
	int get(std::string url);
	std::string get_last_response_body();
	std::map<std::string, std::string> get_last_response_headers();
	size_t get_last_response_size();
};

#endif