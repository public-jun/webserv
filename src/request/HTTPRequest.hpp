#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP
#include <map>
#include <set>
#include <string>

class HTTPRequest {
public:
    HTTPRequest(const std::string& row);
    HTTPRequest(std::string method, std::string uri);
    ~HTTPRequest();
    const std::string&                        GetMethod() const;
    std::string                               GetRequestTarget() const;
    const std::string&                        GetVersion() const;
    int                                       GetStatus() const;
    const std::map<std::string, std::string>& GetHeaders() const;
    const std::string GetHeaderValue(std::string key) const;

    void SetURI(std::string uri);

    // 対応するメソッド一覧。仮でここで宣言
    static std::set<std::string> methods;

    // 改行コード
    static const std::string            crlf;
    static const std::string::size_type crlf_size;

    // ステータスコード
    static const int status_ok                    = 200;
    static const int status_bad_request           = 400;
    static const int status_method_not_allowed    = 405;
    static const int status_version_not_supported = 505;

private:
    std::string                        request_message_;
    std::string                        method_;
    std::string                        request_target_;
    std::string                        HTTPVersion_;
    std::string                        body_;
    const std::string                  row_;
    std::map<std::string, std::string> headers_;
    int                                status_;

    void parse();
    void parseFirstline(const std::string& line);
    void parseHeaderLines(std::string& str);
    void parseHeaderLine(const std::string& line);
    void parseBody(std::string& body);

    void throwErrorBadrequest(const std::string err_message);
    void throwErrorMethodNotAllowed(const std::string err_message);
    void throwErrorVersionNotSupported(const std::string err_message);

    void validateMethod(const std::string& method);
    void validateRequestTarget(const std::string& request_target);
    void validateHTTPVersion(const std::string& version);
    void validateToken(const std::string& token);
    void validateBody(const std::string& body);
    void validateVersionNotSuppoted();
    void validateMethodNotAllowed();

    std::string            trimSpace(const std::string& string,
                                     const std::string  trim_char_set) const;
    std::string::size_type mustFindCRLF(const std::string& str);
    bool                   isdigit(const std::string& str) const;
    bool                   hostExists() const;
};

#endif
