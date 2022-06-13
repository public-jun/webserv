#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "LocationConfig.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class ServerConfig {
public:
    ServerConfig();
    ServerConfig(const ServerConfig& src);
    ~ServerConfig();
    ServerConfig& operator=(const ServerConfig& src);

    void setListen(int listen);
    void setServerName(std::string serverName);
    void setMaxClientBodySize(std::string clientMaxBodySize);
    void setErrorPage(int status_code, std::string location);
    void setLocationConfigs(LocationConfig& location_config);

    int                        getListen() const;
    std::string                getServerName() const;
    std::string                getMaxClientBodySize() const;
    std::map<int, std::string> getErrorPage() const;
    std::map<const std::string, const LocationConfig>
    getLocationConfigs() const;

private:
    int                                               listen_;
    std::string                                       server_name_;
    std::string                                       max_client_body_size_;
    std::map<int, std::string>                        error_page_;
    std::map<const std::string, const LocationConfig> location_configs_;
};

#endif
