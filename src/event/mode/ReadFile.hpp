#ifndef READ_FILE_HPP
#define READ_FILE_HPP

#include <string>

#include "HTTPRequest.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "IOEvent.hpp"
#include "StreamSocket.hpp"

class ReadFile : public IOEvent {
public:
    ReadFile(StreamSocket stream, int fd,
             status::code status_code = status::ok);
    virtual ~ReadFile();

    virtual void     Run(intptr_t offset);
    virtual void     Register();
    virtual void     Unregister();
    virtual IOEvent* RegisterNext();
    virtual int      Close();

    static const std::size_t BUF_SIZE;

private:
    ReadFile();
    void printLogStart();
    void printLogEnd();

    // ReadFile に必要な入力
    StreamSocket stream_;
    HTTPRequest  req_;
    bool         finish_;
    status::code status_code_;

    // ReadFile によってできる出力
    std::string  file_content_;
    HTTPResponse resp_;
};

#endif
