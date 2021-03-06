#ifndef RECV_REQUEST_HPP
#define RECV_REQUEST_HPP

#include <string>

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include <arpa/inet.h>
#include <netdb.h>

class RecvRequest : public IOEvent {
public:
    RecvRequest(StreamSocket stream);
    virtual ~RecvRequest();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

    static IOEvent*           PrepareResponse(const HTTPRequest&  req,
                                              const StreamSocket& stream);
    static const ServerConfig SearchServerConfig(const HTTPRequest&  req,
                                                 const StreamSocket& stream);
    static const std::size_t  BUF_SIZE;

private:
    static std::string getAddrByHostName(std::string host_name);

private:
    RecvRequest();
    void printLog();

    // RecvRequestに必要な入力
    StreamSocket stream_;

    // RecvRequestによってできる出力
    HTTPRequest req_;

    // RecvRequestに必要な入力
    HTTPParser::State state_;
};

#endif
