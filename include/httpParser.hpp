#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include <string>

class HTTPParser
{
private:
    /* data */
public:
    HTTPParser();
    HTTPRequest& parse(std::string request_message);
    ~HTTPParser();
};

#endif // HTTPPARSER_HPP
