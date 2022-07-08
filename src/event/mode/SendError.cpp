#include "SendError.hpp"
#include "EventRegister.hpp"
#include "ReadFile.hpp"
#include "RecvRequest.hpp"
#include "SendResponse.hpp"
#include "ServerConfig.hpp"
#include "URI.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sstream>

SendError::SendError(StreamSocket& stream, status::code code)
    : stream_(stream), status_code_(code) {}

SendError::~SendError() {}

void SendError::Run(intptr_t offset) { UNUSED(offset); }

void SendError::Register() {}

void SendError::Unregister() {}

IOEvent* SendError::RegisterNext() {
    // エラーが起きた場合、sendResponseでデフォルトページを返す
    try {
        ServerConfig server_config =
            RecvRequest::SearchServerConfig(stream_.GetRequest(), stream_);
        std::string error_page = server_config.GetErrorPage()[status_code_];

        if (!existErrorPage(error_page)) {
            return sendResponse();
        }
        // エラーページのpathからlocal pathを取得
        URI uri(server_config, error_page);
        uri.Init();

        int fd = open(uri.GetLocalPath().c_str(), O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            perror("SendError: open");
            return sendResponse();
        }

        return readFile(fd);
    } catch (...) { return sendResponse(); }
}

int SendError::Close() { return 0; }

bool SendError::existErrorPage(const std::string& error_page) {
    return error_page != "";
}

IOEvent* SendError::readFile(int fd) {
    IOEvent* read_file = new ReadFile(stream_, fd, status_code_);
    read_file->Register();
    return read_file;
}

IOEvent* SendError::sendResponse() {
    HTTPResponse resp;
    resp.SetStatusCode(status_code_);

    std::string       body = resp.GetDefaultErrorBody(status_code_);
    std::stringstream body_size;
    body_size << body.size() << std::flush;
    resp.SetBody(body);

    resp.AppendHeader("Content-Length", body_size.str());
    resp.AppendHeader("Content-Type", "text/html; charset=utf-8");
    resp.PrintInfo();

    IOEvent* send_response = new SendResponse(stream_, resp.ConvertToStr());
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}
