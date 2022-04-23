#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <string>

class httpRequest
{
private:
    std::string method_;
    std::string uri_;
    std::string body_;
public:
    httpRequest(std::string method, std::string uri);
    ~httpRequest();
};

#endif