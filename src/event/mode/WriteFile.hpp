#ifndef WRITE_HPP
#define WRITE_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

class WriteFile : public IOEvent {
public:
    WriteFile(StreamSocket stream, int fd, HTTPRequest req);
    virtual ~WriteFile();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

private:
    WriteFile();

private:
    StreamSocket stream_;
    HTTPRequest  req_;
    HTTPResponse resp_;
};

#endif
