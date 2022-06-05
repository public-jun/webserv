#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include "Socket.hpp"

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"

class HTTPParser;
class HTTPRequest;
class HTTPResponse;
class StreamSocket : public Socket {
public:
    enum EventType {
        RECV_REQUEST,
        SEND_RESPONSE,
        WRITE_CGI,
        READ_CGI,
    };

    StreamSocket();
    virtual ~StreamSocket();

    HTTPRequest*     GetHTTPRequest();
    const EventType& GetEventType() const;

    // Add event
    void Recv();
    void Send();
    void Close();

    // Do event
    void OnRecv();
    void OnSend();
    void OnDisConnect();

private:
    void parseRequest();
    void setEventType(EventType type);

private:
    HTTPRequest  req_;
    HTTPParser   parser_;
    HTTPResponse resp_;

    EventType event_type_;
    int       read_size_;
    ssize_t   buf_size_;
    char      buf_[2048];
};

#endif
