#include "httpResponse.hpp"
#include <iostream>

httpResponse::httpResponse(int status_code, std::string header, std::string body, std::size_t body_size)
    : status_code_(status_code), header_(header), body_(body), body_size_(body_size)
{
}

httpResponse::~httpResponse()
{
}

void httpResponse::create()
{
    // 文字列で扱って大丈夫？バイナリの可能性は？
    // TODO:レスポンスのサイズをつくる
    std::size_t no_body_len;

    if (status_code_ == 200)
    {
        response_message_ = "HTTP/1.1 200 OK\r\n" + header_ + "\r\n" + body_;
    }
    else if (status_code_ == 404)
    {
        response_message_ = "HTTP1.1 404 No Found\r\n" + header_ + "\r\n";
    }
    else
    {
        std::cout << "Not support status" << status_code_ << std::endl;
    }
}