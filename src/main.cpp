#include "Server.hpp"
#include <iostream>

void validate_argc(int argc) {
    if (argc != 2) {
        throw std::runtime_error("invalid number of arguments");
    }
}

int main(int argc, char** argv) {
    int     exit_cd = EXIT_SUCCESS;
    Server& server  = Server::Instance();
    try {
        validate_argc(argc);
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
