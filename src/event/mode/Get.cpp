#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "LocationConfig.hpp"
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

Get::Get(StreamSocket stream, URI& uri)
    : stream_(stream), uri_(uri), location_config_(uri_.GetLocationConfig()) {}

Get::~Get() {}

/*

ディレクトリかどうか
- ディレクトリの場合:
  location_configにindexがあるか
  - ある場合:
    そのファイルが存在するか
    - ある場合:
      prepareReadFile

    - ない場合:
      autoindexがONか確認:
      - ONの場合
        autoIndex

      - OFFの場合
        throw 404

  - ない場合:
    autoIndexがONか確認:
    - ONの場合
      autoIndex

    - OFFの場合
      throw 404

- ファイルの場合:
  prepareReadFile

*/

void Get::Run() {
    printLogStart();

    std::pair<int, std::string> redir = location_config_.GetReturn();

    // redirect
    if (hasRedir(redir, uri_.GetRawTarget(), location_config_.GetTarget())) {
        processRedir(redir);
        return;
    }

    const struct stat s = URI::Stat(uri_.GetLocalPath());
    if (S_ISDIR(s.st_mode)) {
        processDir();
    } else {
        prepareReadFile(uri_.GetLocalPath());
    }
    printLogEnd();
}

IOEvent* Get::NextEvent() { return next_event_; }

void Get::processDir() {
    std::string local_path = uri_.GetLocalPath();
    std::string index      = location_config_.GetIndex();
    complementSlash(local_path);

    if (hasIndex(index)) {
        processIndex(local_path + index);
    } else {
        tryAutoIndex();
    }
}

void Get::processRedir(std::pair<int, std::string> redir) {
    std::string       url;
    std::stringstream port;
    if (*redir.second.begin() == '/') {
        // return 301 /html/index.htmlを
        // return 301 http://host:port/html/index.htmlに変換
        port << uri_.GetServerConfig().GetPort();
        url = "http://" + uri_.GetServerConfig().GetHost() + ":" + port.str() +
              redir.second;
        port << "";
    } else {
        // return 301 https://google.com/
        url = redir.second;
    }

    HTTPResponse resp;
    resp.AppendHeader("Location", url);
    std::cout << "Location: " << url << std::endl;
    resp.SetStatusCode(redir.first);
    next_event_ = new SendResponse(stream_, resp.ConvertToStr());
}

void Get::processIndex(std::string fullpath) {
    if (existFile(fullpath)) {
        prepareReadFile(fullpath);
    } else {
        tryAutoIndex();
    }
}

void Get::tryAutoIndex() {
    if (location_config_.GetAutoIndex() == ON) {
        autoIndex();
    } else {
        printLogNotFound();
        throw status::not_found;
    }
}

bool Get::hasRedir(std::pair<int, std::string> redir, std::string path,
                   std::string target) {
    if (redir.first != -1 && path == target) {
        return true;
    }
    return false;
}

bool Get::hasIndex(std::string index) { return index != ""; }

bool Get::existFile(std::string path) {
    errno = 0;
    struct stat s;

    // ENOENT以外のエラーはここでは見ない
    if (stat(path.c_str(), &s) == -1 && errno == ENOENT) {
        return false;
    }
    return true;
}

void Get::prepareSendResponse(std::string content) {
    HTTPResponse      resp;
    std::stringstream size;
    size << content.size();
    resp.AppendHeader("Content-Length", size.str());
    resp.AppendHeader("Content-Type", "text/html; charset=utf-8");
    resp.SetBody(content);
    resp.PrintInfo();
    next_event_ = new SendResponse(stream_, resp.ConvertToStr());
}

void Get::prepareReadFile(std::string path) {
    printLogReadFile();

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

// ####### autoindexの実装 ########

// n - name.size() 個のspaceを返す
std::string Get::spaces(std::string name, int n) {
    if (static_cast<std::string::size_type>(n) < name.size()) {
        return "";
    }
    int         size = n - name.size();
    std::string spaces(size, ' ');
    return spaces;
}

void Get::complementSlash(std::string& path) {
    if (std::string(1, path.back()) != "/") {
        path = path + "/";
    }
}

// <a href="src/">src/</a>
std::string Get::aElement(struct dirent* ent, std::string path) {
    std::stringstream ss;
    std::string       name = ent->d_name;
    if (ent->d_type == DT_DIR) {
        name = name + "/";
    }
    if (std::string(1, path.back()) != "/") {
        path = path + "/";
    }
    if (std::string("../") == name) {
        path = "";
    }

    ss << "<a href=\"" << path << name;
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

// 最小フィールド幅35のファイルサイズを返す
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
    complementSlash(path);
    const std::string fullpath = path + std::string(ent->d_name);

    struct stat s = URI::Stat(fullpath);

    return timeStamp(&s.st_mtime) + fileSize(&s);
}

void Get::autoIndex() {
    printLogAutoIndex();

    const std::string& local_path  = uri_.GetLocalPath();
    const std::string& decode_path = uri_.GetDecodePath();

    errno    = 0;
    DIR* dir = opendir(local_path.c_str());
    if (dir == NULL) {
        perror("opendir");
        if (errno == EACCES) {
            throw status::forbidden;
        }
        throw status::server_error;
    }

    // "."を削除
    std::stringstream ss;
    ss << "<html>" << CRLF << "<head>" << CRLF << "<title>"
       << "Index of " << decode_path << "</title>" << CRLF << "</head>" << CRLF
       << "<body>" << CRLF << "<h1> Index of " << decode_path << "</h1>" << CRLF
       << "<hr>" << CRLF << "<pre>" << CRLF;

    for (struct dirent* ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        if (std::string(".") == ent->d_name) {
            continue;
        }

        ss << aElement(ent, decode_path);
        if (std::string("..") != ent->d_name) {
            ss << fileInfo(ent, uri_.GetLocalPath());
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

// log
void Get::printLogAutoIndex() {
#ifdef WS_DEBUG
    std::cout << "autoIndex" << std::endl;
#endif
}

void Get::printLogStart() {
#ifdef WS_DEBUG
    std::cout << "=== Get ===" << std::endl;
#endif
}

void Get::printLogEnd() {
#ifdef WS_DEBUG
    std::cout << "==========="
              << "\n"
              << std::endl;
#endif
}

void Get::printLogNotFound() {
#ifdef WS_DEBUG
    std::cout << "throw status not found" << std::endl;
    std::cout << "==========="
              << "\n"
              << std::endl;
#endif
}

void Get::printLogReadFile() {
#ifdef WS_DEBUG
    std::cout << "prepareReadFile" << std::endl;
#endif
}
