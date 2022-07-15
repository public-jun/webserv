#ifndef CGI_RESPONSE_HPP
#define CGI_RESPONSE_HPP

#include <map>
#include <string>
#include <utility>

#include "HTTPResponse.hpp"
#include "Response.hpp"

class CGIResponse : public Response {
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

    ResponseType                              GetResponseType() const;
    int                                       GetStatusCode() const;
    const std::map<std::string, std::string>& GetHeader() const;
    const std::string  GetHeaderValue(const std::string key) const;
    const std::string& GetBody() const;

    void GenerateHTTPResponse(HTTPResponse& http_resp);
    void PrintInfo();

private:
    ResponseType                       type_;
};

#endif
