#include "URI.hpp"

#include <cerrno>
#include <string>
#include <sys/stat.h>
#include <utility>

#include "Config.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "SysError.hpp"

URI::URI(const ServerConfig server_config, const std::string target)
    : server_config_(server_config), raw_target_(target) {}

URI::~URI() {}

const ServerConfig& URI::GetServerConfig() const { return server_config_; }
const std::string&  URI::GetRawTarget() const { return raw_target_; }
const std::string&  URI::GetExtension() const { return extension_; }
const std::string&  URI::GetRawPath() const { return raw_path_; }
const std::string&  URI::GetQuery() const { return query_; }

const std::vector<std::string>& URI::GetArgs() const { return args_; }
const LocationConfig&           URI::GetLocationConfig() const {
    return location_config_;
}
const std::string& URI::GetLocalPath() const { return local_path_; }
const struct stat& URI::GetStat() const { return stat_buf_; }

void URI::Init() {
    // raw_targetを'?'で分割する
    divideRawTarget();
    // query_からargsを作成する
    storeArgsFromQuery();
    // location_configを探す
    findLocationConfig();
    // location_configのtargetからlocal_pathを作成する
    storeLocalPath();
    // local_path_のstat取得
    statLocalPath();
}

// raw_target を <raw_path_> '?' <query_> に分割する
void URI::divideRawTarget() {
    std::pair<std::string, std::string> p;
    std::string                         raw_target = raw_target_;

    p         = divideByTheFirstDelimiterFound(raw_target, "?");
    raw_path_ = p.first;
    std::string::size_type dot_pos = raw_path_.rfind(".");
    if (dot_pos != std::string::npos) {
        extension_ = raw_path_.substr(dot_pos);
    }
    query_ = p.second;
}

// str を <first> 'sep' <last> に分割する
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

void URI::storeArgsFromQuery() {
    if (query_.empty()) {
        return;
    }

    // argsを作るか判定
    std::string::size_type pos = query_.find("=");
    // queryに=が含まれていればargsは作らない
    if (pos != std::string::npos) {
        return;
    }

    // queryを+でsplitする
    args_ = split(query_, "+");
}

std::vector<std::string> URI::split(std::string str, std::string sep) {
    typedef std::string::size_type size_type;
    std::vector<std::string>       list;

    size_type sep_len = sep.length();

    if (sep_len == 0) {
        return list;
    } else {
        size_type offset = size_type(0);
        while (true) {
            size_type pos = str.find(sep, offset);
            if (pos == std::string::npos) {
                list.push_back(str.substr(offset));
                break;
            }

            list.push_back(str.substr(offset, pos - offset));
            offset = pos + sep_len;
        }
    }

    return list;
}

void URI::findLocationConfig() {
    typedef std::map<const std::string, const LocationConfig> location_map;
    typedef std::map<const std::string,
                     const LocationConfig>::const_reverse_iterator
        const_reverse_iterator;

    location_map locations = server_config_.getLocationConfigs();

    // location configを決定する
    std::string            location_target_dir;
    const_reverse_iterator it_end = locations.rend();
    for (const_reverse_iterator it = locations.rbegin(); it != it_end; it++) {
        location_target_dir = it->first;
        if (*location_target_dir.rbegin() != '/') {
            location_target_dir += "/";
        }

        // location の target と
        // raw_path_のディレクトリのパス部分が最大長で一致するもの
        if (location_target_dir.length() <= raw_path_.length() &&
            raw_path_.substr(0, location_target_dir.length()) ==
                location_target_dir) {
            location_config_ = it->second;
            break;
        }
    }
}

void URI::storeLocalPath() {
    std::string location_target_dir = location_config_.getTarget();
    if (*location_target_dir.rbegin() != '/') {
        location_target_dir += "/";
    }

    // local_path_を設定する
    std::string alias = location_config_.getAlias();
    if (*alias.rbegin() != '/') {
        alias += "/";
    }
    local_path_ = alias + raw_path_.substr(location_target_dir.length());
}

void URI::statLocalPath() {
    stat(local_path_.c_str(), &stat_buf_);
    // if (stat(local_path_.c_str(), &stat_buf_) < 0) {
    //     throw SysError("stat", errno);
    // }
}