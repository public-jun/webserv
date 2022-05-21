#include "StreamSocket.hpp"

#include <unistd.h>

StreamSocket::StreamSocket() : read_size_(0), buf_size_(2048) {
    // type_ = Socket::STREAM;
    SetSocketType(Socket::STREAM);
}

StreamSocket::~StreamSocket() { Close(); }

void StreamSocket::Close() {
    if (sock_fd_ != 0) {
        close(sock_fd_);
    }
}
