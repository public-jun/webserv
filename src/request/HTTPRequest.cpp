#include "HTTPRequest.hpp"

#include <cctype>
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
    : method_(method), request_target_(uri) {}

HTTPRequest::~HTTPRequest() {}

std::string& HTTPRequest::GetMethod() { return method_; }

std::string& HTTPRequest::GetRequestTarget() { return request_target_; }

std::string& HTTPRequest::GetVersion() { return HTTPVersion_; }

int HTTPRequest::GetStatus() { return status_; }

std::string HTTPRequest::GetHeaderValue(std::string key) {
    return headers_[key];
}

std::map<std::string, std::string> HTTPRequest::GetHeaders() {
    return headers_;
}

void HTTPRequest::SetURI(std::string uri) { request_target_ = uri; }

void HTTPRequest::varidateMethod(std::string& method) {
    if (method.empty() || !isToken(method)) {
        throwErrorBadrequest("error method");
    }

    for (std::string::iterator it = method.begin(); it != method.end(); it++) {
        if (!std::isupper(*it)) {
            throwErrorBadrequest("error method");
        }
    }
}

bool HTTPRequest::isdigit(std::string str) {
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void HTTPRequest::varidateRequestTarget(std::string request_target) {
    if (request_target.empty()) {
        throwErrorBadrequest("error request target");
    }
}

void HTTPRequest::varidateHTTPVersion(std::string version) {
    if (version.empty()) {
        throwErrorBadrequest("error HTTP version");
    }

    std::string::size_type pos = version.find("/");
    if (pos == version.npos) {
        throwErrorBadrequest("error not exist slash");
    }

    std::string name = version.substr(0, pos);
    if (name != "HTTP") {
        throwErrorBadrequest("error protocol name");
    }

    std::string            digit   = version.substr(pos + 1);
    std::string::size_type dot_pos = digit.find_first_of('.');

    // dotがあるか
    // dotが複数ないか
    if (dot_pos == digit.npos || dot_pos != digit.find_last_of('.')) {
        throwErrorBadrequest("error version");
    }

    std::string before_dot = digit.substr(0, dot_pos);
    std::string after_dot  = digit.substr(dot_pos + 1);
    // dotの前後が空か
    // dotの前後が数字か
    if (before_dot.empty() || after_dot.empty() || !isdigit(before_dot) ||
        !isdigit(after_dot)) {
        throwErrorBadrequest("error version");
    }
}

void HTTPRequest::parseFirstline(std::string line) {
    std::istringstream iss(line);
    std::string        method, request_target;
    std::string        version = "HTTP/1.1";

    iss >> method >> request_target >> version;

    if (request_target == "/") {
        request_target = "index.html";
    }

    // バリデート
    varidateMethod(method);
    varidateRequestTarget(request_target);
    varidateHTTPVersion(version);

    method_         = method;
    request_target_ = request_target;
    HTTPVersion_    = version;
}

// tokenとして適切な文字列か判定
bool HTTPRequest::isToken(std::string str) {
    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            return false;
        }
    }
    return true;
}

// 前後のスペースをtrim
std::string HTTPRequest::trimSpace(const std::string& str,
                                   const std::string  trim_char_set = " ") {
    std::string            result;
    std::string::size_type left = str.find_first_not_of(trim_char_set);

    if (left != str.npos) {
        std::string::size_type right = str.find_last_not_of(trim_char_set);
        result                       = str.substr(left, right - left + 1);
    }
    return result;
}

void HTTPRequest::parseHeaderLine(std::string line) {
    std::string::size_type pos = line.find(":");
    std::string            key, value;
    key = line.substr(0, pos);
    if (pos != line.npos) {
        value = line.substr(pos + 1, line.size() - pos);
    }

    if (!isToken(key)) {
        throwErrorBadrequest("error token");
    }

    value = trimSpace(value);
    // OK: \t, \v, \f, \b
    // NG: \r

    headers_.insert(std::make_pair(key, value));
}

bool HTTPRequest::isLastLine(std::string& str) {
    const std::string::size_type crlf_size    = crlf.size();
    std::string::size_type       line_end_pos = str.find(crlf);
    std::string next_line = str.substr(line_end_pos + crlf_size);

    return next_line.size() >= crlf_size &&
           next_line.substr(0, crlf_size) == crlf;
}

void HTTPRequest::parseBody(std::string body) {
    if (body.empty()) {
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

bool HTTPRequest::hostExists() { return !GetHeaderValue("Host").empty(); }

void HTTPRequest::parse() {
    try {
        std::string::size_type line_end_pos = row_.find(crlf);
        if (line_end_pos == row_.npos) {
            throwErrorBadrequest("error request line");
        }

        parseFirstline(row_.substr(0, line_end_pos));

        std::string str = row_;
        while (status_ == 200 && !isLastLine(str)) {
            std::string::size_type line_start_pos = line_end_pos + crlf.size();
            str                                   = str.substr(line_start_pos);
            line_end_pos                          = str.find(crlf);

            if (line_end_pos == row_.npos) {
                throwErrorBadrequest("missing crlf");
            }
            parseHeaderLine(str.substr(0, line_end_pos));
        }
        if (!hostExists()) {
            throwErrorBadrequest("no host");
        }
        std::string body =
            str.substr(str.find(crlf) + crlf.size() + crlf.size());
        parseBody(body);
    } catch (std::runtime_error& e) {
        std::cout << RED << "exception: " << e.what() << RESET << std::endl;
    } catch (std::exception& e) {}

    // バージョンが1.1以外なら505
}
