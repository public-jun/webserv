#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include "StreamSocket.hpp"
#include <string>

class HTTPRequest;
class HTTPParser {
private:
    /* data */
public:
    HTTPParser();
    HTTPRequest* Parse(std::string request_message);
    ~HTTPParser();
};

#endif // HTTPPARSER_HPP
