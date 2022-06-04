#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include <map>
#include <string>

class HTTPResponse {
public:
    HTTPResponse();
    HTTPResponse(int sock, int status_code, std::string body);
    ~HTTPResponse();

    void AppendHeader(std::string key, std::string value);

    void SetBody(std::string body);
    void SetVersion(std::string version);

    std::string                ConvertToStr();
    std::map<int, std::string> status_text;

private:
    int                                sock_;
    int                                status_code_;
    std::map<std::string, std::string> headers_;
    std::string                        HTTPVersion_;
    std::string                        body_;

    std::string message_;
};

#endif
