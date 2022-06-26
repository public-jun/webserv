#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include "StreamSocket.hpp"
#include "mode/SendResponse.hpp"
#include <string>

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

    virtual void     Run();
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

} // namespace HTTPParser

#endif // HTTPPARSER_HPP
