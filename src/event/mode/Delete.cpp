#include "Delete.hpp"
#include <iostream>

Delete::Delete()
    : IOEvent(DELETE), stream_(StreamSocket()), resp_(HTTPResponse()) {}

Delete::Delete(StreamSocket stream, HTTPRequest req)
    : IOEvent(DELETE), stream_(stream), req_(req), resp_(HTTPResponse()) {}

Delete::~Delete() {}

void Delete::Run() {
    // 1. 削除対象ファイルパス取得
    // 2. 削除対象ファイルのバリデーション
    //    └ ファイルが存在するか
    //     └ 存在しない場合： 404
    //    └ ファイルであるか
    //     └ ディレクトリの場合： 404
    //    └ 権限はあるか
    //     └ ない場合： 403
    // 3.削除する
    // 4.削除できたかの確認
    //    └ できた場合： 204
    //    └ できなかった場合： 500

    std::cout << "delete run" << std::endl;
    return;
    // 1. 削除対象ファイルパス取得
    std::string target_path = req_.GetRequestTarget();

    // 2. 削除対象ファイルのバリデーション
    struct stat stat_buf;
    // ファイルが存在するか
    // 権限はあるか
    if (access(target_path.c_str(), W_OK) == -1)
        return;
    // ファイルであるか
    if (stat(target_path.c_str(), &stat_buf) == -1)
        return;
    if (S_ISDIR(stat_buf.st_mode))
        return;

    // 3.削除する
    // 4.削除できたかの確認
    if (unlink(target_path.c_str()) == -1)
        status_ = 500;
    status_ = 204;
}

IOEvent* Delete::RegisterNext() {
    // レスポンス作成
    resp_.AppendHeader("Server", "Webserv/1.0.0");
    resp_.SetVersion(req_.GetVersion());
    resp_.SetStatusCode(status_);
    resp_.AppendHeader("Content-Length", "0");
    std::cout << resp_.ConvertToStr() << std::endl;
    IOEvent* send_response = new SendResponse(stream_, resp_.ConvertToStr());

    EventRegister::Instance().AddWriteEvent(send_response);

    return send_response;
}
