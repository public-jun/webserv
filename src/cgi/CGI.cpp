#include "CGI.hpp"

#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <map>
#include <signal.h> // signal()
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "ExecveArray.hpp"
#include "HTTPRequest.hpp"
#include "HTTPStatus.hpp"
#include "SysError.hpp"
#include "URI.hpp"

#include <iostream>

const std::map<std::string, std::string> CGI::BINARIES = CGI::setBinaries();

const std::map<std::string, std::string> CGI::COMMANDS = CGI::setCommands();

CGI::CGI(const URI& uri, const HTTPRequest& req) : uri_(uri), req_(req) {
    // pipeの初期化
    std::fill_n(pipe_for_cgi_write_, 2, -1);
    std::fill_n(pipe_for_cgi_read_, 2, -1);
}

CGI::~CGI() {}

bool CGI::IsCGI(const URI& uri, const std::string& method) {

    const LocationConfig& location_config = uri.GetLocationConfig();

    const std::vector<std::string> allowed_methods =
        location_config.GetAllowedMethods();

    // methodが許可されているか
    if (std::find(allowed_methods.begin(), allowed_methods.end(), method) ==
        allowed_methods.end()) {
        return false;
    }

    // 拡張子を取得
    std::string extension = uri.GetExtension();
    if (extension == "") {
        return false;
    }

    const std::vector<std::string> cgi_extensions =
        location_config.GetCgiExtensions();
    // 拡張子がcgi_extentionに含まれているか
    if (std::find(cgi_extensions.begin(), cgi_extensions.end(), extension) ==
        cgi_extensions.end()) {
        return false;
    }

    return true;
}

void CGI::Run() {
    cgiParseRequest();
    exec_binary_ = makeExecutableBinary();
    args_        = makeArgs();
    envs_        = makeEnvs();
    createPipe();
    cgiFork();
}

void CGI::ShutDown() {
    if (req_.GetMethod() == "POST") {
        close(pipe_for_cgi_read_[1]);
    }
    close(pipe_for_cgi_write_[0]);
}

int CGI::FdForReadFromCGI() { return pipe_for_cgi_write_[0]; }

int CGI::FdForWriteToCGI() { return pipe_for_cgi_read_[1]; }

void CGI::cgiParseRequest() {}

std::string CGI::makeExecutableBinary() {
    // 拡張子によって実行ファイルのパスを決定する
    return BINARIES.find(uri_.GetExtension())->second;
}

std::vector<std::string> CGI::makeArgs() {
    std::vector<std::string> args;

    // command設定
    args.push_back(COMMANDS.find(uri_.GetExtension())->second);
    // 実行するスクリプトファイルのパス設定
    // スクリプトファイルをvalidateする
    const struct stat s = URI::Stat(uri_.GetLocalPath());
    if (S_ISDIR(s.st_mode) || !(s.st_mode & S_IRUSR)) {
        throw status::forbidden;
    }

    args.push_back(uri_.GetLocalPath());
    // 残りの引数設定
    args.insert(args.end(), uri_.GetArgs().begin(), uri_.GetArgs().end());

    return args;
}

std::vector<std::string> CGI::makeEnvs() {
    std::map<std::string, std::string> env_map;
    std::ostringstream                 oss;

    // 環境変数を順に決定
    env_map["AUTH_TYPE"] = req_.GetHeaderValue("authorization");
    if (req_.GetBody().size() > 0) {
        oss << req_.GetBody().size() << std::flush;
        env_map["CONTENT_LENGTH"] = oss.str();
        oss.str("");
    } else {
        env_map["CONTENT_LENGTH"] = "";
    }
    env_map["CONTENT_TYPE"]      = req_.GetHeaderValue("content-type");
    env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    env_map["PATH_INFO"]         = uri_.GetRawPath();
    env_map["PATH_TRANSLATED"]   = uri_.GetLocalPath();
    env_map["QUERY_STRING"]      = uri_.GetQuery();

    env_map["REMOTE_ADDR"] = uri_.GetServerConfig().GetHost(); // ipアドレス
    env_map["REMOTE_HOST"] = uri_.GetServerConfig().GetServerName(); // ipアドレスから逆引きしたホスト名
    env_map["REMOTE_IDENT"] = "";
    env_map["REMOTE_USER"]  = "";

    env_map["REQUEST_METHOD"] = req_.GetMethod();
    env_map["SCRIPT_NAME"]    = uri_.GetDecodePath();
    env_map["SERVER_NAME"]    = uri_.GetServerConfig().GetServerName();

    oss << uri_.GetServerConfig().GetPort() << std::flush;
    env_map["SERVER_PORT"] = oss.str();
    oss.str("");
    env_map["SERVER_PROTOCOL"] = req_.GetVersion();
    env_map["SERVER_SOFTWARE"] = "webserv/1.0.0";

    std::vector<std::string> envs;

    // "key=value" に変換する
    std::map<std::string, std::string>::const_iterator it, it_end;
    it_end = env_map.end();
    for (it = env_map.begin(); it != it_end; it++) {
        envs.push_back((*it).first + "=" + (*it).second);
    }
    return envs;
}

void CGI::createPipe() {
    if (req_.GetMethod() == "POST") {
        if (pipe(pipe_for_cgi_read_) < 0) {
            throw SysError("pipe", errno);
        }
        int flags = fcntl(pipe_for_cgi_read_[0], F_GETFD);
        fcntl(pipe_for_cgi_read_[0], F_SETFD, flags | FD_CLOEXEC);
        flags = fcntl(pipe_for_cgi_read_[0], F_GETFL);
        fcntl(pipe_for_cgi_read_[0], F_SETFL, flags | O_NONBLOCK);
    }
    if (pipe(pipe_for_cgi_write_) < 0) {
        throw SysError("pipe", errno);
    }
    int flags = fcntl(pipe_for_cgi_write_[1], F_GETFD);
    fcntl(pipe_for_cgi_write_[1], F_SETFD, flags | FD_CLOEXEC);
    flags = fcntl(pipe_for_cgi_write_[0], F_GETFL);
    fcntl(pipe_for_cgi_write_[0], F_SETFL, flags | O_NONBLOCK);
}

void CGI::cgiFork() {
    signal(SIGCHLD, SIG_IGN);

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        throw SysError("fork", errno);
    } else if (pid == 0) {
        childOperatePipe();
        execute();
    } else {
        parentOperatePipe();
    }
}

void CGI::childOperatePipe() {
    // stdin
    if (req_.GetMethod() == "POST") {
        close(pipe_for_cgi_read_[1]);
        close(STDIN_FILENO);
        dup2(pipe_for_cgi_read_[0], STDIN_FILENO);
        close(pipe_for_cgi_read_[0]);
    }

    // stdout
    close(pipe_for_cgi_write_[0]);
    close(STDOUT_FILENO);
    dup2(pipe_for_cgi_write_[1], STDOUT_FILENO);
    close(pipe_for_cgi_write_[1]);
}

void CGI::execute() {
    if (execve(exec_binary_.c_str(), ExecveArray(args_).Get(),
               ExecveArray(envs_).Get()) < 0) {
        perror("execve");
        std::exit(1);
    }
}

void CGI::parentOperatePipe() {
    if (req_.GetMethod() == "POST") {
        close(pipe_for_cgi_read_[0]);
    }

    close(pipe_for_cgi_write_[1]);
}

std::map<std::string, std::string> CGI::setBinaries() {
    std::map<std::string, std::string> binaries_map;

    binaries_map[".php"] = "/usr/bin/php";
    binaries_map[".py"]  = "/usr/bin/python3";
    binaries_map[".pl"]  = "/usr/local/bin/perl";

    return binaries_map;
}

std::map<std::string, std::string> CGI::setCommands() {
    std::map<std::string, std::string> commands_map;

    commands_map[".php"] = "php";
    commands_map[".py"]  = "python3";
    commands_map[".pl"]  = "perl";

    return commands_map;
}
