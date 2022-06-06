#include "EventAction.hpp"

#include <cerrno>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "IOEvent.hpp"
#include "SysError.hpp"

#include <iostream>

EventAction::EventAction() : kqueue_fd_(-1) { active_list_.resize(1024); }

EventAction::~EventAction() {}

void EventAction::Init() {
    kqueue_fd_ = kqueue();
    if (kqueue_fd_ < 0) {
        throw SysError("kqueue", errno);
    }
}

void EventAction::ShutDown() {
    if (kqueue_fd_ == -1) {
        return;
    }

    if (close(kqueue_fd_) < 0) {
        throw SysError("close", errno);
    }
    kqueue_fd_ = -1;
}

bool EventAction::AddAcceptEvent(IOEvent* event) {
    if (event == NULL) {
        return false;
    }

    if (event->GetIOEventMode() == IOEvent::ACCEPT_CONNECTION) {
        struct kevent ev;
        EV_SET(&ev, event->GetPolledFd(), EVFILT_READ,
               EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, (void*)event);
        change_list_.push_back(ev);
        return true;
    }
    return false;
}

bool EventAction::AddReadEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           (void*)event);
    change_list_.push_back(ev);
    return true;
}

bool EventAction::AddWriteEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
           (void*)event);
    change_list_.push_back(ev);
    return true;
}

bool EventAction::DelReadEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    change_list_.push_back(ev);
    return true;
}

bool EventAction::DelWriteEvent(IOEvent* event) {
    if (!event) {
        return false;
    }

    struct kevent ev;
    EV_SET(&ev, event->GetPolledFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    change_list_.push_back(ev);
    return true;
}

void EventAction::ProcessEvent() {
    int event_size = 0;

    event_size =
        kevent(kqueue_fd_, change_list_.data(), (int)change_list_.size(),
               active_list_.data(), active_list_.size(), NULL);

    change_list_.clear();
    if (event_size < 0) {
        throw SysError("kevent", errno);
    }

    onEvent(active_list_, event_size);
}

void EventAction::onEvent(std::vector<struct kevent> event_vec,
                          int                        event_size) {
    for (int i = 0; i < event_size; i++) {
        IOEvent* event = reinterpret_cast<IOEvent*>(event_vec[i].udata);
        if (!event) {
            continue;
        }

        try {
            // イベント実行
            doEvent(event);
            // 次のイベントを決定する
            registerEvent(event);
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; }
    }
}

void EventAction::doEvent(IOEvent* event) { event->Run(); }

void EventAction::registerEvent(IOEvent* event) {
    IOEvent* next_event = event->RegisterNext();
    if (event->GetIOEventMode() != IOEvent::ACCEPT_CONNECTION &&
        next_event != event) {
        delete event;
    }
}
