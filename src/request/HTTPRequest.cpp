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

const std::string HTTPRequest::crlf = "\r\n";

const std::string::size_type HTTPRequest::crlf_size = crlf.size();

HTTPRequest::HTTPRequest() : status_(status::ok) {}

HTTPRequest::~HTTPRequest() {}

const std::string& HTTPRequest::GetMethod() const { return method_; }

std::string HTTPRequest::GetRequestTarget() const { return request_target_; }

const std::string& HTTPRequest::GetVersion() const { return HTTPVersion_; }

int HTTPRequest::GetStatus() const { return status_; }

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

void HTTPRequest::SetRequestTarget(const std::string request_target) {
    request_target_ = request_target;
}

void HTTPRequest::SetMethod(const std::string method) { method_ = method; }

void HTTPRequest::SetHTTPVersion(const std::string version) {
    HTTPVersion_ = version;
}

void HTTPRequest::SetStatus(const int status) { status_ = status; }

void HTTPRequest::SetHeader(const std::string key, const std::string value) {
    headers_.insert(std::make_pair(key, value));
}
