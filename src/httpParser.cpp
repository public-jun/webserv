#include "httpParser.hpp"
#include <sstream>
#include <iostream>

httpParser::httpParser()
{
}

httpParser::~httpParser()
{
}

httpRequest& httpParser::parse(std::string request_message)
{
    std::string::iterator it = std::find(request_message.begin(), request_message.end(), '\n');
    std::string line(request_message.begin(), it);

    std::istringstream iss(line);
    std::string method, uri;
    iss >> method >> uri;
    if (method.empty())
    {
        std::cout << "get method error" << std::endl;
    }
    if(uri.empty())
    {
        std::cout << "get uri error" << std::endl;
    }
    httpRequest req(method, uri);
    return req;
}