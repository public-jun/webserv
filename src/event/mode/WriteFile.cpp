#include "WriteFile.hpp"

#include <cerrno>
#include <iostream>
#include <string>
#include <unistd.h>

WriteFile::WriteFile(StreamSocket stream, int fd, HTTPRequest req)
    : IOEvent(fd, WRITE_FILE), stream_(stream), req_(req) {}

WriteFile::~WriteFile() {}

void WriteFile::Run(intptr_t offset) {
    UNUSED(offset);
    std::string content = req_.GetBody();
    ssize_t     size    = write(polled_fd_, content.c_str(), content.size());
    if (size < 0 || static_cast<size_t>(size) != content.size())
        throw SysError("write", errno);
}

void WriteFile::Register() { EventRegister::Instance().AddWriteEvent(this); }

void WriteFile::Unregister() { EventRegister::Instance().DelWriteEvent(this); }

IOEvent* WriteFile::RegisterNext() {
    Unregister();
    resp_.AppendHeader("Content-Length", "0");
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.AppendHeader("Connection", "close");
    resp_.SetStatusCode(status::created);
    resp_.AppendHeader("Content-Length", "0");
    resp_.PrintInfo();
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    send_response->Register();

    return send_response;
}

int WriteFile::Close() {
    if (polled_fd_ == -1) {
        return 0;
    }

    if (close(polled_fd_) == -1) {
        perror("close");
        errno = 0;
        return -1;
    }
    polled_fd_ = -1;
    return 0;
}
