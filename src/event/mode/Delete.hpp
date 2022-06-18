#ifndef DELETE_HPP
#define DELETE_HPP

#include "EventRegister.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "SendResponse.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include <sys/stat.h>
#include <unistd.h>

class Delete : public IOEvent {
public:
    Delete(StreamSocket stream, URI& uri);
    virtual ~Delete();

    virtual void     Run();
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();

private:
    Delete();
    StreamSocket stream_;
    URI&         uri_;
    HTTPResponse resp_;
};

#endif
