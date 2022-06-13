#include "URI.hpp"

#include <string>

#include "ServerConfig.hpp"

URI::URI(const ServerConfig& server_config, const std::string target)
    : server_config_(server_config), raw_target_(target) {
    // ? で splitする
    splitTarget();
}

URI::~URI() {}

// raw_target を <raw_path_> '?' <query_> に分割する
void URI::splitTarget() {
    // ? があるかどうか調べる
    std::string::size_type pos = raw_target_.find("?");

    // <raw_path_> '?' <query_> に分割
    raw_path_ = std::string(raw_target_, 0, pos);
    if (pos == std::string::npos) {
        return;
    }

    query_ = raw_target_.substr(pos + 1);
}