#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <string>
#include <vector>

#include "ExecveArray.hpp"
#include "HTTPRequest.hpp"
class CGI {
public:
    CGI(const HTTPRequest req);
    ~CGI();

    static bool IsCGI(const std::string& target);
    void        Run();

    int FdForReadFromCGI();

private:
    CGI();
    void                     cgiParseRequest();
    std::string              makeExecutableBinary();
    std::vector<std::string> makeArgs();
    std::vector<std::string> makeEnvs();
    void                     createPipe();
    void                     cgiFork();
    void                     childOperatePipe();
    void                     execute();
    void                     parentOperatePipe();

private:
    // To do request から必要な情報を受け取る
    HTTPRequest req_;

    std::string              extension_;
    std::string              local_path_;
    std::string              exec_binary_;
    std::vector<std::string> args_;
    std::vector<std::string> envs_;

    int pipe_for_cgi_write_[2];

    static const std::map<std::string, std::string> binaries;
    static const std::map<std::string, std::string> commands;
    static std::map<std::string, std::string>       setBinaries();
    static std::map<std::string, std::string>       setCommands();
};

#endif
