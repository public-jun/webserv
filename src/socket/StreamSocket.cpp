#include "StreamSocket.hpp"

// #include "HTTPParser.hpp"
#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "SysError.hpp"
#include <cerrno>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

StreamSocket::StreamSocket()
    : req_(HTTPRequest()), parser_(HTTPParser(req_)), res_(NULL),
      event_type_(RECV_REQUEST), read_size_(0), buf_size_(2048) {
    SetSocketType(Socket::STREAM);
}

StreamSocket::~StreamSocket() {
    /* if (parser_) { */
    /*     delete parser_; */
    /*     parser_ = NULL; */
    /* } */
    /* if (req_) { */
    /*     delete req_; */
    /*     req_ = NULL; */
    /* } */
    if (res_) {
        delete res_;
        res_ = NULL;
    }
    Close();
}

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
    parseRequest();
    if (parser_.GetPhase() == HTTPParser::PH_END) {
        actions_->DelRecv(this);
        Send();
    }
}

void StreamSocket::parseRequest() {
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
