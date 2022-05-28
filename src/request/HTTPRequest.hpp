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
    std::string&                       GetMethod();
    std::string&                       GetRequestTarget();
    std::string&                       GetVersion();
    int                                GetStatus();
    std::map<std::string, std::string> GetHeaders();
    std::string                        GetHeaderValue(std::string key);

    void SetURI(std::string uri);

    // 対応するメソッド一覧。仮でここで宣言
    static std::set<std::string> methods;

    // 改行コード
    static const std::string crlf;

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
    std::string                        row_;
    std::map<std::string, std::string> headers_;
    int                                status_;

    void parse();
    void parseFirstline(std::string line);
    void parseHeaderLines(std::string&           str,
                          std::string::size_type line_end_pos);
    void parseHeaderLine(std::string line);
    void parseBody(std::string body);

    std::string trimSpace(const std::string& string,
                          const std::string  trim_char_set);

    void throwErrorBadrequest(std::string err_message);
    void throwErrorMethodNotAllowed(std::string err_message);
    void throwErrorVersionNotSupported(std::string err_message);

    void varidateMethod(std::string& method);
    void varidateRequestTarget(std::string request_target);
    void varidateHTTPVersion(std::string version);
    void varidateToken(std::string token);
    void varidateBody(std::string body);
    void varidateVersionNotSuppoted();
    void varidateMethodNotAllowed();

    bool isLastLine(std::string& str);
    bool isdigit(std::string str);

    bool isSupportedMethod();
    bool isSupportedVersion();
    bool hostExists();
};

#endif
