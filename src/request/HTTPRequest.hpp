#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <map>
#include <set>
#include <string>

class HTTPRequest {
public:
    HTTPRequest();
    ~HTTPRequest();

    const std::string&                        GetMethod() const;
    std::string                               GetRequestTarget() const;
    const std::string&                        GetVersion() const;
    int                                       GetStatus() const;
    const std::map<std::string, std::string>& GetHeaders() const;
    const std::string  GetHeaderValue(std::string key) const;
    const std::string& GetBody() const;

    void SetRequestTarget(const std::string uri);
    void SetMethod(const std::string method);
    void SetHTTPVersion(const std::string version);
    void SetHeader(const std::string key, const std::string value);
    void SetBody(const std::string);

    // 改行コード
    static const std::string            CRLF;
    static const std::string::size_type CRLF_SIZE;

    void PrintInfo() const;

private:
    std::string                        method_;
    std::string                        request_target_;
    std::string                        HTTPVersion_;
    std::string                        body_;
    std::map<std::string, std::string> headers_;
};

#endif
