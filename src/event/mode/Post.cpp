#include "Post.hpp"
#include "CGI.hpp"
#include "WriteCGI.hpp"
#include <iostream>

Post::Post(StreamSocket stream, HTTPRequest req, URI& uri)
    : stream_(stream), uri_(uri), req_(req), write_event_(NULL),
      is_cgi_(false) {}

Post::~Post() {}

void Post::Run() {
    if (CGI::IsCGI(uri_, "POST")) {
        is_cgi_ = true;
        class CGI cgi(uri_, req_);
        cgi.Run();
        fd_read_from_cgi_ = cgi.FdForReadFromCGI();
        write_fd_         = cgi.FdForWriteToCGI();
    } else {
        std::string base_path = uri_.GetLocationConfig().GetUploadPath();
        if (base_path.substr(base_path.length() - 1) != "/")
            base_path += "/";
        std::string        file_name = generateFileName();
        std::string        file_path = base_path + file_name;
        const struct stat& stat_buf  = uri_.Stat(base_path);

        if ((stat_buf.st_mode & S_IRWXU) != S_IRWXU)
            throw status::forbidden;
        write_fd_ = open(file_path.c_str(),
                         O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
        if (write_fd_ < 0)
            throw status::server_error;
    }
}

IOEvent* Post::RegisterNext() {
    if (is_cgi_) {
        write_event_ =
            new WriteCGI(write_fd_, fd_read_from_cgi_, stream_, req_);
    } else {
        write_event_ = new WriteFile(stream_, write_fd_, req_);
    }
    write_event_->Register();
    return (write_event_);
}

std::string Post::generateFileName() {
    time_t            t     = time(NULL);
    struct tm*        local = localtime(&t);
    std::stringstream ss;
    char              buf[128];

    strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", local);
    ss << buf << "_" << rand();
    return (ss.str());
}
