#ifndef READ_FILE_HPP
#define READ_FILE_HPP

#include <string>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class ReadFile : public IOEvent {
public:
    ReadFile(StreamSocket stream, int fd);
    virtual ~ReadFile();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

    static const std::size_t BUF_SIZE;

private:
    ReadFile();
    // ReadFile に必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;
    bool         finish_;

    // ReadFile によってできる出力
    std::string  file_content_;
    HTTPResponse resp_;
};

#endif
