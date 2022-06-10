#include "SendError.hpp"
#include "DefaultErrorPage.hpp"
#include "EventRegister.hpp"
#include "SendResponse.hpp"
#include <sstream>

SendError::SendError(StreamSocket& stream, HTTPResponse& resp)
    : stream_(stream), resp_(resp) {}

SendError::~SendError() {}

void SendError::Run() {}

void SendError::Register() {}

void SendError::Unregister() {}

IOEvent* SendError::RegisterNext() {
    // config見てデフォルトエラーページの項目あるか確認
    // あったらReadFileを作成、登録
    resp_.SetBody(DefaultErrorPage::bad_request);
    std::stringstream ss;
    ss << DefaultErrorPage::bad_request.size() << std::flush;
    resp_.AppendHeader("Content-Length", ss.str());

    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());
    EventRegister::Instance().AddWriteEvent(send_response);
    return send_response;
}
