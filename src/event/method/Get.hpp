#ifndef GET_HPP
#define GET_HPP

#include "HTTPRequest.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include <dirent.h>

class Get {
public:
    Get(StreamSocket stream, URI& uri, const HTTPRequest& req);
    ~Get();

    void     Run();
    IOEvent* NextEvent();

    static const std::string CRLF;

private:
    void processDir();
    void processRedir(std::pair<int, std::string> redir);
    void processIndex(std::string fullpath);
    void tryAutoIndex();
    void prepareReadFile(std::string path);
    void prepareSendResponse(std::string content);
    void prepareEmptySendResponse();
    bool existFile(std::string path);
    bool hasRedir(std::pair<int, std::string> redir, std::string path,
                  std::string target);
    bool hasIndex(std::string index);
    void complementSlash(std::string& path);

    // autoindex
    void        autoIndex();
    std::string aElement(struct dirent* ent, std::string path);
    std::string fileInfo(struct dirent* ent, std::string path);
    std::string spaces(std::string name, int n);
    std::string timeStamp(time_t* time);
    std::string fileSize(struct stat* s);
    std::string generateAutoIndexHTML(DIR* dir);

    // log
    void printLogStart();
    void printLogEnd();
    void printLogNotFound();
    void printLogAutoIndex();
    void printLogReadFile();

    StreamSocket          stream_;
    IOEvent*              next_event_;
    URI&                  uri_;
    const HTTPRequest&    req_;
    const LocationConfig& location_config_;
};

#endif // GET_HPP
