#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include "StreamSocket.hpp"
#include "mode/SendResponse.hpp"
#include <string>

// TODO: 例外クラス実装、GETメソッド実装

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
    Phase&       Phase();
    HTTPRequest& Request();

private:
    std::string  buf_;
    enum Phase   phase_;
    HTTPRequest& req_;
    /* std::size_t  size_; //初期化する */
};

void update_state(State& state, const std::string buf);

} // namespace HTTPParser

#endif // HTTPPARSER_HPP
