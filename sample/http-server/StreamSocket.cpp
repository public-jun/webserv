#include "StreamSocket.hpp"

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

StreamSocket::StreamSocket() : read_size_(0), buf_size_(2048) {
    SetSocketType(Socket::STREAM);
    req_ = NULL;
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
    // Request Parser
    parseRequest();
    actions_->DelRecv(this);
    Send();
}

void StreamSocket::OnSend() {
    // int res = send(sock_fd_, buf_, read_size_, 0);
    res_->sendMessage();
    delete res_;
    res_ = NULL;
    // if (res < 0) {
    // err
    // }

    // actions_->DelSend(this);
    // Recv();

    // del event
    OnDisConnect();
}

void StreamSocket::parseRequest() {
    parser_ = new HTTPParser();
    req_    = parser_->parse(std::string(buf_));
    delete parser_;
    parser_ = NULL;

    if (req_->getMethod() == "GET") {
        if (req_->getURI() == "/") {
            req_->setURI(std::string("/index.html"));
        }

        // uri で指定されたファイルを読み取る
        std::ifstream ifs(req_->getURI().erase(0, 1));
        delete req_;
        req_ = NULL;
        std::string tmp, file_cotent;
        if (ifs.fail()) {
            // err
            std::cerr << "faile open file" << std::endl;
            return;
        }
        while (std::getline(ifs, tmp)) {
            file_cotent += tmp + "\n";
        }

        std::ostringstream oss;
        std::string        length;
        oss << file_cotent.length() << std::flush;
        length = oss.str();
        std::string header("Content-Length: " + length);
        res_ = new HTTPResponse(sock_fd_, 200, header, file_cotent);
        res_->create();
    }
}

void StreamSocket::OnDisConnect() {
    std::cout << "CALL DELEVENT" << std::endl;
    close(sock_fd_);
    if (actions_) {
        actions_->DelEvent(this);
    }
}
