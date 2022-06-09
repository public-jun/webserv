#include "ReadCGI.hpp"

#include <cerrno>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

ReadCGI::ReadCGI(int fd_read_from_cgi, StreamSocket stream, HTTPRequest req)
    : IOEvent(fd_read_from_cgi, READ_CGI), stream_(stream), req_(req),
      is_finish_(false), resp_(HTTPResponse()) {}

ReadCGI::~ReadCGI() {
    int fd = polled_fd_;
    if (fd != -1) {
        close(fd);
        polled_fd_ = -1;
    }
}

void ReadCGI::Run() {
    std::cout << "Read From CGI Start" << std::endl;
    char buf[2048];
    int  fd_from_cgi = polled_fd_;

    int read_size = read(fd_from_cgi, buf, 2048 - 1);
    if (read_size < 0) {
        throw SysError("read", errno);
    } else if (read_size == 0) {
        is_finish_ = true;
    } else {
        buf[read_size] = '\0';
        cgi_output_.append(buf, read_size);
    }
}

void ReadCGI::Register() { EventRegister::Instance().AddReadEvent(this); }

void ReadCGI::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* ReadCGI::RegisterNext() {
    if (!is_finish_) {
        return this;
    }

    // response 作成
    std::stringstream ss;
    ss << cgi_output_.size() << std::flush;
    resp_.AppendHeader("Content-Length", ss.str());
    resp_.SetBody(cgi_output_);
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    this->Unregister();
    send_response->Register();
    return send_response;
}
