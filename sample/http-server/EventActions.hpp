#ifndef EVENT_ACTIONS_HPP
#define EVENT_ACTIONS_HPP

#include <sys/event.h>
#include <vector>

#include "Socket.hpp"

class Socket;
class EventActions
{
public:
    EventActions();
    ~EventActions();

    bool Init();

    // Add event
    bool AddAcceptEvent(Socket* sock);
    bool AddRecvEvent(Socket* sock);
    bool AddSendEvent(Socket* sock);

    // Del event
    bool DelEvent(Socket* sock);
    bool DelRecv(Socket* sock);
    bool DelSend(Socket* sock);

    void ProcessEvent();

private:
    void onEvent(std::vector<struct kevent> active_list, int event_size);

    int                        kqueue_fd_;
    std::vector<struct kevent> change_list_;
    std::vector<struct kevent> active_list_;
};

#endif
