#include "Post.hpp"
#include <iostream>

Post::Post(StreamSocket stream, HTTPRequest req, URI& uri)
    : stream_(stream), uri_(uri), req_(req) {}

Post::~Post() {}

void Post::Run() {
    std::string base_path = uri_.GetLocationConfig().GetUploadPath();
    if (base_path.substr(base_path.length() - 1) != "/")
        base_path += "/";
    std::string file_name = generateFileName();
    std::string file_path = base_path + file_name;

    fd_ = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK,
               0644);
    if (fd_ < 0)
        throw status::server_error;
}

IOEvent* Post::RegisterNext() {
    IOEvent* write_file = new WriteFile(stream_, fd_, req_);
    write_file->Register();
    return (write_file);
}

std::string Post::generateFileName() {
    time_t            t     = time(NULL);
    struct tm*        local = localtime(&t);
    std::stringstream ss;
    char              buf[128];

    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", local);
    ss << buf;
    return (ss.str());
}
