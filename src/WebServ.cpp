#include <WebServ.hpp>

#include <ServerSocket.hpp>

WebServ::WebServ() {}

WebServ::~WebServ() {
    delete s_sock_;
}

int WebServ::recvRequestMessage(int sock, char* request_message, unsigned int buf_size)
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

        // httpserver

        // close
    }
    // close
}

void WebServ::_serverSocketRun()
{
    ServerSocket *s_sock_ = new ServerSocket();
    s_sock_->bindSocket();
    s_sock_->listenSocket();
}