#ifndef WRITE_CGI_HPP
#define WRITE_CGI_HPP

#include "CGI.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class WriteCGI : public IOEvent {
public:
    WriteCGI(class CGI cgi, StreamSocket stream, HTTPRequest req);
    virtual ~WriteCGI();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

private:
    WriteCGI();

private:
    class CGI    cgi_;
    StreamSocket stream_;
    HTTPRequest  req_;
};

#endif
