#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <utility>

#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

class URI {
public:
    URI(const ServerConfig& server_config, const std::string target);

    ~URI();

    void Init();

    const ServerConfig&      GetServerConfig() const { return server_config_; }
    const std::string        GetRawTarget() const { return raw_target_; }
    const std::string        GetRawPath() const { return raw_path_; }
    const std::string        GetQuery() const { return query_; }
    std::vector<std::string> GetArgs() { return args_; }

private:
    URI();
    void divideRawTarget();
    std::pair<std::string, std::string>
    divideByTheFirstDelimiterFound(std::string str, std::string delimiter);

    void                     storeArgsFromQuery();
    std::vector<std::string> split(std::string str, std::string sep);

private:
    const ServerConfig& server_config_;
    const std::string   raw_target_;

    std::string              raw_path_;
    std::string              query_;
    std::vector<std::string> args_;
    std::string              local_path_;
    LocationConfig           location_config_;
};

#endif
