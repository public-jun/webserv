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
    StreamSocket();
    virtual ~StreamSocket();

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

private:
    HTTPParser*   parser_;
    HTTPRequest*  req_;
    HTTPResponse* res_;

    int     read_size_;
    ssize_t buf_size_;
    char    buf_[2048];
};

#endif
