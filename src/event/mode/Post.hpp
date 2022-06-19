#ifndef POST_HPP
#define POST_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

class Post : public IOEvent {
public:
    Post(StreamSocket stream, HTTPRequest req);
    Post();
    virtual ~Post();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    void             openFile();

private:
    // Post に必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;
    std::string  generateFileName();

    // Post によってできる出力
    HTTPResponse resp_;
    std::string  content_;
};

#endif
