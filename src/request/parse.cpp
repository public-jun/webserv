#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "ServerConfig.hpp"
#include "URI.hpp"
#include <iostream>
#include <sstream>

namespace {

static const std::string            crlf      = "\r\n";
static const std::string::size_type crlf_size = crlf.size();

// token  = 1*tchar
// tchar  = "!" / "#" / "$" / "%" / "&" / "'" / "*"
//       / "+" / "-" / "." / "^" / "_" / "`" / "|" / "~"
//       / DIGIT / ALPHA
void validate_token(const std::string& token) {
    if (token.empty()) {
        throw status::bad_request;
    }
    const std::string           special = "!#$%&'*+-.^_`|~";
    std::string::const_iterator end     = token.end();
    for (std::string::const_iterator it = token.begin(); it != end; it++) {
        if (!std::isalnum(*it) && special.find(*it) == special.npos) {
            throw status::bad_request;
        }
    }
}

void validate_method(const std::string& method) {
    validate_token(method);

    std::string::const_iterator end = method.end();
    for (std::string::const_iterator it = method.begin(); it != end; it++) {
        if (!std::isupper(*it) && *it != '_' && *it != '-') {
            throw status::bad_request;
        }
    }

    if (method != "GET" && method != "POST" && method != "DELETE") {
        throw status::not_implemented;
    }
}

void validate_request_target(const std::string& request_target) {
    if (request_target.empty()) {
        throw status::bad_request;
    }
}

bool isdigit(const std::string& str) {
    std::string::const_iterator end = str.end();
    for (std::string::const_iterator it = str.begin(); it != end; it++) {
        if (!std::isdigit(*it)) {
            return false;
        }
    }
    return true;
}

void validate_version(const std::string& version) {
    if (version.empty()) {
        throw status::bad_request;
    }

    std::string::size_type pos = version.find("/");
    if (pos == version.npos) {
        throw status::bad_request;
    }

    std::string name = version.substr(0, pos);
    if (name != "HTTP") {
        throw status::bad_request;
    }

    std::string            digit   = version.substr(pos + 1);
    std::string::size_type dot_pos = digit.find_first_of('.');

    // dotがあるか
    // dotが複数ないか
    if (dot_pos == digit.npos || dot_pos != digit.find_last_of('.')) {
        throw status::bad_request;
    }

    std::string before_dot = digit.substr(0, dot_pos);
    std::string after_dot  = digit.substr(dot_pos + 1);
    // dotの前後が空か
    // dotの前後が数字か
    if (before_dot.empty() || after_dot.empty() || !isdigit(before_dot) ||
        !isdigit(after_dot)) {
        throw status::bad_request;
    }
}

void validate_host(const HTTPRequest& req) {
    if (req.GetHeaderValue("host").empty()) {
        throw status::bad_request;
    }
}

void validate_version_not_suppoted(const std::string& version) {
    if (version != "HTTP/1.1") {
        throw status::version_not_suppoted;
    }
}

void validate_transfer_encoding(const std::string& encoding) {
    if (encoding != "" && encoding != "chunked") {
        throw status::unsupported_media_type;
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

    // example: HOST -> host
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);

    value = trim_space(value);
    if (value.find_first_of(HTTPRequest::CRLF) != value.npos) {
        throw status::bad_request;
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
    std::string::size_type line_end_pos = buf.find(HTTPRequest::CRLF);
    if (line_end_pos == buf.npos) {
        return false;
    }

    line = buf.substr(0, line_end_pos);
    buf  = buf.substr(line_end_pos + HTTPRequest::CRLF_SIZE);
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

void might_set_body_with_content_length(HTTPParser::State& state,
                                        const std::string& buf,
                                        unsigned long      content_length) {
    if (buf.size() >= content_length) {
        state.Request().SetBody(std::string(buf, 0, content_length));
        state.Phase() = HTTPParser::DONE;
    }
}

bool needs_parse_body(const std::string& method) { return method == "POST"; }

bool has_done_header_line(const std::string& line) { return line == ""; }

void validate_after_parse_header(HTTPRequest& req) {
    validate_host(req);
    validate_version_not_suppoted(req.GetVersion());
    validate_transfer_encoding(req.GetHeaderValue("transfer-encoding"));
}

unsigned long try_parse_chunk_size(std::string size) {
    // chunk_sizeが全て数字か
    if (!isdigit(size)) {
        std::cerr << "[ERROR] chunk size is not digit" << std::endl;
        throw status::bad_request;
    }
    return str_to_ulong(size);
}

std::string try_parse_chunked_data(std::string buf, unsigned long chunk_size) {
    std::string chunked_data = buf.substr(0, chunk_size);
    // chunked_dataの後ろにCRLFがあるか
    if (buf.substr(chunk_size).size() >= crlf_size &&
        buf.substr(chunk_size, crlf_size) != crlf) {
        std::cout << "[ERROR] not exist crlf after chunked_data" << std::endl;
        throw status::bad_request;
    }
    return chunked_data;
}

bool try_is_last_chunk(std::string last_chunk) {
    const std::string::size_type chunk_size = last_chunk.size();

    if (chunk_size > crlf_size) {
        std::cerr << "last chunk size is too large" << std::endl;
        throw status::bad_request;
    }
    if (chunk_size < crlf_size) {
        return false;
    }
    if (last_chunk == crlf) {
        return true;
    }
    std::cerr << "buf doesn't match last_chunk" << std::endl;
    throw status::bad_request;
}

void parse_chunked_body(HTTPParser::State& state) {
    std::string& body = state.BodyBuf();
    std::string& buf  = state.Buf();

    for (;;) {
        std::string::size_type crlf_pos = buf.find(crlf);
        if (crlf_pos == std::string::npos) {
            return;
        }
        unsigned long chunk_size =
            try_parse_chunk_size(buf.substr(0, crlf_pos));
        // size0の場合、終端まであるか
        if (chunk_size == 0) {
            if (try_is_last_chunk(buf.substr(crlf_pos + crlf_size))) {
                state.Request().SetBody(body);
                state.Phase() = HTTPParser::DONE;
            }
            return;
        }
        // size+crlf_size以上のdataがあるか
        if (buf.substr(crlf_pos + crlf_size).size() < chunk_size + crlf_size) {
            return;
        }
        std::string chunked_data = try_parse_chunked_data(
            buf.substr(crlf_pos + crlf_size), chunk_size);
        // buf更新
        body.append(chunked_data, 0, chunk_size);
        buf = buf.substr(crlf_pos + crlf_size + chunk_size + crlf_size);
    }
}

/*

chunked-body   = *chunk
                 last-chunk
                 trailer-part
                 CRLF

chunk          = chunk-size [ chunk-ext ] CRLF
                 chunk-data CRLF
chunk-size     = 1*HEXDIG
last-chunk     = 1*("0") [ chunk-ext ] CRLF
chunk-data     = 1*OCTET ; 長さ chunk-size のオクテット列

Example Body:
7\r\n
Mozilla\r\n
9\r\n
Developer\r\n
7\r\n
Network\r\n
0\r\n
\r\n

*/
void might_set_chunked_body(HTTPParser::State& state) {
    parse_chunked_body(state);
}

void try_find_method(const std::vector<std::string> methods,
                     const std::string&             method) {
    const std::vector<std::string>::const_iterator it =
        std::find(methods.begin(), methods.end(), method);
    if (it == methods.end()) {
        throw status::method_not_allowed;
    }
}

void validate_body_size(const std::string& body, ServerConfig server_config) {
    if (body.size() > server_config.GetMaxClientBodySize()) {
        throw status::request_entity_too_large;
    }
}

// location configが空 -> server configを採用
void validate_allowed_method(const URI& uri, const std::string& method) {

    const std::vector<std::string>& location_methods =
        uri.GetLocationConfig().GetAllowedMethods();

    if (!location_methods.empty()) {
        try_find_method(location_methods, method);
    } else {
        try_find_method(uri.GetServerConfig().GetAllowedMethods(), method);
    }
}

} // namespace

namespace HTTPParser {

void validate_request(const URI& uri, const HTTPRequest& req) {
    validate_body_size(req.GetBody(), uri.GetServerConfig());
    validate_allowed_method(uri, req.GetMethod());
}

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
                if (req.GetHeaderValue("transfer-encoding") == "chunked") {
                    might_set_chunked_body(state);
                } else if (req.GetHeaderValue("content-length") == "") {
                    throw status::length_required;
                } else {
                    might_set_body_with_content_length(
                        state, buf,
                        str_to_ulong(req.GetHeaderValue("content-length")));
                }

                return;

            case DONE:
                return;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "update_state: " << e.what() << std::endl;
        throw status::server_error;
    }
}
} // namespace HTTPParser
