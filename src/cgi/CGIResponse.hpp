#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include <map>
#include <string>
#include <utility>

#include "HTTPResponse.hpp"

class CGIResponse {
public:
    enum ResponseType {
        NONE,
        DOCUMENT_RES,
        LOCAL_REDIR_RES,
        CLIENT_REDIR_RES,
        CLIENT_REDIR_DOC_RES,
    };

    CGIResponse();
    ~CGIResponse();

    void SetResponseType(ResponseType type);
    void SetStatusCode(int status);
    void SetBody(std::string body);
    void AppendHeader(std::string key, std::string value);
    void AppendHeader(std::pair<std::string, std::string> pair);

    ResponseType                              GetResponseType() const;
    int                                       GetStatusCode() const;
    const std::map<std::string, std::string>& GetHeader() const;
    const std::string  GetHeaderValue(const std::string key) const;
    const std::string& GetBody() const;

    void GenerateHTTPResponse(HTTPResponse& http_resp);
    void PrintInfo();

private:
    ResponseType                       type_;
    int                                status_code_;
    std::map<std::string, std::string> header_;
    std::string                        body_;
};

#endif
