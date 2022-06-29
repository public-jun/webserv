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
#include "RecvRequest.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

const std::size_t ReadCGI::BUF_SIZE = 2048;

ReadCGI::ReadCGI(int fd_read_from_cgi, StreamSocket stream, HTTPRequest req)
    : IOEvent(fd_read_from_cgi, READ_CGI), stream_(stream), req_(req),
      is_finish_(false), cgi_resp_(CGIResponse()), resp_(HTTPResponse()),
      cgi_parser_(cgi_resp_) {}

ReadCGI::~ReadCGI() {
    int fd = polled_fd_;
    if (fd != -1) {
        close(fd);
        polled_fd_ = -1;
    }
}

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

    IOEvent* new_event = NULL;
    // cgi_resp_.PrintInfo();

    if (cgi_resp_.GetResponseType() == CGIResponse::LOCAL_REDIR_RES) {
        req_.SetMethod("GET");
        req_.SetRequestTarget(cgi_resp_.GetHeaderValue("location"));
        req_.SetBody("");
        // req_.PrintInfo();
        new_event = RecvRequest::PrepareResponse(req_, stream_);
    } else {
        cgi_resp_.GenerateHTTPResponse(resp_);

        resp_.SetVersion(req_.GetVersion());
        resp_.AppendHeader("Server", "Webserv/1.0.0");

        new_event = new SendResponse(stream_, resp_.ConvertToStr());
        new_event->Register();
    }

    this->Unregister();
    return new_event;
}
