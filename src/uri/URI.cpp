#include "URI.hpp"

#include <string>

#include "ServerConfig.hpp"

URI::URI(const ServerConfig& server_config, const std::string& target)
    : server_config_(server_config), target_(target) {}

URI::~URI() {}