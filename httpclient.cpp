#include <string>
#include <map>
#include <curl/curl.h>

#include "httpclient.hpp"

using namespace std;

// Receive data callback
size_t HttpClient::write_function(void* buffer, size_t size, size_t nmemb, void* userp)
{
	HttpClient* that = static_cast<HttpClient*>(userp);
	size_t real_size = size * nmemb;
	// Not optimal. I will consider the use of stringstream but this works for the moment
	that->last_response_body.append((const char*)buffer, real_size);
	that->last_response_size += real_size;
	return real_size;
}

// Receive header callback
size_t HttpClient::header_function(void* buffer, size_t size, size_t nmemb, void* userp)
{
	// HttpClient instance that makes the call
	HttpClient* that = static_cast<HttpClient*>(userp);
	// Buffer
	const char* buffer_str = (const char*)buffer;
	// Header name string pointer
	const char* header_name = 0;
	// Header name string length
	size_t header_name_length = 0;
	// Header value string pointer
	const char* header_value = 0;
	// Header value string length
	size_t header_value_length = 0;
	// Real size of the "buffer" data
	size_t real_size = size * nmemb;
	// State of parser
	int state = 0;

	// Parse headers using a state-based machine
	for(size_t i = 0; i < real_size; i++)
	{
		switch(state)
		{
			// State 0: Waiting for header name
			case 0:
				if(buffer_str[i] != 0x0A && buffer_str[i] != 0x0D)
				{
					state = 1;
					i--;
					continue; // avoid returning back to state 0
				}
				break;

			// State 1: Found header name
			case 1:
				header_name = buffer_str + i;
				state = 2;
				i--;
				break;

			// State 2: Parse header name, wait for separator
			case 2:
				if(buffer_str[i] == ':')
				{
					state = 3;
					header_name_length = buffer_str + i - header_name;
					i++;
				}
				break;

			// State 3: Found header value
			case 3:
				header_value = buffer_str + i;
				state = 4;
				i--;
				break;

			// State 4: Wait for line end
			case 4:
				if(buffer_str[i] == 0x0A || buffer_str[i] == 0x0D)
				{
					header_value_length = buffer_str + i - header_value;
					that->last_response_headers[string(header_name, header_name_length)] = string(header_value, header_value_length);
					header_name = 0;
					header_name_length = 0;
					header_value = 0;
					header_value_length = 0;
					state = 0;
				}
				break;
		}
		// For any reason we have line end -> header is not complete, then go back to state 0
		if(buffer_str[i] == 0x0A || buffer_str[i] == 0x0D)
		{
			header_value = 0;
			state = 0;
		}
	}

	// We have incomplete header value -> add it to map
	if(header_value != 0) {
		header_value_length = buffer_str + real_size - header_value - 1;
		that->last_response_headers[string(header_name, header_name_length)] = string(header_value, header_value_length);
	}
	
	return real_size;
}

// Constructor: Initialize underlying curl
HttpClient::HttpClient()
{
	this->curl = curl_easy_init();
	curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, HttpClient::write_function);
	curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, (void*)this);
	curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, HttpClient::header_function);
	curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, (void*)this);
}

// Destructor: Free underlying curl
HttpClient::~HttpClient()
{
	curl_easy_cleanup(this->curl);
	this->curl = 0;
}

// Do a GET request
int HttpClient::get(std::string url)
{
	this->last_response_body = "";
	this->last_response_size = 0;
	this->last_response_headers.clear();
	curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
	if(curl_easy_perform(this->curl) == 0)
	{
		curl_easy_getinfo(this->curl, CURLINFO_HTTP_CODE, &this->last_http_code);
	} else {
		this->last_http_code = 0;
	}
	return this->last_http_code;
}

// Get last response body
std::string HttpClient::get_last_response_body()
{
	return this->last_response_body;
}

// Get last response headers
std::map<std::string, std::string> HttpClient::get_last_response_headers()
{
	return this->last_response_headers;
}

// Get last response size
size_t HttpClient::get_last_response_size()
{
	return this->last_response_size;
}