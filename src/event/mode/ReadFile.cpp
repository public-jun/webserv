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

ReadFile::ReadFile()
    : IOEvent(READ_FILE), stream_(StreamSocket()), resp_(HTTPResponse()) {
    openFile();
}

ReadFile::ReadFile(StreamSocket stream, HTTPRequest req)
    : IOEvent(READ_FILE), stream_(stream), req_(req), resp_(HTTPResponse()) {
    openFile();
}

ReadFile::~ReadFile() {
    int fd = polled_fd_;
    if (fd != -1) {
        close(fd);
        polled_fd_ = -1;
    }
}

void ReadFile::Run() {
    std::cout << "Read File start" << std::endl;
    char buf[2048];
    int  fd = polled_fd_;

    int read_size  = read(fd, buf, 2048 - 1);
    buf[read_size] = '\0';
    file_content_.append(buf, read_size);
    std::cout << "Read File end" << std::endl;
}

IOEvent* ReadFile::RegisterNext() {
    // response 作成
    std::stringstream ss;
    ss << file_content_.size() << std::flush;
    resp_.AppendHeader("Content-Length", ss.str());
    resp_.SetBody(file_content_);
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    EventRegister::Instance().DelReadEvent(this);
    EventRegister::Instance().AddWriteEvent(send_response);

    return send_response;
}

void ReadFile::openFile() {
    if (req_.GetRequestTarget() == "/") {
        req_.SetRequestTarget(std::string("/index.html"));
    }

    int fd = open(req_.GetRequestTarget().c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        throw SysError("open", errno);
    }

    polled_fd_ = fd;
}
