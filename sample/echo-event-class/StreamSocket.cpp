#include "StreamSocket.hpp"

#include <unistd.h>

StreamSocket::StreamSocket() : read_size_(0), buf_size_(2048) {
    SetSocketType(Socket::STREAM);
}

StreamSocket::~StreamSocket() { Close(); }

void StreamSocket::Recv() {
    if (actions_) {
        actions_->AddRecvEvent(this);
    }
}

void StreamSocket::Send() {
    if (actions_) {
        actions_->AddSendEvent(this);
    }
}

void StreamSocket::Close() {
    if (sock_fd_ != 0) {
        close(sock_fd_);
    }
}

void StreamSocket::OnRecv() {
    read_size_ = recv(sock_fd_, buf_, buf_size_, 0);
    if (read_size_ < 0) {
        // err
    }
    buf_[read_size_] = '\0';
    actions_->DelRecv(this);
    Send();
}

void StreamSocket::OnSend() {
    int res = send(sock_fd_, buf_, read_size_, 0);
    if (res < 0) {
        // err
    }
    actions_->DelSend(this);
    Recv();
}