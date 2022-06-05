#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include <map>
#include <string>

class HTTPResponse {
public:
    HTTPResponse();
    ~HTTPResponse();

    void AppendHeader(std::string key, std::string value);

    void SetBody(std::string body);
    void SetVersion(std::string version);
    void SetStatusCode(int status);

    std::string ConvertToStr();

private:
    static std::map<int, std::string> status_text;

    std::string                        HTTPVersion_;
    int                                status_code_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;

    std::string message_;
};

#endif
