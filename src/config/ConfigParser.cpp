#include "ConfigParser.hpp"

ConfigParser* ConfigParser::instance_ = 0;

ConfigParser* ConfigParser::Instance() {
    if (instance_ == 0)
        instance_ = new ConfigParser;
    return instance_;
}

ConfigParser::ConfigParser() {}

ConfigParser::ConfigParser(const ConfigParser& src) { *this = src; }

ConfigParser::~ConfigParser() {}

ConfigParser& ConfigParser::operator=(const ConfigParser& src) {
    if (this != &src) {
        (void)src;
    }
    return (*this);
}

void ConfigParser::storeToken(std::vector<std::string>& tokens,
                              const std::string line, size_pos pos[2]) {
    std::string token = line.substr(pos[BEGIN], pos[END] - pos[BEGIN]);
    token.erase(0, token.find_first_not_of(" "));
    token.erase(token.find_last_not_of(" ") + 1);
    pos[BEGIN] = pos[END];
    if (token.size() > 0)
        tokens.push_back(token);
}

void ConfigParser::storeDelimiter(std::vector<std::string>& tokens,
                                  const std::string line, size_pos pos[2]) {
    if (pos[BEGIN] == std::string::npos)
        return;
    while (Config::DELIMITERS.find(line[pos[BEGIN]]) != std::string::npos) {
        pos[END]++;
        storeToken(tokens, line, pos);
    }
}

std::vector<std::string> ConfigParser::tokenize(std::ifstream& ifs) {
    std::string              line;
    std::vector<std::string> tokens;
    std::string::size_type   pos[2];

    while (std::getline(ifs, line)) {
        pos[BEGIN] = 0;
        pos[END]   = 0;
        while (pos[BEGIN] != std::string::npos) {
            pos[END] = line.find_first_of(Config::DELIMITERS, pos[BEGIN]);
            storeToken(tokens, line, pos);
            storeDelimiter(tokens, line, pos);
        }
    }
    return (tokens);
}

void ConfigParser::ParseConfigFile(const std::string conf_path) {
    std::ifstream            ifs(conf_path);
    std::vector<std::string> tokens;

    if (!ifs)
        throw(std::runtime_error(ERR_MSG_IVLD_FILE));
    tokens = tokenize(ifs);
    ifs.close();
    try {
        ConfigValidator::ValidateConfigFile(tokens);
        setupConfig(tokens);
    } catch (...) { throw; }
}

void ConfigParser::setupConfig(std::vector<std::string> tokens) {
    setupServerConfig(tokens);
}

void ConfigParser::setupServerConfig(const std::vector<std::string> tokens) {
    str_vec_itr it[2];
    it[BEGIN] = std::find(tokens.begin(), tokens.end(),
                          Config::DERECTIVE_NAMES.at(SRVR));

    while (it[BEGIN] != tokens.end()) {
        it[BEGIN] = std::find(it[BEGIN], tokens.end(), "{");
        it[END]   = it[BEGIN];
        Utils::FindEndBrace(++it[END]);

        ServerConfig server_config = ServerConfig();

        setupListen(it, server_config);
        setupServerName(it, server_config);
        setupMaxClientBodySize(it, server_config);
        setupErrorPage(it, server_config);
        setupIndex(it, server_config);
        setupAllowedMethod(it, server_config);
        setupRoot(it, server_config);
        setupLocationConfig(it[BEGIN], it[END], server_config);
        Config::AddServerConfig(server_config);
        it[BEGIN] = std::find(it[BEGIN], tokens.end(),
                              Config::DERECTIVE_NAMES.at(SRVR));
    }
}

void ConfigParser::setupLocationConfig(str_vec_itr begin, str_vec_itr end,
                                       ServerConfig& server_config) {
    str_vec_itr it[2];
    it[BEGIN] = std::find(begin, end, Config::DERECTIVE_NAMES.at(LCTN));
    while (it[BEGIN] != end) {
        std::string target = *++it[BEGIN];
        it[BEGIN]          = std::find(it[BEGIN], end, "{");
        it[END]            = it[BEGIN];
        Utils::FindEndBrace(++it[END]);

        LocationConfig location_config = LocationConfig();
        setupTarget(target, location_config);
        setupAllowedMethod(it, location_config);
        setupAlias(it, location_config);
        setupAutoIndex(it, location_config);
        setupIndex(it, location_config);
        setupRoot(it, location_config);
        setupCgiExtensions(it, location_config);
        setupReturn(it, location_config);
        setupUploadPath(it, location_config);

        server_config.SetLocationConfigs(location_config);
        it[BEGIN] = std::find(it[BEGIN], end, Config::DERECTIVE_NAMES.at(LCTN));
    }
}

void ConfigParser::setupListen(str_vec_itr it[2], ServerConfig& server_config) {
    str_vec_itr listen =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(LSTN));
    if (listen != it[END]) {
        size_t      index = (*++listen).find(":");
        std::string host;
        std::string port;
        if (index == std::string::npos) {
            host = "127.0.0.1";
            port = *listen;
        } else {
            host = (*listen).substr(0, index);
            port = (*listen).substr(index + 1);
        }
        server_config.SetHost(host);
        server_config.SetPort(std::strtol(port.c_str(), NULL, 10));
    }
}

void ConfigParser::setupServerName(str_vec_itr   it[2],
                                   ServerConfig& server_config) {
    str_vec_itr server_name =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(SRVR_NM));
    if (server_name != it[END])
        server_config.SetServerName(*++server_name);
}

void ConfigParser::setupMaxClientBodySize(str_vec_itr   it[2],
                                          ServerConfig& server_config) {
    str_vec_itr max_client_body_size = std::find(
        it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(MX_CLNT_BDY_SZ));
    if (max_client_body_size != it[END])
        server_config.SetMaxClientBodySize(*++max_client_body_size);
}

void ConfigParser::setupErrorPage(str_vec_itr   it[2],
                                  ServerConfig& server_config) {
    str_vec_itr error_page = it[BEGIN];

    while (error_page != it[END]) {
        error_page =
            std::find(error_page, it[END], Config::DERECTIVE_NAMES.at(ERR_PG));
        if (error_page != it[END])
            server_config.SetErrorPage(
                std::strtol((*++error_page).c_str(), NULL, 10), *++error_page);
    }
}

void ConfigParser::setupTarget(std::string     target,
                               LocationConfig& location_config) {
    location_config.SetTarget(target);
}

void ConfigParser::setupAutoIndex(str_vec_itr     it[2],
                                  LocationConfig& location_config) {
    str_vec_itr auto_index =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(AUTO_INDX));
    if (auto_index != it[END])
        location_config.SetAutoIndex(*++auto_index);
}

void ConfigParser::setupCgiExtensions(str_vec_itr     it[2],
                                      LocationConfig& location_config) {
    str_vec_itr cgi_extensions =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(CGI));
    if (cgi_extensions != it[END])
        while (*++cgi_extensions != ";")
            location_config.SetCgiExtensions(*cgi_extensions);
}

void ConfigParser::setupReturn(str_vec_itr     it[2],
                               LocationConfig& location_config) {
    str_vec_itr rtrn =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(RTRN));
    if (rtrn != it[END])
        location_config.SetReturn(strtol((*++rtrn).c_str(), NULL, 10), *++rtrn);
}

void ConfigParser::setupUploadPath(str_vec_itr it[2], LocationConfig& target) {
    str_vec_itr upload_path =
        std::find(it[BEGIN], it[END], Config::DERECTIVE_NAMES.at(UPLD_PATH));
    if (upload_path != it[END])
        target.SetUploadPath(*++upload_path);
}
