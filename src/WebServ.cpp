#include <ServerSocket.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ClientSocket.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"
#include "WebServ.hpp"

#include <sys/select.h>
#include <poll.h>
#include <unistd.h>
#include <sys/event.h>

#define BUF_SIZE 1024

WebServ::WebServ() {}

WebServ::~WebServ() { delete s_sock_; }

void WebServ::run()
{
    _serverSocketRun();

    int ret;
      int kq;
      struct kevent kev;

      kq = kqueue();
      if (kq == -1) {
        perror("kqueue");
        exit(1);
      }
      EV_SET(&kev, s_sock_->getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
      ret = kevent(kq, &kev, 1, NULL, 0, NULL);
      if (ret == -1) {
        perror("kevent");
        exit(1);
      }
    while (true)
    {
       int n;
       char buf[BUF_SIZE];
       int sock;

       n = kevent(kq, NULL, 0, &kev, 1, NULL);

       if (n == -1) {
         perror("kevent");
         exit(1);
       }
       if ((int)kev.ident == s_sock_->getSocket()){
         int acc = accept(kev.ident, NULL, NULL);
         if (acc != -1){
           EV_SET(&kev, acc, EVFILT_READ, EV_ADD, 0, 0, NULL);
           n = kevent(kq, &kev, 1, NULL, 0, NULL);
           if (n == -1) {
             perror("kevent");
             exit(1);
           }
         }
       } else {
         sock = kev.ident;
         printf("ディスクリプタ %d 番が読み込み可能です。\n", sock);
         recv(sock, buf, sizeof(buf), 0);

        // メッセージをパースしてHTTPRequestを作る
        HTTPParser  parser;
        HTTPRequest req = parser.parse(std::string(buf));

        if (req.getMethod() == "GET")
        {
            if (req.getURI() == "/")
            {
                req.setURI(std::string("/index.html"));
            }

            // uriで指定されたファイルを読み取る
            std::ifstream ifs(req.getURI().erase(0, 1));
            std::string   tmp, file_content;
            if (ifs.fail())
            {
                std::cout << "fail open file" << std::endl;
                return;
            }
            while (std::getline(ifs, tmp))
            {
                file_content += tmp + "\n";
            }

            // レスポンスヘッダーを作る
            std::ostringstream oss;
            std::string        length;
            oss << file_content.length() << std::flush;
            length = oss.str();
            std::string  header("Content-Length: " + length);
            HTTPResponse response(sock, 200, header,
                                  file_content);

            // レスポンスを作成して送信
            response.create();
            response.sendMessage();
        }
        else
        {
            // 404
        }

      }
        // close
    }
    // close
}

void WebServ::_serverSocketRun()
{
    /* ServerSocket* s_sock_ = new ServerSocket(); */
    s_sock_ = new ServerSocket();
    s_sock_->bindSocket();
    s_sock_->listenSocket();
}
