#include <ServerSocket.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ClientSocket.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"
#include "WebServ.hpp"

#define BUF_SIZE 1024

WebServ::WebServ() {}

WebServ::~WebServ() { delete s_sock_; }

void WebServ::run()
{
    _serverSocketRun();
    while (true)
    {
        // クライアントと接続する
        ClientSocket c_sock(s_sock_->getSocket());

        // クライアントのソケットからメッセージを読み取る
        char buf[BUF_SIZE];
        if (recv(c_sock.getSocket(), buf, BUF_SIZE, 0) == -1)
        {
            throw std::runtime_error("recv error");
        }

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
            HTTPResponse response(c_sock.getSocket(), 200, header,
                                  file_content);

            // レスポンスを作成して送信
            response.create();
            response.sendMessage();
        }
        else
        {
            // 404
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
