#include "StreamSocket.hpp"

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "SysError.hpp"
#include <cerrno>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

StreamSocket::StreamSocket()
    : req_(HTTPRequest()), parser_(HTTPParser(req_)), resp_(HTTPResponse()),
      event_type_(RECV_REQUEST), read_size_(0), buf_size_(2048) {
    SetSocketType(Socket::STREAM);
}

StreamSocket::~StreamSocket() { Close(); }

HTTPRequest* StreamSocket::GetHTTPRequest() { return &req_; }

const StreamSocket::EventType& StreamSocket::GetEventType() const {
    return event_type_;
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
    setEventType(SEND_RESPONSE);
}

void StreamSocket::Close() {
    if (sock_fd_ <= 0) {
        if (close(sock_fd_) < 0) {
            throw SysError("close", errno);
        }
        sock_fd_ = 0;
    }
}

void StreamSocket::OnRecv() {
    read_size_ = recv(sock_fd_, buf_, buf_size_, 0);
    if (read_size_ < 0) {
        throw SysError("recv", errno);
    }
    buf_[read_size_] = '\0';
    // Request Parser
    parser_.Parse(std::string(buf_));
    if (parser_.GetPhase() != HTTPParser::PH_END) {
        return;
    }

    if (req_.GetMethod() == "GET") {
        if (req_.GetRequestTarget() == "/") {
            req_.SetRequestTarget(std::string("/index.html"));
        }

        // uri で指定されたファイルを読み取る
        std::ifstream ifs(req_.GetRequestTarget());
        std::string   tmp, file_content;
        if (ifs.fail()) {
            // err
            std::cerr << "fail to open file" << std::endl;
            return;
        }
        while (std::getline(ifs, tmp)) {
            file_content += tmp + "\n";
        }

        resp_.SetBody(file_content);
        resp_.AppendHeader("Server", "Webserv/1.0.0");
        resp_.SetVersion(req_.GetVersion());

        actions_->DelRecv(this);
        Send();
    }
}

void StreamSocket::parseRequest() {}

void StreamSocket::OnSend() {
    // res_->SendMessage();

    std::string resp = resp_.ConvertToStr();
    std::cout << "response: \n" << resp << std::endl;
    if (send(sock_fd_, resp.c_str(), resp.size(), 0) == -1) {
        throw SysError("send", errno);
    }
    // del event
    OnDisConnect();
}

void StreamSocket::OnDisConnect() {
    if (actions_) {
        actions_->DelEvent(this);
    }
}

void StreamSocket::setEventType(StreamSocket::EventType type) {
    event_type_ = type;
}
