#include "RecvRequest.hpp"

#include <cerrno>
#include <iostream>
#include <map>
#include <unistd.h>

#include "Delete.hpp"
#include "EventRegister.hpp"
#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "LocationConfig.hpp"
#include "Post.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"

#include <iostream>
#include <utility>
#include <vector>

const size_t RecvRequest::BUF_SIZE = 2048;

RecvRequest::RecvRequest()
    : IOEvent(RECV_REQUEST), stream_(StreamSocket()), req_(HTTPRequest()),
      state_(HTTPParser::State(req_)) {}

RecvRequest::RecvRequest(StreamSocket stream)
    : IOEvent(stream.GetSocketFd(), RECV_REQUEST), stream_(stream),
      req_(HTTPRequest()), state_(HTTPParser::State(req_)) {}

RecvRequest::~RecvRequest() {}

void RecvRequest::Run(intptr_t offset) {
    UNUSED(offset);
    char buf[BUF_SIZE];
    int  recv_size = recv(stream_.GetSocketFd(), buf, BUF_SIZE, 0);

    try {
        HTTPParser::update_state(state_, std::string(buf, recv_size));
    } catch (status::code code) {
        throw std::make_pair(stream_, code);
        // EventExecutor::onEventでcatch
    }
}

void RecvRequest::Register() { EventRegister::Instance().AddReadEvent(this); }

void RecvRequest::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* RecvRequest::RegisterNext() {
    if (state_.Phase() != HTTPParser::DONE) {
        return this;
    }
#ifdef WS_DEBUG
    req_.PrintInfo();
#else
    std::cout << req_.GetMethod() << " " << req_.GetRequestTarget() << " "
              << req_.GetVersion() << std::endl;
#endif

    // methodによって次のイベントが分岐
    try {
        this->Unregister();
        IOEvent* new_event = PrepareResponse(req_, stream_);
        return new_event;
    } catch (status::code code) {
        throw std::make_pair(stream_, code);
    } catch (const std::exception& e) {
        throw std::make_pair(stream_, status::server_error);
    }

    return NULL;
}

int RecvRequest::Close() { return 0; }

IOEvent* RecvRequest::PrepareResponse(const HTTPRequest&  req,
                                      const StreamSocket& stream) {
    // URI クラス作成
    URI uri(SearchServerConfig(req, stream), req.GetRequestTarget());
    uri.Init();

    HTTPParser::validate_request(uri, req);

    if (req.GetMethod() == "GET") {
        Get get(stream, uri, req);
        get.Run();
        return get.NextEvent();
    } else if (req.GetMethod() == "POST") {
        Post post(stream, req, uri);
        post.Run();
        return post.RegisterNext();
    }
    if (req.GetMethod() == "DELETE") {
        Delete dlt(stream, uri);
        dlt.Run();
        return dlt.RegisterNext();
    }
    return NULL;
}

const ServerConfig RecvRequest::SearchServerConfig(const HTTPRequest&  req,
                                                   const StreamSocket& stream) {
    typedef std::vector<const ServerConfig>::const_iterator const_iterator;

    const std::vector<const ServerConfig> config_list =
        stream.GetServerConfig();

    const_iterator       it     = config_list.begin();
    const const_iterator it_end = config_list.end();

    for (; it != it_end; it++) {
        std::string host      = req.GetHeaderValue("host");
        std::string host_name = host.substr(0, host.find(":"));
        std::string addr      = getAddrByHostName(host_name);

        if (it->GetServerName() == host_name || it->GetHost() == addr) {
            return *it;
        }
    }
    return *config_list.begin();
}

std::string RecvRequest::getAddrByHostName(std::string host_name) {
    struct addrinfo  hints = {};
    struct addrinfo* info;
    struct in_addr   addr;

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_ADDRCONFIG | AI_NUMERICSERV;
    hints.ai_family   = PF_INET;

    if (getaddrinfo(host_name.c_str(), NULL, &hints, &info) != 0)
        throw status::server_error;
    addr.s_addr = ((struct sockaddr_in*)(info->ai_addr))->sin_addr.s_addr;
    freeaddrinfo(info);

    return inet_ntoa(addr);
}
