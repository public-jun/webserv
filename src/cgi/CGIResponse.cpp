#include "CGIResponse.hpp"

#include <sstream>
#include <string>
#include <utility>

#include <iostream>

CGIResponse::CGIResponse() : type_(NONE), status_code_(200) {}

CGIResponse::~CGIResponse() {}

void CGIResponse::SetResponseType(ResponseType type) { type_ = type; }

void CGIResponse::SetStatusCode(int status) { status_code_ = status; }

void CGIResponse::SetBody(std::string body) { body_ = body; }

void CGIResponse::AppendHeader(std::string key, std::string value) {
    header_.insert(std::make_pair(key, value));
}

void CGIResponse::AppendHeader(std::pair<std::string, std::string> pair) {
    header_.insert(pair);
}

CGIResponse::ResponseType CGIResponse::GetResponseType() const { return type_; }

int CGIResponse::GetStatusCode() const { return status_code_; }

const std::map<std::string, std::string>& CGIResponse::GetHeader() const {
    return header_;
}

const std::string CGIResponse::GetHeaderValue(const std::string key) const {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    const_iterator it = header_.find(key);
    if (it == header_.end()) {
        return "";
    }
    return it->second;
}

const std::string& CGIResponse::GetBody() const { return body_; }

void CGIResponse::GenerateHTTPResponse(HTTPResponse& http_resp) {
    std::stringstream ss;

    if (!body_.empty()) {
        ss << body_.size() << std::flush;
        header_.insert(std::make_pair(std::string("Content-Length"), ss.str()));
        ss << "";
    }

    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    for (const_iterator it = header_.begin(); it != header_.end(); it++) {
        if (it->first == "status") {
            continue;
        }
        http_resp.AppendHeader(it->first, it->second);
    }
    http_resp.SetStatusCode(status_code_);
    http_resp.SetBody(body_);
}

void CGIResponse::PrintInfo() {
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

    std::string type;
    if (type_ == NONE) {
        type = "NONE";
    } else if (type_ == DOCUMENT_RES) {
        type = "DOCUMENT_RES";
    } else if (type_ == CLIENT_REDIR_RES) {
        type = "CLIENT_REDIR_RES";
    } else if (type_ == CLIENT_REDIR_DOC_RES) {
        type = "CLIENT_REDIR_DOCUMENT_RES";
    }

    std::cout << "====== CGI Response ======" << std::endl;
    std::cout << "TYPE :" << type << std::endl;

    std::cout << "======    HEADER    ======" << std::endl;
    for (const_iterator it = header_.begin(); it != header_.end(); it++) {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    std::cout << "======  HEADER END  ======" << std::endl;

    std::cout << "======     BODY     ======" << std::endl;
    std::cout << body_ << std::endl;
    std::cout << "======   BODY END   ======" << std::endl;
}
