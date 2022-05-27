#include <iostream>
#include <map>

#include "EventActions.hpp"
#include "ListeningSocket.hpp"
#include "Socket.hpp"
#include "StreamSocket.hpp"

int main(void) {
    try {
        // Event Action set up
        EventActions event_actions;
        event_actions.Init();

        // Listening Socket set up
        ListeningSocket ls1, ls2;
        ls1.SetEventActions(&event_actions);
        ls1.Bind("127.0.0.1", 5000);
        ls1.Listen();

        ls2.SetEventActions(&event_actions);
        ls2.Bind("127.0.0.1", 5001);
        ls2.Listen();

        // Event loop
        while (true) {
            event_actions.ProcessEvent();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
