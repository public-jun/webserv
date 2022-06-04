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

void HTTPResponse::SetStatus(int status) { status_code_ = status; }

void HTTPResponse::Create() {
    std::ostringstream oss;
    std::string        length;
    oss << body_.size() << std::flush;
    length = oss.str();
    std::string size("Content-Length: " + length);

    if (status_code_ == 200) {
        message_ = "HTTP/1.1 200 OK\r\n" + size + "\r\n\r\n" + body_;
    } else if (status_code_ == 404) {
        message_ = "HTTP/1.1 404 Not Found\r\n" + size + "\r\n";
    } else {
        std::cout << "Not support status" << status_code_ << std::endl;
    }

    std::cout << "status: " << status_code_ << std::endl;
    std::cout << "body: " << body_ << std::endl;
}

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

void HTTPResponse::SendMessage() {
    int ret = send(sock_, message_.c_str(), message_.size(), 0);
    if (ret == -1) {
        throw SysError("send", errno);
    }
}
