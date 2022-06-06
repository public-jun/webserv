#ifndef EVENT_ACTION_HPP
#define EVENT_ACTION_HPP

#include <sys/event.h>
#include <vector>

#include "IOEvent.hpp"

class EventAction {
public:
    static EventAction& GetInstance() {
        static EventAction instance;
        return instance;
    }

    void Init();
    void ShutDown();

    // Add I/O event
    bool AddAcceptEvent(IOEvent* event);
    bool AddReadEvent(IOEvent* event);
    bool AddWriteEvent(IOEvent* event);

    // Del I/O event
    bool DelReadEvent(IOEvent* event);
    bool DelWriteEvent(IOEvent* event);

    void ProcessEvent();

private:
    // Singleton pattern
    EventAction();
    EventAction(const EventAction&);
    EventAction& operator=(const EventAction&);
    ~EventAction();

    void onEvent(std::vector<struct kevent> event_vec, int event_size);
    void doEvent(IOEvent* event);
    void registerEvent(IOEvent* event);

private:
    int                        kqueue_fd_;
    std::vector<struct kevent> change_list_;
    std::vector<struct kevent> active_list_;
};

#endif
