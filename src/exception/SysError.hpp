#ifndef SYSERROR_HPP
#define SYSERROR_HPP

#include <stdexcept>
#include <string>

#define NOEXCEPT throw()

class SysError : public std::runtime_error {
public:
    SysError(const std::string& func_name, int err_no);
    virtual ~SysError() NOEXCEPT;
};

#endif
