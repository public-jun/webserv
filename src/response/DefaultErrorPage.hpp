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
}

#endif // DEFAULT_ERROR_PAGE_HPP
