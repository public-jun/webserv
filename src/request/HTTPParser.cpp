#include "HTTPParser.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include <iostream>

namespace HTTPParser {

State::State(HTTPRequest& req) : phase_(FIRST_LINE), req_(req) {}

std::string& State::Buf() { return buf_; }

std::string& State::BodyBuf() { return body_buf_; }

Phase& State::Phase() { return phase_; }

HTTPRequest& State::Request() { return req_; }

} // namespace HTTPParser
