#include "HTTPResponse.hpp"
#include "SysError.hpp"
#include <cerrno>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>

HTTPResponse::HTTPResponse(int sock, int status_code, std::string header,
                           std::string body)
    : sock_(sock), status_code_(status_code), header_(header), body_(body) {}

HTTPResponse::~HTTPResponse() {}

HTTPResponse::HTTPResponse(const HTTPResponse& other) { *this = other; }

HTTPResponse& HTTPResponse::operator=(const HTTPResponse& other) {
    if (this != &other) {
        sock_        = other.sock_;
        status_code_ = other.status_code_;
        header_      = other.header_;
        body_        = other.body_;
    }
    return *this;
}

void HTTPResponse::Create() {
    // 文字列で扱って大丈夫？バイナリの可能性は？
    // TODO:レスポンスのサイズをつくる

    if (status_code_ == 200) {
        response_message_ =
            "HTTP/1.1 200 OK\r\n" + header_ + "\r\n\r\n" + body_;
    } else if (status_code_ == 404) {
        response_message_ = "HTTP1.1 404 No Found\r\n" + header_ + "\r\n";
    } else {
        std::cout << "Not support status" << status_code_ << std::endl;
    }
    message_size_ = response_message_.size();
}

void HTTPResponse::SendMessage() {
    std::cout << "<<message>>\n" << response_message_ << std::endl;
    int ret = send(sock_, response_message_.c_str(), message_size_, 0);
    if (ret == -1) {
        throw SysError("send", errno);
        // std::cout << "send error" << std::endl;
    }
}
