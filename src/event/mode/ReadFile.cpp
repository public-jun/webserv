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

const std::size_t ReadFile::buf_size = 2048;

ReadFile::ReadFile(StreamSocket stream, int fd)
    : IOEvent(fd, READ_FILE), stream_(stream) {}

ReadFile::ReadFile()
    : IOEvent(READ_FILE), stream_(StreamSocket()), resp_(HTTPResponse()) {}

ReadFile::~ReadFile() {
    int fd = polled_fd_;
    if (fd != -1) {
        close(fd);
        polled_fd_ = -1;
    }
}

void ReadFile::Run() {
    char buf[buf_size];
    int  fd = polled_fd_;

    int read_size = read(fd, buf, buf_size);
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
