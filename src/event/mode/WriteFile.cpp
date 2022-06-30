#include "WriteFile.hpp"

#include <cerrno>
#include <iostream>
#include <string>
#include <unistd.h>

WriteFile::WriteFile(StreamSocket stream, int fd, HTTPRequest req)
    : IOEvent(fd, WRITE_FILE), stream_(stream), req_(req) {}

WriteFile::~WriteFile() {
    if (polled_fd_ != -1) {
        close(polled_fd_);
        polled_fd_ = -1;
    }
}

void WriteFile::Run(intptr_t offset) {
    (void)offset;
    std::string content = req_.GetBody();
    int         size    = write(polled_fd_, content.c_str(), content.size());
    if (size < 0 || static_cast<size_t>(size) != content.size())
        throw SysError("write", errno);
}

void WriteFile::Register() { EventRegister::Instance().AddWriteEvent(this); }

void WriteFile::Unregister() { EventRegister::Instance().DelWriteEvent(this); }

IOEvent* WriteFile::RegisterNext() {
    Unregister();
    resp_.AppendHeader("Content-Length", "0");
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetStatusCode(status::created);
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    send_response->Register();

    return send_response;
}
