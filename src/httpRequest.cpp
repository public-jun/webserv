#include "HTTPRequest.hpp"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>

HTTPRequest::HTTPRequest(std::string method, std::string uri)
    : method_(method), uri_(uri)
{}

HTTPRequest::~HTTPRequest() {}
