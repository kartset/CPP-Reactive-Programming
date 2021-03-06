// SimpleClient.cpp 

#include "stdafx.h"

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#else
# include <sys/time.h>
#endif

////////////////////////////////////////////////
// A Simple HTTP Client to Demonstrate REST SDK Client Programming model
// The Toy sample shows how one can read contents of a web page and 
// persists into a local file ( named ... results.html
//
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char* argv[]){
	auto fileStream = std::make_shared<ostream>();
	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.html")).
		then([=](ostream outFile){
		*fileStream = outFile;
		// Create http_client to send the request.
		http_client client(U("http://www.google.com/"));
		// Build request URI and start the request.
		uri_builder builder(U(""));
		return client.request(methods::GET, builder.to_string());
	}).then([=](http_response response){
		printf("Received response status code:%u\n", response.status_code());
		return response.body().read_to_end(fileStream->streambuf());
	}).then([=](size_t){ return fileStream->close();});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	// The above code was just composing task continuations 
	try{
		requestTask.wait();
	}
	catch (const std::exception &e){
		printf("Error exception:%s\n", e.what());
	}

	getchar();

	return 0;
}