#include "Response.hpp"

#include <utility>
#include "HTTPStatus.hpp"

Response::Response() : status_code_(status::ok) {}

Response::~Response() {}

void Response::SetStatusCode(int status) { status_code_ = status; }

void Response::SetBody(std::string body) { body_ = body; }

void Response::AppendHeader(std::string key, std::string value) {
    headers_.insert(std::make_pair(key, value));
}

void Response::AppendHeader(std::pair<std::string, std::string> pair) {
    headers_.insert(pair);
}
