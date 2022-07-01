#include "WriteCGI.hpp"

#include <cerrno>
#include <string>
#include <unistd.h>

#include "CGI.hpp"
#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "ReadCGI.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

#include <iostream>

WriteCGI::WriteCGI(class CGI cgi, StreamSocket stream, HTTPRequest req)
    : IOEvent(cgi.FdForWriteToCGI(), WRITE_CGI), cgi_(cgi), stream_(stream),
      req_(req) {}

WriteCGI::~WriteCGI() {
    int fd = polled_fd_;
    if (fd != -1) {
        close(fd);
        polled_fd_ = -1;
    }
}

void WriteCGI::Run(intptr_t offset) {
    UNUSED(offset);
    int fd_write_to_cgi = polled_fd_;
    int write_size =
        write(fd_write_to_cgi, req_.GetBody().c_str(), req_.GetBody().size());
    if (write_size < 0 ||
        static_cast<size_t>(write_size) != req_.GetBody().size()) {
        throw SysError("write", errno);
    }
}

void WriteCGI::Register() { EventRegister::Instance().AddWriteEvent(this); }

void WriteCGI::Unregister() { EventRegister::Instance().DelWriteEvent(this); }

IOEvent* WriteCGI::RegisterNext() {
    Unregister();

    IOEvent* read_cgi = new ReadCGI(cgi_.FdForReadFromCGI(), stream_, req_);
    read_cgi->Register();
    return read_cgi;
}
