#include "URI.hpp"

#include <cerrno>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <utility>

#include "Config.hpp"
#include "HTTPStatus.hpp"
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
const std::string&  URI::GetDecodePath() const { return decode_path_; }
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

    p            = divideByTheFirstDelimiterFound(raw_target, "?");
    raw_path_    = p.first;
    decode_path_ = urlDecode(raw_path_);
    std::string::size_type dot_pos = decode_path_.rfind(".");
    if (dot_pos != std::string::npos) {
        extension_ = decode_path_.substr(dot_pos);
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

    location_map locations = server_config_.GetLocationConfigs();

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
        if (location_target_dir.length() <= decode_path_.length() &&
            decode_path_.substr(0, location_target_dir.length()) ==
                location_target_dir) {
            location_config_ = it->second;
            break;
        }
    }
}

void URI::storeLocalPath() {
    if (location_config_.GetAlias() != "") {
        std::string location_target_dir = location_config_.GetTarget();
        if (*location_target_dir.rbegin() != '/') {
            location_target_dir += "/";
        }

        // local_path_を設定する
        std::string alias          = location_config_.GetAlias();
        if (*alias.rbegin() != '/') {
            alias += "/";
        }
        local_path_ = alias + decode_path_.substr(location_target_dir.length());
    } else if (location_config_.GetRoot() != "") {
        std::string root = location_config_.GetRoot();
        if (*root.rbegin() != '/') {
            root += "/";
        }
        local_path_ = root + decode_path_.substr(1);
    } else if (server_config_.GetRoot() != "") {
        std::string root = server_config_.GetRoot();
        if (*root.rbegin() != '/') {
            root += "/";
        }
        local_path_ = root + decode_path_.substr(1);
    } else {
        throw status::bad_request;
    }
}

void URI::statLocalPath() {
    stat(local_path_.c_str(), &stat_buf_);
    // if (stat(local_path_.c_str(), &stat_buf_) < 0) {
    //     throw SysError("stat", errno);
    // }
}

// URLエンコーディングをデコードする
std::string URI::urlDecode(std::string raw_path) {
    // %E3%81%82%E3%81%84%E3%81%86
    std::string decode_path;

    std::string::size_type per_pos = raw_path.find("%");
    // %が見つからなかったら即時リターン
    if (per_pos == std::string::npos) {
        return raw_path;
    }

    // raw_pathの最初から%の直前まで追加
    decode_path.insert(decode_path.end(), raw_path.begin(),
                       raw_path.begin() + per_pos);

    while (per_pos != std::string::npos) {
        decode_path += percentDecode(raw_path, per_pos);

        std::string::size_type perse_start_pos = per_pos;

        per_pos = raw_path.find("%", perse_start_pos);
        if (per_pos == std::string::npos) {
            decode_path.insert(decode_path.end(),
                               raw_path.begin() + perse_start_pos,
                               raw_path.end());
        } else {
            decode_path.append(raw_path, perse_start_pos,
                               per_pos - perse_start_pos);
        }
    }
    return decode_path;
}

// %エンコード1byte分だけ実行
std::string URI::percentDecode(std::string             str,
                               std::string::size_type& per_pos) {
    // %E3(16進数) を charに変換
    std::string hex = str.substr(per_pos + 1, 2);
    char        c   = hexToChar(hex);
    // %E3をスキップした次の文字から解析
    per_pos += 3;
    return std::string(1, c);
}

char URI::hexToChar(std::string hex) {
    char* endp = NULL;
    long  n    = std::strtol(hex.c_str(), &endp, 16);
    if (*endp != '\0' || n < 0) {
        throw status::bad_request;
    } else if ((n == LONG_MAX || n == LONG_MIN) && errno == ERANGE) {
        throw status::bad_request;
    }
    return static_cast<char>(n);
}
