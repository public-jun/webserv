#ifndef POST_HPP
#define POST_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include <sys/stat.h>
#include <unistd.h>

class Post : public IOEvent {
public:
    Post(StreamSocket stream, HTTPRequest req);
    virtual ~Post();

    virtual void     Run();
    virtual IOEvent* RegisterNext();

private:
    Post();
    // Post に必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;

    // Post によってできる出力
    HTTPResponse resp_;
};

#endif
