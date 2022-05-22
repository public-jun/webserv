#include <map>

#include "EventActions.hpp"
#include "ListeningSocket.hpp"
#include "Socket.hpp"
#include "StreamSocket.hpp"

std::map<int, ListeningSocket*> ListeningSocket::all_listener_;

int main(void) {
    // Event Action set up
    EventActions event_actions;
    event_actions.Init();

    // Listenign Socket set up
    ListeningSocket ls1;
    ls1.SetEventActions(&event_actions);
    ls1.Bind("127.0.0.1", 5000);
    ls1.Listen();

    ListeningSocket ls2;
    ls2.SetEventActions(&event_actions);
    ls2.Bind("127.0.0.1", 5001);
    ls2.Listen();

    // Event loop
    while (true) {
        event_actions.ProcessEvent();
    }

    return 0;
}