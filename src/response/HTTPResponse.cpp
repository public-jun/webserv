#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include <sstream>

const std::string HTTPResponse::DEFAULT_VERSION = "HTTP/1.1";

std::map<int, std::string> make_status_text() {
    std::map<int, std::string> status_text;
    status_text[status::ok]                     = "OK";
    status_text[status::found]                  = "Found";
    status_text[status::bad_request]            = "Bad Request";
    status_text[status::forbidden]              = "Forbidden";
    status_text[status::not_found]              = "Not Found";
    status_text[status::method_not_allowed]     = "Method Not Allowed";
    status_text[status::unsupported_media_type] = "Unsupported Methid Type";
    status_text[status::server_error]           = "Internal Server Error";
    status_text[status::version_not_suppoted]   = "HTTP Version Not Supported";
    return status_text;
}

std::map<int, std::string> HTTPResponse::status_text_ = make_status_text();

std::map<status::code, std::string> HTTPResponse::default_error_body_ =
    setDefaultErrorBody();

HTTPResponse::HTTPResponse()
    : HTTPVersion_(DEFAULT_VERSION), status_code_(status::ok) {}

HTTPResponse::~HTTPResponse() {}

std::string HTTPResponse::GetDefaultErrorBody(status::code code) {
    return default_error_body_[code];
}

void HTTPResponse::AppendHeader(std::string key, std::string value) {
    headers_.insert(std::make_pair(key, value));
}

void HTTPResponse::SetBody(std::string body) { body_ = body; }

void HTTPResponse::SetVersion(std::string version) { HTTPVersion_ = version; }

void HTTPResponse::SetStatusCode(int status) { status_code_ = status; }

std::string HTTPResponse::ConvertToStr() const {
    std::stringstream ss;
    ss << HTTPVersion_ << " " << status_code_ << " "
       << status_text_[status_code_] << "\r\n";

    for (std::map<std::string, std::string>::const_iterator it =
             headers_.begin();
         it != headers_.end(); it++) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    ss << body_;

    return ss.str();
}
