#ifndef EVENT_REGISTER_HPP
#define EVENT_REGISTER_HPP

#include <sys/event.h>
#include <vector>

#include "IOEvent.hpp"

class EventRegister {
public:
    static EventRegister& Instance() {
        static EventRegister instance;
        return instance;
    }

    std::vector<struct kevent>& RegisteredEvents();

    // Add I/O event
    bool AddReadEvent(IOEvent* event);
    bool AddWriteEvent(IOEvent* event);

    // Del I/O event
    bool DelReadEvent(IOEvent* event);
    bool DelWriteEvent(IOEvent* event);

private:
    EventRegister();
    ~EventRegister();
    EventRegister& operator=(const EventRegister&);

private:
    std::vector<struct kevent> registered_events_;
};

#endif
