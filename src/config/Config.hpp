#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "ServerConfig.hpp"
#include <fstream>
#include <map>
#include <string>
#include <vector>

#define DEFAULT_CONF_PATH "./config/default.conf"

typedef std::vector<const ServerConfig>  server_config_vec;
typedef std::map<int, server_config_vec> server_config_map;

class Config {
public:
    static Config& Instance() {
        static Config instance;
        return instance;
    }
    static const std::string                           DELIMITERS;
    static const int                                   MAX_PORT_NUM;
    static const std::map<const EDrctvCd, std::string> DERECTIVE_NAMES;
    static const std::map<const EDrctvCd, std::vector<std::string> >
                                          DERECTIVE_MAP;
    static const std::vector<std::string> ALLOWED_METHODS;
    static void AddServerConfig(const ServerConfig& server_config);
    static std::map<int, std::vector<const ServerConfig> > GetServerConfigs();
    static void                                            PrintConfigs();

protected:
    Config();
    Config(const Config& src);
    ~Config();
    Config& operator=(const Config& src);

private:
    static Config*                                     instance_;
    static server_config_map                           server_configs_;
    static const std::map<const EDrctvCd, std::string> createDerectiveNames();
    static const std::map<const EDrctvCd, std::vector<std::string> >
                                          createDerectiveMap();
    static const std::vector<std::string> createAllowedMethodsVec();
    static void                           setupConfig();
};

#endif
