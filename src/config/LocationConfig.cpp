#include "LocationConfig.hpp"

LocationConfig::LocationConfig() {
    this->auto_index_  = OFF;
    this->return_      = std::make_pair(-1, "");
    this->upload_path_ = "./upload_files";
}

LocationConfig::LocationConfig(const LocationConfig& src) { *this = src; }

LocationConfig::~LocationConfig() {}

LocationConfig& LocationConfig::operator=(const LocationConfig& src) {
    if (this != &src) {
        this->target_          = src.target_;
        this->allowed_methods_ = src.allowed_methods_;
        this->alias_           = src.alias_;
        this->auto_index_      = src.auto_index_;
        this->index_           = src.index_;
        this->root_            = src.root_;
        this->cgi_extensions_  = src.cgi_extensions_;
        this->return_          = src.return_;
        this->upload_path_     = src.upload_path_;
    }
    return (*this);
}

void LocationConfig::SetTarget(std::string target) { this->target_ = target; }

void LocationConfig::SetAllowedMethods(std::string allowed_method) {
    this->allowed_methods_.push_back(allowed_method);
}

void LocationConfig::SetAlias(std::string alias) { this->alias_ = alias; }

void LocationConfig::SetRoot(std::string root) { this->root_ = root; }

void LocationConfig::SetAutoIndex(std::string auto_index) {
    this->auto_index_ = auto_index == "on" ? ON : OFF;
}

void LocationConfig::SetIndex(std::string index) { this->index_ = index; }

void LocationConfig::SetCgiExtensions(std::string cgi_extension) {
    this->cgi_extensions_.push_back(cgi_extension);
}

void LocationConfig::SetReturn(int status_code, std::string url) {
    this->return_ = std::make_pair(status_code, url);
}

void LocationConfig::SetUploadPath(std::string upload_path) {
    this->upload_path_ = upload_path;
}

std::string LocationConfig::GetTarget() const { return (this->target_); }

std::vector<std::string> LocationConfig::GetAllowedMethods() const {
    return (this->allowed_methods_);
}

std::string LocationConfig::GetAlias() const { return (this->alias_); }

std::string LocationConfig::GetRoot() const { return (this->root_); }

EAutoIndexType LocationConfig::GetAutoIndex() const {
    return (this->auto_index_);
}

std::string LocationConfig::GetIndex() const { return (this->index_); }

std::vector<std::string> LocationConfig::GetCgiExtensions() const {
    return (this->cgi_extensions_);
}

std::pair<int, std::string> LocationConfig::GetReturn() const {
    return (this->return_);
}

std::string LocationConfig::GetUploadPath() const {
    return (this->upload_path_);
}
