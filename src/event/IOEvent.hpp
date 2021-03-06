#ifndef IOEVENT_HPP
#define IOEVENT_HPP

#include <sys/event.h>

#define UNUSED(x) ((void)(x))
class IOEvent {
public:
    enum IOEventMode {
        ACCEPT_CONNECTION,
        RECV_REQUEST,
        SEND_RESPONSE,
        READ_FILE,
        WRITE_FILE,
        DELETE,
        READ_CGI,
        WRITE_CGI,
    };

    IOEvent() : polled_fd_(-1), mode_(ACCEPT_CONNECTION) {}
    IOEvent(int fd) : polled_fd_(fd), mode_(ACCEPT_CONNECTION) {}
    IOEvent(IOEventMode mode) : polled_fd_(-1), mode_(mode) {}
    IOEvent(int fd, IOEventMode mode) : polled_fd_(fd), mode_(mode) {}
    virtual ~IOEvent() {}

    virtual void     Run(intptr_t offset) = 0;
    virtual void     Register()           = 0;
    virtual void     Unregister()         = 0;
    virtual IOEvent* RegisterNext()       = 0;
    virtual int      Close()              = 0;

    void SetPolledFd(int fd) { polled_fd_ = fd; }
    int  GetPolledFd() { return polled_fd_; }

    void               SetEventMode(IOEventMode mode) { mode_ = mode; }
    const IOEventMode& GetIOEventMode() const { return mode_; }

protected:
    int         polled_fd_;
    IOEventMode mode_;
    int         status_;
};

#endif
