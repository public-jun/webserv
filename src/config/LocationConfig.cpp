#include "LocationConfig.hpp"

LocationConfig::LocationConfig() {
  this->auto_index_ = OFF;
  this->return_ = std::make_pair(-1, "");
}

LocationConfig::LocationConfig(const LocationConfig &src)  {
  *this = src;
}

LocationConfig::~LocationConfig() {
}

LocationConfig &LocationConfig::operator=(const LocationConfig &src) {
  if (this != &src) {
    this->allowed_methods_ = src.allowed_methods_;
    this->root_ = src.root_;
    this->auto_index_ = src.auto_index_;
    this->index_ = src.index_;
    this->cgi_extensions_ = src.cgi_extensions_;
    this->return_ = src.return_;
  }
  return (*this);
}

void LocationConfig::setAllowedMethods(std::string allowed_method) {
  this->allowed_methods_.push_back(allowed_method);
}

void LocationConfig::setRoot(std::string root) {
  this->root_ = root;
}

void LocationConfig::setAutoIndex(std::string auto_index) {
  this->auto_index_ = auto_index == "on" ? ON : OFF;
}

void LocationConfig::setIndex(std::string index) {
  this->index_ = index;
}

void LocationConfig::setCgiExtensions(std::string cgi_extension) {
  this->cgi_extensions_.push_back(cgi_extension);
}

void LocationConfig::setReturn(int status_code, std::string url) {
  this->return_ = std::make_pair(status_code, url);
}

std::vector<std::string> LocationConfig::getAllowedMethods() const {
  return (this->allowed_methods_);
}

std::string LocationConfig::getRoot() const {
  return (this->root_);
}

e_auto_index_type LocationConfig::getAutoIndex() const {
  return (this->auto_index_);
}

std::string LocationConfig::getIndex() const {
  return (this->index_);
}

std::vector<std::string> LocationConfig::getCgiExtensions() const {
  return (this->cgi_extensions_);
}

std::pair<int, std::string> LocationConfig::getReturn() const {
  return (this->return_);
}
