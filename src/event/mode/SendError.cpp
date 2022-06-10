#include "SendError.hpp"
#include "DefaultErrorPage.hpp"
#include "EventRegister.hpp"
#include "SendResponse.hpp"
#include <sstream>

SendError::SendError(StreamSocket& stream, status::code code)
    : stream_(stream), status_code_(code) {}

SendError::~SendError() {}

void SendError::Run() {}

void SendError::Register() {}

void SendError::Unregister() {}

IOEvent* SendError::RegisterNext() {
    // config見てデフォルトエラーページの項目あるか確認
    // あったらReadFileを作成、登録
    HTTPResponse resp;
    resp.SetStatusCode(status_code_);

    switch (status_code_) {
    case status::bad_request:
        resp.SetBody(DefaultErrorPage::bad_request);
        break;
    case status::method_not_allowed:
        break;
    }

    std::stringstream ss;
    ss << DefaultErrorPage::bad_request.size() << std::flush;
    resp.AppendHeader("Content-Length", ss.str());

    IOEvent* send_response = new SendResponse(stream_, resp.ConvertToStr());
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}
