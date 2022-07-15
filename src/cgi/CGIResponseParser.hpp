#ifndef CGI_RESPONSE_PARSER_HPP
#define CGI_RESPONSE_PARSER_HPP

#include <string>
#include <utility>
#include <vector>

#include "CGIResponse.hpp"

class CGIResponseParser {
public:
    enum Phase { HEADER_LINE, BODY, DONE };

    CGIResponseParser(CGIResponse& resp);
    ~CGIResponseParser();
    void operator()(std::string new_buf, ssize_t read_size, intptr_t offset);

    const Phase&       GetPhase() const { return phase_; }
    const std::string& GetLeftBuf() const { return left_buf_; }

    static const std::string CRLF;
    static const std::string LF;

private:
    CGIResponseParser();

    bool canParseLine(std::string& left_buf, std::string& line,
                      std::string& nl);
    bool splitToLine(std::string& buf, std::string& line, std::string& nl);
    std::pair<std::string, std::string>
                  parseHeaderLine(const std::string& line);
    void          validateToken(const std::string& token);
    bool          hasDoneHeaderLine(const std::string& line);
    void          validateAfterParseHeader();
    std::string   trimSpace(const std::string& str, const std::string);
    bool          hasAtLeastOneCgiField();
    bool          hasContentLen();
    unsigned long strToUlong(const std::string& str);
    Phase         mightSetContentLenBody(const std::string& buf,
                                         unsigned long      content_length);

    bool isDocumentResponse(const CGIResponse& cgi_resp) const;
    bool isLocalRedirResponse(const CGIResponse& cgi_resp) const;
    bool isClientRedirResponse(const CGIResponse& cgi_resp) const;
    bool isClientRedirDocumentResponse(const CGIResponse& cgi_resp) const;

    void selectResponse();

private:
    Phase                                 phase_;
    CGIResponse&                          cgi_resp_;
    std::string                           left_buf_;
    static const std::vector<std::string> MUST_CGI_FIELD;
    static std::vector<std::string>       setMustCGIField();
};

#endif
