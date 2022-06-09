#include "CGI.hpp"

#include <algorithm>
#include <cerrno>
#include <fcntl.h>
#include <map>
#include <signal.h> // signal()
#include <string>
#include <unistd.h>
#include <vector>

#include "ExecveArray.hpp"
#include "HTTPRequest.hpp"
#include "SysError.hpp"

#include <iostream>

const std::map<std::string, std::string> CGI::binaries = CGI::setBinaries();

const std::map<std::string, std::string> CGI::commands = CGI::setCommands();

CGI::CGI(const HTTPRequest req)
    : req_(req), extension_(std::string()), local_path_(std::string()) {
    // pipeの初期化
    std::fill_n(pipe_for_cgi_write_, 2, -1);
    std::fill_n(pipe_for_cgi_read_, 2, -1);
}

CGI::~CGI() {}

bool CGI::IsCGI(const std::string& target) {
    if (target.length() >= 3 && target.substr(target.length() - 3) == ".pl") {
        return true;
    } else if (target.length() >= 4 &&
               target.substr(target.length() - 4) == ".php") {
        return true;
    } else if (target.length() >= 3 &&
               target.substr(target.length() - 3) == ".py") {
        return true;
    }
    return false;
}

void CGI::Run() {
    cgiParseRequest();
    exec_binary_ = makeExecutableBinary();
    args_        = makeArgs();
    envs_        = makeEnvs();
    createPipe();
    cgiFork();
}

int CGI::FdForReadFromCGI() { return pipe_for_cgi_write_[0]; }

int CGI::FdForWriteToCGI() { return pipe_for_cgi_read_[1]; }

void CGI::cgiParseRequest() {
    std::string target = req_.GetRequestTarget();

    if (target.length() >= 3 && target.substr(target.length() - 3) == ".pl") {
        extension_ = ".pl";
    } else if (target.length() >= 4 &&
               target.substr(target.length() - 4) == ".php") {
        extension_ = ".php";
    } else if (target.length() >= 3 &&
               target.substr(target.length() - 3) == ".py") {
        extension_ = ".py";
    }

    local_path_ = target;
    method_     = req_.GetMethod();
}

std::string CGI::makeExecutableBinary() {
    // 拡張子によって実行ファイルのパスを決定する
    return binaries.find(extension_)->second;
}

std::vector<std::string> CGI::makeArgs() {
    std::vector<std::string> args;

    // command設定
    args.push_back(commands.find(extension_)->second);
    // 実行するスクリプトファイルのパス設定
    args.push_back("." + local_path_);
    // 残りの引数設定
    // args.push_back("arg0");
    // args.push_back("arg1");

    return args;
}

std::vector<std::string> CGI::makeEnvs() {
    std::map<std::string, std::string> env_map;

    // 環境変数を順に決定
    env_map["AUTH_TYPE"]         = "";
    env_map["CONTENT_LENGTH"]    = "5";
    env_map["CONTENT_TYPE"]      = "";
    env_map["GATEWAY_INTERFACE"] = "";
    env_map["PATH_INFO"]         = "";
    env_map["PATH_TRANSLATED"]   = "";
    env_map["QUERY_STRING"]      = "";
    env_map["REMOTE_ADDR"]       = "";
    env_map["REMOTE_HOST"]       = "";
    env_map["REMOTE_IDENT"]      = "";
    env_map["REMOTE_USER"]       = "";
    env_map["REQUEST_METHOD"]    = "";
    env_map["SCRIPT_NAME"]       = "";
    env_map["SERVER_NAME"]       = "";
    env_map["SERVER_PORT"]       = "";
    env_map["SERVER_PROTOCOL"]   = "";
    env_map["SERVER_SOFTWARE"]   = "";

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
    if (method_ == "POST") {
        if (pipe(pipe_for_cgi_read_) < 0) {
            throw SysError("pipe", errno);
        }
        int flags = fcntl(pipe_for_cgi_write_[0], F_GETFD);
        fcntl(pipe_for_cgi_write_[0], F_SETFD, flags | FD_CLOEXEC);
        flags = fcntl(pipe_for_cgi_write_[0], F_GETFL);
        fcntl(pipe_for_cgi_write_[0], F_SETFL, flags | O_NONBLOCK);
    }
    if (pipe(pipe_for_cgi_write_) < 0) {
        throw SysError("pipe", errno);
    }
    int flags = fcntl(pipe_for_cgi_write_[1], F_GETFD);
    fcntl(pipe_for_cgi_write_[1], F_SETFD, flags | FD_CLOEXEC);
}

void CGI::cgiFork() {
    signal(SIGCHLD, SIG_IGN);

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        throw SysError("fork", errno);
    } else if (pid == 0) {
        std::cout << "IN CHILD" << std::endl;
        childOperatePipe();
        execute();
    } else {
        parentOperatePipe();
    }
}

void CGI::childOperatePipe() {
    // stdin
    close(pipe_for_cgi_read_[1]);
    close(STDIN_FILENO);
    dup2(pipe_for_cgi_read_[0], STDIN_FILENO);
    close(pipe_for_cgi_read_[0]);

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
    close(pipe_for_cgi_read_[0]);

    close(pipe_for_cgi_write_[1]);
    fcntl(pipe_for_cgi_write_[0], F_SETFL, O_NONBLOCK);
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