#include "SysError.hpp"

#include <cstring>
#include <string>

SysError::SysError(const std::string& func_name, int err_no)
    : std::runtime_error(func_name + " : " +
                         std::string(std::strerror(err_no))) {}

SysError::~SysError() NOEXCEPT {}