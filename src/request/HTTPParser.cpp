#include "HTTPParser.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include <exception>
#include <iostream>
#include <sstream>

namespace {
void throw_error_badrequest(const std::string err_message = "bad request") {
    /* req.SetStatus(HTTPRequest::status_bad_request); */
    throw std::runtime_error(err_message);
}

void throw_error_method_not_allowed(
    const std::string err_message = "method not allowed") {
    /* req.SetStatus(HTTPRequest::status_method_not_allowed); */
    throw std::runtime_error(err_message);
}

void throw_error_version_not_supported(
    const std::string err_message = "version not supported") {
    /* req.SetStatus(HTTPRequest::status_version_not_supported); */
    throw std::runtime_error(err_message);
}

// token  = 1*tchar
// tchar  = "!" / "#" / "$" / "%" / "&" / "'" / "*"
//       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
//       / DIGIT / ALPHA
void validate_token(const std::string& token) {
    if (token.empty()) {
        throw_error_badrequest("empty token");
    }
    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::const_iterator it = token.begin(); it != token.end();
         it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            throw_error_badrequest("error token");
        }
    }
}

void validate_method(const std::string& method) {
    validate_token(method);

    for (std::string::const_iterator it = method.begin(); it != method.end();
         it++) {
        if (!std::isupper(*it) && *it != '_' && *it != '-') {
            throw_error_badrequest("error method");
        }
    }
}

void validate_request_target(const std::string& request_target) {
    // TODO: バリデート
    if (request_target.empty()) {
        throw_error_badrequest("error request target");
    }
}

bool isdigit(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void validate_version(const std::string& version) {
    if (version.empty()) {
        throw_error_badrequest("error HTTP version");
    }

    std::string::size_type pos = version.find("/");
    if (pos == version.npos) {
        throw_error_badrequest("error not exist slash");
    }

    std::string name = version.substr(0, pos);
    if (name != "HTTP") {
        throw_error_badrequest("error protocol name");
    }

    std::string            digit   = version.substr(pos + 1);
    std::string::size_type dot_pos = digit.find_first_of('.');

    // dotがあるか
    // dotが複数ないか
    if (dot_pos == digit.npos || dot_pos != digit.find_last_of('.')) {
        throw_error_badrequest("error version");
    }

    std::string before_dot = digit.substr(0, dot_pos);
    std::string after_dot  = digit.substr(dot_pos + 1);
    // dotの前後が空か
    // dotの前後が数字か
    if (before_dot.empty() || after_dot.empty() || !isdigit(before_dot) ||
        !isdigit(after_dot)) {
        throw_error_badrequest("error version");
    }
}

void validate_host(const HTTPRequest& req) {
    // TODO: find使って調べる
    if (req.GetHeaderValue("Host").empty()) {
        throw_error_badrequest("empty host");
    }
}

void validate_version_not_suppoted(const std::string& version) {
    if (version != "HTTP/1.1") {
        throw_error_version_not_supported();
    }
}

void validate_method_not_allowed(const HTTPRequest& req) {
    if (req.GetMethod() != "GET") {
        throw_error_method_not_allowed();
    }
}

// 前後のスペースをtrim
std::string trim_space(const std::string& str,
                       const std::string  trim_char_set = " ") {
    std::string            result;
    std::string::size_type left = str.find_first_not_of(trim_char_set);

    if (left != str.npos) {
        std::string::size_type right = str.find_last_not_of(trim_char_set);
        result                       = str.substr(left, right - left + 1);
    }
    return result;
}

void parse_header_line(HTTPRequest& req, const std::string& line) {
    std::string::size_type pos = line.find(":");
    std::string            key, value;
    key = line.substr(0, pos);
    if (pos != line.npos) {
        value = line.substr(pos + 1, line.size() - pos);
    }

    validate_token(key);

    // example: HOST -> Host
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    if (key.size() >= 1) {
        key[0] = std::toupper(key[0]);
    }

    value = trim_space(value);
    if (value.find_first_of(HTTPRequest::crlf) != value.npos) {
        throw_error_badrequest("error value");
    }

    req.SetHeader(key, value);
}

void parse_firstline(HTTPRequest& req, const std::string& line) {
    std::istringstream iss(line);
    std::string        method, request_target;
    std::string        version = "HTTP/1.1";

    iss >> method >> request_target >> version;

    if (request_target == "/") {
        request_target = "index.html";
    }
    // validate
    validate_method(method);
    validate_request_target(request_target);
    validate_version(version);

    // set
    req.SetMethod(method);
    req.SetRequestTarget(request_target);
    req.SetHTTPVersion(version);
}
} // namespace

namespace Parser {

SendError::SendError(StreamSocket stream) : stream_(stream) {}

void SendError::Run() {}

// TODO: どのstreamか分からん要相談
IOEvent* SendError::RegisterNext() {
    IOEvent* send_response = new SendResponse(stream_, "test");
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}

State::State(HTTPRequest& req) : phase_(FIRST_LINE), req_(req) {}

std::string& State::Buf() { return buf_; }

Phase& State::Phase() { return phase_; }

HTTPRequest& State::Request() { return req_; }

void parse(State& state, const std::string new_buf) {
    HTTPRequest& req   = state.Request();
    std::string& buf   = state.Buf();
    Phase&       phase = state.Phase();

    try {
        buf.append(new_buf.c_str(), new_buf.size());

        for (;;) {
            // 改行があるか判定
            std::string::size_type line_end_pos = buf.find(HTTPRequest::crlf);
            if (line_end_pos == buf.npos) {
                return;
            }

            std::string line = buf.substr(0, line_end_pos);
            buf = buf.substr(line_end_pos + HTTPRequest::crlf_size);

            switch (phase) {
            case FIRST_LINE:
                parse_firstline(req, line);
                phase = HEADER_LINE;
                break;

            case HEADER_LINE:
                if (line == "") {
                    validate_host(req);
                    validate_version_not_suppoted(req.GetVersion());
                    validate_method_not_allowed(req);
                    phase = DONE;
                    break;
                }
                parse_header_line(req, line);
                break;

            case DONE:
                return;
            }
        }
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

} // namespace Parser
