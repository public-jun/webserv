#ifndef RECV_REQUEST_HPP
#define RECV_REQUEST_HPP

#include <string>

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class RecvRequest : public IOEvent {
public:
    RecvRequest(StreamSocket stream);
    virtual ~RecvRequest();

    virtual void     Run();
    virtual IOEvent* RegisterNext();

    static const int buf_size;

private:
    RecvRequest();
    IOEvent* prepareResponse();
    // RecvRequestに必要な入力
    StreamSocket stream_;

    // RecvRequestによってできる出力
    HTTPRequest req_;

    // RecvRequestに必要な入力
    Parser::State state_;
};

#endif