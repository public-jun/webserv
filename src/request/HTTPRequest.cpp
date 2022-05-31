#include "HTTPRequest.hpp"

#include <algorithm>
#include <cctype>
#include <sys/socket.h>
#include <sys/types.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::set<std::string> HTTPRequest::methods;

const std::string HTTPRequest::crlf = "\r\n";

const std::string::size_type HTTPRequest::crlf_size = crlf.size();

HTTPRequest::HTTPRequest() {}

HTTPRequest::HTTPRequest(const std::string& row)
    : row_(row), status_(status_ok) {
    HTTPRequest::methods.insert("GET");
    parse();
}

HTTPRequest::HTTPRequest(std::string method, std::string uri)
    : method_(method), request_target_(uri) {}

HTTPRequest::~HTTPRequest() {}

const std::string& HTTPRequest::GetMethod() const { return method_; }

std::string HTTPRequest::GetRequestTarget() const { return request_target_; }

const std::string& HTTPRequest::GetVersion() const { return HTTPVersion_; }

int HTTPRequest::GetStatus() const { return status_; }

const std::string HTTPRequest::GetHeaderValue(const std::string key) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(key);
    if (it == headers_.end()) {
        return "";
    }
    return (*it).second;
}

const std::map<std::string, std::string>& HTTPRequest::GetHeaders() const {
    return headers_;
}

void HTTPRequest::SetURI(const std::string uri) { request_target_ = uri; }

void HTTPRequest::SetMethod(const std::string method) { method_ = method; }

void HTTPRequest::SetHTTPVersion(const std::string version) {
    HTTPVersion_ = version;
}

void HTTPRequest::SetStatus(const int status) { status_ = status; }

void HTTPRequest::throwErrorBadrequest(
    const std::string err_message = "bad request") {
    status_ = status_bad_request;
    throw std::runtime_error(err_message);
}

void HTTPRequest::throwErrorMethodNotAllowed(
    const std::string err_message = "method not allowed") {
    status_ = status_method_not_allowed;
    throw std::runtime_error(err_message);
}

void HTTPRequest::throwErrorVersionNotSupported(
    const std::string err_message = "version not supported") {
    status_ = status_version_not_supported;
    throw std::runtime_error(err_message);
}

void HTTPRequest::validateMethod(const std::string& method) {
    validateToken(method);

    for (std::string::const_iterator it = method.begin(); it != method.end();
         it++) {
        if (!std::isupper(*it)) {
            throwErrorBadrequest("error method");
        }
    }
}

void HTTPRequest::validateBody(const std::string& body) {
    if (body.size() >= crlf_size && body.substr(0, crlf_size) == crlf) {
        // 改行が3つ連続していた場合
        throwErrorBadrequest("error body");
    }
}

void HTTPRequest::validateRequestTarget(const std::string& request_target) {
    // TODO: バリデート
    if (request_target.empty()) {
        throwErrorBadrequest("error request target");
    }
}

void HTTPRequest::validateHTTPVersion(const std::string& version) {
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

// token          = 1*tchar
// tchar          = "!" / "#" / "$" / "%" / "&" / "'" / "*"
//               / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
//               / DIGIT / ALPHA
void HTTPRequest::validateToken(const std::string& token) {
    if (token.empty()) {
        throwErrorBadrequest("empty token");
    }
    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::const_iterator it = token.begin(); it != token.end();
         it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            throwErrorBadrequest("error token");
        }
    }
}

void HTTPRequest::validateVersionNotSuppoted() {
    if (HTTPVersion_ != "HTTP/1.1") {
        throwErrorVersionNotSupported();
    }
}

void HTTPRequest::validateMethodNotAllowed() {
    if (methods.find(method_) == methods.end()) {
        throwErrorMethodNotAllowed();
    }
}

// リクエストの1行目をパースしてバリデート
void HTTPRequest::parseFirstline(const std::string& line) {
    std::istringstream iss(line);
    std::string        method, request_target;
    std::string        version = "HTTP/1.1";

    iss >> method >> request_target >> version;

    if (request_target == "/") {
        request_target = "index.html";
    }

    validateMethod(method);
    validateRequestTarget(request_target);
    validateHTTPVersion(version);

    method_         = method;
    request_target_ = request_target;
    HTTPVersion_    = version;
}

// 前後のスペースをtrim
std::string
HTTPRequest::trimSpace(const std::string& str,
                       const std::string  trim_char_set = " ") const {
    std::string            result;
    std::string::size_type left = str.find_first_not_of(trim_char_set);

    if (left != str.npos) {
        std::string::size_type right = str.find_last_not_of(trim_char_set);
        result                       = str.substr(left, right - left + 1);
    }
    return result;
}

bool HTTPRequest::isdigit(const std::string& str) const {
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

// 改行コードが見つからなかったら例外を投げる
std::string::size_type HTTPRequest::mustFindCRLF(const std::string& str) {
    std::string::size_type line_end_pos = str.find(crlf);
    if (line_end_pos == row_.npos) {
        throwErrorBadrequest("missing crlf");
    }
    return line_end_pos;
}

void HTTPRequest::parseHeaderLine(const std::string& line) {
    std::string::size_type pos = line.find(":");
    std::string            key, value;
    key = line.substr(0, pos);
    if (pos != line.npos) {
        value = line.substr(pos + 1, line.size() - pos);
    }

    validateToken(key);

    // example: HOST -> Host
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (key.size() >= 1) {
        key[0] = std::toupper(key[0]);
    }

    value = trimSpace(value);
    if (value.find_first_of(crlf) != value.npos) {
        throwErrorBadrequest("error value");
    }

    headers_.insert(std::make_pair(key, value));
}

bool HTTPRequest::hostExists() const { return !GetHeaderValue("Host").empty(); }

void HTTPRequest::parseBody(std::string& body) {
    if (body.empty()) {
        return;
    }
    validateBody(body);
    body_ = body;
}

// 2行目以降のヘッダーをパース
void HTTPRequest::parseHeaderLines(std::string& str) {
    while (str.substr(0, crlf_size) != crlf) {
        std::string::size_type line_end_pos = mustFindCRLF(str);
        parseHeaderLine(str.substr(0, line_end_pos));
        str = str.substr(line_end_pos + crlf_size);
        mustFindCRLF(str);
    }
    if (!hostExists()) {
        throwErrorBadrequest("no host");
    }
}

void HTTPRequest::parse() {
    try {
        std::string::size_type line_end_pos = mustFindCRLF(row_);
        parseFirstline(row_.substr(0, line_end_pos));

        std::string str = row_.substr(line_end_pos + crlf_size);
        parseHeaderLines(str);

        std::string body = str.substr(crlf_size);
        parseBody(body);

        // 400以外のエラー処理
        validateVersionNotSuppoted();
        validateMethodNotAllowed();
    } catch (std::runtime_error& e) {
        std::cout << "exception: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}

