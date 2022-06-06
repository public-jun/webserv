#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include "Config.hpp"
#include "ConfigValidator.hpp"

#define DEFAULT_CONF_PATH "./config/default.conf"

class ConfigParser {
public:
  static ConfigParser* instance();
  static void parseConfigFile(const std::string confPath = DEFAULT_CONF_PATH);
protected:
  ConfigParser();
  ConfigParser(const ConfigParser &src);
  ~ConfigParser();
  ConfigParser &operator=(const ConfigParser &src);
private:
  static ConfigParser *instance_;
  typedef std::string::size_type size_pos;
  static std::vector<std::string> tokenize(std::ifstream &ifs);
  static void storeToken(std::vector<std::string> &tokens, const std::string line, size_pos pos[2]);
  static void storeDelimiter(std::vector<std::string> &tokens, const std::string line, size_pos pos[2]);
  static bool validateConfigFile(const std::vector<std::string> &tokens);
  static void setupConfig(std::vector<std::string> tokens);
  static void setupServerConfig(const std::vector<std::string> tokens);
  static void setupLocationConfig(str_vec_itr begin, str_vec_itr end, ServerConfig &server_config);
  static void setupListen(str_vec_itr it[2], ServerConfig &server_config);
  static void setupServerName(str_vec_itr it[2], ServerConfig &server_config);
  static void setupMaxClientBodySize(str_vec_itr it[2], ServerConfig &server_config);
  static void setupErrorPage(str_vec_itr it[2], ServerConfig &server_config);
  static void setupAllowedMethod(str_vec_itr it[2], LocationConfig &location_config);
  static void setupRoot(str_vec_itr it[2], LocationConfig &location_config);
  static void setupAutoIndex(str_vec_itr it[2], LocationConfig &location_config);
  static void setupIndex(str_vec_itr it[2], LocationConfig &location_config);
  static void setupCgiExtensions(str_vec_itr it[2], LocationConfig &location_config);
  static void setupReturn(str_vec_itr it[2], LocationConfig &location_config);
};

#endif
