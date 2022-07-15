#include "HTTPRequest.hpp"
#include "HTTPStatus.hpp"

#include <algorithm>
#include <cctype>
#include <sys/socket.h>
#include <sys/types.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

const std::string HTTPRequest::CRLF = "\r\n";

const std::string::size_type HTTPRequest::CRLF_SIZE = CRLF.size();

HTTPRequest::HTTPRequest() {}

HTTPRequest::~HTTPRequest() {}

const std::string& HTTPRequest::GetMethod() const { return method_; }

std::string HTTPRequest::GetRequestTarget() const { return request_target_; }

const std::string& HTTPRequest::GetVersion() const { return HTTPVersion_; }

const std::string HTTPRequest::GetHeaderValue(const std::string key) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(key);
    if (it == headers_.end()) {
        return "";
    }
    return (*it).second;
}

const std::map<std::string, std::string>& HTTPRequest::GetHeaders() const {
    return headers_;
}

const std::string& HTTPRequest::GetBody() const { return body_; }

void HTTPRequest::SetRequestTarget(const std::string request_target) {
    request_target_ = request_target;
}

void HTTPRequest::SetMethod(const std::string method) { method_ = method; }

void HTTPRequest::SetHTTPVersion(const std::string version) {
    HTTPVersion_ = version;
}

void HTTPRequest::SetHeader(const std::string key, const std::string value) {
    headers_.insert(std::make_pair(key, value));
}

void HTTPRequest::SetBody(const std::string body) { body_ = body; }

void HTTPRequest::PrintInfo() const {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;
    std::cout << "====== HTTP Request ======" << std::endl;
    std::cout << "======  First Line  ======" << std::endl;
    std::cout << "Method: [" << method_ << "]" << std::endl;
    std::cout << "Target: [" << request_target_ << "]" << std::endl;
    std::cout << "Version: [" << HTTPVersion_ << "]" << std::endl;
    std::cout << "======    Header    ======" << std::endl;
    for (const_iterator it = headers_.begin(); it != headers_.end(); it++) {
        std::cout << it->first << ": [" << it->second << "]" << std::endl;
    }
    std::cout << "======  Header END  ======" << std::endl;

    std::cout << "======     BODY     ======" << std::endl;
    std::cout << body_ << std::endl;
    std::cout << "======   BODY END   ======" << std::endl;
    std::cout << "===== HTTP Request END =====\n\n" << std::endl;
}
