#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include "mode/SendResponse.hpp"
#include <string>
#include <sys/event.h>

namespace HTTPParser {
enum Phase { FIRST_LINE, HEADER_LINE, BODY, DONE };

class SendError {
public:
    void SetStream(StreamSocket stream);

protected:
    StreamSocket stream_;
};

class SendBadrequest : public SendError, public IOEvent {
public:
    SendBadrequest();
    SendBadrequest(StreamSocket stream);
    virtual ~SendBadrequest();

    virtual void     Run(intptr_t offset);
    virtual IOEvent* RegisterNext();
};

class State {
public:
    State(HTTPRequest& req);

    std::string& Buf();
    std::string& BodyBuf();
    Phase&       Phase();
    HTTPRequest& Request();

private:
    std::string  buf_;
    std::string  body_buf_;
    enum Phase   phase_;
    HTTPRequest& req_;
};

void update_state(State& state, const std::string buf);

void validate_request(const URI& uri, const HTTPRequest& req);

} // namespace HTTPParser

#endif // HTTPPARSER_HPP
