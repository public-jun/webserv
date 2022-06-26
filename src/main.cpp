// #include "EventAction.hpp"
#include "EventExecutor.hpp"
#include "EventRegister.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <vector>

#include <iostream>

int main(int argc, char** argv) {
    try {
        if (argc != 2)
            throw(std::runtime_error("invalid number of arguments"));
        Server::Instance().InitServer(argv[1]);

        EventExecutor executor;
        executor.Init();
        while (true) {
            executor.ProcessEvent();
        }

        executor.ShutDown();
        Server::Instance().ShutDownServer();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        executor.ShutDown();
        Server::Instance().ShutDownServer();
        return 1;
    }

    return 0;
}
