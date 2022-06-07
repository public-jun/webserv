#include "EventAction.hpp"
#include "ListeningSocket.hpp"

#include "AcceptConn.hpp"
#include "IOEvent.hpp"
#include <unistd.h>

#include <iostream>

int main(void) {
    try {
        EventAction::GetInstance().Init();

        ListeningSocket ls;
        ls.Bind("127.0.0.1", 5000);
        ls.Listen();
        // イベントの追加

        IOEvent* event = new AcceptConn(ls);
        EventAction::GetInstance().AddAcceptEvent(event);

        while (true) {
            EventAction::GetInstance().ProcessEvent();
        }

        ls.Close();
        // 全てのsocketをclose
        EventAction::GetInstance().ShutDown();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}