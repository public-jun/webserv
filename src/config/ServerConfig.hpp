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

    void SetListen(int listen);
    void SetServerName(std::string server_name);
    void SetMaxClientBodySize(std::string client_max_body_size);
    void SetErrorPage(int status_code, std::string location);
    void SetIndex(std::string index);
    void SetRoot(std::string root);
    void SetAllowedMethods(std::string allowed_method);
    void SetLocationConfigs(LocationConfig& location_config);

    int                        GetListen() const;
    std::string                GetServerName() const;
    std::string                GetMaxClientBodySize() const;
    std::map<int, std::string> GetErrorPage() const;
    std::string                GetIndex() const;
    std::string                GetRoot() const;
    std::vector<std::string>   GetAllowedMethods() const;
    std::map<const std::string, const LocationConfig>
    GetLocationConfigs() const;

private:
    int                                               listen_;
    std::string                                       server_name_;
    std::string                                       max_client_body_size_;
    std::map<int, std::string>                        error_page_;
    std::string                                       index_;
    std::string                                       root_;
    std::vector<std::string>                          allowed_methods_;
    std::map<const std::string, const LocationConfig> location_configs_;
};

#endif
