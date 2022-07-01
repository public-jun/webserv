#ifndef READ_CGI_HPP
#define READ_CGI_HPP

#include "CGIResponse.hpp"
#include "CGIResponseParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

#include <string>

class ReadCGI : public IOEvent {
public:
    ReadCGI(int fd_read_from_cgi, StreamSocket stream, HTTPRequest req);
    virtual ~ReadCGI();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

    static const std::size_t BUF_SIZE;

private:
    ReadCGI();

private:
    // ReadCGIに必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;
    bool         is_finish_;
    CGIResponse  cgi_resp_;

    // ReadCGIによってできる出力
    HTTPResponse resp_;
    // ReadCGIに必要な入力
    CGIResponseParser cgi_parser_;
    // ReadCGIによってできる出力
    std::string cgi_output_;
};

#endif
