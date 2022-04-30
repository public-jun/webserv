#ifndef SOCKET_HPP
#define SOCKET_HPP

class Socket
{
protected:
    int sock_;

public:
    Socket();
    ~Socket();

    int getSocket() const;
};

#endif
