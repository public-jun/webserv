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
    virtual void     Close();

    static IOEvent*          PrepareResponse(const HTTPRequest&  req,
                                             const StreamSocket& stream);
    static const ServerConfig SearchServerConfig(const HTTPRequest&  req,
                                          const StreamSocket& stream);
    static const std::size_t BUF_SIZE;

private:

private:
    RecvRequest();

    // RecvRequestに必要な入力
    StreamSocket stream_;

    // RecvRequestによってできる出力
    HTTPRequest req_;

    // RecvRequestに必要な入力
    HTTPParser::State state_;
};

#endif
