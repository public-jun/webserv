#include "EventRegister.hpp"

#include <sys/event.h>
#include <vector>

#include "IOEvent.hpp"

EventRegister::EventRegister() {}

EventRegister::~EventRegister() {}

std::vector<struct kevent>& EventRegister::RegisteredEvents() {
    return registered_events_;
}

bool EventRegister::AddReadEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           (void*)event);
    registered_events_.push_back(ev);
    return true;
}

bool EventRegister::AddWriteEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           (void*)event);
    registered_events_.push_back(ev);
    return true;
}

bool EventRegister::DelReadEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    registered_events_.push_back(ev);
    return true;
}

bool EventRegister::DelWriteEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    registered_events_.push_back(ev);
    return true;
}
