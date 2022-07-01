#ifndef SEND_RESPONSE_HPP
#define SEND_RESPONSE_HPP

#include <string>

#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class SendResponse : public IOEvent {
public:
    SendResponse(StreamSocket stream, std::string buf);
    virtual ~SendResponse();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

private:
    SendResponse();
    void printLog();

    // SendResponseに必要な入力
    StreamSocket stream_;
    std::string  all_buf_;

    // SendResponseによってできる出力
};

#endif
