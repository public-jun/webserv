#include <ServerSocket.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "ClientSocket.hpp"
#include "HTTPParser.hpp"
#include "HTTPResponse.hpp"
#include "WebServ.hpp"

WebServ::WebServ() {}

WebServ::~WebServ() { delete s_sock_; }

int WebServ::recvRequestMessage(int sock, char* request_message,
                                unsigned int buf_size)
{
    int recv_size;

    recv_size = recv(sock, request_message, buf_size, 0);

    return recv_size;
}

void WebServ::run()
{
    _serverSocketRun();
    while (true)
    {
        // accept
        ClientSocket c_sock(s_sock_->getSocket());

        // httpserver
        char buf[1024];
        recv(c_sock.getSocket(), buf, 1024, 0);
        HTTPParser  parser;
        HTTPRequest req = parser.parse(std::string(buf));
        if (req.getMethod() == "GET")
        {
            if (req.getURI() == "/")
            {
                req.setURI(std::string("/index.html"));
            }
            // getProcessing
            // uriを受け取ってbodyとstatusを取得
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

            std::ostringstream oss;
            std::string        length;
            oss << file_content.length() << std::flush;
            length = oss.str();
            std::string  header("Content-Length: " + length);
            HTTPResponse response(c_sock.getSocket(), 200, header,
                                  file_content);
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
