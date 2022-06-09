#ifndef READ_CGI_HPP
#define READ_CGI_HPP

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
    virtual IOEvent* RegisterNext();

private:
    ReadCGI();

private:
    // ReadCGIに必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;
    bool is_finish_;

    // ReadCGIによってできる出力
    HTTPResponse resp_;
    std::string  cgi_output_;
};

#endif