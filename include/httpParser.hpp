#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include <string>
#include "httpRequest.hpp"

class httpParser
{
private:
    /* data */
public:
    httpParser();
    httpRequest& parse(std::string request_message);
    ~httpParser();
};

#endif // HTTPPARSER_HPP
