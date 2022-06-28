// #include "EventAction.hpp"
#include "EventRegister.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <vector>

#include <iostream>

int main(int argc, char** argv) {
    int exit_cd = 0;
    try {
        if (argc != 2)
            throw(std::runtime_error("invalid number of arguments"));
        Server::Instance().InitServer(argv[1]);

        while (true) {
            EventExecutor::Instance().ProcessEvent();
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit_cd = 1;
    }
    try {
        Server::Instance().ShutDownServer();
    } catch (const std::exception& e) { std::cerr << e.what() << std::endl; }
    return exit_cd;
}
