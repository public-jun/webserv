#include "URI.hpp"

#include <string>
#include <utility>

#include "ServerConfig.hpp"

URI::URI(const ServerConfig& server_config, const std::string target)
    : server_config_(server_config), raw_target_(target) {}

URI::~URI() {}

void URI::Init() {
    // ? で divideする
    divideRawTarget();
}

// raw_target を <raw_path_> '?' <query_> に分割する
void URI::divideRawTarget() {
    std::pair<std::string, std::string> p;
    std::string                         raw_target = raw_target_;

    p         = divideByTheFirstDelimiterFound(raw_target, "?");
    raw_path_ = p.first;
    query_    = p.second;
}

// str を <first> '?' <last> に分割する
std::pair<std::string, std::string>
URI::divideByTheFirstDelimiterFound(std::string str, std::string delimiter) {
    std::string first, last;
    // delimiter があるかどうか調べる
    std::string::size_type pos = str.find(delimiter);

    // <first> 'delimiter' <last> に分割
    first = std::string(str, 0, pos);
    if (pos == std::string::npos) {
        return std::make_pair(first, last);
    }

    last = str.substr(pos + 1);

    return std::make_pair(first, last);
}

// void parseQuery() {
//     // argsを作るか判定
//     // queryに=が含まれていればargsは作らない

// }