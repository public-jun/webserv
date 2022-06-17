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

/*
<head>
<title>Index of /dir/</title>
</head>
<body>
<h1>Index of /dir/</h1>
<hr>
<pre>
<a href="../">../</a>
<a href="hoge.html">hoge.html«/a>
17-Jun-2022 03:14 5
</pre>
<hr>
</body>
</html>
*/

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
        ss << "<a href=\"" << file_name << ">" << file_name << "</a>\r\n";
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
    std::cout << resp.ConvertToStr() << std::endl;
    next_event_ = new SendResponse(stream_, resp.ConvertToStr());
}

void Get::Run() {
    (void)uri_;
    /* autoIndex(); */
    /* exit(0); */

    std::string path;
    /* path = "index.html"; */
    path = "./src";
    /* path = "no_auth.txt"; */
    /* path = "no_such_file"; */

    struct stat ss;
    stat(path.c_str(), &ss);

    // 404, 403の処理

    if (!((ss.st_mode & S_IRUSR) == S_IRUSR)) {
        std::cout << "permission denied " << std::endl;
        throw status::forbidden;
    }

    // ディレクトリか
    if (S_ISDIR(ss.st_mode)) {
        std::cout << "is dir" << std::endl;
        autoIndex(path);
        return;
    } else {
        int fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
        std::cout << "fd: " << fd << std::endl;
        if (fd == -1) {
            // internal server error;
            std::cout << "error" << std::endl;
        }
        // ReadFileを登録
        next_event_ = new ReadFile(stream_, fd);
    }
}

IOEvent* Get::NextEvent() { return next_event_; }
