#include "ReadFile.hpp"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "SysError.hpp"

const std::size_t ReadFile::BUF_SIZE = 2048;

ReadFile::ReadFile(StreamSocket stream, int fd, status::code status_code)
    : IOEvent(fd, READ_FILE), stream_(stream), finish_(false),
      status_code_(status_code) {}

ReadFile::ReadFile()
    : IOEvent(READ_FILE), stream_(StreamSocket()), finish_(false),
      resp_(HTTPResponse()) {}

ReadFile::~ReadFile() {}

void ReadFile::Run(intptr_t offset) {
    printLogStart();
    char buf[BUF_SIZE];
    int  fd = polled_fd_;

    ssize_t read_size = read(fd, buf, BUF_SIZE);
    if (read_size == -1) {
        perror("read");
        throw status::server_error;
    }

    if (read_size == 0 || read_size == offset) {
        finish_ = true;
    }
    file_content_.append(buf, read_size);
}

void ReadFile::Register() { EventRegister::Instance().AddReadEvent(this); }

void ReadFile::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* ReadFile::RegisterNext() {
    if (!finish_) {
        return this;
    }
    // response 作成
    std::stringstream ss;
    ss << file_content_.size() << std::flush;
    resp_.SetStatusCode(status_code_);
    resp_.AppendHeader("Content-Length", ss.str());
    resp_.SetBody(file_content_);
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.PrintInfo();

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    this->Unregister();
    send_response->Register();

    printLogEnd();
    return send_response;
}

int ReadFile::Close() {
    if (polled_fd_ == -1) {
        return 0;
    }

    if (close(polled_fd_) == -1) {
        perror("close");
        std::cerr << "fd: " << polled_fd_ << std::endl;
        errno = 0;
        return -1;
    }
    polled_fd_ = -1;
    return 0;
}

void ReadFile::printLogStart() {
#ifdef WS_DEBUG
    std::cout << "=== ReadFile ===" << std::endl;
#endif
}

void ReadFile::printLogEnd() {
#ifdef WS_DEBUG
    std::cout << "================\n" << std::endl;
#endif
}
