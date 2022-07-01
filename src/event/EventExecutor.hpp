#ifndef EVENT_EXECUTOR_HPP
#define EVENT_EXECUTOR_HPP

#include <sys/event.h>
#include <vector>

#include "IOEvent.hpp"

class EventExecutor {
public:
    static EventExecutor& Instance() {
        static EventExecutor instance;
        return instance;
    }

    void Init();
    void ShutDown();

    void ProcessEvent();

private:
    EventExecutor();
    ~EventExecutor();
    void onEvent(std::vector<struct kevent> event_vec, int event_size);
    void doEvent(IOEvent* event, intptr_t offset);
    void nextEvent(IOEvent* event);
    void errorNextEvent(IOEvent* prev, IOEvent* next);

private:
    int                         kqueue_fd_;
    std::vector<struct kevent>& registered_list_;
    std::vector<struct kevent>  active_list_;
};

#endif
