#include "CGIResponseParser.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "CGIResponse.hpp"
#include "HTTPStatus.hpp"

const std::string CGIResponseParser::CRLF = "\r\n";
const std::string CGIResponseParser::LF   = "\n";

const std::vector<std::string> CGIResponseParser::MUST_CGI_FIELD =
    CGIResponseParser::setMustCGIField();

CGIResponseParser::CGIResponseParser(CGIResponse& resp)
    : phase_(HEADER_LINE), cgi_resp_(resp) {}

CGIResponseParser::~CGIResponseParser() {}

bool CGIResponseParser::splitToLine(std::string& buf, std::string& line,
                                    std::string& nl) {
    std::string::size_type line_end_pos = buf.find(LF);
    if (line_end_pos == std::string::npos) {
        return false;
    }

    if (line_end_pos > 0 && buf.at(line_end_pos - 1) == '\r') {
        line_end_pos -= 1;
        nl = CRLF;
    } else {
        nl = LF;
    }

    line = buf.substr(0, line_end_pos);
    buf  = buf.substr(line_end_pos + nl.size());
    return true;
}

bool CGIResponseParser::canParseLine(std::string& buf, std::string& line,
                                     std::string& nl) {
    return splitToLine(buf, line, nl);
}

void CGIResponseParser::validateToken(const std::string& token) {
    if (token.empty()) {
        throw status::bad_gateway;
    }

    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::const_iterator it = token.begin(); it != token.end();
         it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            throw status::bad_gateway;
        }
    }
}

std::string
CGIResponseParser::trimSpace(const std::string& str,
                             const std::string  trim_char_set = " ") {
    std::string            result;
    std::string::size_type left = str.find_first_not_of(trim_char_set);

    if (left != std::string::npos) {
        std::string::size_type right = str.find_last_not_of(trim_char_set);
        result                       = str.substr(left, right - left + 1);
    }
    return result;
}

bool CGIResponseParser::hasDoneHeaderLine(const std::string& line) {
    return line == "";
}

std::pair<std::string, std::string>
CGIResponseParser::parseHeaderLine(const std::string& line) {
    std::string::size_type delimiter_pos = line.find(":");

    std::string key, value;
    key = line.substr(0, delimiter_pos);
    if (delimiter_pos != std::string::npos) {
        value = line.substr(delimiter_pos + 1, line.size() - delimiter_pos);
    }

    validateToken(key);

    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    value = trimSpace(value);
    if (value.find_first_of(LF) != value.npos) {
        throw status::bad_gateway;
    }

    return std::make_pair(key, value);
}

bool CGIResponseParser::hasAtLeastOneCgiField() {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    const std::map<std::string, std::string>& header = cgi_resp_.GetHeader();

    for (const_iterator it = header.begin(); it != header.end(); it++) {
        if (std::find(MUST_CGI_FIELD.begin(), MUST_CGI_FIELD.end(),
                      it->first) != MUST_CGI_FIELD.end()) {
            return true;
        }
    }
    return false;
}

void CGIResponseParser::validateAfterParseHeader() {
    if (!hasAtLeastOneCgiField()) {
        throw status::bad_gateway;
    }
}

bool CGIResponseParser::hasContentLen() {
    if (cgi_resp_.GetHeaderValue("content-length") == "") {
        return false;
    } else {
        return true;
    }
}

unsigned long CGIResponseParser::strToUlong(const std::string& str) {
    const char*   p = str.c_str();
    char*         end;
    unsigned long value = std::strtoul(p, &end, 10);
    if (p == end) {
        throw std::invalid_argument("str_to_ulong");
    }
    if (errno == ERANGE) {
        throw std::out_of_range("str_to_ulong");
    }
    return value;
}

CGIResponseParser::Phase
CGIResponseParser::mightSetContentLenBody(const std::string& buf,
                                          unsigned long      content_length) {
    if (buf.size() >= content_length) {
        cgi_resp_.SetBody(std::string(buf, 0, content_length));
        return DONE;
    }
    return BODY;
}

bool CGIResponseParser::isDocumentResponse(const CGIResponse& cgi_resp) const {
    // locationがある場合、DocumentResponseではない
    if (cgi_resp.GetHeaderValue("location") != "") {
        return false;
    }

    // Content-Typeの指定は必須
    if (cgi_resp.GetHeaderValue("content-type") == "") {
        return false;
    }

    return true;
}

bool CGIResponseParser::isLocalRedirResponse(
    const CGIResponse& cgi_resp) const {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;
    // Location以外は指定できない

    const const_iterator it_end = cgi_resp.GetHeader().end();
    for (const_iterator it = cgi_resp.GetHeader().begin(); it != it_end; it++) {
        if (it->first != "location") {
            return false;
        }
    }

    // Locationは必須
    std::string absolute_path = cgi_resp.GetHeaderValue("location");
    if (absolute_path == "") {
        return false;
    }

    // TODO absolute_pathをvalidate
    if (*absolute_path.begin() != '/') {
        return false;
    }

    return true;
}

bool CGIResponseParser::isClientRedirResponse(
    const CGIResponse& cgi_resp) const {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;
    // Locationは必須
    std::string absolute_url = cgi_resp.GetHeaderValue("location");
    if (absolute_url == "") {
        return false;
    }

    // TODO absoluteURLをvalidate

    const std::map<std::string, std::string>& header = cgi_resp.GetHeader();
    for (const_iterator it = header.begin(); it != header.end(); it++) {
        // locationもしくはx-cgi-が接頭語についているフィールドを判定する
        if ((it->first == "location") || it->first.find("x-cgi-") == 0) {
            continue;
        } else {
            return false;
        }
    }

    return true;
}

bool CGIResponseParser::isClientRedirDocumentResponse(
    const CGIResponse& cgi_resp) const {

    // location && status && content-type 全て必須
    for (std::vector<std::string>::const_iterator it = MUST_CGI_FIELD.begin();
         it != MUST_CGI_FIELD.end(); it++) {

        if (cgi_resp.GetHeaderValue(*it) == "") {
            return false;
        }
    }

    std::string absoluteURL = cgi_resp.GetHeaderValue("location");
    // TODO absoluteURLをvalidate

    return true;
}

void CGIResponseParser::selectResponse() {

    // Document Response
    if (isDocumentResponse(cgi_resp_)) {
        cgi_resp_.SetResponseType(CGIResponse::DOCUMENT_RES);
        return;
    }

    // Local Redir
    if (isLocalRedirResponse(cgi_resp_)) {
        cgi_resp_.SetResponseType(CGIResponse::LOCAL_REDIR_RES);
        return;
    }

    // Client Redir
    if (isClientRedirResponse(cgi_resp_)) {
        cgi_resp_.SetResponseType(CGIResponse::CLIENT_REDIR_RES);
        std::string status = cgi_resp_.GetHeaderValue("status");
        if (status == "") {
            cgi_resp_.SetStatusCode(status::found);
        } else {
            cgi_resp_.SetStatusCode(strToUlong(status));
        }
        return;
    }

    // Client Redir Document
    if (isClientRedirDocumentResponse(cgi_resp_)) {
        cgi_resp_.SetResponseType(CGIResponse::CLIENT_REDIR_DOC_RES);
        std::string status = cgi_resp_.GetHeaderValue("status");
        if (status == "") {
            cgi_resp_.SetStatusCode(status::found);
        } else {
            cgi_resp_.SetStatusCode(strToUlong(status));
        }
        return;
    }
    std::cout << "bad gateway" << std::endl;

    throw status::bad_gateway;
}

void CGIResponseParser::operator()(std::string new_buf, ssize_t read_size,
                                   intptr_t offset) {
    try {
        left_buf_.append(new_buf.c_str(), new_buf.size());

        for (;;) {
            std::string                         line, nl;
            std::pair<std::string, std::string> element;
            switch (phase_) {
            case HEADER_LINE:
                if (!canParseLine(left_buf_, line, nl)) {
                    return;
                }

                if (hasDoneHeaderLine(line)) {
                    validateAfterParseHeader();
                    phase_ = BODY;
                } else {
                    element = parseHeaderLine(line);
                    cgi_resp_.AppendHeader(element);
                }
                break;

            case BODY:
                if (hasContentLen()) {
                    // content len
                    phase_ = mightSetContentLenBody(
                        left_buf_,
                        strToUlong(cgi_resp_.GetHeaderValue("content-length")));
                    selectResponse();
                    break;
                } else if (read_size == 0 || read_size == offset) {
                    // EOF
                    cgi_resp_.SetBody(left_buf_);
                    phase_ = DONE;
                    selectResponse();
                    break;
                } else {
                    return;
                }

            case DONE:
                return;
            }
        }
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; }
}

std::vector<std::string> CGIResponseParser::setMustCGIField() {
    std::vector<std::string> result;
    result.push_back("content-type");
    result.push_back("location");
    result.push_back("status");
    return result;
}