#ifndef WRITE_CGI_HPP
#define WRITE_CGI_HPP

#include "CGI.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class WriteCGI : public IOEvent {
public:
    WriteCGI(int fd_for_write_to_cgi, int fd_for_read_form_cgi,
             StreamSocket stream, HTTPRequest req);
    virtual ~WriteCGI();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

private:
    WriteCGI();

private:
    int          fd_for_write_to_cgi_;
    int          fd_for_read_form_cgi_;
    StreamSocket stream_;
    HTTPRequest  req_;
};

#endif
