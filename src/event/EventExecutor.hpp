#ifndef EVENT_EXECUTOR_HPP
#define EVENT_EXECUTOR_HPP

#include <sys/event.h>
#include <vector>

#include "IOEvent.hpp"

class EventExecutor {
public:
    EventExecutor();
    ~EventExecutor();

    void Init();
    void ShutDown();

    void ProcessEvent();

private:
    void onEvent(std::vector<struct kevent> event_vec, int event_size);
    void doEvent(IOEvent* event);
    void nextEvent(IOEvent* event);

private:
    int                        kqueue_fd_;
    std::vector<struct kevent>& registered_list_;
    std::vector<struct kevent> active_list_;
};

#endif