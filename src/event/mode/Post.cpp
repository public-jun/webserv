#include "Post.hpp"
#include <iostream>

Post::Post() : IOEvent(POST), stream_(StreamSocket()), resp_(HTTPResponse()) {}

Post::Post(StreamSocket stream, HTTPRequest req)
    : IOEvent(POST), stream_(stream), req_(req), resp_(HTTPResponse()) {}

Post::~Post() {}

void Post::Run() {

    // 1. 保存先パス&ファイル名取得
    std::string base_path = "./";
    std::string file_name = generateFileName();
    std::string file_path = base_path + file_name;

    // 2. 書き込み内容取得
    content_ = req_.GetBody().c_str();
    // 3. ファイルオープン
    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK,
                  0644);
    if (fd < 0)
        throw SysError("open", errno);
    // 4. ファイル書き込み
    int size = write(fd, content_.c_str(), content_.size());
    // 5. 書き込みできたかの確認?
    if (size < 0 || static_cast<size_t>(size) != content_.size())
        throw SysError("write", errno);
}

void Post::Register() { EventRegister::Instance().AddWriteEvent(this); }

void Post::Unregister() { EventRegister::Instance().DelWriteEvent(this); }

IOEvent* Post::RegisterNext() {
    resp_.AppendHeader("Content-Length", content_);
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());
    resp_.SetStatusCode(status_);
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    this->Unregister();
    send_response->Register();

    return send_response;
}

void Post::openFile() {}

std::string Post::generateFileName() {
    time_t            t     = time(NULL);
    struct tm*        local = localtime(&t);
    std::stringstream ss;
    char              buf[128];

    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", local);
    ss << buf;
    return (ss.str());
}
