#ifndef CGI_HPP
#define CGI_HPP

#include <map>
#include <string>
#include <vector>

#include "ExecveArray.hpp"
#include "HTTPRequest.hpp"
#include "URI.hpp"
class CGI {
public:
    CGI(const URI& uri, const HTTPRequest& req);
    ~CGI();

    static bool IsCGI(const URI& uri, const std::string& method);
    void        Run();
    void        ShutDown();

    int FdForReadFromCGI();
    int FdForWriteToCGI();

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
    const URI&         uri_;
    const HTTPRequest& req_;

    std::string              exec_binary_;
    std::vector<std::string> args_;
    std::vector<std::string> envs_;

    int pipe_for_cgi_write_[2];
    int pipe_for_cgi_read_[2];

    static const std::map<std::string, std::string> binaries;
    static const std::map<std::string, std::string> commands;
    static std::map<std::string, std::string>       setBinaries();
    static std::map<std::string, std::string>       setCommands();
};

#endif
