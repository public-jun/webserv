#include "HTTPResponse.hpp"

namespace {

std::string error_html(std::string message) {
    const std::string server_name = "webserv/1.0.0";
    return "<html>\r\n"
           "<head><title>" +
           message +
           "</title></head>\r\n"
           "<body>\r\n"
           "<center><h1>" +
           message +
           "</h1></center>\r\n"
           "<hr><center>" +
           server_name +
           "</center>\r\n"
           "</body>\r\n"
           "</html>\r\n";
}

} // namespace

std::map<status::code, std::string> HTTPResponse::setDefaultErrorBody() {
    std::map<status::code, std::string> body;

    body[status::bad_request]        = error_html("400 Bad Request");
    body[status::forbidden]          = error_html("403 Forbidden");
    body[status::not_found]          = error_html("404 Not Found");
    body[status::method_not_allowed] = error_html("405 Not Allowed");
    body[status::length_required]    = error_html("411 Length Required");
    body[status::request_entity_too_large] =
        error_html("413 Request Entity Too Long");
    body[status::uri_too_long] = error_html("414 URI Too Long");
    body[status::unsupported_media_type] =
        error_html("415 Unsupported Media Type");
    body[status::server_error]    = error_html("500 Internal Server Error");
    body[status::not_implemented] = error_html("501 Not Implemented");
    body[status::bad_gateway]     = error_html("502 Bad Gateway");
    body[status::version_not_suppoted] =
        error_html("505 HTTP Version Not Supported");

    return body;
}
