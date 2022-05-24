#include "HTTPRequest.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>

HTTPRequest::HTTPRequest(std::string method, std::string uri)
    : method_(method), uri_(uri) {}

HTTPRequest::~HTTPRequest() {}

std::string& HTTPRequest::GetMethod() { return method_; }
std::string& HTTPRequest::GetURI() { return uri_; }

void HTTPRequest::SetURI(std::string uri) { uri_ = uri; }
