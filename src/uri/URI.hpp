#ifndef URI_HPP
#define URI_HPP

#include <string>

#include "ServerConfig.hpp"

class URI
{
public:
    URI(const ServerConfig& server_config, const std::string& target);
    ~URI();

private:
    URI();

private:
    const ServerConfig& server_config_;
    const std::string& target_;
};

#endif
