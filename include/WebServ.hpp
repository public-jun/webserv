#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <ServerSocket.hpp>

class WebServ
{
public:
    WebServ();
    ~WebServ();
    void run();

private:
    ServerSocket* s_sock_;

private:
    void _serverSocketRun();
};

#endif
