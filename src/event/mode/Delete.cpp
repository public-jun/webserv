#include "Delete.hpp"
#include <iostream>

Delete::Delete(StreamSocket stream, URI& uri)
    : IOEvent(DELETE), stream_(stream), uri_(uri), resp_(HTTPResponse()) {}

Delete::~Delete() {}

void Delete::Register() {}

void Delete::Unregister() {}

void Delete::Run() {
    std::string        target_path = uri_.GetLocalPath();
    const struct stat& stat_buf    = uri_.GetStat();

    if (S_ISDIR(stat_buf.st_mode) || access(target_path.c_str(), W_OK) == -1) {
        status_ = status::forbidden;
        return;
    }
    if (unlink(target_path.c_str()) == -1)
        status_ = status::server_error;
    status_ = status::no_content;
}

IOEvent* Delete::RegisterNext() {
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetStatusCode(status_);
    resp_.AppendHeader("Content-Length", "0");
    std::cout << resp_.ConvertToStr() << std::endl;
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    send_response->Register();

    return send_response;
}
