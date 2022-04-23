#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <ServerSocket.hpp>

class WebServ
{
public:
    WebServ();
    ~WebServ();
    void run();
    int recvRequestMessage(int sock, char* request_message, unsigned int buf_size);

private:
    ServerSocket* s_sock_;

private:
    void _serverSocketRun();
};

#endif