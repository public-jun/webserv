#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include <fstream>
#include <map>
#include <string>
#include <vector>

#define DEFAULT_CONF_PATH "./config/default.conf"

class Config {
public:
    static Config*                                       instance();
    static const std::string                             DELIMITERS;
    static const int                                     MAX_PORT_NUM;
    static const std::map<const e_drctv_cd, std::string> DERECTIVE_NAMES;
    static const std::map<const e_drctv_cd, std::vector<std::string> >
                                          DERECTIVE_MAP;
    static const std::vector<std::string> ALLOWED_METHODS;
    static void addServerConfig(const ServerConfig& server_config);
    static const std::map<int, std::vector<const ServerConfig> >
                getServerConfigs();
    static void printConfigs();

protected:
    Config();
    Config(const Config& src);
    ~Config();
    Config& operator=(const Config& src);

private:
    static Config*                                         instance_;
    static std::map<int, std::vector<const ServerConfig> > server_configs_;
    static const std::map<const e_drctv_cd, std::string> createDerectiveNames();
    static const std::map<const e_drctv_cd, std::vector<std::string> >
                                          createDerectiveMap();
    static const std::vector<std::string> createAllowedMethodsVec();
    static void                           setupConfig();
};

#endif
