#ifndef ACCEPT_CONNECTION_HPP
#define ACCEPT_CONNECTION_HPP

#include "IOEvent.hpp"
#include "ListeningSocket.hpp"
#include "StreamSocket.hpp"

class AcceptConn : public IOEvent {
public:
    AcceptConn(ListeningSocket listener);
    virtual ~AcceptConn();

    ListeningSocket& GetListeningSocket() { return listener_; }

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

private:
    AcceptConn();
    void printLog();

    // Accept に必要な入力
    ListeningSocket listener_;

    // Accept でできる出力
    StreamSocket stream_sock_;
};

#endif
