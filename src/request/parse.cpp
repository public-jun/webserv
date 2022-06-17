#include "DefaultErrorPage.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include <iostream>
#include <sstream>

namespace {
void throw_code_badrequest(const std::string err_message = "bad request") {
    std::cerr << err_message << std::endl;
    throw status::bad_request;
}

void throw_code_version_not_supported(
    const std::string err_message = "version not supported") {
    std::cerr << err_message << std::endl;
    throw status::version_not_suppoted;
}

// token  = 1*tchar
// tchar  = "!" / "#" / "$" / "%" / "&" / "'" / "*"
//       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
//       / DIGIT / ALPHA
void validate_token(const std::string& token) {
    if (token.empty()) {
        throw_code_badrequest("empty token");
    }
    const std::string special = "!#$%&'*+-.^_`|~";
    for (std::string::const_iterator it = token.begin(); it != token.end();
         it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            throw_code_badrequest("error token");
        }
    }
}

void validate_method(const std::string& method) {
    validate_token(method);

    for (std::string::const_iterator it = method.begin(); it != method.end();
         it++) {
        if (!std::isupper(*it) && *it != '_' && *it != '-') {
            throw_code_badrequest("error method");
        }
    }
}

void validate_request_target(const std::string& request_target) {
    // TODO: バリデート
    if (request_target.empty()) {
        throw_code_badrequest("error request target");
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
        throw_code_badrequest("error HTTP version");
    }

    std::string::size_type pos = version.find("/");
    if (pos == version.npos) {
        throw_code_badrequest("error not exist slash");
    }

    std::string name = version.substr(0, pos);
    if (name != "HTTP") {
        throw_code_badrequest("error protocol name");
    }

    std::string            digit   = version.substr(pos + 1);
    std::string::size_type dot_pos = digit.find_first_of('.');

    // dotがあるか
    // dotが複数ないか
    if (dot_pos == digit.npos || dot_pos != digit.find_last_of('.')) {
        throw_code_badrequest("error version");
    }

    std::string before_dot = digit.substr(0, dot_pos);
    std::string after_dot  = digit.substr(dot_pos + 1);
    // dotの前後が空か
    // dotの前後が数字か
    if (before_dot.empty() || after_dot.empty() || !isdigit(before_dot) ||
        !isdigit(after_dot)) {
        throw_code_badrequest("error version");
    }
}

void validate_host(const HTTPRequest& req) {
    // TODO: find使って調べる
    if (req.GetHeaderValue("Host").empty()) {
        throw_code_badrequest("empty host");
    }
}

void validate_version_not_suppoted(const std::string& version) {
    if (version != "HTTP/1.1") {
        throw_code_version_not_supported();
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
        throw_code_badrequest("error value");
    }

    req.SetHeader(key, value);
}

void parse_firstline(HTTPRequest& req, const std::string& line) {
    std::istringstream iss(line);
    std::string        method, request_target;
    std::string        version = "HTTP/1.1";

    iss >> method >> request_target >> version;

    // validate
    validate_method(method);
    validate_request_target(request_target);
    validate_version(version);

    // set
    req.SetMethod(method);
    req.SetRequestTarget(request_target);
    req.SetHTTPVersion(version);
}

bool split_to_line(std::string& buf, std::string& line) {
    std::string::size_type line_end_pos = buf.find(HTTPRequest::crlf);
    if (line_end_pos == buf.npos) {
        return false;
    }

    line = buf.substr(0, line_end_pos);
    buf  = buf.substr(line_end_pos + HTTPRequest::crlf_size);
    return true;
}

bool can_parse_line(std::string& buf, std::string& line) {
    return split_to_line(buf, line);
}

unsigned long str_to_ulong(const std::string& str) {
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

void might_set_body(HTTPParser::State& state, const std::string& buf,
                    unsigned long content_length) {
    if (buf.size() >= content_length) {
        state.Request().SetBody(std::string(buf, 0, content_length));
        state.Phase() = HTTPParser::DONE;
    }
}

bool needs_parse_body(const std::string& method) { return method != "GET"; }

bool has_done_header_line(const std::string& line) { return line == ""; }

void validate_after_parse_header(HTTPRequest& req) {
    validate_host(req);
    validate_version_not_suppoted(req.GetVersion());
}

} // namespace

namespace HTTPParser {
void update_state(State& state, const std::string new_buf) {
    HTTPRequest& req   = state.Request();
    std::string& buf   = state.Buf();
    Phase&       phase = state.Phase();

    try {
        buf.append(new_buf.c_str(), new_buf.size());

        for (;;) {
            std::string line;
            switch (phase) {
            case FIRST_LINE:
                if (!can_parse_line(buf, line)) {
                    return;
                }
                parse_firstline(req, line);
                phase = HEADER_LINE;
                break;

            case HEADER_LINE:
                if (!can_parse_line(buf, line)) {
                    return;
                }
                if (has_done_header_line(line)) {
                    validate_after_parse_header(req);
                    phase = BODY;
                } else {
                    parse_header_line(req, line);
                }
                break;

            case BODY:
                if (!needs_parse_body(req.GetMethod())) {
                    phase = DONE;
                    return;
                }
                might_set_body(
                    state, buf,
                    str_to_ulong(req.GetHeaderValue("Content-length")));
                return;

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
} // namespace HTTPParser
