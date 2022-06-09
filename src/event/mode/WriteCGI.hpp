#ifndef WRITE_CGI_HPP
#define WRITE_CGI_HPP

#include "CGI.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class WriteCGI : public IOEvent {
public:
    WriteCGI(CGI cgi, StreamSocket stream, HTTPRequest req);
    virtual ~WriteCGI();

    virtual void     Run();
    virtual IOEvent* RegisterNext();

    virtual void Unregister();

private:
    WriteCGI();

private:
    CGI          cgi_;
    StreamSocket stream_;
    HTTPRequest  req_;
};

#endif
