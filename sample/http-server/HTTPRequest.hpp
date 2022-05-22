#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include "StreamSocket.hpp"
#include <string>

class HTTPRequest
{
private:
    std::string request_message_;
    std::string method_;
    std::string uri_;
    std::string body_;

public:
    HTTPRequest(std::string method, std::string uri);
    ~HTTPRequest();
    std::string& getMethod();
    std::string& getURI();
    void         setURI(std::string uri);
};

#endif