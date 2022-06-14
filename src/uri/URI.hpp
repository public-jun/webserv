#ifndef URI_HPP
#define URI_HPP

#include <string>
#include <sys/stat.h>
#include <utility>

#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

class URI {
public:
    URI(const ServerConfig& server_config, const std::string target);

    ~URI();

    void Init();

    const ServerConfig&             GetServerConfig() const;
    const std::string&              GetRawTarget() const;
    const std::string&              GetRawPath() const;
    const std::string&              GetQuery() const;
    const std::vector<std::string>& GetArgs() const;
    const LocationConfig&           GetLocationConfig() const;
    const std::string&              GetLocalPath() const;
    const struct stat&              GetStat() const;

private:
    URI();

    void divideRawTarget();
    void storeArgsFromQuery();
    void findLocationConfig();
    void storeLocalPath();
    void statLocalPath();

    std::pair<std::string, std::string>
    divideByTheFirstDelimiterFound(std::string str, std::string delimiter);
    std::vector<std::string> split(std::string str, std::string sep);

private:
    const ServerConfig& server_config_;
    const std::string   raw_target_;

    std::string              raw_path_;
    std::string              query_;
    std::vector<std::string> args_;
    std::string              local_path_;
    LocationConfig           location_config_;
    struct stat              stat_buf_;
};

#endif
