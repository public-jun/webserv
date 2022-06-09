#include "RecvRequest.hpp"

#include <cerrno>
#include <unistd.h>

#include "CGI.hpp"
#include "EventRegister.hpp"
#include "ReadCGI.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"

#include <iostream>

RecvRequest::RecvRequest()
    : IOEvent(RECV_REQUEST), stream_(StreamSocket()), req_(HTTPRequest()),
      parser_(HTTPParser(req_)) {}

RecvRequest::RecvRequest(StreamSocket stream)
    : IOEvent(stream.GetSocketFd(), RECV_REQUEST), stream_(stream),
      req_(HTTPRequest()), parser_(HTTPParser(req_)) {}

RecvRequest::~RecvRequest() {}

void RecvRequest::Run() {
    char buf[2048];
    int  recv_size = recv(stream_.GetSocketFd(), buf, 2048 - 1, 0);
    buf[recv_size] = '\0';
    all_buf_.append(buf, recv_size);
    parser_.Parse(std::string(buf));
}

IOEvent* RecvRequest::RegisterNext() {
    if (parser_.GetPhase() != HTTPParser::PH_END) {
        return this;
    }
    // methodによって次のイベントが分岐
    IOEvent* new_event = prepareResponse();

    return new_event;
}

IOEvent* RecvRequest::prepareResponse() {
    IOEvent* new_event = NULL;

    if (req_.GetMethod() == "GET") {
        // Uriのパスや拡張子によって ReadFile or ReadCGI
        if (CGI::IsCGI(req_.GetRequestTarget())) {
            CGI cgi(req_);
            cgi.Run();
            new_event = new ReadCGI(cgi.FdForReadFromCGI(), stream_, req_);
        } else {
            new_event = new ReadFile(stream_, req_);
        }

        EventRegister::Instance().DelReadEvent(this);
        EventRegister::Instance().AddReadEvent(new_event);
        return new_event;
    }
    return NULL;
}
