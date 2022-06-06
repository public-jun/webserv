#ifndef LOCATION_CONFIG_HPP
# define LOCATION_CONFIG_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include "Utils.hpp"

class LocationConfig {
public:
  LocationConfig();
  LocationConfig(const LocationConfig &src);
  ~LocationConfig();
  LocationConfig &operator=(const LocationConfig &src);

  void setAllowedMethods(std::string allowed_method);
  void setRoot(std::string root);
  void setAutoIndex(std::string auto_index);
  void setIndex(std::string index);
  void setCgiExtensions(std::string cgi_extension);
  void setReturn(int status_code, std::string url);

  std::vector<std::string> getAllowedMethods() const;
  std::string getRoot() const;
  e_auto_index_type getAutoIndex() const;
  std::string getIndex() const;
  std::vector<std::string> getCgiExtensions() const;
  std::pair<int, std::string> getReturn() const;
private:
  std::vector<std::string> allowed_methods_;
  std::string root_;
  e_auto_index_type auto_index_;
  std::string index_;
  std::vector<std::string> cgi_extensions_;
  std::pair<int, std::string> return_;
};

#endif
