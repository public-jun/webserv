#include "HTTPRequest.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>

HTTPRequest::HTTPRequest(std::string method, std::string uri)
    : method_(method), uri_(uri)
{}

HTTPRequest::~HTTPRequest() {}

std::string& HTTPRequest::getMethod() { return method_; }
std::string& HTTPRequest::getURI() { return uri_; }

void HTTPRequest::setURI(std::string uri) { uri_ = uri; }
