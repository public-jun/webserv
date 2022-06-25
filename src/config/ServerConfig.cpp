#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {
    this->port_                 = -1;
    this->max_client_body_size_ = 1000000;
    this->root_                 = "./";
}

ServerConfig::ServerConfig(const ServerConfig& src) { *this = src; }

ServerConfig::~ServerConfig() {}

ServerConfig& ServerConfig::operator=(const ServerConfig& src) {
    if (this != &src) {
        this->host_                 = src.host_;
        this->port_                 = src.port_;
        this->server_name_          = src.server_name_;
        this->max_client_body_size_ = src.max_client_body_size_;
        this->error_page_           = src.error_page_;
        this->index_                = src.index_;
        this->allowed_methods_      = src.allowed_methods_;
        this->root_                 = src.root_;
        this->location_configs_     = src.location_configs_;
    }
    return (*this);
}

void ServerConfig::SetHost(std::string host) { this->host_ = host; }

void ServerConfig::SetPort(int port) { this->port_ = port; }

void ServerConfig::SetServerName(std::string server_name) {
    this->server_name_ = server_name;
}

void ServerConfig::SetMaxClientBodySize(unsigned int max_client_body_size) {
    std::cout << "max_client_body_size :" << max_client_body_size << std::endl;
    this->max_client_body_size_ = max_client_body_size;
}

void ServerConfig::SetErrorPage(int status_code, std::string location) {
    this->error_page_[status_code] = location;
}

void ServerConfig::SetAllowedMethods(std::string allowed_method) {
    this->allowed_methods_.push_back(allowed_method);
}

void ServerConfig::SetRoot(std::string root) { this->root_ = root; }

void ServerConfig::SetIndex(std::string index) { this->index_ = index; }

void ServerConfig::SetLocationConfigs(LocationConfig& location_config) {
    this->location_configs_.insert(
        std::make_pair(location_config.GetTarget(), location_config));
}

std::string ServerConfig::GetHost() const { return (this->host_); }

int ServerConfig::GetPort() const { return (this->port_); }

std::string ServerConfig::GetServerName() const { return (this->server_name_); }

unsigned int ServerConfig::GetMaxClientBodySize() const {
    return (this->max_client_body_size_);
}

std::map<int, std::string> ServerConfig::GetErrorPage() const {
    return (this->error_page_);
}

std::vector<std::string> ServerConfig::GetAllowedMethods() const {
    return (this->allowed_methods_);
}

std::string ServerConfig::GetRoot() const { return (this->root_); }

std::string ServerConfig::GetIndex() const { return (this->index_); }

std::map<const std::string, const LocationConfig>
ServerConfig::GetLocationConfigs() const {
    return (this->location_configs_);
}
