#include "Post.hpp"
#include <iostream>

Post::Post() : IOEvent(POST), stream_(StreamSocket()), resp_(HTTPResponse()) {}

Post::Post(StreamSocket stream, HTTPRequest req)
    : IOEvent(POST), stream_(stream), req_(req), resp_(HTTPResponse()) {}

Post::~Post() {}

void Post::Run() {
    // ファイル書き込みの場合
    // 1. 保存先パス&ファイル名取得
    // 2. 書き込み内容取得
    // 3. ファイル書き込み or ファイルオープン
    // 4. 書き込みできたかの確認?
}

IOEvent* Post::RegisterNext() {
    // ファイル書き込みイベント
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());
    resp_.SetStatusCode(303);
    resp_.AppendHeader("Content-Length", "0");
    std::cout << resp_.ConvertToStr() << std::endl;
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    EventRegister::Instance().AddWriteEvent(send_response);

    return send_response;
}
