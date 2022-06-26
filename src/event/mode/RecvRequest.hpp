#ifndef RECV_REQUEST_HPP
#define RECV_REQUEST_HPP

#include <string>

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"

class RecvRequest : public IOEvent {
public:
    RecvRequest(StreamSocket stream);
    virtual ~RecvRequest();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

    static const std::size_t BUF_SIZE;

private:
    const ServerConfig searchServerConfig();

private:
    RecvRequest();
    IOEvent* prepareResponse();

    bool isDigit(std::string str);

    // RecvRequestに必要な入力
    StreamSocket stream_;

    // RecvRequestによってできる出力
    HTTPRequest req_;

    // RecvRequestに必要な入力
    HTTPParser::State state_;
};

#endif
