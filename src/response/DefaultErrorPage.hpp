#ifndef DEFAULT_ERROR_PAGE_HPP
#define DEFAULT_ERROR_PAGE_HPP
#include <string>

namespace DefaultErrorPage {

const std::string bad_request =
    std::string("<html>\r\n") +
    std::string("<head><title>400 Bad Request</title></head>\r\n") +
    std::string("<body>\r\n") +
    std::string("<center><h1>400 Bad Request</h1></center>\r\n") +
    std::string("<hr><center>webserv</center>\r\n") +
    std::string("</body>\r\n") + std::string("</html>\r\n");

const std::string method_not_allowed =
    std::string("<html>\r\n") +
    std::string("<head><title>405 Not Allowed</title></head>\r\n") +
    std::string("<body>\r\n") +
    std::string("<center><h1>405 Not Allowed</h1></center>\r\n") +
    std::string("<hr><center>webserv</center>\r\n") +
    std::string("</body>\r\n") + std::string("</html>\r\n");

const std::string version_not_suppoted =
    std::string("<html>\r\n") +
    std::string(
        "<head><title>505 HTTP Version Not Supported</title></head>\r\n") +
    std::string("<body>\r\n") +
    std::string(
        "<center><h1>505 HTTP Version Not Supported</h1></center>\r\n") +
    std::string("<hr><center>webserv</center>\r\n") +
    std::string("</body>\r\n") + std::string("</html>\r\n");
} // namespace DefaultErrorPage

#endif // DEFAULT_ERROR_PAGE_HPP
