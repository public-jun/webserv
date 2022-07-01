#ifndef GET_HPP
#define GET_HPP

#include "IOEvent.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"

class Get {
public:
    Get(StreamSocket stream, URI& uri);
    ~Get();

    void     Run();
    IOEvent* NextEvent();

    static const std::string CRLF;

private:
    void processDir(std::string index, std::string path);
    void processIndex(std::string fullpath, std::string location_path);
    void tryAutoIndex(std::string path);
    void prepareReadFile(std::string path);
    void prepareSendResponse(std::string content);
    bool existFile(std::string path);
    bool hasIndex(std::string index);
    void complementSlash(std::string& path);

    // autoindex
    void        autoIndex(std::string path);
    std::string aElement(struct dirent* ent, std::string path);
    std::string fileInfo(struct dirent* ent, std::string path);
    std::string spaces(std::string name, int n);
    std::string timeStamp(time_t* time);
    std::string fileSize(struct stat* s);

    // log
    void printLogStart();
    void printLogEnd();
    void printLogNotFound();
    void printLogAutoIndex();
    void printLogReadFile();

    StreamSocket          stream_;
    IOEvent*              next_event_;
    URI&                  uri_;
    const LocationConfig& location_config_;
};

#endif // GET_HPP
