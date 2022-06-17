#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "URI.hpp"

#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

Get::Get(StreamSocket stream, URI& uri) : stream_(stream), uri_(uri) {}

Get::~Get() {}

void Get::autoIndex(std::string path) {
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        return;
    }

    std::stringstream ss;
    ss << "<html>\r\n"
       << "<head>\r\n"
       << "<title>"
       << "Index of " << path << "</title>\r\n"
       << "</head>\r\n"
       << "<body>\r\n"
       << "<h1> Index of " << path << "</h1>\r\n"
       << "<hr>\r\n"
       << "<pre>\r\n";

    for (struct dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        std::string file_name = ent->d_name;
        ss << "<a href=\"" << file_name << "\">" << file_name << "</a>\r\n";
    }
    ss << "</pre>\r\n"
       << "<hr>\r\n"
       << "</body>\r\n"
       << "</html>\r\n";

    HTTPResponse      resp;
    std::string       content = ss.str();
    std::stringstream size;
    size << content.size();
    resp.AppendHeader("Content-Length", size.str());
    resp.SetBody(content);
    next_event_ = new SendResponse(stream_, resp.ConvertToStr());
}

void Get::prepareReadFile(std::string path) {
    int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("open");
        // internal server error;
        std::cout << "error" << std::endl;
    }
    next_event_ = new ReadFile(stream_, fd);
}

void Get::Run() {
    std::string path = uri_.GetLocalPath();
    struct stat s    = uri_.GetStat();

    // 403の処理
    if (!((s.st_mode & S_IRUSR) == S_IRUSR)) {
        std::cout << "permission denied " << std::endl;
        throw status::forbidden;
    }

    if (S_ISDIR(s.st_mode)) {
        autoIndex(path);
    } else {
        prepareReadFile(path);
    }
}

IOEvent* Get::NextEvent() { return next_event_; }
