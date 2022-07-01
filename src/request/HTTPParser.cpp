#include "HTTPParser.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include <iostream>

namespace HTTPParser {

SendBadrequest::SendBadrequest() {}

/* SendBadrequest::SendBadrequest(StreamSocket stream) : stream_(stream) {} */

SendBadrequest::~SendBadrequest() {}

void SendBadrequest::Run(intptr_t offset) { UNUSED(offset); }

// TODO: 要相談
// parse関数内でthrowするときにstreamをsetするのがめんどい
// parse内でRecvRequestで一回catchしてstreamをsetしてthrowする？
IOEvent* SendBadrequest::RegisterNext() {
    HTTPResponse resp;
    // set
    resp.SetStatusCode(status::bad_request);

    IOEvent* send_response = new SendResponse(stream_, resp.ConvertToStr());
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}

State::State(HTTPRequest& req) : phase_(FIRST_LINE), req_(req) {}

std::string& State::Buf() { return buf_; }

std::string& State::BodyBuf() { return body_buf_; }

Phase& State::Phase() { return phase_; }

HTTPRequest& State::Request() { return req_; }

} // namespace HTTPParser
