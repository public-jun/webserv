#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <map>
#include <set>
#include <string>

class HTTPRequest
{
public:
    HTTPRequest(const std::string& row);
    HTTPRequest(std::string method, std::string uri);
    ~HTTPRequest();
    std::string&                       GetMethod();
    std::string&                       GetURI();
    std::string&                       GetVersion();
    int                                GetStatus();
    std::map<std::string, std::string> GetHeaders();
    std::string                        GetHeaderValue(std::string key);
    void                               SetURI(std::string uri);

    // 対応するメソッド一覧。仮でここで宣言
    static std::set<std::string> methods;

    static const std::string crlf;

private:
    std::string                        request_message_;
    std::string                        method_;
    std::string                        uri_;
    std::string                        version_;
    std::string                        body_;
    std::string                        row_;
    std::map<std::string, std::string> headers_;
    int                                status_;

    void parse();
    void parseFirstline(std::string line);
    void parseHeaderLine(std::string line);
    void parseBody(std::string body);
    bool isLastLine(std::string& str);
    void throwErrorBadrequest(std::string err_message);
};

#endif
