#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include <map>
#include <string>

class HTTPResponse {
public:
    HTTPResponse(int sock, int status_code, std::string header,
                 std::string body);
    ~HTTPResponse();

    void SetHeader(std::string key, std::string value);

    void Create();
    void SendMessage();

private:
    int                                sock_;
    int                                status_code_;
    std::map<std::string, std::string> headers_;
    std::string                        header_;
    std::string                        body_;
    std::string                        response_message_;
    std::size_t                        message_size_;
};

#endif
