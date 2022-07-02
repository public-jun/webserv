#include "SendResponse.hpp"

#include <cerrno>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "EventRegister.hpp"
#include "RecvRequest.hpp"
#include "SysError.hpp"

#include <iostream>

SendResponse::SendResponse()
    : IOEvent(SEND_RESPONSE), stream_(StreamSocket()) {}

SendResponse::SendResponse(StreamSocket stream, std::string buf)
    : IOEvent(stream.GetSocketFd(), SEND_RESPONSE), stream_(stream),
      all_buf_(buf) {}

SendResponse::~SendResponse() {}

void SendResponse::Run(intptr_t offset) {
    UNUSED(offset);
    printLog();

    int ret = send(stream_.GetSocketFd(), all_buf_.c_str(), all_buf_.size(), 0);
    if (ret < 0) {
        throw SysError("send", errno);
    }
}

void SendResponse::Register() { EventRegister::Instance().AddWriteEvent(this); }

void SendResponse::Unregister() {
    EventRegister::Instance().DelWriteEvent(this);
}

IOEvent* SendResponse::RegisterNext() {
    // TODO: keep-alive
    if (close(stream_.GetSocketFd()) == -1) {
        perror("close");
    }
    this->Unregister();

    return NULL;
}

int SendResponse::Close() { return 0; }

void SendResponse::printLog() {
#ifdef WS_DEBUG
    std::cout << "=== SendResponse ==="
              << "\n"
              << "stream fd: " << stream_.GetSocketFd() << "\n"
              << "===================="
              << "\n"
              << std::endl;
#endif
}
