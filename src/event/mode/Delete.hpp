#ifndef DELETE_HPP
#define DELETE_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include <sys/stat.h>
#include <unistd.h>

class Delete : public IOEvent {
public:
    Delete(StreamSocket stream, HTTPRequest req);
    virtual ~Delete();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

private:
    Delete();
    // Delete に必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;

    // Delete によってできる出力
    HTTPResponse resp_;
};

#endif
