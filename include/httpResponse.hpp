#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include <string>

class HTTPResponse
{
public:
    HTTPResponse(int sock, int status_code, std::string header,
                 std::string body);
    ~HTTPResponse();
    void create();
    void sendMessage();

private:
    int         sock_;
    int         status_code_;
    std::string header_;
    std::string body_;
    std::string response_message_;
    std::size_t message_size_;
};

#endif
