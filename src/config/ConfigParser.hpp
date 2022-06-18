#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "Config.hpp"
#include "ConfigValidator.hpp"
#include <fstream>
#include <string>
#include <vector>

#define DEFAULT_CONF_PATH "./config/default.conf"

class ConfigParser {
public:
    static ConfigParser* Instance();
    static void
    ParseConfigFile(const std::string conf_path = DEFAULT_CONF_PATH);

protected:
    ConfigParser();
    ConfigParser(const ConfigParser& src);
    ~ConfigParser();
    ConfigParser& operator=(const ConfigParser& src);

private:
    static ConfigParser*            instance_;
    typedef std::string::size_type  size_pos;
    static std::vector<std::string> tokenize(std::ifstream& ifs);
    static void                     storeToken(std::vector<std::string>& tokens,
                                               const std::string line, size_pos pos[2]);
    static void storeDelimiter(std::vector<std::string>& tokens,
                               const std::string line, size_pos pos[2]);
    static bool validateConfigFile(const std::vector<std::string>& tokens);
    static void setupConfig(std::vector<std::string> tokens);
    static void setupServerConfig(const std::vector<std::string> tokens);
    static void setupLocationConfig(str_vec_itr begin, str_vec_itr end,
                                    ServerConfig& server_config);
    static void setupListen(str_vec_itr it[2], ServerConfig& server_config);
    static void setupServerName(str_vec_itr it[2], ServerConfig& server_config);
    static void setupMaxClientBodySize(str_vec_itr   it[2],
                                       ServerConfig& server_config);
    static void setupErrorPage(str_vec_itr it[2], ServerConfig& server_config);
    static void setupTarget(std::string     target,
                            LocationConfig& location_config);
    template <class T>
    static void setupAllowedMethod(str_vec_itr it[2], T& location_config);
    template <class T>
    static void setupAlias(str_vec_itr it[2], T& location_config);
    template <class T>
    static void setupRoot(str_vec_itr it[2], T& location_config);
    static void setupAutoIndex(str_vec_itr     it[2],
                               LocationConfig& location_config);
    template <class T>
    static void setupIndex(str_vec_itr it[2], T& location_config);
    static void setupCgiExtensions(str_vec_itr     it[2],
                                   LocationConfig& location_config);
    static void setupReturn(str_vec_itr it[2], LocationConfig& location_config);
};

template <class T>
void ConfigParser::setupIndex(str_vec_itr it[2], T& target) {
    str_vec_itr index =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(INDX));
    if (index != it[END])
        target.SetIndex(*++index);
}

template <class T>
void ConfigParser::setupAllowedMethod(str_vec_itr it[2], T& target) {
    str_vec_itr allowed_method =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(ALLWD_MTHD));
    if (allowed_method != it[END])
        while (*++allowed_method != ";")
            target.SetAllowedMethods(*allowed_method);
}

template <class T>
void ConfigParser::setupAlias(str_vec_itr it[2], T& target) {
    str_vec_itr alias =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(ALIAS));
    if (alias != it[END])
        target.SetAlias(*++alias);
}

template <class T>
void ConfigParser::setupRoot(str_vec_itr it[2], T& target) {
    str_vec_itr root =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(ROOT));
    if (root != it[END])
        target.SetRoot(*++root);
}
#endif
