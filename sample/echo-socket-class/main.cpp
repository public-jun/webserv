
#include <map>
#include <vector>

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

#include "ListeningSocket.hpp"
#include "Socket.hpp"
#include "StreamSocket.hpp"

// std::map<int, Socket*> Socket::all_socket_map_;
std::map<int, ListeningSocket*> ListeningSocket::all_listener_;

int main(void) {
    // Event Action set up
    std::vector<int>           all_listener;
    std::vector<struct kevent> change_list;

    // Listenign Socket set up
    ListeningSocket ls1;
    ls1.Bind("127.0.0.1", 5000);
    ls1.Listen();

    struct kevent ev;
    EV_SET(&ev, ls1.GetSocketFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
           (void*)&ls1);
    change_list.push_back(ev);

    int kq = kqueue();
    if (kq < 0) {
        std::cerr << "KQUEUE ERR" << std::endl;
        ls1.Close();
    }
    std::vector<struct kevent> active_list(1024);
    std::vector<StreamSocket*> all_stream;

    // Event loop
    while (true) {
        int event_size = 0;

        event_size =
            kevent(kq, &*change_list.begin(), (int)change_list.size(),
                   &*active_list.begin(), (int)active_list.size(), NULL);
        change_list.clear();
        if (event_size < 0) {
            ls1.Close();
            close(kq);
            for (auto& e : all_stream) {
                // close(e);
                delete e;
            }
        }

        struct kevent event;
        struct kevent del;
        std::cout << "\nQUE_NUM: " << event_size << std::endl;
        for (int i = 0; i < event_size; i++) {
            std::cout << "sockfd: " << active_list[i].ident << std::endl;
            Socket* sock = reinterpret_cast<Socket*>(active_list[i].udata);
            std::cout << "sock type: " << sock->GetSocketType() << std::endl;
            if (sock->GetSocketType() == Socket::LISTENING) {
                std::cout << "Accept NEW SOCKET!" << std::endl;
                struct sockaddr_in peer_sin;
                int                len = sizeof(peer_sin);

                int new_socket =
                    accept(sock->GetSocketFd(), (struct sockaddr*)&peer_sin,
                           (socklen_t*)&len);
                StreamSocket* s_sock = new StreamSocket();
                s_sock->SetSocketFd(new_socket);
                s_sock->SetAddress(peer_sin);
                all_stream.push_back(s_sock);
                EV_SET(&event, s_sock->GetSocketFd(), EVFILT_READ,
                       EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, (void*)s_sock);
                change_list.push_back(event);
            } else if (sock->GetSocketType() == Socket::STREAM &&
                       active_list[i].filter == EVFILT_READ) {
                std::cout << "READ EVENT" << std::endl;
                StreamSocket* s_sock = dynamic_cast<StreamSocket*>(sock);
                s_sock->GetReadSize() =
                    recv(active_list[i].ident, s_sock->GetBuf(),
                         s_sock->GetBufSize(), 0);

                if (s_sock->GetReadSize() < 0) {
                    std::cerr << "RECV ERR" << std::endl;
                    ls1.Close();
                    close(kq);
                    for (auto& e : all_stream) {
                        delete e;
                    }
                }
                s_sock->GetBuf()[s_sock->GetReadSize()] = '\0';
                EV_SET(&del, active_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0,
                       (void*)s_sock);
                change_list.push_back(del);

                EV_SET(&event, active_list[i].ident, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, (void*)s_sock);
                change_list.push_back(event);
            } else if (sock->GetSocketType() == Socket::STREAM &&
                       active_list[i].filter == EVFILT_WRITE) {
                std::cout << "WRITE EVENT" << std::endl;
                StreamSocket* s_sock = dynamic_cast<StreamSocket*>(sock);

                int res = send(active_list[i].ident, s_sock->GetBuf(),
                               s_sock->GetReadSize(), 0);
                if (res < 0) {
                    std::cerr << "SEND ERR" << std::endl;
                    ls1.Close();
                    close(kq);
                    for (auto& e : all_stream) {
                        delete e;
                    }
                }
                EV_SET(&del, active_list[i].ident, EVFILT_WRITE, EV_DELETE, 0,
                       0, (void*)s_sock);
                change_list.push_back(del);

                EV_SET(&event, active_list[i].ident, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, (void*)s_sock);
                change_list.push_back(event);
            }
        }
    }
    return 0;
}