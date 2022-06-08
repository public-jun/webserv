#include "HTTPParser.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include <iostream>

namespace Parser {

SendError::SendError(StreamSocket stream) : stream_(stream) {}

SendError::~SendError() {}

void SendError::Run() {}

// TODO: 要相談
IOEvent* SendError::RegisterNext() {
    IOEvent* send_response = new SendResponse(stream_, "test");
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}

State::State(HTTPRequest& req) : phase_(FIRST_LINE), req_(req) {}

std::string& State::Buf() { return buf_; }

Phase& State::Phase() { return phase_; }

HTTPRequest& State::Request() { return req_; }

} // namespace Parser
