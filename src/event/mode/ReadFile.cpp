#include "ReadFile.hpp"

#include <cerrno>
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

ReadFile::ReadFile(StreamSocket stream, int fd)
    : IOEvent(fd, READ_FILE), stream_(stream) {}

ReadFile::ReadFile()
    : IOEvent(READ_FILE), stream_(StreamSocket()), resp_(HTTPResponse()) {}

ReadFile::~ReadFile() {}

void ReadFile::Run() {
    char buf[BUF_SIZE];
    int  fd = polled_fd_;

    int read_size = read(fd, buf, BUF_SIZE);
    // TODO: エラー処理
    file_content_.append(buf, read_size);
}

void ReadFile::Register() { EventRegister::Instance().AddReadEvent(this); }

void ReadFile::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* ReadFile::RegisterNext() {
    // response 作成
    std::stringstream ss;
    ss << file_content_.size() << std::flush;
    resp_.AppendHeader("Content-Length", ss.str());
    resp_.SetBody(file_content_);
    resp_.AppendHeader("Server", "Webserv/1.0.0");

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    this->Unregister();
    send_response->Register();

    return send_response;
}

void ReadFile::Close() {
    if (polled_fd_ != -1) {
        close(polled_fd_);
        polled_fd_ = -1;
    }
}
