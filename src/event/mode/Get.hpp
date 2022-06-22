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
    void        autoIndex(std::string path);
    void        prepareReadFile(std::string path);
    void        prepareSendResponse(std::string content);
    std::string aElement(struct dirent* ent);
    std::string fileInfo(struct dirent* ent, std::string path);
    std::string spaces(std::string name, int n);
    std::string timeStamp(time_t* time);
    std::string fileSize(struct stat* s);
    bool        existFile(std::string path);
    bool        hasIndex(std::string index);

    StreamSocket          stream_;
    IOEvent*              next_event_;
    URI&                  uri_;
    const LocationConfig& location_config_;
};

#endif // GET_HPP
