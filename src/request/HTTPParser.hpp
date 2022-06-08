#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP
#include "HTTPRequest.hpp"
#include "mode/SendResponse.hpp"
#include <string>

// TODO: parser実装、例外クラス実装、GETメソッド実装

namespace Parser {
enum Phase { FIRST_LINE, HEADER_LINE, DONE };

class SendError : public IOEvent {
public:
    SendError(StreamSocket stream);
    virtual ~SendError();

    virtual void     Run();
    virtual IOEvent* RegisterNext();

private:
    StreamSocket stream_;
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
};

void parse(State& state, const std::string buf);

} // namespace Parser

#endif // HTTPPARSER_HPP
