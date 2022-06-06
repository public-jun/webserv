#ifndef CONFIG_VALIDATOR_HPP
# define CONFIG_VALIDATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <iostream>
#include "Utils.hpp"
#include "Config.hpp"

#define ERR_MSG_IVLD_FILE "invalid file"
#define ERR_MSG_IVLD_NUM_BRCKT "invalid number of braces"
#define ERR_MSG_UNEXPCTD_TKN "unexpected token"
#define ERR_MSG_EMPTY_FILE "empty file"
#define ERR_MSG_DRCTV_NOT_EXST "derective not exist"
#define ERR_MSG_DPLCT_DRCTV "duplicated derective"
#define ERR_MSG_INVLD_DRCTV "invalid derective"
#define ERR_MSG_INVLD_DRCTV_PLC "invalid derective place"
#define ERR_MSG_INVLD_VALUE_NUM "invalid value num"
#define ERR_MSG_DPLCTD_PORT "duplicated port"
#define ERR_MSG_INVLD_LSTN "invalid listen"
#define ERR_MSG_INVLD_ERR_PG "invalid error page"
#define ERR_MSG_INVLD_ALWD_MTHD "invalid allowed method"
#define ERR_MSG_INVLD_AUTO_INDEX "invalid auto index"
#define ERR_MSG_INVLD_RTRN "invalid return"
#define ERR_MSG_DPLCTD_LCTN "duplicated location"

class ConfigValidator {
public:
  static ConfigValidator* instance();
  static void validateConfigFile(const std::vector<std::string> tokens);
protected:
  ConfigValidator();
  ConfigValidator(const ConfigValidator &src);
  ~ConfigValidator();
  ConfigValidator &operator=(const ConfigValidator &src);
private:
  static ConfigValidator *instance_;
  static std::map<int, std::string> listen_server_name_map;
  static bool isValidBraceNum(const std::vector<std::string> tokens);
  static bool isValidBracePlace(const std::vector<std::string> tokens, const std::string target);
  static bool isServerExist(const std::vector<std::string> tokens);
  static void checkDerective(const std::vector<std::string> tokens, const std::string target);
  static void scanDerective(str_vec_itr it[2], const std::string target);
  static bool isDerectiveDuplicated(str_vec_itr begin, str_vec_itr end);
  static bool isDirective(str_vec_itr it);
  static e_drctv_cd getDirectiveCode(std::string target);
  static bool isValidDirectiveName(str_vec_itr it, std::string target);
  static void checkMainDirectives(const std::vector<std::string> tokens);
  static bool isValidValueNum(str_vec_itr it);
  static bool isLocationDuplicated(str_vec_itr begin, str_vec_itr end);
  static bool isValidErrorPage(str_vec_itr begin);
  static bool isValidAllowedMethod(str_vec_itr begin);
  static bool isValidAutoIndex(str_vec_itr begin);
  static bool isValidReturn(str_vec_itr begin);
  static bool isValidListen(str_vec_itr begin);
};

#endif
