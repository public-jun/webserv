#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include <string>

class HTTPRequest;
class HTTPParser {

public:
    enum Phase { PH_FIRST_LINE, PH_HEADER_LINE, PH_END };
    HTTPParser(HTTPRequest& req);
    ~HTTPParser();

    void ParsePart(const std::string& buf);
    void Parse(std::string request_message);

private:
    void parseFirstline(const std::string& line);
    void parseHeaderLine(const std::string& line);

    void throwErrorBadrequest(const std::string err_message);
    void throwErrorMethodNotAllowed(const std::string err_message);
    void throwErrorVersionNotSupported(const std::string err_message);

    void validateMethod(const std::string& method);
    void validateRequestTarget(const std::string& request_target);
    void validateHTTPVersion(const std::string& version);
    void validateToken(const std::string& token);
    void validateVersionNotSuppoted();
    void validateMethodNotAllowed();

    std::string trimSpace(const std::string& string,
                          const std::string  trim_char_set) const;
    bool        isdigit(const std::string& str) const;

    std::string  buf_;
    Phase        phase_;
    HTTPRequest& req_;
};

#endif // HTTPPARSER_HPP
