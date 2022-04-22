#include <ServerSocket.hpp>
#include <iostream>

int main()
{
    ServerSocket s_sock;
    s_sock.bindSocket();
    s_sock.listenSocket();
    return 0;
}


// 初期化 // サーバーソケットクラス、

// 入力 // クライアントソケット、httpServerクラス、httpRequest、httpParse

// 処理 // httpProcess

// 出力 httpResponse
