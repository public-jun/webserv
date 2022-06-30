#include "ReadCGI.hpp"

#include <cerrno>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "CGIResponse.hpp"
#include "CGIResponseParser.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

const std::size_t ReadCGI::BUF_SIZE = 2048;

ReadCGI::ReadCGI(int fd_read_from_cgi, StreamSocket stream, HTTPRequest req)
    : IOEvent(fd_read_from_cgi, READ_CGI), stream_(stream), req_(req),
      is_finish_(false), cgi_resp_(CGIResponse()), resp_(HTTPResponse()),
      cgi_parser_(cgi_resp_) {}

ReadCGI::~ReadCGI() {}

void ReadCGI::Run() {
    char buf[BUF_SIZE];
    int  fd_from_cgi = polled_fd_;

    int read_size = read(fd_from_cgi, buf, BUF_SIZE);
    if (read_size < 0) {
        throw std::make_pair(stream_, status::server_error);
    }

    try {
        cgi_parser_(std::string(buf, read_size), read_size);
    } catch (status::code code) { throw std::make_pair(stream_, code); }
}
void ReadCGI::Register() { EventRegister::Instance().AddReadEvent(this); }

void ReadCGI::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* ReadCGI::RegisterNext() {
    if (cgi_parser_.GetPhase() != CGIResponseParser::DONE) {
        return this;
    }

    // cgi_resp_.LogInfo();
    cgi_resp_.GenerateHTTPResponse(resp_);

    resp_.SetVersion(req_.GetVersion());

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    this->Unregister();
    send_response->Register();
    return send_response;
}

void ReadCGI::Close() {
    if (polled_fd_ != -1) {
        close(polled_fd_);
        polled_fd_ = -1;
    }
}
