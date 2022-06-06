#include "RecvRequest.hpp"

#include <cerrno>
#include <unistd.h>

#include "EventAction.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"

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
    if (req_.GetMethod() == "GET") {
        ReadFile* read_file = new ReadFile(stream_, req_);

        EventAction& action = EventAction::GetInstance();
        action.DelReadEvent(this);
        action.AddReadEvent(read_file);
        return read_file;
    }
    return NULL;
}
