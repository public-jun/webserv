#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {
  this->listen_ = -1;
  this->max_client_body_size_ = "1m";
}

ServerConfig::ServerConfig(const ServerConfig &src)  {
  *this = src;
}

ServerConfig::~ServerConfig() {
}

ServerConfig &ServerConfig::operator=(const ServerConfig &src) {
  if (this != &src) {
    this->listen_ = src.listen_;
    this->server_name_ = src.server_name_;
    this->max_client_body_size_ = src.max_client_body_size_;
    this->error_page_ = src.error_page_;
    this->location_configs = src.location_configs;
  }
  return (*this);
}

void ServerConfig::setListen(int listen) {
  this->listen_ = listen;
}

void ServerConfig::setServerName(std::string server_name) {
  this->server_name_ = server_name;
}

void ServerConfig::setMaxClientBodySize(std::string max_client_body_size) {
  this->max_client_body_size_ = max_client_body_size;
}

void ServerConfig::setErrorPage(int status_code, std::string location) {
  this->error_page_[status_code] = location;
}

void ServerConfig::setLocationConfigs(LocationConfig &location_config) {
  this->location_configs.push_back(location_config);
}

int ServerConfig::getListen() const {
  return (this->listen_);
}

std::string ServerConfig::getServerName() const {
  return (this->server_name_);
}

std::string ServerConfig::getMaxClientBodySize() const {
  return (this->max_client_body_size_);
}

std::map<int, std::string> ServerConfig::getErrorPage() const {
  return (this->error_page_);
}

std::vector<LocationConfig> ServerConfig::getLocationConfigs() const {
  return (this->location_configs);
}
