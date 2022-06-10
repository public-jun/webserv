#include "Config.hpp"

Config*                                       Config::instance_    = 0;
const int                                     Config::MAX_PORT_NUM = 65535;
const std::string                             Config::DELIMITERS   = " {};";
const std::map<const e_drctv_cd, std::string> Config::DERECTIVE_NAMES =
    Config::createDerectiveNames();
const std::map<const e_drctv_cd, std::vector<std::string> >
    Config::DERECTIVE_MAP = Config::createDerectiveMap();
const std::vector<std::string> Config::ALLOWED_METHODS =
    Config::createAllowedMethodsVec();
std::map<int, const ServerConfig> Config::server_configs_;

Config* Config::instance() {
    if (instance_ == 0)
        instance_ = new Config;
    return instance_;
}

Config::Config() {}

Config::Config(const Config& src) { *this = src; }

Config::~Config() {}

Config& Config::operator=(const Config& src) {
    if (this != &src) {
        (void)src;
    }
    return (*this);
}

const std::map<const e_drctv_cd, std::string> Config::createDerectiveNames() {
    std::map<const e_drctv_cd, std::string> directive_names;

    directive_names[SRVR]           = "server";
    directive_names[LCTN]           = "location";
    directive_names[LSTN]           = "listen";
    directive_names[SRVR_NM]        = "server_name";
    directive_names[MX_CLNT_BDY_SZ] = "max_client_body_size";
    directive_names[ALLWD_MTHD]     = "allowed_method";
    directive_names[ALIAS]          = "alias";
    directive_names[AUTO_INDX]      = "auto_index";
    directive_names[INDX]           = "index";
    directive_names[RTRN]           = "return";
    directive_names[ERR_PG]         = "error_page";
    directive_names[CGI]            = "cgi_extension";
    return (directive_names);
}

const std::map<const e_drctv_cd, std::vector<std::string> >
Config::createDerectiveMap() {
    std::map<const e_drctv_cd, std::vector<std::string> > derective_map;
    std::vector<std::string>                              main_dirs;
    std::vector<std::string>                              server_dirs;
    std::vector<std::string>                              location_dirs;

    main_dirs.push_back(DERECTIVE_NAMES.at(SRVR));
    server_dirs.push_back(DERECTIVE_NAMES.at(LCTN));
    server_dirs.push_back(DERECTIVE_NAMES.at(LSTN));
    server_dirs.push_back(DERECTIVE_NAMES.at(SRVR_NM));
    server_dirs.push_back(DERECTIVE_NAMES.at(MX_CLNT_BDY_SZ));
    server_dirs.push_back(DERECTIVE_NAMES.at(ERR_PG));
    location_dirs.push_back(DERECTIVE_NAMES.at(ALLWD_MTHD));
    location_dirs.push_back(DERECTIVE_NAMES.at(ALIAS));
    location_dirs.push_back(DERECTIVE_NAMES.at(AUTO_INDX));
    location_dirs.push_back(DERECTIVE_NAMES.at(INDX));
    location_dirs.push_back(DERECTIVE_NAMES.at(RTRN));
    location_dirs.push_back(DERECTIVE_NAMES.at(CGI));
    derective_map[MAIN] = main_dirs;
    derective_map[SRVR] = server_dirs;
    derective_map[LCTN] = location_dirs;
    return (derective_map);
}

const std::vector<std::string> Config::createAllowedMethodsVec() {
    std::vector<std::string> allowed_methods;

    allowed_methods.push_back("GET");
    allowed_methods.push_back("POST");
    allowed_methods.push_back("DELETE");
    return (allowed_methods);
}

void Config::addServerConfig(const ServerConfig& server_config) {
    int port = server_config.getListen();
    server_configs_.insert(std::make_pair(port, server_config));
}

void Config::printConfigs() {
    std::map<int, const ServerConfig>::iterator itr = server_configs_.begin();
    while (itr != server_configs_.end()) {
        std::cout << "--------- server config ------------" << std::endl;
        ServerConfig server_config = itr->second;
        std::cout << "listen\t\t\t:" << server_config.getListen() << std::endl;
        std::cout << "server name\t\t:" << server_config.getServerName()
                  << std::endl;
        std::cout << "max client body size\t:"
                  << server_config.getMaxClientBodySize() << std::endl;
        std::map<int, std::string> error_page   = server_config.getErrorPage();
        std::map<int, std::string>::iterator it = error_page.begin();
        while (it != error_page.end()) {
            std::cout << "error page\t\t:" << it->first << " > " << it->second
                      << std::endl;
            it++;
        }

        size_t                      j = -1;
        std::vector<LocationConfig> location_configs =
            server_config.getLocationConfigs();
        while (++j < location_configs.size()) {
            std::cout << "\t--------- location config" << std::endl;
            LocationConfig location_config = location_configs[j];
            std::cout << "\ttarget\t\t:" << location_config.getTarget()
                      << std::endl;
            std::cout << "\tallowed method\t:";
            std::vector<std::string> allowed_methods =
                location_config.getAllowedMethods();
            size_t k = -1;
            while (++k < allowed_methods.size())
                std::cout << " " << allowed_methods[k];
            std::cout << std::endl;
            std::cout << "\talias\t\t:" << location_config.getAlias()
                      << std::endl;
            std::cout << "\tauto index\t:"
                      << (location_config.getAutoIndex() == ON ? "on" : "off")
                      << std::endl;
            std::cout << "\tindex\t\t:" << location_config.getIndex()
                      << std::endl;
            std::cout << "\treturn\t\t:";
            std::pair<int, std::string> rtrn = location_config.getReturn();
            std::cout << rtrn.first << " > " << rtrn.second << std::endl;
            std::cout << "\tcgi extension\t:";
            std::vector<std::string> cgi_extensions =
                location_config.getCgiExtensions();
            size_t l = -1;
            while (++l < cgi_extensions.size())
                std::cout << " " << cgi_extensions[l];
            std::cout << std::endl;
        }
        itr++;
    }
}
