#include "httpRequest.hpp"

httpRequest::httpRequest(std::string method, std::string uri): method_(method), uri_(uri)
{
}

httpRequest::~httpRequest(){}