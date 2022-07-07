#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP
#include "HTTPStatus.hpp"
#include "Response.hpp"
#include <map>
#include <string>

class HTTPResponse : public Response {
public:
    HTTPResponse();
    ~HTTPResponse();

    std::string GetDefaultErrorBody(status::code code);

    void SetVersion(std::string version);

    std::string ConvertToStr() const;
    void        PrintInfo() const;

    static const std::string DEFAULT_VERSION;

private:
    static std::map<int, std::string>          status_text_;
    static std::map<status::code, std::string> default_error_body_;
    static std::map<status::code, std::string> setDefaultErrorBody();

    std::string                        HTTPVersion_;

    std::string message_;
};

#endif
