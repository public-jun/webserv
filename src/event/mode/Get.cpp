#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "URI.hpp"

#include "HTTPStatus.hpp"
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

Get::Get(StreamSocket stream, URI& uri) : stream_(stream), uri_(uri) {}

Get::~Get() {}

void Get::Run() {
    const std::string& path = uri_.GetLocalPath();
    const struct stat  s    = uri_.Stat(path);

    if (S_ISDIR(s.st_mode)) {
        autoIndex(path);
    } else {
        prepareReadFile(path);
    }
}

// n - name.size() 個のspaceを返す
std::string Get::spaces(std::string name, int n) {
    // TODO: マイナスになったばあい
    int         size = n - name.size();
    std::string spaces(size, ' ');
    return spaces;
}

IOEvent* Get::NextEvent() { return next_event_; }

// TODO:
// - "."削除
// - "ファイルのタイムスタンプ表示"
// - "ファイルサイズ表示"
std::string Get::aElement(struct dirent* ent) {
    std::stringstream ss;
    std::string       name = ent->d_name;

    ss << "<a href=\"" << name;
    if (ent->d_type == DT_DIR) {
        ss << "/";
    }
    ss << "\">" << name << "</a>";

    return ss.str();
}

std::string Get::fileInfo(struct dirent* ent, std::string path) {
    std::stringstream ss;
    std::string       time_stamp("19-Jun-2022 11:46");

    std::string name     = ent->d_name;
    std::string fullpath = "." + path + "/" + name;
    std::cout << "path: " << fullpath << std::endl;

    struct stat s;
    stat(fullpath.c_str(), &s);

    s.st_mtime;

    ss << spaces(name, 50) << time_stamp << spaces(time_stamp, 35) << "-";
    return ss.str();
}

void Get::autoIndex(std::string path) {
    errno    = 0;
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        perror("opendir");
        if (errno == EACCES) {
            throw status::forbidden;
        }
        throw status::server_error;
    }
    path = path.substr(1);

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
        if (std::string(".") == ent->d_name) {
            continue;
        }

        ss << aElement(ent) << fileInfo(ent, path) << "\r\n";
    }
    ss << "</pre>\r\n"
       << "<hr>\r\n"
       << "</body>\r\n"
       << "</html>\r\n";
    if (errno == EBADF) {
        perror("readdir");
        throw status::server_error;
    }

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
        if (errno == EACCES) {
            throw status::forbidden;
        }
        throw status::server_error;
    }
    next_event_ = new ReadFile(stream_, fd);
}

