#include "StreamSocket.hpp"

#include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

StreamSocket::StreamSocket()
    : parser_(NULL), req_(NULL), res_(NULL), read_size_(0), buf_size_(2048) {
    SetSocketType(Socket::STREAM);
}

StreamSocket::~StreamSocket() {
    if (parser_) {
        delete parser_;
        parser_ = NULL;
    }
    if (req_) {
        delete req_;
        req_ = NULL;
    }
    if (res_) {
        delete res_;
        res_ = NULL;
    }
    Close();
}

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
        sock_fd_ = 0;
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

void StreamSocket::parseRequest() {
    parser_ = new HTTPParser();
    req_    = parser_->Parse(std::string(buf_));

    if (req_->GetMethod() == "GET") {
        if (req_->GetURI() == "/") {
            req_->SetURI(std::string("/index.html"));
        }

        // uri で指定されたファイルを読み取る
        std::ifstream ifs(req_->GetURI().erase(0, 1));
        std::string   tmp, file_content;
        if (ifs.fail()) {
            // err
            std::cerr << "fail to open file" << std::endl;
            return;
        }
        while (std::getline(ifs, tmp)) {
            file_content += tmp + "\n";
        }

        std::ostringstream oss;
        std::string        length;
        oss << file_content.length() << std::flush;
        length = oss.str();
        std::string header("Content-Length: " + length);
        res_ = new HTTPResponse(sock_fd_, 200, header, file_content);
        res_->Create();
    }
}

void StreamSocket::OnSend() {
    res_->SendMessage();
    // if (res < 0) {
    // err
    // }
    // del event
    OnDisConnect();
}

void StreamSocket::OnDisConnect() {
    if (actions_) {
        actions_->DelEvent(this);
    }
}
