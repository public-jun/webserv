#include "Post.hpp"
#include <iostream>

Post::Post(StreamSocket stream, HTTPRequest req)
    : IOEvent(POST), stream_(stream), req_(req), resp_(HTTPResponse()) {}

Post::~Post() {}

void Post::Run() {
    std::string base_path = "./upload_files/";
    std::string file_name = generateFileName();
    std::string file_path = base_path + file_name;

    std::cout << file_path << std::endl;
    content_ = req_.GetBody().c_str();
    int fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK,
                  0644);
    if (fd < 0)
        throw SysError("open", errno);
    int size = write(fd, content_.c_str(), content_.size());
    if (size < 0 || static_cast<size_t>(size) != content_.size())
        throw SysError("write", errno);
    status_ = status::created;
}

void Post::Register() {}

void Post::Unregister() {}

IOEvent* Post::RegisterNext() {
    resp_.AppendHeader("Content-Length", "0");
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetStatusCode(status_);
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

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
