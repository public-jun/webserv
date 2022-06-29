#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include "HTTPStatus.hpp"
#include <map>
#include <string>

class HTTPResponse {
public:
    HTTPResponse();
    ~HTTPResponse();

    std::string GetDefaultErrorBody(status::code code);

    void SetBody(std::string body);
    void SetVersion(std::string version);
    void SetStatusCode(int status);
    void AppendHeader(std::string key, std::string value);

    std::string ConvertToStr() const;
    void        PrintInfo() const;

    static const std::string DEFAULT_VERSION;

private:
    static std::map<int, std::string>          status_text_;
    static std::map<status::code, std::string> default_error_body_;
    static std::map<status::code, std::string> setDefaultErrorBody();

    std::string                        HTTPVersion_;
    int                                status_code_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;

    std::string message_;
};

#endif
