#include "Config.hpp"

Config*                                         Config::instance_    = 0;
const int                                       Config::MAX_PORT_NUM = 65535;
const std::string                               Config::DELIMITERS   = " {};";
std::map<int, std::vector<const ServerConfig> > Config::server_configs_;
const std::map<const EDrctvCd, std::string>     Config::DERECTIVE_NAMES =
    Config::createDerectiveNames();
const std::map<const EDrctvCd, std::vector<std::string> >
    Config::DERECTIVE_MAP = Config::createDerectiveMap();
const std::vector<std::string> Config::ALLOWED_METHODS =
    Config::createAllowedMethodsVec();

Config* Config::Instance() {
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

const std::map<const EDrctvCd, std::string> Config::createDerectiveNames() {
    std::map<const EDrctvCd, std::string> directive_names;

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
    directive_names[ROOT]           = "root";
    directive_names[UPLD_PATH]      = "upload_path";
    return (directive_names);
}

const std::map<const EDrctvCd, std::vector<std::string> >
Config::createDerectiveMap() {
    std::map<const EDrctvCd, std::vector<std::string> > derective_map;
    std::vector<std::string>                            main_dirs;
    std::vector<std::string>                            server_dirs;
    std::vector<std::string>                            location_dirs;

    main_dirs.push_back(DERECTIVE_NAMES.at(SRVR));
    server_dirs.push_back(DERECTIVE_NAMES.at(LCTN));
    server_dirs.push_back(DERECTIVE_NAMES.at(LSTN));
    server_dirs.push_back(DERECTIVE_NAMES.at(SRVR_NM));
    server_dirs.push_back(DERECTIVE_NAMES.at(MX_CLNT_BDY_SZ));
    server_dirs.push_back(DERECTIVE_NAMES.at(ERR_PG));
    server_dirs.push_back(DERECTIVE_NAMES.at(ALLWD_MTHD));
    server_dirs.push_back(DERECTIVE_NAMES.at(INDX));
    server_dirs.push_back(DERECTIVE_NAMES.at(ROOT));
    location_dirs.push_back(DERECTIVE_NAMES.at(ALLWD_MTHD));
    location_dirs.push_back(DERECTIVE_NAMES.at(ALIAS));
    location_dirs.push_back(DERECTIVE_NAMES.at(AUTO_INDX));
    location_dirs.push_back(DERECTIVE_NAMES.at(INDX));
    location_dirs.push_back(DERECTIVE_NAMES.at(RTRN));
    location_dirs.push_back(DERECTIVE_NAMES.at(CGI));
    location_dirs.push_back(DERECTIVE_NAMES.at(ROOT));
    location_dirs.push_back(DERECTIVE_NAMES.at(UPLD_PATH));
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

void Config::AddServerConfig(const ServerConfig& server_config) {
    int port = server_config.GetPort();
    std::map<int, std::vector<const ServerConfig> >::iterator target =
        server_configs_.find(port);

    if (target == server_configs_.end()) {
        std::vector<const ServerConfig> new_servers(1, server_config);
        server_configs_.insert(std::make_pair(port, new_servers));
    } else
        target->second.push_back(server_config);
}

std::map<int, std::vector<const ServerConfig> > Config::GetServerConfigs() {
    return server_configs_;
}

void Config::PrintConfigs() {
    std::map<int, std::vector<const ServerConfig> >::iterator map_itr =
        server_configs_.begin();
    while (map_itr != server_configs_.end()) {
        std::vector<const ServerConfig>::iterator sc_itr =
            map_itr->second.begin();
        std::cout << "-------------------------------------------" << std::endl;
        while (sc_itr != map_itr->second.end()) {
            std::cout << "--------- server config ------------" << std::endl;
            ServerConfig server_config = *sc_itr;
            std::cout << "host\t\t\t:" << server_config.GetHost() << std::endl;
            std::cout << "port\t\t\t:" << server_config.GetPort() << std::endl;
            std::cout << "server name\t\t:" << server_config.GetServerName()
                      << std::endl;
            std::cout << "max client body size\t:"
                      << server_config.GetMaxClientBodySize() << std::endl;
            std::cout << "index\t\t\t:" << server_config.GetIndex()
                      << std::endl;
            std::cout << "allowed method\t\t:";
            std::vector<std::string> allowed_methods =
                server_config.GetAllowedMethods();
            size_t k = -1;
            while (++k < allowed_methods.size())
                std::cout << allowed_methods[k] << " ";
            std::cout << std::endl;
            std::cout << "root\t\t\t:" << server_config.GetRoot() << std::endl;
            std::map<int, std::string> error_page =
                server_config.GetErrorPage();
            std::map<int, std::string>::iterator it = error_page.begin();
            while (it != error_page.end()) {
                std::cout << "error page\t\t:" << it->first << " > "
                          << it->second << std::endl;
                it++;
            }

            std::map<const std::string, const LocationConfig> location_configs =
                server_config.GetLocationConfigs();
            std::map<const std::string, const LocationConfig>::iterator
                location_it = location_configs.begin();
            while (location_it != location_configs.end()) {
                std::cout << "\t--------- location config" << std::endl;
                LocationConfig location_config = location_it->second;
                std::cout << "\ttarget\t\t:" << location_config.GetTarget()
                          << std::endl;
                std::cout << "\tallowed method\t:";
                std::vector<std::string> allowed_methods =
                    location_config.GetAllowedMethods();
                size_t k = -1;
                while (++k < allowed_methods.size())
                    std::cout << allowed_methods[k] << " ";
                std::cout << std::endl;
                std::cout << "\talias\t\t:" << location_config.GetAlias()
                          << std::endl;
                std::cout << "\tauto index\t:"
                          << (location_config.GetAutoIndex() == ON ? "on"
                                                                   : "off")
                          << std::endl;
                std::cout << "\tindex\t\t:" << location_config.GetIndex()
                          << std::endl;
                std::cout << "\troot\t\t:" << server_config.GetRoot()
                          << std::endl;
                std::cout << "\treturn\t\t:";
                std::pair<int, std::string> rtrn = location_config.GetReturn();
                std::cout << rtrn.first << " > " << rtrn.second << std::endl;
                std::cout << "\tupload_path\t:"
                          << location_config.GetUploadPath() << std::endl;
                std::cout << "\tcgi extension\t:";
                std::vector<std::string> cgi_extensions =
                    location_config.GetCgiExtensions();
                size_t l = -1;
                while (++l < cgi_extensions.size())
                    std::cout << " " << cgi_extensions[l];
                std::cout << std::endl;
                location_it++;
            }
            sc_itr++;
        }
        map_itr++;
    }
}
