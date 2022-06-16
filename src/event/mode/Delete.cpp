#include "Delete.hpp"
#include <iostream>

Delete::Delete()
    : IOEvent(DELETE), stream_(StreamSocket()), resp_(HTTPResponse()) {}

Delete::Delete(StreamSocket stream, HTTPRequest req)
    : IOEvent(DELETE), stream_(stream), req_(req), resp_(HTTPResponse()) {}

Delete::~Delete() {}

void Delete::Register() {}

void Delete::Unregister() {}

void Delete::Run() {
    std::string target_path = req_.GetRequestTarget();
    struct stat stat_buf;
    if (stat(target_path.c_str(), &stat_buf) == -1) {
        status_ = status::not_found;
        return;
    }
    if (S_ISDIR(stat_buf.st_mode) || access(target_path.c_str(), W_OK) == -1) {
        status_ = status::forbidden;
        return;
    }
    if (unlink(target_path.c_str()) == -1)
        status_ = 500;
    status_ = 204;
}

IOEvent* Delete::RegisterNext() {
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());
    resp_.SetStatusCode(status_);
    resp_.AppendHeader("Content-Length", "0");
    std::cout << resp_.ConvertToStr() << std::endl;
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    send_response->Register();

    return send_response;
}
