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

private:
    void        autoIndex(std::string path);
    void        prepareReadFile(std::string path);
    std::string aElement(struct dirent* ent);
    std::string fileInfo(struct dirent* ent, std::string path);
    std::string spaces(std::string name, int n);
    std::string timeStamp(time_t* time);
    std::string fileSize(struct stat* s);

    StreamSocket stream_;
    IOEvent*     next_event_;
    URI&         uri_;
};

#endif // GET_HPP
