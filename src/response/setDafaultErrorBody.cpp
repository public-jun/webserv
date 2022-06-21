#include "HTTPResponse.hpp"

namespace {

std::string apply_to_template_body(std::string message) {
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
    body[status::bad_request] = apply_to_template_body("400 Bad Request");
    body[status::forbidden]   = apply_to_template_body("403 Forbidden");
    body[status::not_found]   = apply_to_template_body("404 Not Found");
    body[status::method_not_allowed] =
        apply_to_template_body("405 Not Allowed");
    body[status::server_error] =
        apply_to_template_body("500 Internal Server Error");
    body[status::version_not_suppoted] =
        apply_to_template_body("505 HTTP Version Not Supported");

    return body;
}
