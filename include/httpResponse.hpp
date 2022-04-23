#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include <string>

class httpResponse
{
public:
    httpResponse(int status_code, std::string header, std::string body, std::size_t body_size);
    ~httpResponse();
    void create();
private:
    int status_code_;
    std::size_t body_size_;
    std::string header_;
    std::string body_;
    std::string response_message_;
};

#endif