#ifndef POST_HPP
#define POST_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"
#include "URI.hpp"
#include "WriteFile.hpp"
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

class Post {
public:
    Post(StreamSocket stream, HTTPRequest req, URI& uri);
    virtual ~Post();

    virtual void     Run();
    virtual IOEvent* RegisterNext();

private:
    StreamSocket stream_;
    URI&         uri_;
    HTTPRequest  req_;
    IOEvent*     write_event_;
    bool         is_cgi_;
    int          fd_read_from_cgi_;
    int          write_fd_;
    std::string  generateFileName();
};

#endif
