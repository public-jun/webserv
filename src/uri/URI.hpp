#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <utility>

#include "ServerConfig.hpp"

class URI {
public:
    URI(const ServerConfig& server_config, const std::string target);

    ~URI();

    void Init();

    const ServerConfig& GetServerConfig() const { return server_config_; }
    const std::string   GetRawTarget() const { return raw_target_; }
    const std::string   GetRawPath() const { return raw_path_; }
    const std::string   GetQuery() const { return query_; }

private:
    URI();
    void divideRawTarget();
    std::pair<std::string, std::string>
    divideByTheFirstDelimiterFound(std::string str, std::string delimiter);

    // void parseQuery();

private:
    const ServerConfig& server_config_;
    const std::string   raw_target_;

    std::string              raw_path_;
    std::string              query_;
    std::vector<std::string> args;
};

#endif
