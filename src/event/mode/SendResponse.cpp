#include "SendResponse.hpp"

#include <cerrno>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "EventAction.hpp"
#include "RecvRequest.hpp"
#include "SysError.hpp"

#include <iostream>

SendResponse::SendResponse()
    : IOEvent(SEND_RESPONSE), stream_(StreamSocket()) {}

SendResponse::SendResponse(StreamSocket stream, std::string buf)
    : IOEvent(stream.GetSocketFd(), SEND_RESPONSE), stream_(stream),
      all_buf_(buf) {}

SendResponse::~SendResponse() {
    if (stream_.GetSocketFd() == -1) {
        close(stream_.GetSocketFd());
    }
}

void SendResponse::Run() {
    int ret = send(stream_.GetSocketFd(), all_buf_.c_str(), all_buf_.size(), 0);
    if (ret < 0) {
        throw SysError("send", errno);
    }
}

IOEvent* SendResponse::RegisterNext() {
    EventAction::GetInstance().DelWriteEvent(this);

    // To Do Keep alive

    return NULL;
}
