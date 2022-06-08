#include "RecvRequest.hpp"

#include <cerrno>
#include <iostream>
#include <unistd.h>

#include "EventRegister.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"

const int RecvRequest::buf_size = 2048;

RecvRequest::RecvRequest()
    : IOEvent(RECV_REQUEST), stream_(StreamSocket()), req_(HTTPRequest()),
      state_(Parser::State(req_)) {}

RecvRequest::RecvRequest(StreamSocket stream)
    : IOEvent(stream.GetSocketFd(), RECV_REQUEST), stream_(stream),
      req_(HTTPRequest()), state_(Parser::State(req_)) {}

RecvRequest::~RecvRequest() {}

void RecvRequest::Run() {
    char buf[buf_size];
    int  recv_size = recv(stream_.GetSocketFd(), buf, buf_size, 0);

    Parser::parse(state_, std::string(buf, recv_size));
}

IOEvent* RecvRequest::RegisterNext() {
    if (state_.Phase() != Parser::DONE) {
        return this;
    }

    // methodによって次のイベントが分岐
    IOEvent* new_event = prepareResponse();

    return new_event;
}

IOEvent* RecvRequest::prepareResponse() {
    if (req_.GetMethod() == "GET") {
        ReadFile* read_file = new ReadFile(stream_, state_.Request());

        EventRegister::Instance().DelReadEvent(this);
        EventRegister::Instance().AddReadEvent(read_file);
        return read_file;
    }
    return NULL;
}
