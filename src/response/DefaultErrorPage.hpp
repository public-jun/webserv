#ifndef DEFAULT_ERROR_PAGE_HPP
#define DEFAULT_ERROR_PAGE_HPP
#include <string>

namespace DefaultErrorPage {

const std::string bad_request =
    "<html>\r\n"
    "<head><title>400 Bad Request</title></head>\r\n"
    "<body>\r\n"
    "<center><h1>400 Bad Request</h1></center>\r\n"
    "<hr><center>webserv</center>\r\n"
    "</body>\r\n"
    "</html>\r\n";

const std::string method_not_allowed =
    "<html>\r\n"
    "<head><title>405 Not Allowed</title></head>\r\n"
    "<body>\r\n"
    "<center><h1>405 Not Allowed</h1></center>\r\n"
    "<hr><center>webserv</center>\r\n"
    "</body>\r\n"
    "</html>\r\n";

const std::string version_not_suppoted =
    "<html>\r\n"
    "<head><title>505 HTTP Version Not Supported</title></head>\r\n"
    "<body>\r\n"
    "<center><h1>505 HTTP Version Not Supported</h1></center>\r\n"
    "<hr><center>webserv</center>\r\n"
    "</body>\r\n"
    "</html>\r\n";
} // namespace DefaultErrorPage

#endif // DEFAULT_ERROR_PAGE_HPP
