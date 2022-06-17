#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {
    this->listen_               = -1;
    this->max_client_body_size_ = "1m";
}

ServerConfig::ServerConfig(const ServerConfig& src) { *this = src; }

ServerConfig::~ServerConfig() {}

ServerConfig& ServerConfig::operator=(const ServerConfig& src) {
    if (this != &src) {
        this->listen_               = src.listen_;
        this->server_name_          = src.server_name_;
        this->max_client_body_size_ = src.max_client_body_size_;
        this->error_page_           = src.error_page_;
        this->location_configs_     = src.location_configs_;
    }
    return (*this);
}

void ServerConfig::SetListen(int listen) { this->listen_ = listen; }

void ServerConfig::SetServerName(std::string server_name) {
    this->server_name_ = server_name;
}

void ServerConfig::SetMaxClientBodySize(std::string max_client_body_size) {
    this->max_client_body_size_ = max_client_body_size;
}

void ServerConfig::SetErrorPage(int status_code, std::string location) {
    this->error_page_[status_code] = location;
}

void ServerConfig::SetLocationConfigs(LocationConfig& location_config) {
    this->location_configs_.insert(
        std::make_pair(location_config.GetTarget(), location_config));
}

int ServerConfig::GetListen() const { return (this->listen_); }

std::string ServerConfig::GetServerName() const { return (this->server_name_); }

std::string ServerConfig::GetMaxClientBodySize() const {
    return (this->max_client_body_size_);
}

std::map<int, std::string> ServerConfig::GetErrorPage() const {
    return (this->error_page_);
}

std::map<const std::string, const LocationConfig>
ServerConfig::GetLocationConfigs() const {
    return (this->location_configs_);
}
