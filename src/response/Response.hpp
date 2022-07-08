#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

class Response {
public:
    Response();
    ~Response();

    void SetStatusCode(int status);
    void SetBody(std::string body);
    void AppendHeader(std::string key, std::string value);
    void AppendHeader(std::pair<std::string, std::string> pair);

protected:
    int                                status_code_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;
};

#endif
