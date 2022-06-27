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

    void SetHost(std::string host);
    void SetPort(int port);
    void SetServerName(std::string server_name);
    void SetMaxClientBodySize(unsigned int client_max_body_size);
    void SetErrorPage(int status_code, std::string location);
    void SetIndex(std::string index);
    void SetRoot(std::string root);
    void SetAllowedMethods(std::string allowed_method);
    void SetLocationConfigs(LocationConfig& location_config);

    std::string                GetHost() const;
    int                        GetPort() const;
    std::string                GetServerName() const;
    unsigned int               GetMaxClientBodySize() const;
    std::map<int, std::string> GetErrorPage() const;
    std::string                GetIndex() const;
    std::string                GetRoot() const;
    std::vector<std::string>   GetAllowedMethods() const;
    std::map<const std::string, const LocationConfig>
    GetLocationConfigs() const;

private:
    std::string                                       host_;
    int                                               port_;
    std::string                                       server_name_;
    unsigned int                                      max_client_body_size_;
    std::map<int, std::string>                        error_page_;
    std::string                                       index_;
    std::string                                       root_;
    std::vector<std::string>                          allowed_methods_;
    std::map<const std::string, const LocationConfig> location_configs_;
};

#endif
