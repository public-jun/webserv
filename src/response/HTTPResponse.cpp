#include "HTTPResponse.hpp"
#include "SysError.hpp"
#include <cerrno>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>

HTTPResponse::HTTPResponse() : status_code_(200) {
    status_text[200] = "OK";
    status_text[400] = "Bad Request";
    status_text[404] = "Not Found";
}

HTTPResponse::HTTPResponse(int sock, int status_code, std::string body)
    : sock_(sock), status_code_(status_code), body_(body) {}

HTTPResponse::~HTTPResponse() {}

void HTTPResponse::AppendHeader(std::string key, std::string value) {
    headers_.insert(std::make_pair(key, value));
}

void HTTPResponse::SetBody(std::string body) {
    std::ostringstream oss;
    oss << body.size() << std::flush;
    AppendHeader("Content-Length", oss.str());
    body_ = body;
}

void HTTPResponse::SetVersion(std::string version) { HTTPVersion_ = version; }

std::string HTTPResponse::ConvertToStr() {
    std::stringstream ss;
    ss << HTTPVersion_ << " " << status_code_ << " "
       << status_text[status_code_] << "\r\n";

    for (std::map<std::string, std::string>::iterator it = headers_.begin();
         it != headers_.end(); it++) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    ss << body_;

    return ss.str();
}
