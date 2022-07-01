// #include "EventAction.hpp"
#include "EventRegister.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <vector>

#include <iostream>

int main(int argc, char** argv) {
    int     exit_cd = EXIT_SUCCESS;
    Server& server  = Server::Instance();
    try {

        server.ValidateArgc(argc);
        server.InitServer(argv[1]);
        server.Run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit_cd = EXIT_FAILURE;
    }
    try {
        server.ShutDownServer();
    } catch (const std::exception& e) { std::cerr << e.what() << std::endl; }
    return exit_cd;
}
