#include "SendError.hpp"
#include "EventRegister.hpp"
#include "SendResponse.hpp"
#include <sstream>

SendError::SendError(StreamSocket& stream, status::code code)
    : stream_(stream), status_code_(code) {}

SendError::~SendError() {}

void SendError::Run(intptr_t offset) { UNUSED(offset); }

void SendError::Register() {}

void SendError::Unregister() {}

IOEvent* SendError::RegisterNext() {
    // config見てデフォルトエラーページの項目あるか確認
    // あったらReadFileを作成、登録
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
