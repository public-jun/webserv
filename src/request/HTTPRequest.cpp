#include "HTTPRequest.hpp"

#include <sys/socket.h>
#include <sys/types.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[39m"

std::set<std::string> HTTPRequest::methods;

const std::string HTTPRequest::crlf = "\r\n";

HTTPRequest::HTTPRequest(const std::string& row) : row_(row), status_(200) {
    HTTPRequest::methods.insert("GET");
    parse();
}

HTTPRequest::HTTPRequest(std::string method, std::string uri)
    : method_(method), uri_(uri) {}

HTTPRequest::~HTTPRequest() {}

std::string& HTTPRequest::GetMethod() { return method_; }

std::string& HTTPRequest::GetURI() { return uri_; }

std::string& HTTPRequest::GetVersion() { return version_; }

int HTTPRequest::GetStatus() { return status_; }

std::string HTTPRequest::GetHeaderValue(std::string key) {
    return headers_[key];
}

std::map<std::string, std::string> HTTPRequest::GetHeaders() {
    return headers_;
}

void HTTPRequest::SetURI(std::string uri) { uri_ = uri; }

void HTTPRequest::parseFirstline(std::string line) {
    std::istringstream iss(line);
    std::string        method, uri;
    std::string        version = "HTTP/1.1";

    iss >> method >> uri >> version;

    if (uri == "/") {
        uri = "index.html";
    }

    // バリデート
    if (method.empty() || uri.empty() || !isToken(method)) {
        throwErrorBadrequest("error request line");
    }

    // バージョンが1.1以外なら505

    method_  = method;
    uri_     = uri;
    version_ = version;
}

// tokenとして適切な文字列か判定
//
// note: こういうのクラスのメソッドである意味がないからクラスの外に出したい
// その場合、request/utils/isToken.cpp になる？
bool HTTPRequest::isToken(std::string str) {
    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            return false;
        }
    }
    return true;
}

void HTTPRequest::parseHeaderLine(std::string line) {
    std::cout << GREEN << "line: " << line << RESET << std::endl;
    std::size_t pos = line.find(":");
    std::string key, value;
    key = line.substr(0, pos);
    if (pos != line.npos) {
        value = line.substr(pos + 1, line.size() - pos);
    }

    if (!isToken(key)) {
        throwErrorBadrequest("error token");
    }

    // valueの前後のスペースはtrim

    // valueの間にスペースはNG

    headers_.insert(std::make_pair(key, value));
}

bool HTTPRequest::isLastLine(std::string& str) {
    const std::size_t crlf_size    = crlf.size();
    std::size_t       line_end_pos = str.find(crlf);
    std::string       next_line    = str.substr(line_end_pos + crlf_size);

    return next_line.size() >= crlf_size &&
           next_line.substr(0, crlf_size) == crlf;
}

void HTTPRequest::parseBody(std::string body) {
    std::cout << "body: " << body << std::endl;
    if (body.empty()) {
        std::cout << "empty body" << std::endl;
        return;
    }
    if (body.size() >= crlf.size() && body.substr(0, crlf.size()) == crlf) {
        // 改行が3つ連続していた場合
        throwErrorBadrequest("error body");
    }
    body_ = body;
}

void HTTPRequest::throwErrorBadrequest(std::string err_message) {
    status_ = 400;
    throw std::runtime_error(err_message);
}

void HTTPRequest::parse() {
    try {
        std::size_t line_end_pos = row_.find(crlf);
        if (line_end_pos == row_.npos) {
            throwErrorBadrequest("error request line");
        }

        parseFirstline(row_.substr(0, line_end_pos));

        std::string str = row_;
        while (status_ == 200 && !isLastLine(str)) {
            std::size_t line_start_pos = line_end_pos + crlf.size();
            str                        = str.substr(line_start_pos);
            line_end_pos               = str.find(crlf);

            if (line_end_pos == row_.npos) {
                throwErrorBadrequest("missing crlf");
            }
            parseHeaderLine(str.substr(0, line_end_pos));
        }
        std::string body =
            str.substr(str.find(crlf) + crlf.size() + crlf.size());
        parseBody(body);
    } catch (std::runtime_error& e) {
        std::cout << RED << "exception: " << e.what() << RESET << std::endl;
    } catch (std::exception& e) {}
}
