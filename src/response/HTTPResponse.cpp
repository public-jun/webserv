#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include <iostream>
#include <sstream>

const std::string HTTPResponse::DEFAULT_VERSION = "HTTP/1.1";

std::map<int, std::string> make_status_text() {
    std::map<int, std::string> status_text;
    status_text[status::ok]                       = "OK";
    status_text[status::created]                  = "Created";
    status_text[status::no_content]               = "No Content";
    status_text[status::moved_permanently]        = "Moved Permanently";
    status_text[status::found]                    = "Found";
    status_text[status::see_other]                = "See Other";
    status_text[status::temporary_redirect]       = "Temporary Redirect";
    status_text[status::permanent_redirect]       = "Permanent Redirect";
    status_text[status::bad_request]              = "Bad Request";
    status_text[status::forbidden]                = "Forbidden";
    status_text[status::not_found]                = "Not Found";
    status_text[status::method_not_allowed]       = "Method Not Allowed";
    status_text[status::request_entity_too_large] = "Request Entity Too Large";
    status_text[status::uri_too_long]             = "Request URI Too Long";
    status_text[status::unsupported_media_type]   = "Unsupported Methid Type";
    status_text[status::server_error]             = "Internal Server Error";
    status_text[status::not_implemented]          = "Not Implemented";
    status_text[status::bad_gateway]              = "Bad Gateway";
    status_text[status::version_not_suppoted] = "HTTP Version Not Supported";
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

void HTTPResponse::PrintInfo() const {
#ifdef WS_DEBUG
    typedef std::map<std::string, std::string>::const_iterator const_iterator;
    std::cout << "====== HTTP Response ======"
              << "\n"
              << "StatusCode: " << status_code_ << "\n"
              << "======     Header    ======"
              << "\n";
    for (const_iterator it = headers_.begin(); it != headers_.end(); it++) {
        std::cout << it->first << ": [" << it->second << "]" << std::endl;
    }
    std::cout << "======  Header END  ======" << std::endl;

    std::cout << "======     BODY     ======" << std::endl;
    std::cout << "body size: " << body_.size() << std::endl;
    std::cout << "======   BODY END   ======" << std::endl;
    std::cout << "===== HTTP Response END =====\n" << std::endl;
#else
    std::cout << status_code_ << " " << body_.size() << std::endl;
#endif
}
