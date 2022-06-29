#include "Server.hpp"

Server::Server() {}

Server::~Server() {}

void Server::InitServer(std::string config_file_path) {
    ConfigParser::ParseConfigFile(config_file_path);
    initListeners();
}

void Server::initListeners() {
    server_config_map server_config_map =
        Config::Instance()->GetServerConfigs();
    server_config_map::const_iterator it_end = server_config_map.end();

    for (server_config_map::const_iterator it = server_config_map.begin();
         it != it_end; it++) {
        initListeningSockets(it->first, it->second);
    }
}

void Server::initListeningSockets(int                      port,
                                  const server_config_vec& server_configs) {
    server_config_vec::const_iterator it = server_configs.begin();
    while (it != server_configs.end()) {
        std::string host = it->GetHost();
        if (!isHostDuplicated(it, server_configs, host))
            initListeningSocket(server_configs, host, port);
        it++;
    }
}

void Server::initListeningSocket(const server_config_vec& server_configs,
                                 std::string host, int port) {
    std::stringstream sstream;
    sstream << port;
    std::string     string_port = sstream.str();
    struct addrinfo hints       = {};

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
    hints.ai_family   = PF_INET;

    struct addrinfo* addr_list;

    getaddrinfo(host.c_str(), string_port.c_str(), &hints, &addr_list);

    ListeningSocket ls(server_configs);
    ls.Bind(host, port);
    ls.Listen();
    listeners_.push_back(ls);
    IOEvent* event = new AcceptConn(ls);
    event->Register();
    registered_events_.push_back(event);
}

bool Server::isHostDuplicated(server_config_vec::const_iterator it,
                              const server_config_vec&          configs,
                              std::string                       host) {
    server_config_vec::const_iterator checker = configs.begin();
    while (checker != it) {
        if (checker->GetHost() == host)
            return (true);
        checker++;
    }
    return (false);
}

void Server::ShutDownServer() {
    for (std::vector<ListeningSocket>::iterator it = listeners_.begin();
         it != listeners_.end(); it++) {
        it->Close();
    }
    for (std::vector<IOEvent*>::iterator it = registered_events_.begin();
         it != registered_events_.end(); it++) {
        close((*it)->GetPolledFd());
        delete (*it);
    }
}
