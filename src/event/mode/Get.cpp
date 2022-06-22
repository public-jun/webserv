#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "URI.hpp"

#include "HTTPStatus.hpp"
#include <cstdio>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>

const std::string Get::CRLF = "\r\n";

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

IOEvent* Get::NextEvent() { return next_event_; }

// n - name.size() 個のspaceを返す
std::string Get::spaces(std::string name, int n) {
    if (std::string::size_type(n) < name.size()) {
        return "";
    }
    int         size = n - name.size();
    std::string spaces(size, ' ');
    return spaces;
}

// <a href="src/">src/</a>
std::string Get::aElement(struct dirent* ent) {
    std::stringstream ss;
    std::string       name = ent->d_name;
    if (ent->d_type == DT_DIR) {
        name = name + "/";
    }

    ss << "<a href=\"" << name;
    ss << "\">" << name << "</a>" << spaces(name, 50);

    return ss.str();
}

// "19-Jun-2022 11:46"
std::string Get::timeStamp(time_t* time) {
    struct tm*        tm      = localtime(time);
    const std::string month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << tm->tm_mday << "-"
       << month[tm->tm_mon] << "-" << 1900 + tm->tm_year << " " << std::setw(2)
       << tm->tm_hour << ":" << std::setw(2) << tm->tm_min;
    return ss.str();
}

std::string Get::fileSize(struct stat* s) {
    std::string size;
    if (S_ISDIR(s->st_mode)) {
        size = "-";
    } else {
        std::stringstream ss;
        ss << s->st_size;
        size = ss.str();
    }
    return spaces(size, 35) + size;
}

std::string Get::fileInfo(struct dirent* ent, std::string path) {
    std::stringstream ss;

    std::string name = ent->d_name;
    std::string slash;
    if (std::string(1, path.back()) != "/") {
        slash = "/";
    }
    std::string fullpath = "." + path + slash + name;
    std::cout << "path: " << fullpath << std::endl;

    struct stat s          = URI::Stat(fullpath);
    std::string time_stamp = timeStamp(&s.st_mtime);

    ss << time_stamp << fileSize(&s);
    return ss.str();
}

void Get::autoIndex(std::string path) {
    std::stringstream ss;
    ss << "<html>" << CRLF << "<head>" << CRLF << "<title>"
       << "Index of " << path << "</title>" << CRLF << "</head>" << CRLF
       << "<body>" << CRLF << "<h1> Index of " << path << "</h1>" << CRLF
       << "<hr>" << CRLF << "<pre>" << CRLF;

    errno    = 0;
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        perror("opendir");
        if (errno == EACCES) {
            throw status::forbidden;
        }
        throw status::server_error;
    }
    // "."を削除
    path = path.substr(1);

    for (struct dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        if (std::string(".") == ent->d_name) {
            continue;
        }

        ss << aElement(ent);
        if (std::string("..") != ent->d_name) {
            ss << fileInfo(ent, path);
        }
        ss << CRLF;
    }
    if (errno == EBADF) {
        perror("readdir");
        throw status::server_error;
    }
    ss << "</pre>" << CRLF << "<hr>" << CRLF << "</body>" << CRLF << "</html>"
       << CRLF;

    prepareSendResponse(ss.str());
}

void Get::prepareSendResponse(std::string content) {
    HTTPResponse      resp;
    std::stringstream size;
    size << content.size();
    resp.AppendHeader("Content-Length", size.str());
    resp.AppendHeader("Content-Type", "text/html; charset=utf-8");
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

