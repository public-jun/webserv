#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include <iostream>
#include <sstream>

HTTPParser::HTTPParser(HTTPRequest& req) : phase_(PH_FIRST_LINE), req_(req){};

HTTPParser::~HTTPParser() {}

HTTPParser::Phase HTTPParser::GetPhase() const { return phase_; }

void HTTPParser::Parse(std::string request_message) {
    std::string::iterator it =
        std::find(request_message.begin(), request_message.end(), '\n');
    std::string line(request_message.begin(), it);

    std::istringstream iss(line);
    std::string        method, uri;
    iss >> method >> uri;
    if (method.empty()) {
        std::cout << "get method error" << std::endl;
    }
    if (uri.empty()) {
        std::cout << "get uri error" << std::endl;
    }
    req_.SetMethod(method);
    req_.SetURI(uri);
}

void HTTPParser::throwErrorBadrequest(
    const std::string err_message = "bad request") {
    req_.SetStatus(HTTPRequest::status_bad_request);
    throw std::runtime_error(err_message);
}

void HTTPParser::throwErrorMethodNotAllowed(
    const std::string err_message = "method not allowed") {
    req_.SetStatus(HTTPRequest::status_method_not_allowed);
    throw std::runtime_error(err_message);
}

void HTTPParser::throwErrorVersionNotSupported(
    const std::string err_message = "version not supported") {
    req_.SetStatus(HTTPRequest::status_version_not_supported);
    throw std::runtime_error(err_message);
}

void HTTPParser::validateMethod(const std::string& method) {
    validateToken(method);

    for (std::string::const_iterator it = method.begin(); it != method.end();
         it++) {
        if (!std::isupper(*it) && *it != '_' && *it != '-') {
            throwErrorBadrequest("error method");
        }
    }
}

void HTTPParser::validateRequestTarget(const std::string& request_target) {
    // TODO: バリデート
    if (request_target.empty()) {
        throwErrorBadrequest("error request target");
    }
}

bool HTTPParser::isdigit(const std::string& str) const {
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void HTTPParser::validateHTTPVersion(const std::string& version) {
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

// token  = 1*tchar
// tchar  = "!" / "#" / "$" / "%" / "&" / "'" / "*"
//       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
//       / DIGIT / ALPHA
void HTTPParser::validateToken(const std::string& token) {
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

// 前後のスペースをtrim
std::string HTTPParser::trimSpace(const std::string& str,
                                  const std::string trim_char_set = " ") const {
    std::string            result;
    std::string::size_type left = str.find_first_not_of(trim_char_set);

    if (left != str.npos) {
        std::string::size_type right = str.find_last_not_of(trim_char_set);
        result                       = str.substr(left, right - left + 1);
    }
    return result;
}

void HTTPParser::parseHeaderLine(const std::string& line) {
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
    if (value.find_first_of(HTTPRequest::crlf) != value.npos) {
        throwErrorBadrequest("error value");
    }

    std::cout << "key: " << key << std::endl;
    std::cout << "value: " << value << std::endl;
    // TODO: set header
    /* headers_.insert(std::make_pair(key, value)); */
}

void HTTPParser::parseFirstline(const std::string& line) {
    std::cout << "line: " << line << std::endl;

    std::istringstream iss(line);
    std::string        method, request_target;
    std::string        version = "HTTP/1.1";

    iss >> method >> request_target >> version;

    if (request_target == "/") {
        request_target = "index.html";
    }
    // validate
    validateMethod(method);
    validateRequestTarget(request_target);
    validateHTTPVersion(version);

    // set
    req_.SetMethod(method);
    req_.SetHTTPVersion(version);
}

void HTTPParser::ParsePart(const std::string& buf) {
    buf_ += buf;

    for (;;) {
        // 改行があるか判定
        std::string::size_type line_end_pos =
            buf_.find_first_of(HTTPRequest::crlf);
        if (line_end_pos == buf_.npos) {
            return;
        }

        std::string line = buf_.substr(0, line_end_pos);
        buf_             = buf_.substr(line_end_pos + HTTPRequest::crlf_size);

        switch (phase_) {
        case PH_FIRST_LINE:
            parseFirstline(line);
            phase_ = PH_HEADER_LINE;
            break;

        case PH_HEADER_LINE:
            if (line == "") {
                phase_ = PH_END;
                break;
            }
            parseHeaderLine(line);
            break;

        case PH_END:
            return;
        }
    }
}

/*
 * 実装案
 * - 改行ごとにパース
 *  1. 読み込み
 *  2. (GETの場合)空行あるか判定
 *    - あれば最後までパースしてHTTPRequestを返す
 *  3. 改行があるか判定
 *    - なければ1にもどる
 *    - あれば改行までパース->改行後の文字列は保持->3
 *  - 改行がなかった場合、エラー吐かずに一生読み込もうとする
 *
 *
 */
