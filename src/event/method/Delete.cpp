#include "Delete.hpp"
#include <iostream>

Delete::Delete(StreamSocket stream, URI& uri)
    : stream_(stream), uri_(uri), resp_(HTTPResponse()) {}

Delete::~Delete() {}

void Delete::Run() {
    std::string        target_path = uri_.GetLocalPath();
    const struct stat& stat_buf    = uri_.Stat(target_path);

    if (S_ISDIR(stat_buf.st_mode) || (stat_buf.st_mode & S_IWOTH) != S_IWOTH)
        throw status::forbidden;
    if (unlink(target_path.c_str()) == -1)
        throw status::server_error;
}

IOEvent* Delete::RegisterNext() {
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetStatusCode(status::no_content);
    resp_.AppendHeader("Content-Length", "0");
    resp_.AppendHeader("Connection", "close");
    resp_.PrintInfo();
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    send_response->Register();

    return send_response;
}
