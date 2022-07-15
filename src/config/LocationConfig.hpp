#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include "Utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class LocationConfig {
public:
    LocationConfig();
    LocationConfig(const LocationConfig& src);
    ~LocationConfig();
    LocationConfig& operator=(const LocationConfig& src);

    void SetTarget(std::string target);
    void SetAllowedMethods(std::string allowed_method);
    void SetAlias(std::string alias);
    void SetAutoIndex(std::string auto_index);
    void SetIndex(std::string index);
    void SetRoot(std::string root);
    void SetCgiExtensions(std::string cgi_extension);
    void SetReturn(int status_code, std::string url);
    void SetUploadPath(std::string upload_path);

    std::string                 GetTarget() const;
    std::vector<std::string>    GetAllowedMethods() const;
    std::string                 GetAlias() const;
    EAutoIndexType              GetAutoIndex() const;
    std::string                 GetIndex() const;
    std::string                 GetRoot() const;
    std::vector<std::string>    GetCgiExtensions() const;
    std::pair<int, std::string> GetReturn() const;
    std::string                 GetUploadPath() const;

private:
    std::string                 target_;
    std::vector<std::string>    allowed_methods_;
    std::string                 alias_;
    EAutoIndexType              auto_index_;
    std::string                 index_;
    std::string                 root_;
    std::vector<std::string>    cgi_extensions_;
    std::pair<int, std::string> return_;
    std::string                 upload_path_;
};

#endif
