#include "ConfigValidator.hpp"

ConfigValidator* ConfigValidator::instance_ = 0;

ConfigValidator* ConfigValidator::Instance() {
    if (instance_ == 0)
        instance_ = new ConfigValidator;
    return instance_;
}

ConfigValidator::ConfigValidator() {}

ConfigValidator::ConfigValidator(const ConfigValidator& src) { *this = src; }

ConfigValidator::~ConfigValidator() {}

ConfigValidator& ConfigValidator::operator=(const ConfigValidator& src) {
    if (this != &src) {
        (void)src;
    }
    return (*this);
}

void ConfigValidator::ValidateConfigFile(
    const std::vector<std::string> tokens) {
    if (tokens.size() == 0)
        return;
    if (!isValidBraceNum(tokens))
        throw(std::runtime_error(ERR_MSG_IVLD_NUM_BRCKT));
    if (!isValidBracePlace(tokens, Config::DERECTIVE_NAMES.at(SRVR)))
        throw(std::runtime_error(ERR_MSG_UNEXPCTD_TKN));
    if (!isValidBracePlace(tokens, Config::DERECTIVE_NAMES.at(LCTN)))
        throw(std::runtime_error(ERR_MSG_UNEXPCTD_TKN));

    try {
        checkMainDirectives(tokens);
        checkDerective(tokens, Config::DERECTIVE_NAMES.at(SRVR));
        checkDerective(tokens, Config::DERECTIVE_NAMES.at(LCTN));
    } catch (...) { throw; }
}

bool ConfigValidator::isValidBraceNum(const std::vector<std::string> tokens) {
    int open_brace_num  = std::count(tokens.begin(), tokens.end(), "{");
    int close_brace_num = std::count(tokens.begin(), tokens.end(), "}");
    return (open_brace_num == close_brace_num);
}

bool ConfigValidator::isValidBracePlace(const std::vector<std::string> tokens,
                                        std::string                    target) {
    int         pad = target == Config::DERECTIVE_NAMES.at(SRVR) ? 1 : 2;
    int         cnt = -1;
    str_vec_itr it  = tokens.begin();
    while ((it = std::find(it, tokens.end(), target)) != tokens.end()) {
        while (++cnt < pad)
            if (++it == tokens.end())
                return (false);
        if (*it != "{")
            return (false);
        cnt = -1;
    }
    return (true);
}

bool ConfigValidator::isServerExist(const std::vector<std::string> tokens) {
    return (std::find(tokens.begin(), tokens.end(),
                      Config::DERECTIVE_NAMES.at(SRVR)) != tokens.end());
}

void ConfigValidator::checkDerective(const std::vector<std::string> tokens,
                                     const std::string              target) {
    str_vec_itr it[2];
    it[BEGIN] = std::find(tokens.begin(), tokens.end(), target);

    while (it[BEGIN] != tokens.end()) {
        it[BEGIN] = std::find(it[BEGIN], tokens.end(), "{");
        it[END]   = it[BEGIN];
        Utils::FindEndBrace(++it[END]);
        try {
            scanDerective(it, target);
        } catch (...) { throw; }
        it[BEGIN] = std::find(it[BEGIN], tokens.end(), target);
    }
}

void ConfigValidator::scanDerective(str_vec_itr       it[2],
                                    const std::string target) {
    while (++it[BEGIN] != it[END]) {
        if (*it[BEGIN] == "{")
            Utils::FindEndBrace(++it[BEGIN]);
        if (!isValidDirectiveName(it[BEGIN], target))
            throw(std::runtime_error(ERR_MSG_INVLD_DRCTV));
        if (isDerectiveDuplicated(it[BEGIN], it[END]))
            throw(std::runtime_error(ERR_MSG_DPLCT_DRCTV));
        if (!isValidValueNum(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_VALUE_NUM));
        if (!isValidListen(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_LSTN));
        if (!isValidErrorPage(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_ERR_PG));
        if (!isValidAllowedMethod(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_ALWD_MTHD));
        if (!isValidAutoIndex(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_AUTO_INDEX));
        if (!isValidReturn(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_RTRN));
        if (!isValidMaxClientBodySize(it[BEGIN]))
            throw(std::runtime_error(ERR_MSG_INVLD_MX_CLNT_BDY_SZ));
        if (!isLocationDuplicated(it[BEGIN], it[END]))
            throw(std::runtime_error(ERR_MSG_DPLCTD_LCTN));
    }
}

bool ConfigValidator::isDerectiveDuplicated(str_vec_itr begin,
                                            str_vec_itr end) {
    std::string target = *begin;
    if (!isDirective(begin) || target == Config::DERECTIVE_NAMES.at(LCTN) ||
        target == Config::DERECTIVE_NAMES.at(ERR_PG))
        return (false);
    while (++begin != end) {
        if (*begin == "{")
            Utils::FindEndBrace(++begin);
        if (!isDirective(begin))
            continue;
        if (target == *begin)
            return (true);
    }
    return (false);
}

bool ConfigValidator::isDirective(str_vec_itr it) {
    return (Config::DELIMITERS.find(*(it - 1)) != std::string::npos &&
            Config::DELIMITERS.find(*it) == std::string::npos);
}

EDrctvCd ConfigValidator::getDirectiveCode(std::string target) {
    std::map<const EDrctvCd, std::string> directive_names =
        Config::DERECTIVE_NAMES;
    std::map<const EDrctvCd, std::string>::iterator it =
        directive_names.begin();
    while (it != Config::DERECTIVE_NAMES.end() && it->second != target)
        it++;
    return (it->first);
}

bool ConfigValidator::isValidDirectiveName(str_vec_itr it, std::string target) {
    if (!isDirective(it))
        return (true);
    std::vector<std::string> dirs =
        Config::DERECTIVE_MAP.at(getDirectiveCode(target));
    return (std::find(dirs.begin(), dirs.end(), *it) != dirs.end());
}

void ConfigValidator::checkMainDirectives(
    const std::vector<std::string> tokens) {
    str_vec_itr it[2];
    it[BEGIN] = tokens.begin();
    it[END]   = tokens.end();
    while (it[BEGIN] != it[END]) {
        if (*it[BEGIN] == "{")
            Utils::FindEndBrace(++it[BEGIN]);
        if (*it[BEGIN] != "}" && *it[BEGIN] != Config::DERECTIVE_NAMES.at(SRVR))
            throw(std::runtime_error(ERR_MSG_INVLD_DRCTV));
        it[BEGIN]++;
    }
}

bool ConfigValidator::isValidValueNum(str_vec_itr it) {
    size_t      cnt = 0;
    std::string directive;

    if (!isDirective(it))
        return (true);
    directive = *it++;
    while (Config::DELIMITERS.find(*it++) == std::string::npos)
        cnt++;
    if (cnt == 0)
        return (false);
    if (directive == Config::DERECTIVE_NAMES.at(CGI))
        return (true);
    if (cnt == 2 && directive != Config::DERECTIVE_NAMES.at(ERR_PG) &&
        directive != Config::DERECTIVE_NAMES.at(ALLWD_MTHD) &&
        directive != Config::DERECTIVE_NAMES.at(RTRN))
        return (false);
    if ((directive == Config::DERECTIVE_NAMES.at(ERR_PG) ||
         directive == Config::DERECTIVE_NAMES.at(RTRN)) &&
        cnt != 2)
        return (false);
    if (cnt == 3 && directive != Config::DERECTIVE_NAMES.at(ALLWD_MTHD))
        return (false);
    if (cnt >= 4)
        return (false);
    return (true);
}

bool ConfigValidator::isLocationDuplicated(str_vec_itr begin, str_vec_itr end) {
    if (*begin != Config::DERECTIVE_NAMES.at(LCTN))
        return (true);
    std::string target = *++begin;
    while (++begin != end) {
        if (!isDirective(begin) || *begin != Config::DERECTIVE_NAMES.at(LCTN))
            continue;
        if (target == *++begin)
            return (false);
    }
    return (true);
}

bool ConfigValidator::isValidErrorPage(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(ERR_PG))
        return (true);
    try {
        if (!isDigit(*++begin))
            return (false);
        int status_code = std::strtol((*begin).c_str(), NULL, 10);
        if (!(300 <= status_code && status_code <= 599))
            return (false);
    } catch (...) { return (false); }
    return (true);
}

bool ConfigValidator::isValidAllowedMethod(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(ALLWD_MTHD))
        return (true);
    while (*++begin != ";")
        if (std::find(Config::ALLOWED_METHODS.begin(),
                      Config::ALLOWED_METHODS.end(),
                      *begin) == Config::ALLOWED_METHODS.end())
            return (false);
    return (true);
}

bool ConfigValidator::isValidAutoIndex(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(AUTO_INDX))
        return (true);
    begin++;
    return (*begin == "on" || *begin == "off");
}

bool ConfigValidator::isValidReturn(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(RTRN))
        return (true);
    try {
        if (!isDigit(*++begin))
            return (false);
        int status_code = std::strtol((*begin).c_str(), NULL, 10);
        if (!(0 <= status_code && status_code <= 599))
            return (false);
    } catch (...) { return (false); }
    return (true);
}

bool ConfigValidator::isValidListen(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(LSTN))
        return (true);
    if (!isDigit(*++begin)) {
        if (!isValidHostAndPort(*begin))
            return (false);
    } else {
        if (!isValidPort(*begin))
            return (false);
    }
    return (true);
}

bool ConfigValidator::isValidHostAndPort(const std::string& target) {
    size_t index = target.find(":");
    if (index == std::string::npos)
        return (false);
    std::string host = target.substr(0, index);
    std::string port = target.substr(index + 1);
    if (!isValidPort(port))
        return (false);
    if (!isValidHost(host))
        return (false);
    return (true);
}

bool ConfigValidator::isValidHost(const std::string& host) {
    size_t      offset = 0;
    std::string addr;
    size_t      period_index;

    if (std::count(host.begin(), host.end(), '.') != 3)
        return (false);
    while ((period_index = host.find(".", offset)) != std::string::npos) {
        addr = host.substr(offset, period_index - offset);
        if (!isDigit(addr) || !isValidAddr(addr))
            return (false);
        offset = period_index + 1;
    }
    addr = host.substr(offset);
    if (!isDigit(addr) || !isValidAddr(addr))
        return (false);
    return (true);
}

bool ConfigValidator::isValidPort(const std::string& target) {
    int port = strtol(target.c_str(), NULL, 10);
    return (0 <= port && port <= Config::MAX_PORT_NUM);
}

bool ConfigValidator::isValidAddr(const std::string& target) {
    int addr = strtol(target.c_str(), NULL, 10);
    return (0 <= addr && addr <= 255);
}

bool ConfigValidator::isValidMaxClientBodySize(str_vec_itr begin) {
    if (*begin != Config::DERECTIVE_NAMES.at(MX_CLNT_BDY_SZ))
        return (true);
    int len = (*++begin).length();
    if (len < 2)
        return (false);
    std::string units = "kKmM";
    std::string size  = (*begin).substr(0, len - 1);
    std::string unit  = (*begin).substr(len - 1, 1);
    if (!isDigit(size))
        return (false);
    if (unit.length() != 1 || units.find(unit) == std::string::npos)
        return (false);
    int mag = (unit == "K" || unit == "k") ? 1000 : 1000 * 1000;
    if (strtol(size.c_str(), NULL, 10) * mag > 1000 * 1000 * 1000)
        return (false);
    return (true);
}

bool ConfigValidator::isDigit(const std::string& str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}
