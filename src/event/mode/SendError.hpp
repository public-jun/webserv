#ifndef SEND_ERROR_HPP
#define SEND_ERROR_HPP
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class SendError : public IOEvent {
public:
    SendError(StreamSocket& stream, HTTPResponse& resp);
    virtual ~SendError();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

private:
    StreamSocket  stream_;
    HTTPResponse& resp_;
};
#endif // SEND_ERROR_HPP
