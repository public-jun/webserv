#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <bitset>
#include <iostream>
#include <vector>
class Buffer
{
public:
    Buffer() : read_size_(0), buf_size_(2048) {}
    ~Buffer() {}
    int&    GetReadSize() { return read_size_; }
    ssize_t GetBufSize() { return buf_size_; }
    char*   GetBuf() { return buf_; }

private:
    int     read_size_;
    ssize_t buf_size_;
    char    buf_[2048];
};

void debugBuff(Buffer* buffer) {
    std::cout << "Buff INFO: " << buffer << "\n"
              << "read_size: " << buffer->GetReadSize() << "\n"
              << "buf      : " << buffer->GetBuf() << std::endl;
}

void debugFilter(int filter, int event_fil) {
    std::cout << "FILTER" << std::bitset<16>(filter) << "\n"
              << "EVENT " << std::bitset<16>(static_cast<unsigned>(event_fil))
              << "\n"
              << "&     "
              << std::bitset<16>(static_cast<unsigned>(filter & event_fil))
              << std::endl;
}

int setUpListener(const std::string& ip, const int port,
                  std::vector<int>*           all_listening_socket,
                  std::vector<struct kevent>* change_list) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "SOCKET ERR" << std::endl;
    }
    all_listening_socket->push_back(sock_fd);
    int sock_optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &sock_optval,
               sizeof(sock_optval));

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    int ret = bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        std::cerr << "BIND ERR" << std::endl;
        close(sock_fd);
    }
    ret = listen(sock_fd, SOMAXCONN);
    if (ret < 0) {
        std::cerr << "LISTEN ERR" << std::endl;
        close(sock_fd);
    }
    ret = fcntl(sock_fd, F_SETFL, O_NONBLOCK);
    if (ret < 0) {
        std::cerr << "FCNTL ERR" << std::endl;
        close(sock_fd);
    }
    struct kevent ev;
    EV_SET(&ev, sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    (*change_list).push_back(ev);

    return 0;
}

void closeAllListener(std::vector<int>* all_listening_socket) {
    std::vector<int>::iterator it;
    it                                   = all_listening_socket->begin();
    const std::vector<int>::iterator end = all_listening_socket->end();
    for (; it != end; it++) {
        close(*it);
    }
}

int main(void) {
    const std::string          ip    = "127.0.0.1";
    const int                  port1 = 5000;
    const int                  port2 = 5001;
    int                        ret   = 0;
    std::vector<int>           all_listening_socket;
    std::vector<struct kevent> change_list;

    // Listening socket set up
    ret = setUpListener(ip, port1, &all_listening_socket, &change_list);
    ret = setUpListener(ip, port2, &all_listening_socket, &change_list);

    // Event set up
    std::vector<int> stream_socket_list;
    int              kq = kqueue();
    if (kq < 0) {
        std::cerr << "KQUEUE ERROR" << std::endl;
    }
    std::vector<struct kevent> active_list(1024);

    while (true) {
        int ret = 0;

        // イベント登録 && イベント通知待機
        ret = kevent(kq, &*change_list.begin(), (int)change_list.size(),
                     &*active_list.begin(), (int)active_list.size(), NULL);
        change_list.clear();
        if (ret < 0) {
            closeAllListener(&all_listening_socket);
            close(kq);
            for (auto& e : stream_socket_list) {
                close(e);
            }
        }

        // struct kevent {
        //     uintptr_t ident;  /* identifier for this event */
        //     int16_t   filter; /* filter for event */
        //     uint16_t  flags;  /* general flags */
        //     uint32_t  fflags; /* filter-specific flags */
        //     intptr_t  data;   /* filter-specific data */
        //     void*     udata;  /* opaque user data identifier */
        // };
        struct kevent event;
        struct kevent del;
        std::cout << "\nnum: " << ret << std::endl;
        for (int i = 0; i < ret; i++) {
            std::cout << "sockfd: " << active_list[i].ident << std::endl;
            std::vector<int>::iterator sock_it =
                std::find(all_listening_socket.begin(),
                          all_listening_socket.end(), active_list[i].ident);
            if (sock_it != all_listening_socket.end()) {
                std::cout << "Make NEW SOCKET!" << std::endl;
                // Make new Stream Socket
                struct sockaddr_in peer_sin;
                int                len = sizeof(peer_sin);
                int new_sock = accept(*sock_it, (struct sockaddr*)&peer_sin,
                                      (socklen_t*)&len);
                if (new_sock < 0) {
                    closeAllListener(&all_listening_socket);
                    close(kq);
                    for (auto& e : stream_socket_list) {
                        close(e);
                    }
                }
                stream_socket_list.push_back(new_sock);
                EV_SET(&event, new_sock, EVFILT_READ,
                       EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
                change_list.push_back(event);
            } else if (active_list[i].filter == EVFILT_READ) {
                std::cout << "READ EVENT" << std::endl;

                Buffer* buffer = new Buffer();
                // 読み込み
                buffer->GetReadSize() =
                    recv(active_list[i].ident, buffer->GetBuf(),
                         buffer->GetBufSize(), 0);

                if (buffer->GetReadSize() < 0) {
                    std::cerr << "RECV ERR" << std::endl;
                    closeAllListener(&all_listening_socket);
                    close(kq);
                    for (auto& e : stream_socket_list) {
                        close(e);
                    }
                    return 1;
                }
                buffer->GetBuf()[buffer->GetReadSize()] = '\0';
                EV_SET(&del, active_list[i].ident, EVFILT_READ, EV_DELETE, 0, 0,
                       NULL);
                change_list.push_back(del);

                EV_SET(&event, active_list[i].ident, EVFILT_WRITE,
                       EV_ADD | EV_ENABLE, 0, 0, (void*)buffer);
                debugBuff(buffer);
                change_list.push_back(event);
            } else if (active_list[i].filter == EVFILT_WRITE) {
                std::cout << "WRITE EVENT" << std::endl;
                Buffer* buffer = static_cast<Buffer*>(active_list[i].udata);
                // 書き込み
                int res = send(active_list[i].ident, buffer->GetBuf(),
                               buffer->GetReadSize(), 0);
                debugBuff(buffer);
                delete buffer;
                if (res < 0) {
                    std::cerr << "SEND ERR" << std::endl;
                    closeAllListener(&all_listening_socket);
                    close(kq);
                    for (auto& e : stream_socket_list) {
                        close(e);
                    }
                    return 1;
                }
                EV_SET(&del, active_list[i].ident, EVFILT_WRITE, EV_DELETE, 0,
                       0, NULL);
                change_list.push_back(del);

                EV_SET(&event, active_list[i].ident, EVFILT_READ,
                       EV_ADD | EV_ENABLE, 0, 0, NULL);
                change_list.push_back(event);
            }
        }
    }
    closeAllListener(&all_listening_socket);
    close(kq);
    for (auto& e : stream_socket_list) {
        close(e);
    }
}
