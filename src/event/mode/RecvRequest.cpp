#include "RecvRequest.hpp"

#include <cerrno>
#include <iostream>
#include <map>
#include <unistd.h>

#include "CGI.hpp"
#include "Delete.hpp"
#include "EventRegister.hpp"
#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "LocationConfig.hpp"
#include "Post.hpp"
#include "ReadCGI.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include "WriteCGI.hpp"

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

void RecvRequest::Run() {
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

    // methodによって次のイベントが分岐
    try {
        IOEvent* new_event = prepareResponse();
        return new_event;
    } catch (status::code code) { throw std::make_pair(stream_, code); }
    return NULL;
}

IOEvent* RecvRequest::prepareResponse() {
    IOEvent* new_event = NULL;

    // URI クラス作成
    URI uri(searchServerConfig(), req_.GetRequestTarget());
    uri.Init();

    HTTPParser::validate_request(uri, req_);

    if (req_.GetMethod() == "GET") {
        // Uriのパスや拡張子によって ReadFile or ReadCGI
        if (CGI::IsCGI(uri, "GET")) {
            class CGI cgi(uri, req_);
            cgi.Run();
            new_event = new ReadCGI(cgi.FdForReadFromCGI(), stream_, req_);
        } else {
            Get get(stream_, uri);
            get.Run();
            new_event = get.NextEvent();
        }

        this->Unregister();
        new_event->Register();
        return new_event;
    } else if (req_.GetMethod() == "POST") {
        if (CGI::IsCGI(uri, "POST")) {
            class CGI cgi(uri, req_);
            cgi.Run();
            new_event = new WriteCGI(cgi, stream_, req_);
            this->Unregister();
            new_event->Register();
            return new_event;
        } else {
            Post post(stream_, req_, uri);

            post.Run();
            this->Unregister();
            return post.RegisterNext();
        }
    }
    if (req_.GetMethod() == "DELETE") {
        Delete dlt(stream_, uri);

        dlt.Run();
        this->Unregister();
        return dlt.RegisterNext();
    }
    return NULL;
}

const ServerConfig RecvRequest::searchServerConfig() {
    typedef std::vector<const ServerConfig>::const_iterator const_iterator;

    const std::vector<const ServerConfig> config_list =
        stream_.GetServerConfig();

    const_iterator       it     = config_list.begin();
    const const_iterator it_end = config_list.end();

    for (; it != it_end; it++) {
        std::string host      = req_.GetHeaderValue("host");
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
