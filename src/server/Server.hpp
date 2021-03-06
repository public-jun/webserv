#ifndef SUPERVISOR_HPP
#define SUPERVISOR_HPP

#include "AcceptConn.hpp"
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "EventExecutor.hpp"
#include "IOEvent.hpp"
#include "ListeningSocket.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

class Server {
public:
    static Server& Instance() {
        static Server instance;
        return instance;
    }
    void InitServer(std::string config_file_path);
    void Run();
    void ShutDownServer();

private:
    Server();
    ~Server();
    EventExecutor&               executor_;
    std::vector<ListeningSocket> listeners_;
    std::vector<IOEvent*>        registered_events_;
    void                         initListeners();
    void                         initListeningSockets(int                      port,
                                                      const server_config_vec& server_configs);
    void initListeningSocket(const server_config_vec& server_configs,
                             std::string host, int port);
    bool isHostDuplicated(server_config_vec::const_iterator it,
                          const server_config_vec& configs, std::string host);
};

#endif
