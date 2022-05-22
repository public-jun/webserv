#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include <iostream>
#include <sstream>

HTTPParser::HTTPParser() {}

HTTPParser::~HTTPParser() {}

// HTTPParser::HTTPParser(const HTTPParser& other) { *this = other; }
// HTTPParser& HTTPParser::operator=(const HTTPParser& other) { return *this; }

HTTPRequest* HTTPParser::parse(std::string request_message) {
    std::string::iterator it =
        std::find(request_message.begin(), request_message.end(), '\n');
    std::string line(request_message.begin(), it);

    std::istringstream iss(line);
    std::string        method, uri;
    iss >> method >> uri;
    if (method.empty()) {
        std::cout << "get method error" << std::endl;
    }
    if (uri.empty()) {
        std::cout << "get uri error" << std::endl;
    }
    HTTPRequest* req = new HTTPRequest(method, uri);
    return req;
}
