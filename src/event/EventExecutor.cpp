#include "EventExecutor.hpp"

#include <iostream>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "EventRegister.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "SendError.hpp"
#include "StreamSocket.hpp"
#include "SysError.hpp"

EventExecutor::EventExecutor()
    : kqueue_fd_(-1),
      registered_list_(EventRegister::Instance().RegisteredEvents()) {
    active_list_.resize(1024);
}

EventExecutor::~EventExecutor() {}

void EventExecutor::Init() {
    kqueue_fd_ = kqueue();
    if (kqueue_fd_ < 0) {
        throw SysError("kqueue", errno);
    }
}

void EventExecutor::ShutDown() {
    if (kqueue_fd_ == -1) {
        return;
    }

    if (close(kqueue_fd_) < 0) {
        throw SysError("close", errno);
    }
    kqueue_fd_ = -1;
}

void EventExecutor::ProcessEvent() {
    int event_size = 0;

    event_size = kevent(kqueue_fd_, registered_list_.data(),
                        (int)registered_list_.size(), active_list_.data(),
                        active_list_.size(), NULL);

    registered_list_.clear();
    if (event_size < 0) {
        throw SysError("kevent", errno);
    }

    onEvent(active_list_, event_size);
}

void EventExecutor::onEvent(std::vector<struct kevent> event_vec,
                            int                        event_size) {
    for (int i = 0; i < event_size; i++) {
        IOEvent* event = reinterpret_cast<IOEvent*>(event_vec[i].udata);
        if (!event) {
            continue;
        }

        IOEvent* next_event;
        try {
            // イベント実行
            doEvent(event);
            // 次のイベントを決定する
            next_event = event;
        } catch (std::pair<StreamSocket, status::code>& err) {
            next_event = new SendError(err.first, err.second);
            event->Unregister();
            delete event;
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; }
        nextEvent(next_event);
    }
}

void EventExecutor::doEvent(IOEvent* event) { event->Run(); }

void EventExecutor::nextEvent(IOEvent* event) {
    IOEvent* next_event = event->RegisterNext();
    if (event->GetIOEventMode() != IOEvent::ACCEPT_CONNECTION &&
        next_event != event) {
        delete event;
    }
}
