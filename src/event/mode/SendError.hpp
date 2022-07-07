#ifndef SEND_ERROR_HPP
#define SEND_ERROR_HPP
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class SendError : public IOEvent {
public:
    SendError(StreamSocket& stream, status::code);
    virtual ~SendError();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

private:
    bool     existErrorPage(const std::string& error_page);
    IOEvent* sendResponse();
    IOEvent* readFile(int fd);

    StreamSocket& stream_;
    status::code  status_code_;
};
#endif // SEND_ERROR_HPP
