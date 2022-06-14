// #include "EventAction.hpp"
#include "EventExecutor.hpp"
#include "EventRegister.hpp"
#include "ListeningSocket.hpp"

#include "AcceptConn.hpp"
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "IOEvent.hpp"
#include <unistd.h>
#include <vector>

#include <iostream>

int main(void) {
    try {
        EventExecutor executor;
        executor.Init();

        ConfigParser::parseConfigFile("./config/duplicated_port.conf");
        std::map<int, std::vector<const ServerConfig> > server_configs =
            Config::instance()->GetServerConfigs();

        std::map<int, std::vector<const ServerConfig> >::const_iterator it_end =
            server_configs.end();

        std::vector<ListeningSocket> listeners;
        for (std::map<int, std::vector<const ServerConfig> >::const_iterator
                 it = server_configs.begin();
             it != it_end; it++) {

            int                             port    = it->first;
            std::vector<const ServerConfig> configs = it->second;

            ListeningSocket ls(configs);
            ls.Bind("127.0.0.1", port);
            ls.Listen();
            listeners.push_back(ls);

            IOEvent* event = new AcceptConn(ls);
            event->Register();
        }

        while (true) {
            executor.ProcessEvent();
        }

        // 全てのListeningSocketをclose
        for (std::vector<ListeningSocket>::iterator it = listeners.begin();
             it != listeners.end(); it++) {
            it->Close();
        }
        executor.ShutDown();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}